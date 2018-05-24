/*
 * pseudocode.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef PSEUDOCODE_H
#define PSEUDOCODE_H

#include<UIParaverTraceConfig.h>
#include<pipeline.h>
#include<loops_merge.h>
#include<iostream>
#include<wx/dataview.h>

// line, file, caller
typedef std::tuple<unsigned int, std::string, std::string> Caller; 

class GUIRepresentation
{
    public:
        GUIRepresentation(libparaver::UIParaverTraceConfig* trace_semantic)
            : trace_semantic(trace_semantic) {};
        Caller getCallerAt(unsigned int i) const
            { return this->callpath_str[i]; }
        unsigned int getNCallers() const
            { return this->callpath_str.size(); }
        std::vector<Caller> getCallpath()
            { return this->callpath_str; }
        virtual std::string print() = 0;

        std::string getRepresentation()
            { return "None"; }
        float getDuration()
            { return 0.0; }
        float getMsgSize()
            { return 0.0; }
        float getIPC()
            { return 0.0; }
    protected:
        libparaver::UIParaverTraceConfig* trace_semantic;
        std::string getSemantic(int type, int value)
            { return this->trace_semantic->getEventValue(type, value); }
        std::vector<Caller> callpath_str; 

};

class GUIReducedMPICall : public GUIRepresentation
{
    public:
        GUIReducedMPICall(
                ReducedMPICall* mpicall,
                libparaver::UIParaverTraceConfig* trace_semantic);
        std::string print();
        std::string getRepresentation()
            { return this->mpi_call_str + "()"; }
    private:
        std::string mpi_call_str;
        ReducedMPICall* mpicall;
};

class GUILoop : public GUIRepresentation
{
    public:
        GUILoop(Loop* loop, 
                libparaver::UIParaverTraceConfig* trace_semantic);
        unsigned int getNIterations() const
            { return this->loop->getIterations(); }
        std::vector<GUIRepresentation*> getStatements() const
            { return this->statements; }
        std::string print();
        std::string getRepresentation()
            { return "Loop " 
                + std::to_string(this->loop->getIterations())
                + " iterations"; }
    private:
        std::vector<GUIRepresentation*> statements;
        Loop* loop;
};


std::ostream &operator<<(std::ostream &output, const GUIRepresentation &r);

typedef std::tuple<std::string, float, float, float> PseudocodeItem;

// All GUI* classes are about translate information to a human readable
// information. Also is about discover the actual code position of every
// call. There is a general previous step implementation before the actual 
// GUI.
//
// From here the wxwidgets classes.

class wxPseudocodeItem
{
    public:
        wxPseudocodeItem(){};
        wxPseudocodeItem(std::string line, 
                float duration, float msg_size, float ipc)
        {
            this->line = line;
            this->duration = duration;
            this->msg_size = msg_size;
            this->ipc = ipc;
        }
        void setParent(wxPseudocodeItem* item)
            { this->parent = item; }
        void addChild(wxPseudocodeItem* item)
            { this->ordered_childs.push_back(item); }
        wxPseudocodeItem* GetParent()
            { return this->parent; }
        std::vector<wxPseudocodeItem*> GetChildren()
            { return this->ordered_childs; }
        std::string GetPseudocode()
            { return this->line; }
        float GetDuration()
            { return this->duration; }
        float GetMsgSize()
            { return this->msg_size; }
        float GetIPC()
            { return this->ipc; }
        bool IsContainer()
            { return this->ordered_childs.size() > 0; }
    private:
        std::string line;
        float duration;
        float msg_size;
        float ipc;
        wxPseudocodeItem *parent;
        std::vector<wxPseudocodeItem*> ordered_childs;
};

class wxPseudocode : public wxDataViewModel
{
    public:
        wxPseudocode(){};
        wxPseudocode(std::vector<GUILoop> top_level_loops);

        bool IsContainer(const wxDataViewItem &item) const;
        virtual wxDataViewItem GetParent(const wxDataViewItem &item) const;
        unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& childs) const;
        unsigned int GetColumnCount() const;
        wxString GetColumnType(unsigned int col) const;
        void GetValue(wxVariant &var, const wxDataViewItem& item, unsigned int col) const;
        bool SetValue(const wxVariant& var, const wxDataViewItem& item, unsigned int col);
    private:
        std::vector<wxPseudocodeItem*> roots;
        std::vector<wxPseudocodeItem> items;
        void parseChilds(wxPseudocodeItem* parent, GUILoop& loop);
        std::vector<std::string> coltypes = { "string", "float", 
            "float", "float" };
};

class Pseudocode : public PipelineStage<TopLevelLoopVector, wxPseudocode>
{
    public:
        Pseudocode(libparaver::UIParaverTraceConfig* trace_semantic)
            : PipelineStage<TopLevelLoopVector, wxPseudocode>(
                    "Pseudocode construction",false,false)
        , trace_semantic(trace_semantic) {};
    private:
        libparaver::UIParaverTraceConfig* trace_semantic;
        std::vector<GUILoop> top_level_loops;
        void actual_run(TopLevelLoopVector* input);


};

#endif /* !PSEUDOCODE_H */
