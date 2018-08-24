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
#include <assert.h>
#include <iostream>
#include <boost/assert.hpp>

#define COMPRESS_EPS 0.1f

class RunLenghEncVector 
    : public std::vector<std::pair<unsigned int, long long>>
{
    public:
        RunLenghEncVector(float alias_tolerance = COMPRESS_EPS) 
            : std::vector<std::pair<unsigned int, long long>>()
            , alias_tolerance(alias_tolerance) {}
        void push_back(long long v);
        long long getMean();
        long long operator[](unsigned int i)
        {
            unsigned int pos = 0;
            for (unsigned int j=0; j<this->size(); ++j)
            {
                pos += this->at(j).first;
                if (pos >= i)
                    return this->at(j).second;
            }
            
            std::string msg = "ERROR: Requested it:" + std::to_string(i) 
                + " of " + std::to_string(pos);
            std::cout << msg << std::endl;
            assert(false);
            //BOOST_ASSERT_MSG(false, msg.c_str());
        }
    private:
        float alias_tolerance;
};

class CPUBurst
{
    public:
        CPUBurst()
            : duration(0) {}
        CPUBurst(unsigned int task, long long duration)
            : task(task)
            , duration(duration) {}
        long long getDuration()
            { return duration; }
        std::vector<unsigned int> getHWCTypes()
            { return this->hwc_type; }
        std::vector<unsigned int> getHWCValues()
            { return this->hwc_value; }
        void addHWCounter(unsigned int hwc_type, unsigned int hwc_value);
        unsigned int getTask() {return this->task;};
    private:
        unsigned int task;
        long long duration;
        std::vector<unsigned int> hwc_type;
        std::vector<unsigned int> hwc_value;
};

class ReducedCPUBurst
{
    public:
        ReducedCPUBurst() {}
        ReducedCPUBurst(CPUBurst cpu_burst)
        {
            this->tasks.push_back(cpu_burst.getTask());
            this->repetitions.push_back(1);
            this->durations.push_back(RunLenghEncVector());
            this->durations.back().push_back(cpu_burst.getDuration());

            this->hwc_types.push_back(std::vector<unsigned int>());
            this->hwc_values.push_back(std::vector<RunLenghEncVector>());

            for (unsigned int i=0; i < cpu_burst.getHWCTypes().size(); ++i)
            {
                unsigned int hwc_type = cpu_burst.getHWCTypes()[i];
                unsigned int hwc_value = cpu_burst.getHWCValues()[i];

                this->hwc_types.back().push_back(hwc_type);
                this->hwc_values.back().push_back(RunLenghEncVector());
                this->hwc_values.back().back().push_back(hwc_value);
            }
        }
        void reduce(CPUBurst);
        // TODO: Is just showing the first rank HWC information. Fix it!
        long long getDuration()
        { 
            return this->durations[0].getMean();
            /*
            unsigned int sum=0;
            for (auto it : this->durations)
                sum += it.getMean();
            return sum/this->durations.size();
            */
        }
        // TODO: Is just showing the first rank HWC information. Fix it!
        unsigned int getHWCCount()
            { return (unsigned int)this->hwc_types[0].size(); }
        std::vector<unsigned int>* getTasks()
            { return &(this->tasks); }
        // TODO: Is just showing the first rank HWC information. Fix it!
        std::pair<unsigned int, unsigned int> getHWCAt(unsigned int i)
        {
            return std::make_pair(this->hwc_types[0][i], 
                    this->hwc_values[0][i].getMean());
        }
    private:
        std::vector<unsigned int> repetitions;
        std::vector<unsigned int> tasks;
        std::vector<RunLenghEncVector> durations;
        std::vector<std::vector<unsigned int>> hwc_types;
        std::vector<std::vector<RunLenghEncVector>> hwc_values;
};


class Callpath
{
    public:
        unsigned long int getSignature() { return this->signature; };
        unsigned int getTask() {return this->task;};
        void setPath(std::vector<std::pair<std::string,std::string>> v);
        void setCall(std::vector<std::pair<std::string,std::string>> v);
        void setTimestamp(long long timestamp) {this->timestamp = timestamp;};
        void setTask(unsigned int task) {this->task = task;};
        long long getTimestamp() { return this->timestamp;};
        std::vector<int> getCallpath() { return this->callpath; }
        std::vector<int> getCallers() { return this->caller; }
    protected:
        std::vector<int> callpath; //file, line pair ids (pcf)
        std::vector<int> caller; 
        unsigned long int signature = 0;
        long long timestamp;
        unsigned int task;
};

class MPICall : public Callpath
{
    public:
        MPICall() 
            : Callpath()
            , comm_matched(false) {};
        unsigned int getMPIid() { return this->mpiid; };
        unsigned long int getSignature() { return this->signature+this->mpiid; };
        unsigned int getMpiType() { return this->mpitype; };
        unsigned int getMpiId() { return this->mpiid; };
        void setMPI(std::vector<std::pair<std::string,std::string>> v);
        void setDuration(long long d) { this->duration = d; }
        long long getDuration() { return this->duration; }
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
        void setPreviousCPUBurst(CPUBurst burst)
            { this->previous_cpu_burst = burst; }
        CPUBurst getPreviousCPUBurst()
            { return this->previous_cpu_burst; }
    protected:
        unsigned int mpitype;
        unsigned int mpiid;
        long long duration;
        unsigned int msg_size;
        unsigned int task_partner;
        bool comm_matched;
        CPUBurst previous_cpu_burst;
};

class ReducedMPICall : public MPICall
{
    public:
        ReducedMPICall() : MPICall() {};
        ReducedMPICall(MPICall mpicall, long long texe);
        void reduce(MPICall mpic);
        float getDelta();
        unsigned int getRepetitions();
        unsigned int getInterRepTime();
        long long getTimestampAt(unsigned int i);
        unsigned int getDuration();
        unsigned int getMsgSize();
        std::vector<unsigned int>* getTasks()
            { return &(this->tasks); }
        unsigned int getFirstTimestamp()
            { return this->first_timestamp; }
        ReducedCPUBurst* getPreviousCPUBurst()
        {
            return &(this->previous_cpu_burst);
        }
        long long getPreviousCPUBurstDuration()
        {
            return this->previous_cpu_burst.getDuration();
        }
    private:
        float delta;
        std::vector<unsigned int> tasks;
        std::vector<unsigned int> repetitions;
        std::vector<long long> last_timestamp;
        std::vector<long long> mean_interrep_times;
        std::vector<RunLenghEncVector> interrep_times;
        std::vector<long long> mean_durations;
        std::vector<RunLenghEncVector> durations;
        std::vector<unsigned int> mean_msg_size;
        std::vector<RunLenghEncVector> msg_size;
        std::vector<unsigned int> task_partner;
        ReducedCPUBurst previous_cpu_burst;
        long long  texe;
        unsigned int first_timestamp;

        void add_time(unsigned int timestamp);
};

#endif /* !MPICALL_H */
