/*
 * LoopsIdentification.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LOOPSIDENTIFICATION_H
#define LOOPSIDENTIFICATION_H

#include<mlpack/methods/dbscan/dbscan.hpp>
#include<reducer.h>
#include<vector>
#include<set>

class Loop
{
    public:
        Loop()
            : loop_id(0) {};
        Loop(size_t loop_id, arma::mat centroid)
            : loop_id(loop_id)
            , centroid(centroid) {};
        void insert(ReducedMPICall *mpi_call)
            { this->mpi_calls.push_back(mpi_call); }
        void setLoopId(size_t loop_id)
            { this->loop_id = loop_id; }
        void setCentroid(arma::mat centroid)
            { this->centroid = centroid; }
        float getDelta()
        {
            float sum = 0;
            for (auto it : this->mpi_calls)
                sum += it->getDelta();
            return sum/this->mpi_calls.size();
        }
        unsigned int getIterations() const
            { return this->mpi_calls[0]->getRepetitions(); }
        void setSuperloop(Loop *loop)
            { this->superloop = loop; }
        void setSubloop(Loop* loop)
            { this->subloops.push_back(loop); }
        std::vector<ReducedMPICall*> getMpiCalls()
            { return this->mpi_calls; }
        std::vector<Loop*> getSubloops()
            { return this->subloops; }
        std::set<unsigned int> getTasks();
    private:
        arma::mat centroid;
        size_t loop_id;
        std::vector<ReducedMPICall*> mpi_calls;
        std::vector<Loop*> subloops;
        Loop* superloop;
};

typedef std::vector<Loop> LoopVector;

class LoopsIdentification : public PipelineStage<UniqueMpiVector, LoopVector>
{
    public:
        LoopsIdentification(double eps, size_t minPts)
            : PipelineStage<UniqueMpiVector, std::vector<Loop>>(
                    "Loops indentification", false, false)
            , eps(eps)
            , minPts(minPts) 
    {
    };
    private:
        double eps;
        size_t minPts;
        void actual_run(UniqueMpiVector *input);
};

#endif /* !LOOPSIDENTIFICATION_H */
