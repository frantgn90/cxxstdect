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
    EVT_MENU( ID_TOOL2, Structuredetection::OnUnfoldButtonClick )
    EVT_MENU( ID_TOOL3, Structuredetection::OnFoldButtonClick )
    EVT_MENU( ID_TOOL, Structuredetection::OnToolClick )
    EVT_MENU( ID_TOOL1, Structuredetection::OnInfoButtonClick )
    EVT_MENU( wxID_EXIT, Structuredetection::OnExitClick )
    EVT_UPDATE_UI( ID_TREECTRL, Structuredetection::OnTreectrlUpdate )
    EVT_CHECKBOX( ID_CHECKBOX, Structuredetection::OnCheckboxClick )
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

    if (FindWindow(ID_SASHWINDOW))
        ((wxSplitterWindow*) FindWindow(ID_SASHWINDOW))->SetSashPosition(-170);
    if (FindWindow(ID_SPLITTERWINDOW))
        ((wxSplitterWindow*) FindWindow(ID_SPLITTERWINDOW))->SetSashPosition(-70);
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
    status_bar = NULL;
    main_hsplit = NULL;
    main_split = NULL;
    dataviewtree_pseudocode = NULL;
    config_panel = NULL;
    info_panel = NULL;
////@end Structuredetection member initialisation
}


/*
 * Control creation for Structuredetection
 */

