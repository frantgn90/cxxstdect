/*
 * loops-merge.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "loops_merge.h"
#include<mlpack/methods/dbscan/dbscan.hpp>


struct loops_sort
{
    // hidden superloops always must be sorted after actual loops
    inline bool operator() (const Loop* l1, const Loop* l2)
    {
        if (l1->getIterations() == l2->getIterations())
        {
            if (l1->isHiddenSuperloop()) return true;
            if (l2->isHiddenSuperloop()) return false;
        }
        return (l1->getIterations() > l2->getIterations());
    }
};

void TopLevelLoop::merge()
{
    std::sort(this->loops.begin(), this->loops.end(), loops_sort());

    //TODO: Reverse merge for data condition detection
    //
    auto it_from = this->loops.begin();
    while (it_from != this->loops.end()-1)
    {
        auto it_to = it_from+1;
        while (it_to != this->loops.end())
        {
            if ((*it_from)->isHiddenSuperloop() 
                    and (*it_from)->getIterations() == (*it_to)->getIterations())
            {
                (*it_to)->digest(*it_from);
            }
            else if ((*it_from)->isSubloopOf(*it_to))
            {
                (*it_from)->setSuperloop(*it_to);
                (*it_to)->setSubloop(*it_from);
                break;
            }
            it_to++;
        }
        it_from++;
    }
    // TODO : Append all subloops that have not been merged with anybody
    this->superloop = this->loops.back();
}

std::vector<unsigned int> LoopsMerge::getDeltas()
{
    std::vector<unsigned int> result;
    for (auto it : *(this->result))
        result.push_back(it.getDelta()*100);
    return result;
}

void LoopsMerge::actual_run(LoopVector *input)
{
    // First step is to classify loops by their delta
    mlpack::dbscan::DBSCAN<> dbscan(this->eps, this->minPts);

    arma::mat data(1,input->size()); // 2-dimensions
    arma::Row<size_t> assignements;
    arma::mat centroids;

    for (int i=0; i < input->size(); ++i)
        data(0,i) = input->at(i).getDelta();

    //data = arma::normalise(data);
    size_t nclusters = dbscan.Cluster(data, assignements, centroids);

    /****/
    //std::cout << "Loops: " << input->size() << std::endl;
    //std::cout << "Top level loops: " << nclusters << std::endl;
    //data.print("DATA: ");
    //assignements.print("ASSIGN: ");
    //centroids.print("CENTR: ");
    /****/

    // Create the top level loops objects
    this->nphases = nclusters;
    this->result = new TopLevelLoopVector(nclusters);
    for (int i=0; i < input->size(); ++i)
    {
        size_t loop_id = assignements[i];
        this->result->at(loop_id).setLoopId(loop_id);
        this->result->at(loop_id).setDelta(input->at(i).getDelta());
        this->result->at(loop_id).insert(&input->at(i));
    }
    
    // Run the merge
    for (auto it = this->result->begin(); it != this->result->end(); ++it)
        it->merge();

    this->done = true;
}
