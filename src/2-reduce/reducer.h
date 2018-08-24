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

#define STATE_RUN 1

typedef std::unordered_map<long unsigned int, ReducedMPICall> UniqueMpiMap;
typedef std::unordered_map<long long, NPComm> CommMap;
typedef std::vector<ReducedMPICall> UniqueMpiVector;

class Reducer : public PipelineStage<NPRecord, UniqueMpiVector>
{
    public:
        Reducer(long long texe, float lb, unsigned int ntasks)
            : PipelineStage<NPRecord, UniqueMpiVector>("Reducer", true,false)
            , lbound(lb)
            , texe(texe)
            , alias_tolerance(0.1f)
        {
            this->result = new UniqueMpiVector();
            last_mpicall = std::vector<MPICall>(ntasks);
            last_cpu_burst = std::vector<CPUBurst>(ntasks);
            comm_match = std::vector<CommMap>(ntasks);

            this->addConfigField<float>("Lower bound", lb, &(this->lbound));
            this->addConfigField<float>("Alias tolerance", alias_tolerance, 
                    &(this->alias_tolerance));
            this->debug = true;
        }

        virtual void print_result()
        {
            std::cout << "[DEBUG " << this->phasename << "] " 
                << this->result->size() << " unique mpi calls;" << std::endl;

            /*
            for (auto it : *(this->result))
                std::cout << "  " << it.getRepetitions() << " : " 
                    << it.getSignature() << std::endl;
            */
        }
    private:
        void process(NPEvent *event);
        void process(NPComm *comm);
        void process(NPStat *stat);
        void filter();

        void open_mpi(NPEvent *event);
        void close_mpi(NPEvent *event);
        void hwc_event(unsigned int task, 
                std::pair<std::string, std::string> event);
        void actual_run(NPRecord *input);
        void actual_clear()
        {
            this->result->clear();
            //delete this->result;

            //unique_mpicalls.clear();
            this->partial_result.clear();

            unsigned int ntasks = (unsigned int)last_mpicall.size();
            last_mpicall.clear();
            last_cpu_burst.clear();
            comm_match.clear();

            last_mpicall = std::vector<MPICall>(ntasks);
            last_cpu_burst = std::vector<CPUBurst>(ntasks);
            comm_match = std::vector<CommMap>(ntasks);

        }

        //UniqueMpiMap unique_mpicalls;
        UniqueMpiMap partial_result;
        float lbound;
        long long texe;
        std::vector<MPICall> last_mpicall;
        std::vector<CPUBurst> last_cpu_burst;
        std::vector<CommMap> comm_match;
        float alias_tolerance;
};

#endif /* !REDUCER_H */
