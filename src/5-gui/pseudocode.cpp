/*
 * pseudocode.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "pseudocode.h"
#include <iostream>
#include <regex>
#include <algorithm>
#include <stdlib.h>

GUIReducedCPUBurst::GUIReducedCPUBurst(
        ReducedCPUBurst *cpuburst,
        libparaver::UIParaverTraceConfig* trace_semantic)
    : GUIRepresentation(trace_semantic)
    , cpuburst(cpuburst)
{

    for (unsigned int i=0; i<cpuburst->getHWCCount(); ++i)
    {
        auto hwc_v = cpuburst->getHWCAt(i);
        std::string hwc_name = trace_semantic->getEventType(hwc_v.first);
        this->hwc.push_back(std::make_pair(hwc_name, hwc_v.second));
    }
}


GUIReducedMPICall::GUIReducedMPICall(
        ReducedMPICall *mpicall,
        libparaver::UIParaverTraceConfig* trace_semantic)
    : GUIRepresentation(trace_semantic)
    , cpu_burst(new GUIReducedCPUBurst(
            mpicall->getPreviousCPUBurst(),trace_semantic))
{

    std::string srcpos_regex = "^([0-9]+) \\(((.+))(,.+)?\\)$";
    std::regex regex(srcpos_regex);

    for (int i=0; i < mpicall->getCallpath().size(); ++i)
    {
        std::string caller = this->getSemantic(
                70000000+i+1, mpicall->getCallers()[i]);
        if (caller.find("Unresolved") != std::string::npos)
            continue; // ignore unknown caller

        std::string srcpos = this->getSemantic(
                80000000+i+1, mpicall->getCallpath()[i]);

        std::smatch m;
        int match = std::regex_match(srcpos, m, regex);
        if (!match)
        {
            continue;
            //std::cout << "Error parsing Event value" << std::endl;
            //exit(-1);
        }

        unsigned int line = std::atoll(std::string(m[1]).c_str());
        std::string file = m[2];

        this->callpath_str.push_back(make_tuple(line, file, caller));
    }
    this->mpi_call_str = this->getSemantic(
            mpicall->getMpiType(), mpicall->getMpiId());
    this->mpicall = mpicall;
}

struct pA_comp {
    bool operator() (const GUIRepresentation* l, 
            const GUIRepresentation* r) const { 
        int N = std::min(l->getNCallers(), r->getNCallers());
        for (int i=0; i<N; ++i)
        {
            Caller my = l->getCallerAt(i);
            Caller its = r->getCallerAt(i);

            if (std::get<2>(my) != std::get<2>(its)) // different file
                return false; // it is random...
            else
            {
                if (std::get<0>(my) < std::get<0>(its))
                    return true;
                else if (std::get<0>(my) > std::get<0>(its))
                    return false;
            }
        }
        // It should never happen!
        assert (false);

    }
};

struct pA_comp_tl {
    bool operator() (const GUILoop* l, const GUILoop* r) const { 
        int N = std::min(l->getNCallers(), r->getNCallers());

        std::vector<Caller> my_callers = l->getCallers();
        std::vector<Caller> its_callers = r->getCallers();

        std::reverse(my_callers.begin(), my_callers.end());
        std::reverse(its_callers.begin(), its_callers.end());

        for (int i=0; i<N; ++i)
        {
            Caller my = my_callers[i];
            Caller its = its_callers[i];

            if (std::get<2>(my) != std::get<2>(its)) // different file
                return false; // it is random...
            else
            {
                if (std::get<0>(my) < std::get<0>(its))
                    return true;
                else if (std::get<0>(my) > std::get<0>(its))
                    return false;
            }
        }
        // It should never happen!
        assert (false);

    }
};


GUILoop::GUILoop(Loop* loop,
        libparaver::UIParaverTraceConfig* trace_semantic)
    : GUIRepresentation(trace_semantic)
{
    for (auto it : loop->getMpiCalls())
        this->statements.push_back(new GUIReducedMPICall(it, trace_semantic));
    for (auto it : loop->getSubloops())
        this->statements.push_back(new GUILoop(it, trace_semantic));
 
    this->callpath_str = this->statements[0]->getCallpath();
    this->loop = loop;

    // ordenar this->statements por posicion en el codigo
    std::sort(this->statements.begin(), this->statements.end(), pA_comp());

    // TODO: break up 'previous cpu burst' to new GUIRepresentation objects
    /*for (auto it : this->statements)
    {
        if (!it->isLoop())
        {
            this->statement.insert(it->getGUICPUBurst());
        }
    }*/
}

