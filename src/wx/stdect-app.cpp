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

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "stdect-app.h"

////@begin XPM images
////@end XPM images


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

