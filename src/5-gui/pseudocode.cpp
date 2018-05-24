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

GUIReducedMPICall::GUIReducedMPICall(
        ReducedMPICall *mpicall,
        libparaver::UIParaverTraceConfig* trace_semantic)
    : GUIRepresentation(trace_semantic)
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
    for (auto it : this->loop->getTasks() )
        result += std::to_string(it) + ",";
    result += ")\n";

    for (auto it : this->statements)
        result += (it->print() + "\n");
    result += "End loop";
    return result;
}

wxPseudocode::wxPseudocode(std::vector<GUILoop> top_level_loops)
{
    for (auto it : top_level_loops)
    {
        wxPseudocodeItem root = wxPseudocodeItem(
                it.getRepresentation(), 
                it.getDuration(), 
                it.getMsgSize(), 
                it.getIPC());
        root.setParent(NULL);
        this->items.push_back(root);
        this->roots.push_back(&(this->items.back()));
        this->parseChilds(&(this->items.back()), it);
    }
}

void wxPseudocode::parseChilds(wxPseudocodeItem* parent, GUILoop& loop)
{
    for (auto it : loop.getStatements())
    {
        wxPseudocodeItem child = wxPseudocodeItem(
                it->getRepresentation(),
                it->getDuration(),
                it->getMsgSize(),
                it->getIPC());
        child.setParent(parent);
        this->items.push_back(child);
        parent->addChild(&(this->items.back()));
    }
}

bool IsContainer(wxDataViewItem &item)
{
    return static_cast<wxPseudocodeItem*>(item.GetID())->IsContainer();
}

wxDataViewItem wxPseudocode::GetParent(wxDataViewItem &item)
{
    return wxDataViewItem((void*)static_cast<wxPseudocodeItem*>
            (item.GetID())->GetParent());
}

unsigned int wxPseudocode::GetChildren(wxDataViewItem& item, 
        wxDataViewItemArray& childs)
{
    wxPseudocodeItem* parent = static_cast<wxPseudocodeItem*>(item.GetID());
    for (auto it : parent->GetChildren())
        childs.Add(wxDataViewItem((void*)it));
    return parent->GetChildren().size();
}

unsigned int wxPseudocode::GetColumnCount()
{
    return this->coltypes.size();
}

wxString wxPseudocode::GetColumnType(unsigned int col)
{
    return this->coltypes[col];
}

void wxPseudocode::GetValue(wxVariant &var, wxDataViewItem& item, unsigned int col)
{
    wxPseudocodeItem* pitem = static_cast<wxPseudocodeItem*>(item.GetID());
    switch(col)
    {
        case 0:
            var = pitem->GetPseudocode();
            break;
        case 1:
            var = pitem->GetDuration();
            break;
        case 2:
            var = pitem->GetMsgSize();
            break;
        case 3:
            var = pitem->GetIPC();
            break;
    }
    assert(false);
}

void Pseudocode::actual_run(TopLevelLoopVector* input)
{
    for (auto it = input->begin(); it != input->end(); ++it)
    {
        this->top_level_loops.push_back(
                GUILoop(it->getSuperloop(), this->trace_semantic));
    }

    //for (int i=0; i<this->top_level_loops.size(); ++i)
    //    std::cout << this->top_level_loops[i];
    
    this->result = new wxPseudocode(this->top_level_loops);
}