std::ostream &operator<<(std::ostream &output, GUIRepresentation &r)
{
    output << r.print();
    return output;
}

std::string GUIReducedMPICall::print()
{
    std::string result;
    for (int i=0; i<this->callpath_str.size(); ++i)
        result += "["
            + std::to_string(std::get<0>(this->callpath_str[i])) + "]"
            + std::get<2>(this->callpath_str[i]) 
            + "->";
    result += this->mpi_call_str;
    result += "(";
    for (auto it : *(this->mpicall->getTasks()) )
        result += std::to_string(it) + ",";
    result += ")";
    return result;
}

std::string GUILoop::print()
{
    std::string result = "Loop " + std::to_string(this->loop->getIterations());
    result += "(";
    for (auto it : *(this->loop->getTasks()) )
        result += std::to_string(it) + ",";
    result += ")\n";

    for (auto it : this->statements)
        result += (it->print() + "\n");
    result += "End loop";
    return result;
}

wxPseudocode::wxPseudocode(std::vector<GUILoop*> top_level_loops)
    : show_computations(false)
    , computation_thresshold(0)
{
    srand(123456);
    for (auto it : top_level_loops)
    {
        wxPseudocodeItem* root = new wxPseudocodeItem(it);
        root->setParent(NULL);
        this->items.push_back(root);
        this->roots.push_back(root);
        this->parseChilds(root, it);
    }
}

void wxPseudocode::parseChilds(wxPseudocodeItem* parent, GUILoop* loop)
{
    for (auto it : loop->getStatements())
    {
        std::string color_val;
        if (!it->isLoop())
        {
            GUIReducedMPICall* mpicall = static_cast<GUIReducedMPICall*>(it);
            bool found = false;
            for (auto it2 : color_map)
            {
                auto first_color_v = it2.first;
                auto second_color_v = mpicall->getTasks();

                if (first_color_v->size() != second_color_v->size())
                    continue;
                else
                {
                    std::sort(first_color_v->begin(), first_color_v->end());
                    std::sort(second_color_v->begin(), second_color_v->end());
                    if (std::equal(first_color_v->begin(), first_color_v->end(), 
                                second_color_v->begin()))
                    {
                        color_val = it2.second;
                        found = true;
                        break;
                    }
                }
            }
            
            if (!found)
            {
                unsigned int random = rand() % 16777215;
                std::stringstream stream;
                stream << std::hex << random;
                color_val = stream.str();
                color_map.push_back(make_pair(mpicall->getTasks(), color_val));
            }
        }
        else
            color_val = "000000"; // black by default

        std::string color = "#"+color_val; 

        if (!it->isLoop())
        {
            GUIReducedMPICall* mpicall = static_cast<GUIReducedMPICall*>(it);
            wxPseudocodeItem* burst = new wxPseudocodeItem(
                    mpicall->getPreviousCPUBurst(), "#bbb");
            burst->setParent(parent);
            this->items.push_back(burst);
            parent->addChild(burst);
        }

        wxPseudocodeItem* child = new wxPseudocodeItem(it, color);
        child->setParent(parent);
        this->items.push_back(child);
        parent->addChild(child);

        if (it->isLoop())
            this->parseChilds(child, static_cast<GUILoop*>(it));
    }
}

bool wxPseudocode::IsContainer(const wxDataViewItem &item) const
{
    if (!item.IsOk())
        return true;

    wxPseudocodeItem* node = static_cast<wxPseudocodeItem*>(item.GetID());
    return node->IsContainer();
}

