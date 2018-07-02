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

#include <cxxabi.h>
#include <execinfo.h>
#include <memory>

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
        void setProgressReporter(ProgressReporter* pr)
            { this->progress_reporter = pr; }
        void setLogReporter(LogReporter* lr)
            { this->log_reporter = lr; }
    protected:
        bool msg_printed;
        bool prev_done;
        bool done;
        bool multisend;
        bool multirecv;
        double duration;
        std::string phasename;
        PipelineStageGeneral* next_stage;
        ProgressReporter* progress_reporter;
        LogReporter* log_reporter;
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
    protected:
        T1* input;
        T2* result;
        std::vector<T2*> accum_result; // if no multirecv on target
        void protected_actual_run(T1* input)
        {
            try
            {
                this->actual_run(input);
            }
            catch(...)
            {
                std::string msg(this->phasename+": Default exception catched!");
                if (!this->log_reporter)
                    std::cout << msg << std::endl;
                else
                    this->log_reporter->addMessage(msg);
                backtrace();
                exit(1);
            }
        }
    
        std::string demangle( const char* const symbol )
        {
            const std::unique_ptr< char, decltype( &std::free ) > demangled(
                    abi::__cxa_demangle( symbol, 0, 0, 0 ), &std::free );
            if( demangled ) {
                return demangled.get();
            }
            else {
                return symbol;
            }
        }

        void backtrace()
        {
            // TODO: replace hardcoded limit?
            void* addresses[ 256 ];
            const int n = ::backtrace( addresses, 
                    std::extent< decltype( addresses ) >::value );
            const std::unique_ptr< char*, decltype( &std::free ) > symbols(
                    ::backtrace_symbols( addresses, n ), &std::free );
            for( int i = 0; i < n; ++i ) {
                // we parse the symbols retrieved from backtrace_symbols() to
                // extract the "real" symbols that represent the mangled names.
                char* const symbol = symbols.get()[ i ];
                char* end = symbol;
                while( *end ) {
                    ++end;
                }
                // scanning is done backwards, since the module name
                // might contain both '+' or '(' characters.
                while( end != symbol && *end != '+' ) {
                    --end;
                }
                char* begin = end;
                while( begin != symbol && *begin != '(' ) {
                    --begin;
                }

                if( begin != symbol ) {
                    std::cout << std::string( symbol, ++begin - symbol );
                    *end++ = '\0';
                    std::cout << demangle( begin ) << '+' << end;
                }
                else {
                    std::cout << symbol;
                }
                std::cout << std::endl;
            }
        }

        virtual void actual_run(T1* input) {};
};


#endif /* !STAGE_H */
