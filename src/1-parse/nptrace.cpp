/*
 * nptrace.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "nptrace.h"
#include <iostream>
#include <stdio.h>

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

    std::string line;
    this->file->getline(line);
    this->progress_reporter->addProgress(line.size());

    this->result = NPRecord::build(line);
    while (!result and !this->file->eof())
    {
        this->file->getline(line);
        this->progress_reporter->addProgress(line.size());
        this->result = NPRecord::build(line);
    }

    if (this->file->eof())
    {
        this->done = true;
        this->file->close();
    }

}
