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
            , init(false) 
            , first_record_after_reorder(NULL) 
            , alias_tolerance(0.1)
    {
        this->addConfigField<float>("Alias tolerance", 0.1, 
                &(this->alias_tolerance));
    };
    private:
        void actual_run(std::string *trace_file);
        void readHeader(std::string header);
        NPRecord* getNextRecord();
        void printReorderBuffer();

        TraceStream *file;
        bool init;
        // just reorder records with same timestamp
        std::vector<NPRecord*> reorder_buffer; 
        NPRecord *first_record_after_reorder;
        float alias_tolerance;
};

#endif /* !TRACE_H */
