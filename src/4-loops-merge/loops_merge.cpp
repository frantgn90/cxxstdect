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
    inline bool operator() (const Loop* l1, const Loop* l2)
    {
        return (l1->getIterations() > l2->getIterations());
    }
};

void TopLevelLoop::merge()
{
    std::sort(this->loops.begin(), this->loops.end(), loops_sort());
    auto it1 = this->loops.begin();
    auto it2 = it1 + 1;
    while (it2 != this->loops.end())
    {
        (*it1)->setSuperloop(*it2);
        (*it2)->setSubloop(*it1);
        it1 ++; it2++;
    }
    this->superloop = this->loops.back();
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
    std::cout << "Loops: " << input->size() << std::endl;
    std::cout << "Top level loops: " << nclusters << std::endl;
    data.print("DATA: ");
    assignements.print("ASSIGN: ");
    centroids.print("CENTR: ");
    /****/

    // Create the top level loops objects
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
