/*
 * loops_identification.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "loops_identification.h"
#include <iostream>
#include <limits>

std::pair<long long, long long> Loop::getIterationsBounds(int i)
{
    if (!this->is_hidden_superloop)
    {
        ReducedMPICall* first_mpicall = this->mpi_calls[0];

        long long lower_bound = first_mpicall->getTimestampAt(i);
        long long upper_bound = first_mpicall->getTimestampAt(i+1);
        return std::make_pair(lower_bound, upper_bound);
    }
    else
    {
        // TODO : Problems when there is a hidden superloop
        auto first_mpicall_bounds = this->subloops[0]->getIterationsBounds(
                i*this->subloops[0]->getIterations()/this->getIterations());
        auto last_mpicall_bounds = this->subloops.back()->getIterationsBounds(
                i*this->subloops.back()->getIterations()/this->getIterations());
        
        return std::make_pair(first_mpicall_bounds.first, 
                last_mpicall_bounds.second);
    }
}

void Loop::digest(Loop* l)
{
    assert (l->isHiddenSuperloop());
    
    for (auto it : l->getSubloops())
    {
        it->setSuperloop(this);
        this->setSubloop(it);
    }
}


unsigned int Loop::getLongestIteration()
{
    // TODO : Get the iteration with longest duration
    return 0;
}

unsigned int Loop::getShortestIteration()
{
    // TODO : Get the iteration with shortest duration
    return 0;
}

bool Loop::isSubloopOf(Loop* l)
{
    // Both loops were aliased in same cluster
    if (this->wasAliased() and l->wasAliased())
        if (this->wasAliasedWith() == l->wasAliasedWith())
            return false;

    int i = 0;
    auto just_one_it = l->getIterationsBounds(0);
    auto my_it = this->getIterationsBounds(0);
    while(my_it.second < just_one_it.second)
    {
        if (my_it.first > just_one_it.first)
            return true;
        my_it = this->getIterationsBounds(++i);
    }
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
 
    std::cout << "Self reps: " << this->getIterations() << " - ";
    std::cout << "Self bound: [" << self_it_bounds.first << "," 
        << self_it_bounds.second << "]" << std::endl;
    std::cout << "Other reps: " << l->getIterations() << " - ";
    std::cout << "Other bound: [" << other_it_bounds.first << "," 
        << other_it_bounds.second << "]" << std::endl;
    bool result = self_it_bounds.first > other_it_bounds.first 
        and self_it_bounds.second < other_it_bounds.second;
    std::cout << "RESULT: " << result << std::endl;
    

    return self_it_bounds.first > other_it_bounds.first 
        and self_it_bounds.second < other_it_bounds.second;
}

bool Loop::isAliased(std::vector<ReducedMPICall*> mpi_calls, unsigned int deph)
{
    std::vector<ReducedMPICall*> split;

    // For the two first timestamps should be enough
    
    long long upbound = mpi_calls[0]->getTimestampAt(1);
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
        this->aliased_loops.push_back(new Loop(1000+deph, this->centroid, 
                    this->loop_id));
        for (auto it : mpi_calls)
            this->aliased_loops.back()->insert(it);
        this->isAliased(split, deph+1);
    }
    else if(deph > 0)
    {
        this->aliased_loops.push_back(new Loop(1000+deph, this->centroid, 
                    this->loop_id));
        for (auto it : mpi_calls)
            this->aliased_loops.back()->insert(it);
    }
    return split.size() > 0;
}

void Loop::superloopAnalysis()
{
    return;
    // Only if it is an aliased loop
    if (this->aliased_loops.size() == 0)
        return;

    long long last_init_time = this->aliased_loops[0]
        ->getIterationsBounds(0).first;
    long long last_fini_time = this->aliased_loops[0]
        ->getIterationsBounds(this->aliased_loops[0]->getIterations()-2).second;

    unsigned int sset = 0;
    std::vector<std::vector<Loop*>> superloops;
    superloops.push_back(std::vector<Loop*>());
    superloops[sset].push_back(this->aliased_loops[0]);

    for(unsigned int i=1; i<this->aliased_loops.size(); ++i)
    {
        long long init_time = this->aliased_loops[i]
            ->getIterationsBounds(0).first;
        long long fini_time = this->aliased_loops[i]
            ->getIterationsBounds(this->aliased_loops[i]->getIterations()-2).second;

        if (last_init_time < init_time and init_time < last_fini_time)
        {
            superloops[sset].push_back(this->aliased_loops[i]);
        }
        else
        {
            sset++;
            superloops.push_back(std::vector<Loop*>());
            superloops[sset].push_back(this->aliased_loops[i]);
        }
        last_init_time = init_time;
        last_fini_time = fini_time;
    }

    bool any_superloop = false;
    for (auto it : superloops )
        if (it.size() > 1) any_superloop = true;

    if (!any_superloop)
        return;

    for (auto it : superloops )
    {
        if (it.size() == 1)
        {
            this->hidden_superloops.push_back(it[0]);
        }
        else
        {
            // Ahora hay que contar cuantas iteraciones tienen los superloops
            // encontrados.
            auto first_subloop_bounds = it[0]->getIterationsBounds(0);
            auto second_subloop_bounds = it[1]->getIterationsBounds(1);

            unsigned int subloops_its = 1;
            while(first_subloop_bounds.second < second_subloop_bounds.first)
            {
                subloops_its++;
                first_subloop_bounds = it[0]->getIterationsBounds(subloops_its);
            }
            unsigned int superloop_its = it[0]->getIterations()/subloops_its;

            Loop* new_superloop = new Loop(2000+this->loop_id, this->centroid, 
                    true, superloop_its);
            this->hidden_superloops.push_back(new_superloop);

            for (auto it2 : it)
            {
                new_superloop->setSubloop(it2);
                it2->setSuperloop(new_superloop);
            }
        }
    }
}

std::vector<unsigned int>* Loop::getTasks()
{
    if (!this->tasks)
    {
        std::set<unsigned int> res;
        if (this->is_hidden_superloop)
            for (auto it : this->subloops)
            {
                std::vector<unsigned int>* tasks_set = it->getTasks();
                res.insert(tasks_set->begin(), tasks_set->end());
            }
        else
            for (auto it : this->mpi_calls)
                for (auto it2 : *(it->getTasks()))
                    res.insert(it2);

        this->tasks = new std::vector<unsigned int>(res.size());
        std::copy(res.begin(), res.end(), this->tasks->begin());
    }
    
    return this->tasks;
}

void LoopsIdentification::aliasingAnalysis()
{
    std::vector<Loop*> aliased_loops;
    for (std::vector<Loop>::iterator it = this->result->begin();
            it != this->result->end();)
    {
        if (it->isAliased(it->getMpiCalls(), 0))
        {
            it->superloopAnalysis(); // look for superloop on aliased loops
            std::vector<Loop*> newloops;
            if (it->haveHiddenSuperloops())
                newloops = it->getHiddenSuperloops();
            else
                newloops = it->getAliasedLoops();

            aliased_loops.insert(aliased_loops.end(), 
                    newloops.begin(), newloops.end());

            // TODO : Currently Loop destructur is not freeing aliased loops.
            // Think about where/when/how to do it.
            it = this->result->erase(it);
        }
        else
            ++it;
    }
    for (auto it : aliased_loops)
    {
        this->result->push_back(*it);
    }

    //this->result->insert(this->result->end(),
    //        aliased_loops.begin(), aliased_loops.end());
}

void LoopsIdentification::actual_run(UniqueMpiVector *input)
{
    mlpack::dbscan::DBSCAN<> dbscan(this->eps, this->minPts);

    this->log_reporter->addMessage("Input size: " + std::to_string(input->size()));
    if (input->size() == 0)
    {
        this->log_reporter->addMessage("No loops can be detected");
        this->result = new LoopVector();
        this->done = true;
        return;
    }

    arma::mat data(2, input->size()); // 2-dimensions
    arma::Row<size_t> assignements;
    arma::mat centroids;

    unsigned int max_reps = 0;
    unsigned int max_iit = 0;

    for (unsigned int i=0; i < input->size(); ++i)
    {
        if (input->at(i).getRepetitions() > max_reps)
            max_reps = input->at(i).getRepetitions();
        if (input->at(i).getInterRepTime() > max_iit)
            max_iit = input->at(i).getInterRepTime();
    }

    // Cols are items and rows are dimensions
    for (unsigned int i=0; i < input->size(); ++i)
    {
        double norm_reps = 1.0*input->at(i).getRepetitions()/max_reps;
        double norm_iit = 1.0*input->at(i).getInterRepTime()/max_iit;
        data(0,i) = norm_reps;
        data(1,i) = norm_iit;
    }

    size_t nclusters = dbscan.Cluster(data, assignements, centroids);

    /****/
    //std::cout << "NCLUSTERS: " << nclusters << std::endl;
    //data.print("DATA " + this->phasename + ": ");
    //assignements.print("ASSIGN " + this->phasename + ": ");
    //centroids.print("CENTR " + this->phasename + ": ");
    /****/
    
    this->result = new LoopVector(nclusters);
    for (unsigned int i=0; i < input->size(); ++i)
    {
        size_t loop_id = assignements[i];
        this->result->at(loop_id).setLoopId((unsigned int)loop_id);
        this->result->at(loop_id).insert(&input->at(i));
        //arma::mat centroid = centroids[loop_id];
        //this->result->at(loop_id).setCentroid(centroid);
    }

    for (unsigned int i=0; i<this->result->size(); ++i)
        this->result->at(i).sortMpiCalls();

    /*
    for (auto loop : *(this->result))
    {
        std::cout << loop.getId() << " : " << loop.getIterations() << std::endl;
        
        for (int i=0; i<loop.getIterations(); ++i)
            std::cout << loop.getMpiCalls()[0]->getTimestampAt(i) << ",";
        //for (int i=0; i<loop.getMpiCalls().size(); ++i)
        //    std::cout << loop.getMpiCalls()[i]->getSignature() << std::endl;
        std::cout << std::endl << "=======" << std::endl;
    }
    */
    this->aliasingAnalysis();
    /*
    std::cout << "************" << std::endl;
    for (auto loop : *(this->result))
    {
        std::cout << loop.getId() << " : " << loop.getIterations() << std::endl;
        
        for (int i=0; i<loop.getIterations(); ++i)
            std::cout << loop.getMpiCalls()[0]->getTimestampAt(i) << ",";
        //for (int i=0; i<loop.getMpiCalls().size(); ++i)
        //    std::cout << loop.getMpiCalls()[i]->getSignature() << std::endl;
        
        std::cout << std::endl << "=======" << std::endl;
    }
    */
    for (auto it : *(this->result))
        assert(it.getIterations() > 0);

    this->done = true;
}
