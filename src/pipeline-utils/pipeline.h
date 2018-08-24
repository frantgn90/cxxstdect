/*
 * stage.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef STAGE_H
#define STAGE_H

#include <ctime>
#include <assert.h>
#include <vector>
#include <boost/timer.hpp>
#include <iostream>
#include <algorithm>

#include <cxxabi.h>
#include <execinfo.h>
#include <memory>

class PipelineStageGeneral;

class ProgressReporter 
{
    public:
        virtual void setMax(unsigned int m) = 0;
        virtual void addProgress(unsigned int p) = 0;
};

class LogReporter
{
    public:
        virtual void addMessage(std::string msg) = 0;
};

class GeneralConfigField
{
    public:
        virtual std::string getName() = 0;
        virtual void* getManager() = 0;
        virtual void setValuep(void* ptr) = 0;
        void setStage(PipelineStageGeneral* stage)
            { this->stage = stage; }
        PipelineStageGeneral* getStage()
            { return this->stage; }
    protected:
        PipelineStageGeneral* stage;
};

template<class T> 
class ConfigField : public GeneralConfigField
{
    public:
        ConfigField() {};
        ConfigField(std::string name, T df, T* p)
            : name(name)
            , df(df)
            , p(p) {}
        ConfigField(std::string name, T df, void(*setter)(T&))
            : name(name)
            , df(df)
            , setter(setter) {}
        T getDefault()
            { return this->df; }
        virtual std::string getName()
            { return this->name; }
        void setValue(T &v)
            { this->setter(v); }
        void setValuep(void* ptr)
        {
            T* pp = static_cast<T*>(ptr);
            *(this->p) = *pp;
        }
        virtual void* getManager()
            { return myGetManager(this->getDefault()); }
        void* myGetManager(T df)
            { std::cout << "myGetManager not implemented for " << df << std::endl;
                return NULL; }
        void* getPointer()
            { return this->p; }
    private:
        std::string name;
        T df;
        void(*setter)(T&);
        T* p;
};

class PipelineStageGeneral
{
    public:
        PipelineStageGeneral(std::string name, bool mr, bool ms)
            : multirecv(mr)
            , multisend(ms)
            , done(false)
            , prev_done(false)
            , phasename(name) 
            , next_stage(NULL)
            , msg_printed(false)
            , progress_reporter(NULL)
            , log_reporter(NULL)
            , stage_position(0)
        {
        };

        virtual void clear() {};
        virtual void run() {};
        virtual void setInput(void *) {};

        double getDuration() 
            { return duration; }
        bool withMR() 
            { return this->multirecv; }
        void connect(PipelineStageGeneral *next) 
        { 
            this->next_stage = next; 
            next->setStagePosition(this->stage_position+1); 
        }
        void setProgressReporter(ProgressReporter* pr)
            { this->progress_reporter = pr; }
        void setLogReporter(LogReporter* lr)
            { this->log_reporter = lr; }
        template <class T> void addConfigField(std::string name, 
                T df, void(*setter)(T&))
        {
            ConfigField<T> new_cf(name,df,setter);
            this->config_fields.push_back(new_cf);
        }
        template <class T> void addConfigField(std::string name, T df, T* p)
        {
            ConfigField<T> *new_cf = new ConfigField<T>(name,df,p);
            new_cf->setStage(this);
            this->config_fields.push_back(new_cf);
        }
        std::vector<GeneralConfigField*> getConfFields()
            { return this->config_fields; }
        std::string getPhaseName()
            { return this->phasename; }
        unsigned int getStagePosition()
            { return this->stage_position; }
        void setStagePosition(unsigned int p)
            { this->stage_position = p; }
    protected:
        bool multirecv;
        bool multisend;
        bool done;
        bool prev_done;
        std::string phasename;
        PipelineStageGeneral* next_stage;
        bool msg_printed;
        ProgressReporter* progress_reporter;
        LogReporter* log_reporter;
        unsigned int stage_position;
        double duration;
        bool debug = false; // TODO : Should be configured by user
        std::vector<GeneralConfigField*> config_fields;
};

template <class T1,class T2>
class PipelineStage : public PipelineStageGeneral
{
    public:
        PipelineStage(std::string name="unk", bool mr = false, bool ms = false)
            : PipelineStageGeneral(name, mr, ms) {};
        T2* getResult() 
            { return result; }
        void setInput(void *input)
        {
            this->input = static_cast<T1*>(input);
        }
        void run()
        {
            if (!msg_printed)
            {
                std::string msg = "Executing phase: " + this->phasename;
                if (this->multisend)
                    msg += "[msend]";
                if (this->multirecv)
                    msg += "[mrecv]";
                if (!this->log_reporter)
                    std::cout << msg << std::endl;
                else
                    this->log_reporter->addMessage(msg);
                msg_printed = true;
            }
            boost::timer t;
            t.restart();

            if (!this->next_stage)
            {
                this->protected_actual_run(input);
            }
            else
            {
                if (this->multisend)
                {
                    while (!this->done)
                    {
                        this->protected_actual_run(input);
                        if (this->next_stage->withMR())
                        {
                            this->next_stage->setInput((void *)this->getResult());
                            this->next_stage->run();
                        }
                        else
                            this->accum_result.push_back(this->result);
                    }
                    if (!this->next_stage->withMR())
                    {
                        this->next_stage->setInput((void *)&this->accum_result);
                        this->next_stage->run();
                    }
                    if (this->result) // centinell if phase is not sending it
                    {
                        this->next_stage->setInput((void *)NULL);
                        this->next_stage->run();
                    }
                }
                else
                {
                    this->protected_actual_run(input);
                    if (this->done)
                    {
                        this->next_stage->setInput((void *)this->getResult());
                        this->next_stage->run();
                    }
                }
            }
            this->duration = t.elapsed();
        }
        void clear()
        {
            std::string msg = "Clearing phase: " + this->phasename;
            this->log_reporter->addMessage(msg);
            //delete input; // the input should be deleted by previous stage
            //delete result; // every stage should clear its result
            std::for_each(accum_result.begin(), accum_result.end(),
                    [](T2* ptr){ delete ptr; });
            this->actual_clear();
            this->done = false;
        }
    protected:
        T1* input;
        T2* result;
        std::vector<T2*> accum_result; // if no multirecv on target
        bool abort_pipeline;
        void protected_actual_run(T1* input)
        {
            try
            {
                this->actual_run(input);
                if (debug and this->done)
                    this->print_result();
            }
            catch(...)
            {
                std::string msg(this->phasename+": Default exception catched!");
                if (!this->log_reporter)
                    std::cout << msg << std::endl;
                else
                    this->log_reporter->addMessage(msg);
                exit(1);
            }
        }

        virtual void actual_run(T1* input) { *input = (T1)0; };
        virtual void actual_clear() {};
        virtual void print_result() { printf("This is the virtual function."); };
};


#endif /* !STAGE_H */
