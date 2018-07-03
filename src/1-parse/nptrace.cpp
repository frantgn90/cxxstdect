/*
 * nptrace.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "nptrace.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>

struct sort_same_instant_records
{
    inline bool operator() (const NPRecord* r1, const NPRecord* r2)
    {
        if (r1->getType() == 1 and r2->getType() == 1)
        {
            const NPStat* s1 = static_cast<const NPStat*>(r1);
            const NPStat* s2 = static_cast<const NPStat*>(r2);
            return (s1->getEndTime() > s2->getEndTime());
        }
        if (r1->getType() == 1)
            return false;
        if (r2->getType() == 1)
            return true;

        if (r1->getType() == 2)
        {
            const NPEvent * event = static_cast<const NPEvent*>(r1);
            if (event->existEvent("42000") != std::string::npos)
                return true;
        }
        if (r2->getType() == 2)
        {
            const NPEvent * event = static_cast<const NPEvent*>(r2);
            if (event->existEvent("42000") != std::string::npos)
                return false;
        }
        return true;
    }
};

NPRecord* NPTrace::getNextRecord()
{
    if (this->done)
        return NULL;

    std::string line;
    this->file->getline(line);
    this->progress_reporter->addProgress(line.size());
    NPRecord* res = NPRecord::build(line);

    // Just in case line is invalid
    while (!res and !this->file->eof())
    {
        this->file->getline(line);
        this->progress_reporter->addProgress(line.size());
        res = NPRecord::build(line);
    }

    // Finalize the parsing
    if (this->file->eof())
    {
        this->done = true;
        this->file->close();
    }
    return res;
}

void NPTrace::printReorderBuffer()
{
    if (this->reorder_buffer.size() > 1)
    {
        for (auto it : this->reorder_buffer)
        {
            if (it->getType() == 1)
            {
                NPStat* stat = static_cast<NPStat*>(it);
                std::cout << it->getTimestamp() << 
                    " :: STAT : " << stat->getState() << std::endl;
            }
            else if (it->getType() == 2)
            {
                NPEvent* event = static_cast<NPEvent*>(it);
                std::cout << it->getTimestamp() << 
                    " :: EVENT " << std::endl;
                for (auto it2 : event->getEvents())
                {
                    std::cout << "   " << it2.first << ":" << it2.second 
                        << std::endl;
                }
            }
            else if (it->getType() == 3)
            {
                std::cout << it->getTimestamp() << 
                    " :: COMM " << std::endl;
            }
        }
    }
}

void NPTrace::actual_run(std::string *trace_file)
{
    if (!this->init)
    {
        this->file = TraceStream::openFile(*trace_file);
        if ( !this->file->good() )
            exit(-1);
    
        std::string header;
        this->file->getline(header); // dummy
        this->progress_reporter->setMax(
                TraceStream::getTraceFileSize(*trace_file));
        this->progress_reporter->addProgress(header.size());
        this->init = true;
    }

    // Until reorder buffer is void
    if (this->reorder_buffer.size() > 0)
    {
        this->result = this->reorder_buffer[0];
        this->reorder_buffer.erase(this->reorder_buffer.begin());
        return;
    }

    if (!this->first_record_after_reorder)
        this->reorder_buffer.push_back(getNextRecord());
    else
    {
        this->reorder_buffer.push_back(this->first_record_after_reorder);
        this->first_record_after_reorder = NULL;
    }

    NPRecord *next_record = getNextRecord();
    while(next_record != NULL and 
            (next_record->getTimestamp() == reorder_buffer.back()->getTimestamp()))
    {
        this->reorder_buffer.push_back(next_record);
        next_record = getNextRecord();
        if (next_record == NULL)
            break;
    }
    if (next_record != NULL)
        this->first_record_after_reorder = next_record;

    // Reorder the buffer
    //printReorderBuffer();
    //std::cout << "----------------" << std::endl;

    std::sort(this->reorder_buffer.begin(), this->reorder_buffer.end(),
            sort_same_instant_records());

    //printReorderBuffer();
    //std::cout << "==============" << std::endl;

    // Send the first one
    this->result = this->reorder_buffer[0];
    this->reorder_buffer.erase(this->reorder_buffer.begin());
}
