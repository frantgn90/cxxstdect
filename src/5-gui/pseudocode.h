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

#define TIME_UNITS_NS "ns"
#define TIME_UNITS_US "us"
#define TIME_UNITS_MS "ms"
#define TIME_UNITS_S  "s"


#define SIZE_UNITS_B  "Byte"
#define SIZE_UNITS_KB "KByte"
#define SIZE_UNITS_MB "MByte"
#define SIZE_UNITS_GB "GByte"

// line, file, caller
typedef std::tuple<unsigned int, std::string, std::string> Caller; 

class GUIRepresentation
{
    public:
        GUIRepresentation(libparaver::UIParaverTraceConfig* trace_semantic)
            : trace_semantic(trace_semantic) {};
        virtual ~GUIRepresentation() {};
        Caller getCallerAt(unsigned int i) const
            { return this->callpath_str[i]; }
        std::vector<Caller> getCallers() const
            { return this->callpath_str; }
        unsigned int getNCallers() const
            { return (unsigned int)this->callpath_str.size(); }
        std::vector<Caller> getCallpath()
            { return this->callpath_str; }
        virtual std::string print() = 0;

        virtual std::string getRepresentation()
            { return "None"; }
        virtual unsigned int getDuration()
            { return 0.0; }
        virtual unsigned int getMsgSize()
            { return 0.0; }
        virtual float getIPC()
            { return 0.0; }
        virtual float getHWC(std::string hwc_name)
        {   //std::cout << "getHWC not implemented for " << hwc_name << std::endl;
            return 0; 
        }
        virtual bool isLoop()
            { return false; }
        virtual std::vector<unsigned int>* getTasks()
            { return NULL; }
    protected:
        libparaver::UIParaverTraceConfig* trace_semantic;
        std::string getSemantic(int type, int value)
            { return this->trace_semantic->getEventValue(type, value); }
        std::vector<Caller> callpath_str; 
};

class GUIReducedCPUBurst : public GUIRepresentation
{
    public:
        GUIReducedCPUBurst(ReducedCPUBurst* cpuburst,
                libparaver::UIParaverTraceConfig* trace_semantic);
        std::string print()
            { return "CPU BURST"; }
        std::string getRepresentation()
            { return "<i>Computation burst</i>"; }
        std::vector<unsigned int>* getTasks()
            { return this->cpuburst->getTasks(); }
        virtual unsigned int getDuration()
            { return (unsigned int)this->cpuburst->getDuration(); }
        virtual float getHWC(std::string hwc_name)
        {
            for (auto it : this->hwc)
            {
                if (it.first == hwc_name)
                    return (float)it.second;
            }
            return 0;
        }
    private:
        ReducedCPUBurst *cpuburst;
        std::vector<std::pair<std::string, unsigned int>> hwc;
};

class GUIReducedMPICall : public GUIRepresentation
{
    public:
        GUIReducedMPICall(
                ReducedMPICall* mpicall,
                libparaver::UIParaverTraceConfig* trace_semantic);
        ~GUIReducedMPICall()
        {
            delete cpu_burst;
        }
        std::string print();
        std::string getRepresentation()
            { return this->mpi_call_str + "()"; }
        std::vector<unsigned int>* getTasks()
            { return this->mpicall->getTasks(); }
        virtual unsigned int getDuration()
            { return this->mpicall->getDuration(); }
        virtual unsigned int getMsgSize()
            { return this->mpicall->getMsgSize(); }
        GUIReducedCPUBurst* getPreviousCPUBurst()
            { return this->cpu_burst; }
    private:
        std::string mpi_call_str;
        ReducedMPICall* mpicall;
        GUIReducedCPUBurst* cpu_burst;
};

class GUILoop : public GUIRepresentation
{
    public:
        GUILoop(Loop* loop, 
                libparaver::UIParaverTraceConfig* trace_semantic);
        ~GUILoop()
        {
            std::for_each(statements.begin(), statements.end(),
                    [](GUIRepresentation* ptr) 
            { 
                delete ptr;
                /*
                if (ptr->isLoop())
                    delete static_cast<GUILoop*>(ptr);
                else
                    delete static_cast<GUIReducedMPICall*>(ptr); 
                */
            });
            statements.clear();
        }
        unsigned int getNIterations() const
            { return this->loop->getIterations(); }
        std::vector<GUIRepresentation*> getStatements() const
            { return this->statements; }
        std::vector<unsigned int>* getTasks()
            { return this->loop->getTasks(); }
        std::string print();
        std::string getRepresentation()
        { 
            unsigned int iters = this->loop->getIterations();
            if (this->loop->getSuperloop())
                iters /= this->loop->getSuperloop()->getIterations();

            return "Loop " + std::to_string(iters) + " iterations"; 
        }
        virtual bool isLoop()
            { return true; }
        Loop* getLoopObj()
            { return this->loop; }
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
        wxPseudocodeItem(GUILoop* r, std::string color="#000000")
            : is_loop(true)
            , is_computation(false)
        {
            this->init(static_cast<GUIRepresentation*>(r), color);
        }
        wxPseudocodeItem(GUIReducedMPICall* r, std::string color="#000000")
            : is_loop(false)
            , is_computation(false)
        {
            this->init(static_cast<GUIReducedMPICall*>(r), color);
        }
        wxPseudocodeItem(GUIReducedCPUBurst* r, std::string color="#000000")
            : is_loop(false)
            , is_computation(true)
        {
            this->init(static_cast<GUIRepresentation*>(r), color);
        }
        wxPseudocodeItem(GUIRepresentation* r, std::string color="#000000")
            : is_loop(false)
            , is_computation(false)
        {
            this->init(r, color);
        }