wxDataViewItem wxPseudocode::GetParent(const wxDataViewItem &item) const
{
    if (!item.IsOk())
        return wxDataViewItem(0);

    wxPseudocodeItem* node = static_cast<wxPseudocodeItem*>(item.GetID());

    if (node->GetParent() == NULL)
        return wxDataViewItem(0);
    else
        return wxDataViewItem((void*)node->GetParent());
}

unsigned int wxPseudocode::GetChildren(const wxDataViewItem& item, 
        wxDataViewItemArray& childs) const
{
    wxPseudocodeItem* parent = static_cast<wxPseudocodeItem*>(item.GetID());
    int count=0;
    if (!parent)
    {
        for (auto it : this->roots)
        {
            if (this->ranks_filter.size() != 0)
            {
                GUILoop* loop = static_cast<GUILoop*>(it->getActualObject());
                std::vector<unsigned int>* tasks = loop->getTasks();
                for (auto rank : this->ranks_filter)
                    if (std::find(tasks->begin(), tasks->end(), rank) 
                            != tasks->end())
                    {
                        count++;
                        childs.Add(wxDataViewItem(it));
                        break;
                    }
            }
            else
            {
                count++;
                childs.Add(wxDataViewItem(it));
            }
        }
    }
    else
    {
        for (auto it : parent->GetChildren())
        {
            if (this->ranks_filter.size() != 0)
            {
                GUIReducedMPICall* mpicall = static_cast<GUIReducedMPICall*>(
                        it->getActualObject());
                std::vector<unsigned int>* tasks = mpicall->getTasks();
                for (auto rank : this->ranks_filter)
                    if (std::find(tasks->begin(), tasks->end(), rank) 
                            != tasks->end())
                    {
                        if (!it->IsComputation() or (
                                it->IsComputation() and 
                                this->show_computations and 
                                this->computation_thresshold <= it->GetDuration()))
                        {
                            count++;
                            childs.Add(wxDataViewItem(it));
                            break;
                        }
                    }
            }
            else
            {
                if (!it->IsComputation() or (
                        it->IsComputation() and 
                        this->show_computations and 
                        this->computation_thresshold <= it->GetDuration()))
                {
                    count++;
                    childs.Add(wxDataViewItem(it));
                }
            }
        }
    }
    return count;
}

unsigned int wxPseudocode::GetColumnCount() const
{
    return this->coltypes.size();
}

wxString wxPseudocode::GetColumnType(unsigned int col) const
{
    return this->coltypes[col];
}

void wxPseudocode::GetValue(wxVariant &var, const wxDataViewItem& item, 
        unsigned int col) const
{
    wxPseudocodeItem* pitem = static_cast<wxPseudocodeItem*>(item.GetID());
    switch(col)
    {
        case 0:
            var = pitem->GetPseudocode();
            break;
        case 1:
            var = (long int)pitem->GetDuration();
            break;
        case 2:
            var = (long int)pitem->GetMsgSize();
            break;
        case 3:
            var = pitem->GetHWC(this->hwc_column_type);
            break;
    }
    //assert(false);
}


bool wxPseudocode::SetValue(const wxVariant& var, const wxDataViewItem& item, 
        unsigned int col)
{
    return true;
}

void Pseudocode::actual_run(TopLevelLoopVector* input)
{
    for (auto it = input->begin(); it != input->end(); ++it)
    {
        this->top_level_loops.push_back(
                new GUILoop(it->getSuperloop(), this->trace_semantic));
    }

    std::sort(this->top_level_loops.begin(), 
            this->top_level_loops.end(), 
            pA_comp_tl());

    //std::cout << "== Actual pseudocode ==" << std::endl;
    //for (int i=0; i<this->top_level_loops.size(); ++i)
    //    std::cout << this->top_level_loops[i];
    //std::cout << std::endl << "=======================" << std::endl;
    
    this->result = new wxPseudocode(this->top_level_loops);
}
