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
        //~NPRecord();
        static NPRecord *build(const std::string &line);
        unsigned int getTimestamp() { return this->timestamp; };
        unsigned int getTask() { return this->task_id; };
        unsigned int getType() { return this->type; };
    protected:
        unsigned int type;
        unsigned int timestamp;
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
        //~NPEvent();
        int existEvent(const std::string type);
        std::pair<std::string, std::string> getEvent(int i);
        unsigned int getNEvents() { return this->type.size(); };
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
        unsigned int logical_send;
        unsigned int phyisical_send;

        unsigned int cpu_recv_id;
        unsigned int ptask_recv_id;
        unsigned int task_recv_id;
        unsigned int thread_recv_id;
        unsigned int logical_recv;
        unsigned int phyisical_recv;

        unsigned int size;
        unsigned int tag;
};

class NPStat : public NPRecord
{
    public:
        NPStat(const std::string line);
        unsigned int getState()
            { return this->state; }
        unsigned int getBeginTime()
            { return this->begin_time; }
        unsigned int getEndTime()
            { return this->end_time; }
    private:
        unsigned int begin_time;
        unsigned int end_time;
        unsigned int state;
};


#endif /* !NPRECORD_H */
