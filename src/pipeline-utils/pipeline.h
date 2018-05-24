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
        {
        };
        unsigned int getDuration() 
            { return duration; }
        bool withMR() 
            { return this->multirecv; }
        void connect(PipelineStageGeneral *next) 
        { 
            this->next_stage = next; 
        }
        virtual void run() {};
        virtual void setInput(void *) {};
    protected:
        bool msg_printed;
        bool prev_done;
        bool done;
        bool multisend;
        bool multirecv;
        double duration;
        std::string phasename;
        PipelineStageGeneral* next_stage;
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
                std::cout << msg << std::endl;
                msg_printed = true;
            }
            boost::timer t;
            t.restart();

            if (!this->next_stage)
            {
                this->actual_run(input);
            }
            else
            {
                if (this->multisend)
                {
                    while (!this->done)
                    {
                        this->actual_run(input);
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
                    this->actual_run(input);
                    if (this->done)
                    {
                        this->next_stage->setInput((void *)this->getResult());
                        this->next_stage->run();
                    }
                }
            }
            this->duration = t.elapsed();
        }
    protected:
        T1* input;
        T2* result;
        std::vector<T2*> accum_result; // if no multirecv on target

        virtual void actual_run(T1* input) {};
};


#endif /* !STAGE_H */