void Structuredetection::CreateControls()
{    
////@begin Structuredetection content construction
    Structuredetection* itemFrame1 = this;

    status_bar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP|wxNO_BORDER );
    status_bar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(status_bar);

    wxToolBar* itemToolBar3 = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, ID_TOOLBAR );
    wxBitmap itemtool4Bitmap(itemFrame1->GetBitmapResource(wxT("../../../../../Desktop/unfold_more_32.png")));
    wxBitmap itemtool4BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL2, wxEmptyString, itemtool4Bitmap, itemtool4BitmapDisabled, wxITEM_NORMAL, _("Unfold more"), _("Unfold more"));
    wxBitmap itemtool5Bitmap(itemFrame1->GetBitmapResource(wxT("../../../../../Desktop/unfold_less_32.png")));
    wxBitmap itemtool5BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL3, wxEmptyString, itemtool5Bitmap, itemtool5BitmapDisabled, wxITEM_NORMAL, _("Unfold less"), _("Unfold less"));
    wxBitmap itemtool6Bitmap(itemFrame1->GetBitmapResource(wxT("../../../../../Desktop/filter_32.png")));
    wxBitmap itemtool6BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL, wxEmptyString, itemtool6Bitmap, itemtool6BitmapDisabled, wxITEM_NORMAL, _("Show configuration panel"), _("Show configuration panel"));
    wxBitmap itemtool7Bitmap(itemFrame1->GetBitmapResource(wxT("../../../../../Desktop/info_32.png")));
    wxBitmap itemtool7BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL1, wxEmptyString, itemtool7Bitmap, itemtool7BitmapDisabled, wxITEM_NORMAL, _("Show information panel"), _("Show information panel"));
    wxBitmap itemtool8Bitmap(itemFrame1->GetBitmapResource(wxT("../../../../../Desktop/quit_exit_32.png")));
    wxBitmap itemtool8BitmapDisabled;
    itemToolBar3->AddTool(wxID_EXIT, wxEmptyString, itemtool8Bitmap, itemtool8BitmapDisabled, wxITEM_NORMAL, _("Exit program"), _("Exit program"));
    itemToolBar3->Realize();
    itemFrame1->SetToolBar(itemToolBar3);

    main_hsplit = new wxSplitterWindow( itemFrame1, ID_SASHWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    main_hsplit->SetMinimumPaneSize(0);

    main_split = new wxSplitterWindow( main_hsplit, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    main_split->SetMinimumPaneSize(0);
    main_split->SetSashGravity(1);

    dataviewtree_pseudocode = new wxDataViewCtrl( main_split,ID_TREECTRL,wxDefaultPosition,wxDefaultSize,wxDV_ROW_LINES | wxDV_VERT_RULES );
    dataviewtree_pseudocode->SetName(wxT("code_structure"));
    dataviewtree_pseudocode->SetBackgroundColour(wxColour(255, 255, 255));

    config_panel = new wxPanel( main_split, ID_PANEL, wxDefaultPosition, wxSize(-1, 20), wxNO_BORDER|wxTAB_TRAVERSAL );
    config_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticBox* itemStaticBoxSizer13Static = new wxStaticBox(config_panel, wxID_ANY, _("Filter panel"));
    wxStaticBoxSizer* itemStaticBoxSizer13 = new wxStaticBoxSizer(itemStaticBoxSizer13Static, wxHORIZONTAL);
    config_panel->SetSizer(itemStaticBoxSizer13);

    wxStaticText* itemStaticText14 = new wxStaticText( itemStaticBoxSizer13->GetStaticBox(), wxID_STATIC, _("Rank filter"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer13->Add(itemStaticText14, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* itemTextCtrl15 = new wxTextCtrl( itemStaticBoxSizer13->GetStaticBox(), ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer13->Add(itemTextCtrl15, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine16 = new wxStaticLine( itemStaticBoxSizer13->GetStaticBox(), wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemStaticBoxSizer13->Add(itemStaticLine16, 0, wxGROW|wxALL, 5);

    wxCheckBox* itemCheckBox17 = new wxCheckBox( itemStaticBoxSizer13->GetStaticBox(), ID_CHECKBOX, _("Show CPU bursts"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemCheckBox17->SetValue(false);
    itemStaticBoxSizer13->Add(itemCheckBox17, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxTextCtrl* itemTextCtrl18 = new wxTextCtrl( itemStaticBoxSizer13->GetStaticBox(), ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemTextCtrl18->Enable(false);
    itemStaticBoxSizer13->Add(itemTextCtrl18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine19 = new wxStaticLine( itemStaticBoxSizer13->GetStaticBox(), wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
    itemStaticBoxSizer13->Add(itemStaticLine19, 0, wxGROW|wxALL, 5);

    wxButton* itemButton20 = new wxButton( itemStaticBoxSizer13->GetStaticBox(), ID_BUTTON, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticBoxSizer13->Add(itemButton20, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    main_split->SplitHorizontally(dataviewtree_pseudocode, config_panel, -70);
    info_panel = new wxPanel( main_hsplit, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    info_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxStaticBox* itemStaticBoxSizer22Static = new wxStaticBox(info_panel, wxID_ANY, _("Information panel"));
    wxStaticBoxSizer* itemStaticBoxSizer22 = new wxStaticBoxSizer(itemStaticBoxSizer22Static, wxHORIZONTAL);
    info_panel->SetSizer(itemStaticBoxSizer22);

    wxStaticBox* itemStaticBoxSizer23Static = new wxStaticBox(itemStaticBoxSizer22->GetStaticBox(), wxID_ANY, _("General info"));
    wxStaticBoxSizer* itemStaticBoxSizer23 = new wxStaticBoxSizer(itemStaticBoxSizer23Static, wxVERTICAL);
    itemStaticBoxSizer22->Add(itemStaticBoxSizer23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer23->Add(itemBoxSizer24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText25 = new wxStaticText( itemStaticBoxSizer23->GetStaticBox(), wxID_STATIC, _("N. Phases:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer24->Add(itemStaticText25, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText26 = new wxStaticText( itemStaticBoxSizer23->GetStaticBox(), wxID_STATIC, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText26->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer24->Add(itemStaticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer27 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer23->Add(itemBoxSizer27, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText28 = new wxStaticText( itemStaticBoxSizer23->GetStaticBox(), wxID_STATIC, _("Deltas:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer27->Add(itemStaticText28, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemStaticBoxSizer23->GetStaticBox(), wxID_STATIC, _("0,32; 0,4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText29->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer27->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer30Static = new wxStaticBox(itemStaticBoxSizer22->GetStaticBox(), wxID_ANY, _("Selection info"));
    wxStaticBoxSizer* itemStaticBoxSizer30 = new wxStaticBoxSizer(itemStaticBoxSizer30Static, wxVERTICAL);
    itemStaticBoxSizer22->Add(itemStaticBoxSizer30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer30->Add(itemBoxSizer31, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText32 = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("Repetitions:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer31->Add(itemStaticText32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText33 = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("200"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText33->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer31->Add(itemStaticText33, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer30->Add(itemBoxSizer34, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxStaticText* itemStaticText35 = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("MPI Ranks"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer34->Add(itemStaticText35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText36 = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("0,1,2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStaticText36->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer34->Add(itemStaticText36, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    main_hsplit->SplitHorizontally(main_split, info_panel, -170);

////@end Structuredetection content construction

    status_bar->SetStatusText("Executing analysis...", 0);
    status_bar->SetStatusText("Done", 1);
    
    // Initally filter panel should not be shown
    this->main_hsplit->Unsplit(NULL);
    this->main_split->Unsplit(NULL);
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
    if (name == wxT("../../../../../Desktop/unfold_more_32.png"))
    {
        wxBitmap bitmap(wxT("../../../../../Desktop/unfold_more_32.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../../../../../Desktop/unfold_less_32.png"))
    {
        wxBitmap bitmap(wxT("../../../../../Desktop/unfold_less_32.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../../../../../Desktop/filter_32.png"))
    {
        wxBitmap bitmap(wxT("../../../../../Desktop/filter_32.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../../../../../Desktop/info_32.png"))
    {
        wxBitmap bitmap(wxT("../../../../../Desktop/info_32.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
    else if (name == wxT("../../../../../Desktop/quit_exit_32.png"))
    {
        wxBitmap bitmap(wxT("../../../../../Desktop/quit_exit_32.png"), wxBITMAP_TYPE_PNG);
        return bitmap;
    }
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


/*
 * wxEVT_UPDATE_UI event handler for ID_TREECTRL
 */

void Structuredetection::OnTreectrlUpdate( wxUpdateUIEvent& event )
{
    //std::cout << "CC:" << this->dataviewtree_pseudocode->GetColumnCount() << std::endl;
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
 */

void Structuredetection::OnExitClick( wxCommandEvent& event )
{
    this->Close();
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL
 */

void Structuredetection::OnToolClick( wxCommandEvent& event )
{
    if (this->main_split->IsSplit())
        this->main_split->Unsplit(NULL);
    else
        this->main_split->SplitHorizontally(
            this->dataviewtree_pseudocode, 
            this->config_panel, -70);
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL2
 */

void Structuredetection::OnUnfoldButtonClick( wxCommandEvent& event )
{
    wxDataViewItemArray roots;
    this->dataviewtree_pseudocode->GetModel()->GetChildren(wxDataViewItem(0), roots);
    for (auto it : roots)
        this->dataviewtree_pseudocode->Expand(it);
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL3
 */

void Structuredetection::OnFoldButtonClick( wxCommandEvent& event )
{
    wxDataViewItemArray roots;
    this->dataviewtree_pseudocode->GetModel()->GetChildren(wxDataViewItem(0), roots);
    for (auto it : roots)
        this->dataviewtree_pseudocode->Collapse(it);
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL1
 */

void Structuredetection::OnInfoButtonClick( wxCommandEvent& event )
{
    if (this->main_hsplit->IsSplit())
        this->main_hsplit->Unsplit(NULL);
    else
        this->main_hsplit->SplitHorizontally(
            this->main_split, 
            this->info_panel, -120);
}


/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
 */

void Structuredetection::OnCheckboxClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX in Structuredetection.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX in Structuredetection. 
}

