/*
 * reducer.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "reducer.h"
#include <iostream> 
#include <assert.h>

void Reducer::open_mpi(NPEvent *event)
{
    MPICall mpic;
    mpic.setTimestamp(event->getTimestamp());
    mpic.setTask(event->getTask());
    mpic.setMPI(event->getEvents(MPI_REGEX));
    mpic.setPath(event->getEvents(LIN_REGEX));
    mpic.setCall(event->getEvents(CAL_REGEX));

    if (partial_result.find(mpic.getSignature()) != partial_result.end())
        partial_result.at(mpic.getSignature()).reduce(mpic);
    else
        partial_result.insert({mpic.getSignature(), ReducedMPICall(mpic,texe)});
}

void Reducer::close_mpi(NPEvent *event)
{
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

    if (record->getType() == PRV_EVENT)
        this->process(static_cast<NPEvent*>(record));
    //else if (record->getType() == PRV_STATE)
    //    this->process(static_cast<NPStat*>(record));
    //else if (record->getType() == PRV_COMM)
    //    this->process(static_cast<NPComm*>(record));
}

void Reducer::process(NPEvent *event)
{
    int i;
    if ((i = event->existEvent(MPI_REGEX)) != -1)
    {
        if (event->getEvent(i).second == "0")
            this->close_mpi(event);
        else
            this->open_mpi(event);
    }

}

void process(NPComm *comm)
{}
void process(NPStat *stat)
{}

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
