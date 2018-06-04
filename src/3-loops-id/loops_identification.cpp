/*
 * loops_identification.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "loops_identification.h"
#include <iostream>
#include <limits>

std::pair<unsigned int, unsigned int> Loop::getIterationsBounds(int i)
{
    ReducedMPICall* first_mpicall = this->mpi_calls[0];

    unsigned int lower_bound = first_mpicall->getTimestampAt(i);
    unsigned int upper_bound = first_mpicall->getTimestampAt(i+1);
    return std::make_pair(lower_bound, upper_bound);
}

bool Loop::isSubloopOf(Loop* l)
{
    // Both loops were aliased in same cluster
    if (this->wasAliased() and l->wasAliased())
        if (this->wasAliasedWith() == l->wasAliasedWith())
            return false;

    std::pair<unsigned int, unsigned int> other_it_bounds = 
        l->getIterationsBounds(0);
    std::pair<unsigned int, unsigned int> self_it_bounds = 
        this->getIterationsBounds(0);

    if (self_it_bounds.first < other_it_bounds.first)
    {
        other_it_bounds.second = other_it_bounds.first;
        other_it_bounds.first = 0;
    }
 
    /*
    std::cout << "Self reps: " << this->getIterations() << std::endl;
    std::cout << "Self bound: [" << self_it_bounds.first << "," 
        << self_it_bounds.second << "]" << std::endl;
    std::cout << "Other reps: " << l->getIterations() << std::endl;
    std::cout << "Other bound: [" << other_it_bounds.first << "," 
        << other_it_bounds.second << "]" << std::endl;
    */

    return self_it_bounds.first > other_it_bounds.first 
        and self_it_bounds.second < other_it_bounds.second;
}

struct pA_comp {
    bool operator() (ReducedMPICall* left, ReducedMPICall* right) const 
    { 
       return left->getFirstTimestamp() < right->getFirstTimestamp();
    }
};

bool Loop::isAliased(std::vector<ReducedMPICall*> mpi_calls, unsigned int deph)
{
    // Sorted by first timestamp. Remember we do not have information
    // about the position in code of every mpi call so this is the only
    // way to sort them.
    std::sort(mpi_calls.begin(), mpi_calls.end(), pA_comp());

    unsigned int last_time = 0;
    std::vector<ReducedMPICall*> split;

    // For the two first timestamps should be enough
    
    unsigned int upbound = mpi_calls[0]->getTimestampAt(1);
    for (auto it = mpi_calls.begin(); it != mpi_calls.end(); ++it)
    {
        if ((*it)->getTimestampAt(0) >= upbound)
        {
            split.insert(split.begin(), it, mpi_calls.end());
            mpi_calls.erase(it, mpi_calls.end());
            break;
        }
    }

    if (split.size() > 0)
    {
        this->aliased_loops.push_back(Loop(1000+deph, this->centroid, 
                    this->loop_id));
        for (auto it : mpi_calls)
            this->aliased_loops.back().insert(it);
        this->isAliased(split, deph+1);
    }
    else if(deph > 0)
    {
        this->aliased_loops.push_back(Loop(1000+deph, this->centroid, 
                    this->loop_id));
        for (auto it : mpi_calls)
            this->aliased_loops.back().insert(it);
    }
    return split.size() > 0;
}

void Loop::superloopAnalysis()
{
    if (this->aliased_loops.size() == 0)
        return;

    
}

std::set<unsigned int> Loop::getTasks()
{
    std::set<unsigned int> res;
    for (auto it : this->mpi_calls)
        for (auto it2 : *(it->getTasks()))
            res.insert(it2);
    return res;
}

void LoopsIdentification::aliasingAnalysis()
{
    std::vector<Loop> aliased_loops;
    for (std::vector<Loop>::iterator it = this->result->begin();
            it != this->result->end();)
    {
        if (it->isAliased(it->getMpiCalls(), 0))
        {
            std::vector<Loop> newloops = it->getAliasedLoops();
            std::cout << "Aliased loops: " << newloops.size() << std::endl;
            aliased_loops.insert(aliased_loops.begin(), 
                    newloops.begin(), newloops.end());

            it = this->result->erase(it);
        }
        else
            ++it;
    }
    this->result->insert(this->result->end(),
            aliased_loops.begin(), aliased_loops.end());
}

void LoopsIdentification::superloopAnalysis()
{
    for (std::vector<Loop>::iterator it = this->result->begin();
            it != this->result->end(); ++it)
        it->superloopAnalysis();
}

void LoopsIdentification::actual_run(UniqueMpiVector *input)
{
    mlpack::dbscan::DBSCAN<> dbscan(this->eps, this->minPts);

    arma::mat data(2, input->size()); // 2-dimensions
    arma::Row<size_t> assignements;
    arma::mat centroids;

    unsigned int max_reps = 0;
    unsigned int max_iit = 0;

    for (int i=0; i < input->size(); ++i)
    {
        if (input->at(i).getRepetitions() > max_reps)
            max_reps = input->at(i).getRepetitions();
        if (input->at(i).getInterRepTime() > max_iit)
            max_iit = input->at(i).getInterRepTime();
    }

    // Cols are items and rows are dimensions
    for (int i=0; i < input->size(); ++i)
    {
        double norm_reps = 1.0*input->at(i).getRepetitions()/max_reps;
        double norm_iit = 1.0*input->at(i).getInterRepTime()/max_iit;
        data(0,i) = norm_reps;
        data(1,i) = norm_iit;
    }

    //data = arma::normalise(data,2,0);
    //data = arma::normalise(data,2,1);
    size_t nclusters = dbscan.Cluster(data, assignements, centroids);

    /****/
    //std::cout << nclusters << std::endl;
    //data.print("DATA: ");
    //assignements.print("ASSIGN: ");
    //centroids.print("CENTR: ");
    /****/
    
    this->result = new LoopVector(nclusters);
    for (int i=0; i < input->size(); ++i)
    {
        size_t loop_id = assignements[i];
        this->result->at(loop_id).setLoopId(loop_id);
        this->result->at(loop_id).insert(&input->at(i));
        //arma::mat centroid = centroids[loop_id];
        //this->result->at(loop_id).setCentroid(centroid);
    }
    this->aliasingAnalysis();
    this->superloopAnalysis();
    this->done = true;
}
