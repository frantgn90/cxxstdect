/*
 * nprecord.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef NPRECORD_H
#define NPRECORD_H

#include <string>
#include <vector>
#include <iostream>

#define STATEID '1'
#define EVENTID '2'
#define COMMCID '3'

#define PRV_STATE 1
#define PRV_EVENT 2
#define PRV_COMM  3

class NPRecord
{
    public:
        NPRecord(const std::string line);
        static NPRecord *build(const std::string &line);
        long long getTimestamp() { return this->timestamp; };
        unsigned int getTask() { return this->task_id; };
        unsigned int getType() const { return this->type; };
    protected:
        unsigned int type;
        long long timestamp;
        unsigned int cpu_id;
        unsigned int app_id;
        unsigned int task_id;
        unsigned int thread_id;
        std::string specific_info;
};

class NPEvent : public NPRecord
{
    public:
        NPEvent(const std::string line);
        /*~NPEvent()
        {
            std::cout << "event removed" << std::endl;
            type.clear();
            value.clear();
        }*/
        int existEvent(const std::string type) const;
        std::pair<std::string, std::string> getEvent(int i);
        unsigned int getNEvents() { return (unsigned int)this->type.size(); };
        std::vector<std::pair<std::string,std::string>> 
            getEvents(const std::string type);
        std::vector<std::pair<std::string,std::string>> 
            getEvents();
    private:
        std::vector<std::string> type;
        std::vector<std::string> value;
};

class NPComm : public NPRecord
{
    public:
        NPComm(const std::string line);
        //~NPComm();
    //private:
        unsigned int cpu_send_id;
        unsigned int ptask_send_id;
        unsigned int task_send_id;
        unsigned int thread_send_id;
        long long logical_send;
        long long phyisical_send;

        unsigned int cpu_recv_id;
        unsigned int ptask_recv_id;
        unsigned int task_recv_id;
        unsigned int thread_recv_id;
        long long logical_recv;
        long long phyisical_recv;

        unsigned int size;
        unsigned int tag;
};

class NPStat : public NPRecord
{
    public:
        NPStat(const std::string line);
        unsigned int getState() const
            { return this->state; }
        long long getBeginTime() const
            { return this->begin_time; }
        long long getEndTime() const
            { return this->end_time; }
    private:
        long long begin_time;
        long long end_time;
        unsigned int state;
};


#endif /* !NPRECORD_H */
