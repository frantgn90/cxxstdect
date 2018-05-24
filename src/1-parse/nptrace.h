/*
 * trace.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef NPTRACE_H
#define NPTRACE_H

#include "nprecord.h"

#include <pipeline.h>
#include <tracestream.h>
#include <vector>
#include <string>

class NPTrace : public PipelineStage<std::string, NPRecord>
{
    public:
        NPTrace() 
            : PipelineStage<std::string, NPRecord>("Parsing", false, true)
            , init(false) {};
    private:
        void actual_run(std::string *trace_file);
        void readHeader(std::string header);
        TraceStream *file;
        bool init;
};

#endif /* !TRACE_H */