        void init(GUIRepresentation* r, std::string color="#000000")
        {
            this->line = r->getRepresentation();
            this->duration = r->getDuration();
            this->msg_size = r->getMsgSize();
            this->ipc = r->getIPC();
            this->rgb_color = color;
            this->actual_statement = r;
        }
        /*
        wxPseudocodeItem(std::string line, 
                unsigned int duration, float msg_size, float ipc, 
                GUIRepresentation* r, std::string color="#000")
        {
            this->line = line;
            this->duration = duration;
            this->msg_size = msg_size;
            this->ipc = ipc;
            this->rgb_color = color;
            this->actual_statement = r;
        }
        */
        void setParent(wxPseudocodeItem* item)
            { this->parent = item; }
        void addChild(wxPseudocodeItem* item)
            { this->ordered_childs.push_back(item); }
        wxPseudocodeItem* GetParent()
            { return this->parent; }
        std::vector<wxPseudocodeItem*> GetChildren()
            { return this->ordered_childs; }
        std::string GetPseudocode()
        {   
            std::string res = "<span color='" + this->rgb_color +"'>" 
                + this->line + "</span>";
            if (!this->parent)
                res = "<b>" + res + "</b>";
            /*else
                res += "(" + std::to_string(this->actual_statement->
                    getPreviousCPUBurstDuration()) + ")";*/
            return res; 
        }
        unsigned int GetDuration()
            { return this->duration; }
        unsigned int GetMsgSize()
            { return this->msg_size; }
        float GetIPC()
            { return this->ipc; }
        float GetHWC(std::string hwc_name)
            { return this->actual_statement->getHWC(hwc_name); }
        bool IsContainer()
            { return this->ordered_childs.size() > 0; }
        bool IsLoop()
            { return is_loop; }
        bool IsComputation()
            { return is_computation; }
        GUIRepresentation* getActualObject()
            { return this->actual_statement; }
    private:
        std::string line;
        unsigned int duration;
        unsigned int msg_size;
        float ipc;
        wxPseudocodeItem *parent = NULL;
        std::vector<wxPseudocodeItem*> ordered_childs;
        std::string rgb_color;
        GUIRepresentation* actual_statement;
        bool is_loop;
        bool is_computation;
};

class wxPseudocode : public wxDataViewModel
{
    public:
        wxPseudocode(){};
        wxPseudocode(std::vector<GUILoop*> top_level_loops);

        bool IsContainer(const wxDataViewItem &item) const;
        virtual wxDataViewItem GetParent(const wxDataViewItem &item) const;
        unsigned int GetChildren(const wxDataViewItem& item, 
                wxDataViewItemArray& childs) const;
        unsigned int GetColumnCount() const;
        wxString GetColumnType(unsigned int col) const;
        void GetValue(wxVariant &var, const wxDataViewItem& item, unsigned int 
                col) const;
        bool SetValue(const wxVariant& var, const wxDataViewItem& item, 
                unsigned int col);
        std::vector<std::pair<std::vector<unsigned int>*, std::string>> 
        getColormap()
            { return this->color_map; }
        void setRanksFilter(std::vector<unsigned int> ranks)
            { this->ranks_filter = ranks; }
        void showComputations(bool show)
            { this->show_computations = show; }
        void setComputationsThresshold(unsigned int th)
            { this->computation_thresshold = th; }
        void setHWCColumnType(std::string hwc_column_type)
            { this->hwc_column_type = hwc_column_type; }
        void setTimeUnits(std::string units)
        {
            if (units == TIME_UNITS_NS)
                time_factor = 1;
            else if (units == TIME_UNITS_US)
                time_factor = (float)1/1000;
            else if (units == TIME_UNITS_MS)
                time_factor = (float)1/1000000;
            else if (units == TIME_UNITS_S)
                time_factor = (float)1/1000000000;
            else
                assert(false);
            std::cout << this->time_factor << std::endl;
        }
        void setSizeUnits(std::string units)
        {
            if (units == SIZE_UNITS_B)
                size_factor = 1;
            else if (units == SIZE_UNITS_KB)
                size_factor = (float)1/(1<<10);
            else if (units == SIZE_UNITS_MB)
                size_factor = (float)1/(1<<20);
            else if (units == SIZE_UNITS_GB)
                size_factor = (float)1/(1<<30);
            else
                assert(false);
        }
        ~wxPseudocode()
        {
            std::for_each(items.begin(), items.end(),
                    [](wxPseudocodeItem* ptr) { delete ptr; });
            items.clear();
            roots.clear();
        }
    private:
        void parseChilds(wxPseudocodeItem* parent, GUILoop* loop);

        std::vector<wxPseudocodeItem*> roots;
        std::vector<wxPseudocodeItem*> items;
        std::vector<std::string> coltypes = {"string","string","string","string"};
        std::vector<std::pair<std::vector<unsigned int>*, std::string>> color_map;
        std::vector<unsigned int> ranks_filter;
        bool show_computations;
        unsigned int computation_thresshold;
        std::string hwc_column_type;
        float size_factor;
        float time_factor;
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
        std::vector<GUILoop*> top_level_loops;
        void actual_run(TopLevelLoopVector* input);
        void actual_clear()
        {
            //delete trace_semantic;
            delete this->result;
            std::for_each(top_level_loops.begin(), top_level_loops.end(),
                    [](GUILoop* ptr){ delete ptr; });
            top_level_loops.clear();
        }
};

#endif /* !PSEUDOCODE_H */
