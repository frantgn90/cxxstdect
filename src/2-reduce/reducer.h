/*
 * reducer.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef REDUCER_H
#define REDUCER_H

#include "pipeline.h"
#include "nprecord.h"
#include "mpicall.h"
#include <string>
#include <unordered_map>

// TODO: This defines should be in another file

#define HWC_REGEX "420000"
#define MPI_REGEX "500000"
#define GLO_REGEX "501000"
#define CAL_REGEX "700000"
#define LIN_REGEX "800000"
#define LOP_REGEX "990000"
#define ITE_REGEX "991000"
#define NIT_REGEX "992000"

typedef std::unordered_map<unsigned int, ReducedMPICall> UniqueMpiMap;
typedef std::vector<ReducedMPICall> UniqueMpiVector;

class Reducer : public PipelineStage<NPRecord, UniqueMpiVector>
{
    public:
        Reducer(unsigned int texe, float lb)
            : PipelineStage<NPRecord, UniqueMpiVector>("Reducer", true,false)
            , lbound(lb)
            , texe(texe)
        {
            this->result = new UniqueMpiVector();
        }
    private:
        void process(NPEvent *event);
        void process(NPComm *comm);
        void process(NPStat *stat);
        void filter();

        UniqueMpiMap unique_mpicalls;
        void open_mpi(NPEvent *event);
        void close_mpi(NPEvent *event);
        void actual_run(NPRecord *input);

        UniqueMpiMap partial_result;
        float lbound;
        unsigned int texe;
};

#endif /* !REDUCER_H */
