/////////////////////////////////////////////////////////////////////////////
// Name:        stdect-app.h
// Purpose:     
// Author:      Juan Francisco Martínez Vera
// Modified by: 
// Created:     Wed 23 May 2018 11:56:29 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef STDECT_APP_H_
#define STDECT_APP_H_


/*!
 * Includes
 */

////@begin includes
#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include "wx/image.h"
#include "structuredetection.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * StdectApp class declaration
 */

class StdectApp: public wxApp
{    
    DECLARE_CLASS( StdectApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    StdectApp();

    void Init();

    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
    virtual bool OnInit();
    virtual int OnExit();

////@begin StdectApp event handler declarations

////@end StdectApp event handler declarations

////@begin StdectApp member function declarations

////@end StdectApp member function declarations

////@begin StdectApp member variables
    std::string tracefile;
    std::string paraver_pcf;
    double filter_lbound = 0.01;
    double eps = 0.1;
    size_t minPts = 1;
    double eps_tl = 0.1;
    size_t minPts_tl = 1;
////@end StdectApp member variables
};

/*!
 * Application instance declaration 
 */


static const wxCmdLineEntryDesc g_cmdLineDesc [] =
{
    { 
        wxCMD_LINE_OPTION, 
        "t", 
        "trace", 
        "Trace to analyze",
        wxCMD_LINE_VAL_STRING, 
        wxCMD_LINE_OPTION_MANDATORY  
    },
    { 
        wxCMD_LINE_OPTION, 
        "b", 
        "bottom-bound", 
        "Bottom boundary. All mpi calls that represents a fraction bellow"\
            " this value will be dismissed.",
        wxCMD_LINE_VAL_DOUBLE,
        wxCMD_LINE_PARAM_OPTIONAL
    },
    { 
        wxCMD_LINE_OPTION, 
        "e", 
        "eps", 
        "Epsilon for the main clustering",
        wxCMD_LINE_VAL_DOUBLE,
        wxCMD_LINE_PARAM_OPTIONAL
    },
    { 
        wxCMD_LINE_OPTION, 
        "m", 
        "minPts",
        "Minimum points for the main clustering",
        wxCMD_LINE_VAL_DOUBLE,
        wxCMD_LINE_PARAM_OPTIONAL
    },
    { 
        wxCMD_LINE_OPTION, 
        "f", 
        "eps-tl",
        "Epsilon for the delta clustering",
        wxCMD_LINE_VAL_DOUBLE,
        wxCMD_LINE_PARAM_OPTIONAL
    },
    { 
        wxCMD_LINE_OPTION, 
        "g", 
        "minPts-tl", 
        "Minimum points for the delta clustering",
        wxCMD_LINE_VAL_DOUBLE,
        wxCMD_LINE_PARAM_OPTIONAL
    },
    { wxCMD_LINE_NONE }
};

////@begin declare app
DECLARE_APP(StdectApp)
////@end declare app

#endif // _STDECT-APP_H_
