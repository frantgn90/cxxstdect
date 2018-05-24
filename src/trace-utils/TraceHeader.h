/*
 * TraceHeader.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef TRACEHEADER_H
#define TRACEHEADER_H

#include <string>
#include <vector>

class TraceHeader
{
    public:
        TraceHeader(std::string tracefile);

        unsigned int day;
        unsigned int month;
        unsigned int year;
        unsigned int hour;
        unsigned int minutes;
        unsigned int texe;
        std::string texe_units;
        unsigned int nnodes;
        unsigned int napps;
        unsigned int ntasks;
        std::string mapping;
        unsigned int ncomms;
        std::vector<unsigned int> ppn;
    private:
        void readHeader(std::string header);
};

#endif /* !TRACEHEADER_H */
