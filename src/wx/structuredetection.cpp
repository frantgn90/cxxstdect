/////////////////////////////////////////////////////////////////////////////
// Name:        structuredetection.cpp
// Purpose:     
// Author:      Juan Francisco Martínez Vera
// Modified by: 
// Created:     Wed 23 May 2018 11:58:08 CEST
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
#include "wx/dataview.h"
////@end includes

#include "structuredetection.h"

////@begin XPM images
////@end XPM images


/*
 * Structuredetection type definition
 */

IMPLEMENT_CLASS( Structuredetection, wxFrame )


/*
 * Structuredetection event table definition
 */

BEGIN_EVENT_TABLE( Structuredetection, wxFrame )

////@begin Structuredetection event table entries
////@end Structuredetection event table entries

END_EVENT_TABLE()


/*
 * Structuredetection constructors
 */

Structuredetection::Structuredetection()
{
    Init();
}

Structuredetection::Structuredetection( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * Structuredetection creator
 */

bool Structuredetection::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin Structuredetection creation
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    Centre();

    if (FindWindow(ID_SPLITTERWINDOW))
        ((wxSplitterWindow*) FindWindow(ID_SPLITTERWINDOW))->SetSashPosition(50);
    if (FindWindow(ID_SPLITTERWINDOW1))
        ((wxSplitterWindow*) FindWindow(ID_SPLITTERWINDOW1))->SetSashPosition(50);
////@end Structuredetection creation
    return true;
}


/*
 * Structuredetection destructor
 */

Structuredetection::~Structuredetection()
{
////@begin Structuredetection destruction
////@end Structuredetection destruction
}


/*
 * Member initialisation
 */

void Structuredetection::Init()
{
////@begin Structuredetection member initialisation
////@end Structuredetection member initialisation
}


/*
 * Control creation for Structuredetection
 */

void Structuredetection::CreateControls()
{    
////@begin Structuredetection content construction
    Structuredetection* itemFrame1 = this;

    wxSplitterWindow* itemSplitterWindow2 = new wxSplitterWindow( itemFrame1, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    itemSplitterWindow2->SetMinimumPaneSize(0);
    itemSplitterWindow2->SetSashGravity(1);

    itemControl3 = new wxDataViewCtrl( itemSplitterWindow2,ID_TREECTRL,wxDefaultPosition,wxDefaultSize,0 );
    itemControl3->SetName(wxT("code_structure"));
    itemControl3->SetBackgroundColour(wxColour(255, 255, 255));

    wxSplitterWindow* itemSplitterWindow4 = new wxSplitterWindow( itemSplitterWindow2, ID_SPLITTERWINDOW1, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    itemSplitterWindow4->SetMinimumPaneSize(0);

    wxStaticBox* itemStaticBox5 = new wxStaticBox( itemSplitterWindow4, wxID_STATIC, _("Controls"), wxDefaultPosition, wxDefaultSize, 0 );

    wxTextCtrl* itemTextCtrl6 = new wxTextCtrl( itemSplitterWindow4, ID_TEXTCTRL, _("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque ac lobortis lorem. Nam pretium pellentesque tellus non commodo. Nam tristique tristique elit. Aenean auctor purus non vulputate maximus. Proin sed lacinia dolor. Aliquam elit enim, semper vitae faucibus ac, rhoncus eu velit. Ut iaculis sollicitudin nunc nec rhoncus. Sed in posuere mauris. Maecenas lobortis, eros sit amet auctor cursus, leo nibh euismod purus, eget aliquam turpis enim vel tellus. Nulla a mi at turpis aliquam varius. Nulla in interdum diam. Nullam finibus, dolor id ullamcorper eleifend, quam arcu consequat tellus, commodo pellentesque justo urna quis tellus. Donec lobortis porta lacus id porttitor. In eu interdum lacus. Vestibulum erat augue, pulvinar quis libero vitae, sollicitudin sollicitudin ante. "), wxDefaultPosition, wxSize(-1, 50), wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL|wxTE_WORDWRAP );

    itemSplitterWindow4->SplitHorizontally(itemStaticBox5, itemTextCtrl6, 50);
    itemSplitterWindow2->SplitVertically(itemControl3, itemSplitterWindow4, 50);

////@end Structuredetection content construction
}


/*
 * Should we show tooltips?
 */

bool Structuredetection::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap Structuredetection::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin Structuredetection bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end Structuredetection bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon Structuredetection::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin Structuredetection icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end Structuredetection icon retrieval
}
