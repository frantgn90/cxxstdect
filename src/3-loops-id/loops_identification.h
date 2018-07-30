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
            , aliased_with(-1)
            , is_hidden_superloop(false)
            , hidden_superloop_iterations(0)
            , tasks(NULL) {};
        Loop(size_t loop_id, arma::mat centroid)
            : loop_id(loop_id)
            , centroid(centroid)
            , superloop(NULL)
            , aliased_with(-1)
            , is_hidden_superloop(false)
            , hidden_superloop_iterations(0)
            , tasks(NULL) {};
        Loop(size_t loop_id, arma::mat centroid, int aliased_with)
            : loop_id(loop_id)
            , centroid(centroid)
            , superloop(NULL)
            , aliased_with(aliased_with) 
            , is_hidden_superloop(false)
            , hidden_superloop_iterations(0)
            , tasks(NULL) {};
        Loop(size_t loop_id, arma::mat centroid, 
                bool is_hidden_superloop, int its)
            : loop_id(loop_id)
            , centroid(centroid)
            , superloop(NULL)
            , aliased_with(-1)
            , is_hidden_superloop(is_hidden_superloop)
            , hidden_superloop_iterations(its)
            , tasks(NULL) {};
        unsigned int getId()
            { return this->loop_id; }
        void insert(ReducedMPICall *mpi_call)
            { this->mpi_calls.push_back(mpi_call); }
        void setLoopId(size_t loop_id)
            { this->loop_id = loop_id; }
        void setCentroid(arma::mat centroid)
            { this->centroid = centroid; }
        float getDelta()
        {
            if (this->is_hidden_superloop)
                // All subloops have same delta
                return this->subloops[0]->getDelta();
            else
            {
                float sum = 0;
                for (auto it : this->mpi_calls)
                    sum += it->getDelta();
                return sum/this->mpi_calls.size();
            }
        }
        unsigned int getIterations() const
        {   
            if (this->is_hidden_superloop)
                return this->hidden_superloop_iterations;
            else
                return this->mpi_calls[0]->getRepetitions(); 
        }
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
        std::vector<unsigned int>* getTasks();
        bool isSubloopOf(Loop* l);
        std::pair<unsigned int, unsigned int> getIterationsBounds(int i);
        bool isAliased(std::vector<ReducedMPICall*> mpi_calls, unsigned int deph);
        std::vector<Loop*> getAliasedLoops()
            { return this->aliased_loops; }
        void superloopAnalysis();
        int wasAliasedWith()
            { return this->aliased_with; }
        bool wasAliased()
            { return this->aliased_with != -1; }
        bool haveHiddenSuperloops()
            { return this->hidden_superloops.size() > 0; }
        std::vector<Loop*> getHiddenSuperloops()
            { return this->hidden_superloops; }
        bool isHiddenSuperloop() const
            { return this->is_hidden_superloop; }
        void digest(Loop* l);
        unsigned int getLongestIteration();
        unsigned int getShortestIteration();
        ~Loop()
        {
            // WARNING : Shouldnt be freed here. This class is not creating them
            //std::for_each(mpi_calls.begin(), mpi_calls.end(),
            //        [](ReducedMPICall* ptr){ delete ptr; });
            mpi_calls.clear();
            //std::for_each(subloops.begin(), subloops.end(),
            //        [](Loop* ptr){ delete ptr; });
            subloops.clear();
            std::for_each(aliased_loops.begin(), aliased_loops.end(),
                    [](Loop* ptr){ delete ptr; });
            aliased_loops.clear();
            std::for_each(hidden_superloops.begin(), hidden_superloops.end(),
                    [](Loop* ptr){ delete ptr; });
            hidden_superloops.clear();
        }

    private:
        arma::mat centroid;
        size_t loop_id;
        Loop* superloop;
        std::vector<ReducedMPICall*> mpi_calls;
        std::vector<Loop*> subloops;
        std::vector<Loop*> aliased_loops;
        std::vector<Loop*> hidden_superloops;
        int aliased_with;
        bool is_hidden_superloop;
        unsigned int hidden_superloop_iterations;
        std::vector<unsigned int>* tasks;
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
            this->addConfigField<float>("loops eps", eps, 
                    (float*)&(this->eps));
            this->addConfigField<int>("loops minPts", minPts, 
                    (int*)&(this->minPts));
            this->debug = true;
        };
        virtual void print_result()
        {
            std::cout << "[DEBUG " << this->phasename << "] " 
                << this->result->size() << " loops;" << std::endl;
        }
    private:
        double eps;
        size_t minPts;
        void actual_run(UniqueMpiVector *input);
        void actual_clear()
        {
            this->result->clear();
            delete this->result;
        }
        void aliasingAnalysis();
};

#endif /* !LOOPSIDENTIFICATION_H */
