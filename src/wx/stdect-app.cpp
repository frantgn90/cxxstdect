/////////////////////////////////////////////////////////////////////////////
// Name:        stdect-app.cpp
// Purpose:     
// Author:      Juan Francisco Martínez Vera
// Modified by: 
// Created:     Wed 23 May 2018 11:56:29 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

////@begin includes

#include<string>
#include<iostream>

////@end includes


// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "stdect-app.h"

/*
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( StdectApp )
////@end implement app


/*
 * StdectApp type definition
 */

IMPLEMENT_CLASS( StdectApp, wxApp )


/*
 * StdectApp event table definition
 */

BEGIN_EVENT_TABLE( StdectApp, wxApp )

////@begin StdectApp event table entries
////@end StdectApp event table entries

END_EVENT_TABLE()


/*
 * Constructor for StdectApp
 */

StdectApp::StdectApp()
{
    Init();
}


/*
 * Member initialisation
 */

void StdectApp::Init()
{
////@begin StdectApp member initialisation
////@end StdectApp member initialisation
}

/*
 * Initialisation for StdectApp
 */

bool StdectApp::OnInit()
{    
////@begin StdectApp initialisation
	// Remove the comment markers above and below this block
	// to make permanent changes to the code.

    if (!wxApp::OnInit())
        return false;

#if wxUSE_XPM
	wxImage::AddHandler(new wxXPMHandler);
#endif
#if wxUSE_LIBPNG
	wxImage::AddHandler(new wxPNGHandler);
#endif
#if wxUSE_LIBJPEG
	wxImage::AddHandler(new wxJPEGHandler);
#endif
#if wxUSE_GIF
	wxImage::AddHandler(new wxGIFHandler);
#endif
	Structuredetection* mainWindow = new Structuredetection( NULL );
	mainWindow->Show(true);
////@end StdectApp initialisation

    std::string paraver_pcf;
    size_t start_pos = this->tracefile.find(".prv");
    if (start_pos != std::string::npos)
    {
        paraver_pcf = this->tracefile.substr(0, start_pos);
        paraver_pcf += ".pcf";
    }
    else
    {
        std::cout << "Incorrect tracefile: " << this->tracefile << std::endl;
        exit(-1);
    }
    
    std::cout << "Tracefile: " << this->tracefile << std::endl;
    std::cout << "Semantic file: " << paraver_pcf << std::endl;

    struct stat buffer;   
    if (stat (this->tracefile.c_str(), &buffer) != 0)
    {
        std::cout << "ERROR: Paraver tracefile does not exists." << std::endl;
        return 1;
    }
    if (stat (paraver_pcf.c_str(), &buffer) != 0)
    {
        std::cout << "ERROR: Paraver configuration file does not exists." 
            << std::endl;
        return 1;
    }

    mainWindow->run(this->tracefile, paraver_pcf, 
            this->eps, this->minPts,
            this->eps_tl, this->minPts_tl,
            this->filter_lbound);

    return true;
}


void StdectApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);
    parser.SetDesc (g_cmdLineDesc);
    // must refuse '/' as parameter starter or cannot use "/path" style paths
    parser.SetSwitchChars (wxT("-"));
}
 
bool StdectApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    wxApp::OnCmdLineParsed(parser);
    for (wxCmdLineArgs::const_iterator it=parser.GetArguments().begin();
                                   it!=parser.GetArguments().end();
                                   ++it)
    {
        if (it->GetShortName().IsSameAs("t"))
            this->tracefile = it->GetStrVal();
        else if (it->GetShortName().IsSameAs("b"))
            this->filter_lbound = it->GetLongVal();
        else if (it->GetShortName().IsSameAs("e"))
            this->eps = it->GetLongVal();
        else if (it->GetShortName().IsSameAs("m"))
            this->minPts = it->GetLongVal();
        else if (it->GetShortName().IsSameAs("f"))
            this->eps_tl = it->GetLongVal();
        else if (it->GetShortName().IsSameAs("g"))
            this->minPts_tl = it->GetLongVal();
    }
 
    return true;
}

/*
 * Cleanup for StdectApp
 */

int StdectApp::OnExit()
{    
////@begin StdectApp cleanup
	return wxApp::OnExit();
////@end StdectApp cleanup
}

