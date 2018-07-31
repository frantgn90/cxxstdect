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
    std::vector<unsigned int> rr;
    // WARNING : This C++ constructs creates an object copy and after its
    // lifetime (1 iteration) it is destroyed. This destruction implies a
    // pointer freed in such a way it is reachable from now and onward
    //for (auto it : *(this->result))
    //  rr.push_back(it.getDelta()*100);
    for (int i=0; i<this->result->size(); ++i)
        rr.push_back(this->result->at(i).getDelta()*100);
    return rr;
}

void LoopsMerge::preparePlot(std::vector<Loop>* loops,
        arma::mat centroids)
{
    char datafile[L_tmpnam] = "gnudata.txt";
    char plotscript[L_tmpnam] = "gnuplot.gc";

    char * tmpname = tmpnam(datafile);
    char * gnuplotname = tmpnam(plotscript);

    // Generate data file
    std::ofstream tmpfile;
    tmpfile.open(tmpname);

    for (Loop loop : *loops)
        for (ReducedMPICall *mpicall : loop.getMpiCalls())
        {
            unsigned int reps = mpicall->getRepetitions();
            unsigned int iit = mpicall->getInterRepTime();

            tmpfile << reps << "\t" << iit << "\t" << loop.getId() << "\n";
        }
    tmpfile.close();

    // Generate gnuplot script
    std::ofstream gnuplot;
    gnuplot.open(gnuplotname);
    gnuplot << "set title \"Detected loops by clustering\"\n"
            << "set nokey\n"
            << "set grid xtics, ytics\n"
            << "set pointsize 3\n"
            << "set xlabel \"Repetitions\"\n"
            << "set ylabel \"Iterations time\"\n"
            << "set offsets graph 0.5, 0.5, 0.5, 0.5\n"
            << "set palette maxcolors " << loops->size() << "\n"
            << "plot \"" << tmpname << "\" with points palette pointtype 3";

    for (auto r : centroids)
    {
        gnuplot << ", (" << this->texe << "*" << r << ")/x";
    }

    gnuplot << "\npause -1\n";

    gnuplot.close();

    this->clustering_data_file = tmpname;
    this->gnuplot_script_file = gnuplotname;
}

void LoopsMerge::actual_run(LoopVector *input)
{
    // First step is to classify loops by their delta
    mlpack::dbscan::DBSCAN<> dbscan(this->eps, this->minPts);

    this->log_reporter->addMessage("Input size: " + std::to_string(input->size()));
    if (input->size() == 0)
    {
        this->log_reporter->addMessage("No superloops can be detected");
        this->result = new TopLevelLoopVector();
        this->done = true;
        return;
    }

    arma::mat data(1,input->size()); // 2-dimensions
    arma::Row<size_t> assignements;
    arma::mat centroids;

    // ERROR : On re-run seems that input contains pointers previously freed
    for (int i=0; i < input->size(); ++i)
        data(0,i) = input->at(i).getDelta();

    //data = arma::normalise(data);
    size_t nclusters = dbscan.Cluster(data, assignements, centroids);
    this->preparePlot(input, centroids);

    /****/
    //std::cout << "NCLUSTERS: " << nclusters << std::endl;
    //data.print("DATA " + this->phasename + ": ");
    //assignements.print("ASSIGN " + this->phasename + ": ");
    //centroids.print("CENTR " + this->phasename + ": ");
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
