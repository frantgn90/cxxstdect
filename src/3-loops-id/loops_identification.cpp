/*
 * loops_identification.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "loops_identification.h"
#include <iostream>
#include <limits>

std::set<unsigned int> Loop::getTasks()
{
    std::set<unsigned int> res;
    for (auto it : this->mpi_calls)
        for (auto it2 : *(it->getTasks()))
            res.insert(it2);
    return res;
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
    std::cout << nclusters << std::endl;
    data.print("DATA: ");
    assignements.print("ASSIGN: ");
    centroids.print("CENTR: ");
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

    this->done = true;
}
