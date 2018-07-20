/*
 * mpicall.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "mpicall.h"
#include <assert.h>
#include <algorithm>
#include <boost/bind.hpp>
#include <iostream>
#include <numeric>
#include <math.h>

void RunLenghEncVector::push_back(unsigned int v)
{
    if (this->size() == 0)
    {
        std::vector<std::pair<unsigned int,unsigned int>>::push_back(
                std::make_pair(1,v));
    }
    else
    {
        unsigned int lastv = this->back().second;
        unsigned int ubound = ceil((float)lastv*(1+this->alias_tolerance));
        unsigned int bbound = ceil((float)lastv*(1-this->alias_tolerance));

        if (bbound <= v  and v <= ubound)
            this->back().first += 1; // compressed
        else
            std::vector<std::pair<unsigned int,unsigned int>>::push_back(
                    std::make_pair(1,v));
    }
}

std::pair<unsigned int, unsigned int> pair_addition(
        std::pair<unsigned int, unsigned int> a, 
        std::pair<unsigned int, unsigned int> b)
{
    return std::make_pair<unsigned int, unsigned int>(
            a.first+b.first,
            a.second+b.second);
}

unsigned int RunLenghEncVector::getMean()
{
    unsigned long long acc_durations = 0;
    unsigned int acc_repetitions = 0;

    std::vector<std::pair<unsigned int, unsigned int>>::iterator it;
    for (it=this->begin(); it != this->end(); ++it)
    {
        acc_durations += it->first * it->second;
        acc_repetitions += it->first;
    }
    return acc_durations/acc_repetitions;
}

void Callpath::setPath(std::vector<std::pair<std::string,std::string>> v)
{
    std::sort(v.begin(), v.end(),
          boost::bind(&std::pair<std::string,std::string>::first, _1) <
          boost::bind(&std::pair<std::string,std::string>::first, _2));

    this->signature = 0;
    for (int i=0; i<v.size(); ++i)
    {
        int val = std::atoll(v[i].second.c_str());
        this->callpath.push_back(val);
        this->signature += val;
        this->signature *= 100;
    }
}

void Callpath::setCall(std::vector<std::pair<std::string,std::string>> v)
{
    std::sort(v.begin(), v.end(),
          boost::bind(&std::pair<std::string,std::string>::first, _1) <
          boost::bind(&std::pair<std::string,std::string>::first, _2));

    for (int i=0; i<v.size(); ++i)
        this->caller.push_back(std::atoll(v[i].second.c_str()));
}

void MPICall::setMPI(std::vector<std::pair<std::string,std::string>> v)
{
    assert (v.size() == 1);
    this->mpitype = atoll(v[0].first.c_str());
    this->mpiid = atoll(v[0].second.c_str());
}

ReducedMPICall::ReducedMPICall(MPICall mpicall, unsigned int texe)
    : MPICall(mpicall)
    , texe(texe)
    , first_timestamp(mpicall.getTimestamp())
{
    this->tasks.push_back(mpicall.getTask());
    this->last_timestamp.push_back(mpicall.getTimestamp());
    this->interrep_times.push_back(RunLenghEncVector());
    this->repetitions.push_back(1);
    this->durations.push_back(RunLenghEncVector());
    this->durations.back().push_back(mpicall.getDuration());
    this->msg_size.push_back(RunLenghEncVector());
    this->msg_size.back().push_back(mpicall.getMsgSize());
    this->previous_cpu_burst = ReducedCPUBurst(mpicall.getPreviousCPUBurst());
}

void ReducedMPICall::reduce(MPICall mpic)
{
    auto it = std::find(this->tasks.begin(),this->tasks.end(), mpic.getTask());
    if (it == this->tasks.end())
    {
        this->tasks.push_back(mpic.getTask());
        //std::sort(this->tasks.begin(), this->tasks.end());
        this->last_timestamp.push_back(mpic.getTimestamp());
        this->interrep_times.push_back(RunLenghEncVector());
        this->repetitions.push_back(1);
        this->durations.push_back(RunLenghEncVector());
        this->durations.back().push_back(mpic.getDuration());
        this->msg_size.push_back(RunLenghEncVector());
        this->msg_size.back().push_back(mpic.getMsgSize());
    }
    else
    {
        auto ind = std::distance(this->tasks.begin(), it);
        this->repetitions[ind] += 1;

        unsigned int last_duration = mpic.getTimestamp() 
            - this->last_timestamp[ind];
        this->interrep_times[ind].push_back(last_duration);
        this->durations[ind].push_back(mpic.getDuration());
        this->msg_size[ind].push_back(mpic.getMsgSize());
        this->last_timestamp[ind] = mpic.getTimestamp();
    }
    this->previous_cpu_burst.reduce(mpic.getPreviousCPUBurst());
}

unsigned int ReducedMPICall::getRepetitions()
{
    assert (std::adjacent_find(
            this->repetitions.begin(), 
            this->repetitions.end(), 
            std::not_equal_to<unsigned int>()) == this->repetitions.end());
    return this->repetitions[0];
}

unsigned int ReducedMPICall::getInterRepTime()
{
    if (this->mean_interrep_times.size() < this->interrep_times.size())
        for (auto v:this->interrep_times)
            this->mean_interrep_times.push_back(v.getMean());

    return std::accumulate(
            this->mean_interrep_times.begin(),
            this->mean_interrep_times.end(), 0)/this->mean_interrep_times.size();
}

unsigned int ReducedMPICall::getDuration()
{
    if (this->mean_durations.size() < this->durations.size())
        for (auto v:this->durations)
            this->mean_durations.push_back(v.getMean());

    return std::accumulate(
            this->mean_durations.begin(),
            this->mean_durations.end(), 0)/this->mean_durations.size();
}

unsigned int ReducedMPICall::getMsgSize()
{
    if (this->mean_msg_size.size() < this->msg_size.size())
        for (auto v:this->msg_size)
            this->mean_msg_size.push_back(v.getMean());

    return std::accumulate(
            this->mean_msg_size.begin(),
            this->mean_msg_size.end(), 0)/this->mean_msg_size.size();
}


float ReducedMPICall::getDelta()
{
    if (this->getRepetitions() > 1)
        return float(this->getRepetitions()*this->getInterRepTime())/this->texe;
    else
        return 0;
}


unsigned int ReducedMPICall::getTimestampAt(unsigned int i)
{
    unsigned int res=0;
    for(int j=0; j<i; ++j)
        res += this->interrep_times[0][j];
    return res+this->first_timestamp;
}

void CPUBurst::addHWCounter(unsigned int hwc_type, unsigned int hwc_value)
{
    this->hwc_type.push_back(hwc_type);
    this->hwc_value.push_back(hwc_value);
}

void ReducedCPUBurst::reduce(CPUBurst cpu_burst)
{
    auto it = std::find(this->tasks.begin(),this->tasks.end(), cpu_burst.getTask());
    if (it == this->tasks.end())
    {
        this->tasks.push_back(cpu_burst.getTask());
        this->repetitions.push_back(1);
        this->durations.push_back(RunLenghEncVector());
        this->durations.back().push_back(cpu_burst.getDuration());

        this->hwc_types.push_back(std::vector<unsigned int>());
        this->hwc_values.push_back(std::vector<RunLenghEncVector>());

        for (int i=0; i < cpu_burst.getHWCTypes().size(); ++i)
        {
            unsigned int hwc_type = cpu_burst.getHWCTypes()[i];
            unsigned int hwc_value = cpu_burst.getHWCValues()[i];

            this->hwc_types.back().push_back(hwc_type);
            this->hwc_values.back().push_back(RunLenghEncVector());
            this->hwc_values.back().back().push_back(hwc_value);
        }
    }
    else
    {
        auto ind = std::distance(this->tasks.begin(), it);
        this->repetitions[ind] += 1;

        this->durations[ind].push_back(cpu_burst.getDuration());
        for (int i=0; i < cpu_burst.getHWCTypes().size(); ++i)
        {
            unsigned int hwc_type = cpu_burst.getHWCTypes()[i];
            unsigned int hwc_value = cpu_burst.getHWCValues()[i];

            auto it = std::find(this->hwc_types[ind].begin(), 
                    this->hwc_types[ind].end(), 
                    hwc_type);

            if (it != this->hwc_types[ind].end())
            {
                auto ind2 = std::distance(this->hwc_types[ind].begin(), it);
                this->hwc_values[ind][ind2].push_back(hwc_value);
            }
            else
            {
                this->hwc_types[ind].push_back(hwc_type);
                this->hwc_values[ind].push_back(RunLenghEncVector());
                this->hwc_values[ind].back().push_back(hwc_value);
            }
        }
    }
}
