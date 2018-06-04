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
            : loop_id(0)
            , superloop(NULL)
            , aliased_with(-1) {};
        Loop(size_t loop_id, arma::mat centroid)
            : loop_id(loop_id)
            , centroid(centroid)
            , superloop(NULL)
            , aliased_with(-1) {};
        Loop(size_t loop_id, arma::mat centroid, int aliased_with)
            : loop_id(loop_id)
            , centroid(centroid)
            , superloop(NULL)
            , aliased_with(aliased_with) {};
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
        Loop* getSuperloop()
            { return this->superloop; }
        void setSubloop(Loop* loop)
            { this->subloops.push_back(loop); }
        std::vector<ReducedMPICall*> getMpiCalls()
            { return this->mpi_calls; }
        std::vector<Loop*> getSubloops()
            { return this->subloops; }
        std::set<unsigned int> getTasks();
        bool isSubloopOf(Loop* l);
        std::pair<unsigned int, unsigned int> getIterationsBounds(int i);
        bool isAliased(std::vector<ReducedMPICall*> mpi_calls, unsigned int deph);
        std::vector<Loop> getAliasedLoops()
            { return this->aliased_loops; }
        void superloopAnalysis();
        int wasAliasedWith()
            { return this->aliased_with; }
        bool wasAliased()
            { return this->aliased_with != -1; }
    private:
        arma::mat centroid;
        size_t loop_id;
        std::vector<ReducedMPICall*> mpi_calls;
        std::vector<Loop*> subloops;
        Loop* superloop;
        std::vector<Loop> aliased_loops;
        int aliased_with;
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
        void aliasingAnalysis();
        void superloopAnalysis();
};

#endif /* !LOOPSIDENTIFICATION_H */
