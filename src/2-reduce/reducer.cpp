/*
 * reducer.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "reducer.h"
#include <iostream> 
#include <assert.h>

#define TID(event) event->getTask()-1

void Reducer::hwc_event(unsigned int task, 
        std::pair<std::string, std::string> event)
{
    this->last_cpu_burst[task].addHWCounter(
            std::atoll(event.first.c_str()),
            std::atoll(event.second.c_str()));
}

void Reducer::open_mpi(NPEvent *event)
{
    MPICall mpic;
    mpic.setTimestamp(event->getTimestamp());
    mpic.setTask(event->getTask());
    mpic.setMPI(event->getEvents(MPI_REGEX));
    mpic.setPath(event->getEvents(LIN_REGEX));
    mpic.setCall(event->getEvents(CAL_REGEX));

    mpic.setPreviousCPUBurst(this->last_cpu_burst[TID(event)]);

    this->last_mpicall[TID(event)] = mpic;
}

void Reducer::close_mpi(NPEvent *event)
{
    MPICall mpic = this->last_mpicall[event->getTask()-1];
    unsigned int duration = event->getTimestamp() - mpic.getTimestamp();
    mpic.setDuration(duration);

    if (!mpic.isMatched())
    {
        // should match by logical recv
        if(this->comm_match[TID(event)].find(event->getTimestamp()) 
                == this->comm_match[TID(event)].end())
        {
            //std::cout << "Warning: MPI call w/o comms info ("
            //    << mpic.getMPIid() << ")" << std::endl;
        }
        else
        {
            NPComm* comm = this->comm_match[TID(event)].at(event->getTimestamp());
            mpic.matchComm(comm->task_send_id, comm->size);
            this->comm_match[TID(event)].erase(event->getTimestamp());
        }
    }

    if (partial_result.find(mpic.getSignature()) != partial_result.end())
        partial_result.at(mpic.getSignature()).reduce(mpic);
    else
        partial_result.insert({mpic.getSignature(), ReducedMPICall(mpic,texe)});
    //delete &(this->last_mpicall[event->getTask()-1]);
}

void Reducer::actual_run(NPRecord *record)
{
    if (!record) // centinel
    {
        this->filter();
        for (auto it = this->partial_result.begin(); 
                it != partial_result.end(); ++it)
            this->result->push_back(it->second);
        this->done = true;
        return;
    }

    if (record->getType() == PRV_STATE)
        this->process(static_cast<NPStat*>(record));
    else if (record->getType() == PRV_EVENT)
        this->process(static_cast<NPEvent*>(record));
    else if (record->getType() == PRV_COMM)
        this->process(static_cast<NPComm*>(record));
}

void Reducer::process(NPEvent *event)
{
    int i;
    // First look for hwc
    for (auto e : event->getEvents("420000"))
    {
        this->hwc_event(TID(event), e);
    }

    // Then loop for mpi events
    for (auto e : event->getEvents("5000000"))
    {
        if (e.second == "0")
            this->close_mpi(event);
        else
            this->open_mpi(event);
    }

    /*
    if ((i = event->existEvent(MPI_REGEX)) != -1)
    {
        if (event->getEvent(i).second == "0")
            this->close_mpi(event);
        else
            this->open_mpi(event);
    }
    else if((i = event->existEvent(HWC_REGEX)) != -1)
    {
        this->hwc_event(TID(event),event->getEvent(i));
    }
    */
}

void Reducer::process(NPComm *comm)
{
    MPICall *mpic = &(this->last_mpicall[TID(comm)]);
    mpic->matchComm(comm->task_recv_id, comm->size);

    // Save communication for recv partner matching
    this->comm_match[comm->task_recv_id-1].insert({
            std::max(comm->logical_recv, comm->phyisical_recv), comm});
}

void Reducer::process(NPStat *stat)
{
    if (stat->getState() == 1)
    {
        CPUBurst new_burst(stat->getTask(), 
                stat->getEndTime() - stat->getBeginTime());
        this->last_cpu_burst[TID(stat)] = new_burst;
    }
}

void Reducer::filter()
{
    for(auto it = this->partial_result.begin(); 
            it != this->partial_result.end();)
    {
        if (it->second.getDelta() < this->lbound)
            it = this->partial_result.erase(it);
        else
            ++it;
    }
}
