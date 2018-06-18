/*
 * loops-merge.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef LOOPS_MERGE_H
#define LOOPS_MERGE_H

#include <pipeline.h>
#include <loops_identification.h>

//typedef std::vector<std::pair<Loop*, Loop*>> LoopsRelation;

class TopLevelLoop
{
    public:
        TopLevelLoop() 
            : ntll(1) {};
        void merge();
        void insert(Loop *loop)
            { this->loops.push_back(loop); }
        void setLoopId(size_t loop_id)
            { this->loop_id = loop_id; }
        void setDelta(float delta)
            { this->delta = delta; }
        float getDelta()
            { return this->delta; }
        Loop *getSuperloop()
            { return this->superloop; }
    private:
        void reverse_merge();
        void straight_merge();
        void superloop_detect();
        void aliasing_split();

        Loop *superloop;
        std::vector<Loop*> loops;
        size_t loop_id;
        float delta;
        unsigned int ntll; // if aliasing could be tll > 1
};

typedef std::vector<TopLevelLoop> TopLevelLoopVector;

class LoopsMerge : public PipelineStage<LoopVector, TopLevelLoopVector>
{
    public:
        LoopsMerge(double eps, size_t minPts)
            : PipelineStage<LoopVector, TopLevelLoopVector>(
                    "Loops merge", false, false)
            , eps(eps)
            , minPts(minPts) {};
        unsigned int getNPhases()
            { return this->nphases; }
        std::vector<unsigned int> getDeltas();
    private:
        unsigned int nphases;
        double eps;
        size_t minPts;
        void actual_run(LoopVector *input);
};

#endif /* !LOOPS_MERGE_H */
