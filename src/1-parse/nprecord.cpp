/*
 * nprecord.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include <nprecord.h>
#include <boost/algorithm/string.hpp>

NPRecord::NPRecord(const std::string line)
{
    char specific_info[line.size()];
    sscanf(line.c_str(), "%u:%u:%u:%u:%u:%u:%s",
            &(this->type),
            &(this->cpu_id),
            &(this->app_id),
            &(this->task_id),
            &(this->thread_id),
            &(this->timestamp),
            specific_info);
    this->specific_info = std::string(specific_info);
}

NPRecord *NPRecord::build(const std::string &line)
{
    switch(line[0])
    {
        case STATEID:
            return new NPStat(line);
        case EVENTID:
            return new NPEvent(line);
        case COMMCID:
            return new NPComm(line);
    }
    return NULL;
}

NPEvent::NPEvent(const std::string line)
    : NPRecord(line)
{
    std::vector<std::string> fields;
    boost::split(fields, this->specific_info, boost::is_any_of(":"));

    for (int i=0; i<fields.size(); i+=2)
    {
        this->type.push_back(fields[i]);
        this->value.push_back(fields[i+1]);
    }
}

std::pair<std::string, std::string> NPEvent::getEvent(int i)
{
    std::pair<std::string, std::string> res;
    res = std::make_pair(this->type[i], this->value[i]);
    return res;
}

int NPEvent::existEvent(const std::string type) const
{
    for (int i=0; i<this->type.size(); ++i)
    {
        if (this->type[i].find(type) != std::string::npos)
        {
            return i;
        }
    }
    return -1;
}

std::vector<std::pair<std::string,std::string>> 
NPEvent::getEvents(const std::string type)
{
    std::vector<std::pair<std::string, std::string>> res;
    for (int i=0; i<this->type.size(); ++i)
    {
        if (this->type[i].find(type) != std::string::npos)
            res.push_back(make_pair(this->type[i], this->value[i]));
    }
    return res;
}

std::vector<std::pair<std::string,std::string>> 
NPEvent::getEvents()
{
    std::vector<std::pair<std::string, std::string>> res;
    for (int i=0; i<this->type.size(); ++i)
    {
        res.push_back(make_pair(this->type[i], this->value[i]));
    }
    return res;
}

NPComm::NPComm(const std::string line)
    : NPRecord(line)
{
    std::vector<std::string> fields;
    boost::split(fields, this->specific_info, boost::is_any_of(":"));

    this->logical_send = this->timestamp;
    this->phyisical_send = std::atoll(fields[0].c_str());
    this->cpu_recv_id = std::atoll(fields[1].c_str());
    this->ptask_recv_id = std::atoll(fields[2].c_str());
    this->task_recv_id = std::atoll(fields[3].c_str());
    this->thread_recv_id = std::atoll(fields[4].c_str());
    this->logical_recv = std::atoll(fields[5].c_str());
    this->phyisical_recv = std::atoll(fields[6].c_str());
    this->size = std::atoll(fields[7].c_str());
    this->tag = std::atoll(fields[8].c_str());
}

NPStat::NPStat(const std::string line)
    : NPRecord(line)
{
    std::vector<std::string> fields;
    boost::split(fields, this->specific_info, boost::is_any_of(":"));

    this->begin_time = this->timestamp;
    this->end_time = std::atoll(fields[0].c_str());
    this->state = std::atoll(fields[1].c_str());
}

