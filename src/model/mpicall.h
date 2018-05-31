/*
 * mpicall.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef MPICALL_H
#define MPICALL_H

#include <vector>
#include <string>

#define COMPRESS_EPS 0.1

class RunLenghEncVector 
    : public std::vector<std::pair<unsigned int,unsigned int>>
{
    public:
        RunLenghEncVector() 
            : std::vector<std::pair<unsigned int,unsigned int>>() {}
        void push_back(unsigned int v);
        unsigned int getMean();
};

class Callpath
{
    public:
        unsigned int getSignature() { return this->signature; };
        unsigned int getTask() {return this->task;};
        void setPath(std::vector<std::pair<std::string,std::string>> v);
        void setCall(std::vector<std::pair<std::string,std::string>> v);
        void setTimestamp(unsigned int timestamp) {this->timestamp = timestamp;};
        void setTask(unsigned int task) {this->task = task;};
        unsigned int getTimestamp() { return this->timestamp;};
        std::vector<int> getCallpath() { return this->callpath; }
        std::vector<int> getCallers() { return this->caller; }
    protected:
        std::vector<int> callpath; //file, line pair ids (pcf)
        std::vector<int> caller; 
        unsigned long int signature = 0;
        unsigned int timestamp;
        unsigned int task;
};

class MPICall : public Callpath
{
    public:
        MPICall() 
            : Callpath()
            , comm_matched(false) {};
        unsigned int getMPIid() { return this->mpiid; };
        unsigned int getSignature() { return this->signature+this->mpiid; };
        unsigned int getMpiType() { return this->mpitype; };
        unsigned int getMpiId() { return this->mpiid; };
        void setMPI(std::vector<std::pair<std::string,std::string>> v);
        void setDuration(unsigned int d) { this->duration = d; }
        unsigned int getDuration() { return this->duration; }
        void matchComm(unsigned int partner, unsigned int size)
        {
            this->task_partner = partner;
            this->msg_size = size;
            this->comm_matched = true;
        }
        bool isMatched()
            { return this->comm_matched; }
        unsigned int getMsgSize()
            { return this->msg_size; }
    private:
        unsigned int mpitype;
        unsigned int mpiid;
        unsigned int duration;
        unsigned int msg_size;
        unsigned int task_partner;
        bool comm_matched;
};

class ReducedMPICall : public MPICall
{
    public:
        ReducedMPICall() : MPICall() {};
        ReducedMPICall(MPICall mpicall, unsigned int texe);
        void reduce(MPICall mpic);
        float getDelta();
        unsigned int getRepetitions();
        unsigned int getInterRepTime();
        unsigned int getDuration();
        unsigned int getMsgSize();
        std::vector<unsigned int>* getTasks()
            { return &(this->tasks); }
    private:
        float delta;
        std::vector<unsigned int> tasks;
        std::vector<unsigned int> repetitions;
        std::vector<unsigned int> last_timestamp;
        std::vector<unsigned int> mean_interrep_times;
        std::vector<RunLenghEncVector> interrep_times;
        std::vector<unsigned int> mean_durations;
        std::vector<RunLenghEncVector> durations;
        std::vector<unsigned int> mean_msg_size;
        std::vector<RunLenghEncVector> msg_size;
        std::vector<unsigned int> task_partner;
        unsigned int texe;

        void add_time(unsigned int timestamp);
};

#endif /* !MPICALL_H */
