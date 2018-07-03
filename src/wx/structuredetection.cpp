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

// Some utils
#include <TraceHeader.h>


// Include workflow phases
#include <pipeline.h>
#include <nptrace.h>
#include <reducer.h>
#include <loops_identification.h>
#include <loops_merge.h>
#include <pseudocode.h>

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

// Loading dialog
#include "loadingdialog.h"
#include "structuredetection.h"

////@begin XPM images
#include "../../../paraver/git/wxparaver/icons/file_browser.xpm"
#include "../../imgs/derived_add.xpm"
#include "../../../paraver/git/wxparaver/icons/derived_substract.xpm"
#include "../../../paraver/git/wxparaver/icons/cut_trace.xpm"
#include "../../../paraver/git/wxparaver/icons/histo_zoom.xpm"
#include "../../imgs/timeline.xpm"
#include "../../../paraver/git/wxparaver/icons/new_window.xpm"
#include "../../../paraver/git/wxparaver/icons/delete.xpm"
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
    EVT_MENU( ID_TOOL6, Structuredetection::OnOpenTraceClick )
    EVT_MENU( ID_TOOL2, Structuredetection::OnUnfoldButtonClick )
    EVT_MENU( ID_TOOL3, Structuredetection::OnFoldButtonClick )
    EVT_MENU( ID_TOOL, Structuredetection::OnToolClick )
    EVT_MENU( ID_TOOL1, Structuredetection::OnInfoButtonClick )
    EVT_MENU( ID_TOOL5, Structuredetection::OnShowClusteringButtonClick )
    EVT_MENU( wxID_EXIT, Structuredetection::OnExitClick )
    EVT_UPDATE_UI( ID_TREECTRL, Structuredetection::OnTreectrlUpdate )
    EVT_CHECKBOX( ID_CHECKBOX, Structuredetection::OnShowComputationsCheckboxClick )
    EVT_COMBOBOX( ID_COMBOCTRL, Structuredetection::OnHWCComboctrlSelected )
    EVT_BUTTON( ID_BUTTON, Structuredetection::OnFilterButtonClick )
////@end Structuredetection event table entries
    EVT_DATAVIEW_SELECTION_CHANGED( ID_TREECTRL, Structuredetection::OnItemSelection)
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
    trace_file_path_text = NULL;
    main_hsplit = NULL;
    main_split = NULL;
    dataviewtree_pseudocode = NULL;
    config_panel = NULL;
    burst_threshold_text = NULL;
    hwc_combobox = NULL;
    rank_filter_text = NULL;
    info_panel = NULL;
    general_nphases_label = NULL;
    general_deltas_label = NULL;
    callpath_panel = NULL;
    selected_item_callpath = NULL;
    legend_panel = NULL;
    legend_panel_sizer = NULL;
////@end Structuredetection member initialisation
}


/*
 * Control creation for Structuredetection
 */

void Structuredetection::CreateControls()
{    
////@begin Structuredetection content construction
    Structuredetection* itemFrame1 = this;

    status_bar = new wxStatusBar( itemFrame1, ID_STATUSBAR, wxST_SIZEGRIP );
    status_bar->Show(false);
    status_bar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(status_bar);

    wxToolBar* itemToolBar3 = CreateToolBar( wxTB_FLAT|wxTB_VERTICAL, ID_TOOLBAR );
    wxBitmap itemtool4Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/file_browser.xpm")));
    wxBitmap itemtool4BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL6, wxEmptyString, itemtool4Bitmap, itemtool4BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    wxStaticLine* itemStaticLine5 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine5);
    wxBitmap itemtool6Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/derived_add.xpm")));
    wxBitmap itemtool6BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL2, wxEmptyString, itemtool6Bitmap, itemtool6BitmapDisabled, wxITEM_NORMAL, _("Unfold more"), _("Unfold more"));
    wxBitmap itemtool7Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/derived_substract.xpm")));
    wxBitmap itemtool7BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL3, wxEmptyString, itemtool7Bitmap, itemtool7BitmapDisabled, wxITEM_NORMAL, _("Unfold less"), _("Unfold less"));
    wxBitmap itemtool8Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/cut_trace.xpm")));
    wxBitmap itemtool8BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL, wxEmptyString, itemtool8Bitmap, itemtool8BitmapDisabled, wxITEM_NORMAL, _("Show configuration panel"), _("Show configuration panel"));
    wxBitmap itemtool9Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/histo_zoom.xpm")));
    wxBitmap itemtool9BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL1, wxEmptyString, itemtool9Bitmap, itemtool9BitmapDisabled, wxITEM_NORMAL, _("Show information panel"), _("Show information panel"));
    wxStaticLine* itemStaticLine10 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine10);
    wxBitmap itemtool11Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/timeline.xpm")));
    wxBitmap itemtool11BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL4, _("Launch paraver"), itemtool11Bitmap, itemtool11BitmapDisabled, wxITEM_NORMAL, wxEmptyString, _("Launch paraver"));
    wxBitmap itemtool12Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/new_window.xpm")));
    wxBitmap itemtool12BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL5, _("Show clustering"), itemtool12Bitmap, itemtool12BitmapDisabled, wxITEM_NORMAL, wxEmptyString, _("Show clustering"));
    wxStaticLine* itemStaticLine13 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine13);
    wxBitmap itemtool14Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/delete.xpm")));
    wxBitmap itemtool14BitmapDisabled;
    itemToolBar3->AddTool(wxID_EXIT, wxEmptyString, itemtool14Bitmap, itemtool14BitmapDisabled, wxITEM_NORMAL, _("Exit program"), _("Exit program"));
    itemToolBar3->Realize();
    itemFrame1->SetToolBar(itemToolBar3);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer15);

    trace_file_path_text = new wxTextCtrl( itemFrame1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(trace_file_path_text, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 3);

    main_hsplit = new wxSplitterWindow( itemFrame1, ID_SASHWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_BORDER|wxSP_3DSASH|wxDOUBLE_BORDER );
    main_hsplit->SetMinimumPaneSize(0);

    main_split = new wxSplitterWindow( main_hsplit, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    main_split->SetMinimumPaneSize(0);
    main_split->SetSashGravity(1);

    dataviewtree_pseudocode = new wxDataViewCtrl( main_split,ID_TREECTRL,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxHSCROLL|wxDV_ROW_LINES );
    dataviewtree_pseudocode->SetName(wxT("code_structure"));
    dataviewtree_pseudocode->SetBackgroundColour(wxColour(255, 255, 255));

    config_panel = new wxScrolledWindow( main_split, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxHSCROLL|wxTAB_TRAVERSAL );
    config_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    config_panel->SetScrollbars(1, 1, 0, 0);
    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxHORIZONTAL);
    config_panel->SetSizer(itemBoxSizer21);

    wxCheckBox* itemCheckBox22 = new wxCheckBox( config_panel, ID_CHECKBOX, _("Show CPU bursts"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemCheckBox22->SetValue(false);
    itemBoxSizer21->Add(itemCheckBox22, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( config_panel, wxID_STATIC, _("T >"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(itemStaticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    burst_threshold_text = new wxTextCtrl( config_panel, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    burst_threshold_text->Enable(false);
    itemBoxSizer21->Add(burst_threshold_text, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine25 = new wxStaticLine( config_panel, wxID_STATIC, wxDefaultPosition, wxSize(10, -1), wxLI_VERTICAL );
    itemBoxSizer21->Add(itemStaticLine25, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText26 = new wxStaticText( config_panel, wxID_STATIC, _("HWC"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(itemStaticText26, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString hwc_comboboxStrings;
    hwc_combobox = new wxComboBox( config_panel, ID_COMBOCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, hwc_comboboxStrings, wxCB_DROPDOWN );
    itemBoxSizer21->Add(hwc_combobox, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine28 = new wxStaticLine( config_panel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer21->Add(itemStaticLine28, 0, wxGROW|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( config_panel, wxID_STATIC, _("Rank filter"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    rank_filter_text = new wxTextCtrl( config_panel, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(rank_filter_text, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine31 = new wxStaticLine( config_panel, wxID_STATIC, wxDefaultPosition, wxSize(10, -1), wxLI_VERTICAL );
    itemBoxSizer21->Add(itemStaticLine31, 0, wxGROW|wxALL, 5);

    wxButton* itemButton32 = new wxButton( config_panel, ID_BUTTON, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer21->Add(itemButton32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    config_panel->FitInside();
    main_split->SplitHorizontally(dataviewtree_pseudocode, config_panel, -70);
    info_panel = new wxPanel( main_hsplit, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxHSCROLL|wxTAB_TRAVERSAL );
    info_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxVERTICAL);
    info_panel->SetSizer(itemBoxSizer34);

    wxBoxSizer* itemBoxSizer35 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer34->Add(itemBoxSizer35, 1, wxGROW|wxALL, 5);
    wxStaticBox* itemStaticBoxSizer36Static = new wxStaticBox(info_panel, wxID_ANY, _("General info"));
    wxStaticBoxSizer* itemStaticBoxSizer36 = new wxStaticBoxSizer(itemStaticBoxSizer36Static, wxHORIZONTAL);
    itemBoxSizer35->Add(itemStaticBoxSizer36, 0, wxALIGN_TOP|wxALL, 5);
    wxBoxSizer* itemBoxSizer37 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer36->Add(itemBoxSizer37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText38 = new wxStaticText( itemStaticBoxSizer36->GetStaticBox(), wxID_STATIC, _("N. Phases:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer37->Add(itemStaticText38, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    general_nphases_label = new wxStaticText( itemStaticBoxSizer36->GetStaticBox(), wxID_STATIC, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    general_nphases_label->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer37->Add(general_nphases_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer40 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer36->Add(itemBoxSizer40, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText41 = new wxStaticText( itemStaticBoxSizer36->GetStaticBox(), wxID_STATIC, _("Deltas:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer40->Add(itemStaticText41, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    general_deltas_label = new wxStaticText( itemStaticBoxSizer36->GetStaticBox(), wxID_STATIC, _("0,32; 0,4"), wxDefaultPosition, wxDefaultSize, 0 );
    general_deltas_label->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer40->Add(general_deltas_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer43Static = new wxStaticBox(info_panel, wxID_ANY, _("Selection callstack"));
    wxStaticBoxSizer* itemStaticBoxSizer43 = new wxStaticBoxSizer(itemStaticBoxSizer43Static, wxVERTICAL);
    itemBoxSizer35->Add(itemStaticBoxSizer43, 1, wxGROW|wxALL, 5);
    callpath_panel = new wxPanel( itemStaticBoxSizer43->GetStaticBox(), ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    callpath_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    itemStaticBoxSizer43->Add(callpath_panel, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
    selected_item_callpath = new wxBoxSizer(wxVERTICAL);
    callpath_panel->SetSizer(selected_item_callpath);

    main_hsplit->SplitHorizontally(main_split, info_panel, -170);
    itemBoxSizer15->Add(main_hsplit, 1, wxGROW|wxALL, 5);

    legend_panel = new wxPanel( itemFrame1, ID_PANEL2, wxDefaultPosition, wxSize(-1, 25), wxNO_BORDER|wxTAB_TRAVERSAL );
    legend_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    itemBoxSizer15->Add(legend_panel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    legend_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    legend_panel->SetSizer(legend_panel_sizer);

////@end Structuredetection content construction

    //status_bar->SetStatusText("Executing analysis...", 0);
    //status_bar->SetStatusText("Done", 1);
    
    // Initally filter panel should not be shown
    this->main_hsplit->Unsplit(NULL);
    this->main_split->Unsplit(NULL);

    // Adding columns to dataviewctrl
    wxDataViewTextRenderer *tr = new wxDataViewTextRenderer();
    tr->EnableMarkup();
    wxDataViewColumn *column0 =
        new wxDataViewColumn( "Pseudocode", tr, 0, 200, wxALIGN_LEFT,
                wxDATAVIEW_COL_RESIZABLE );
    wxDataViewTextRenderer *tr1 =
        new wxDataViewTextRenderer( "double", wxDATAVIEW_CELL_INERT );
    wxDataViewTextRenderer *tr2 =
        new wxDataViewTextRenderer( "long", wxDATAVIEW_CELL_INERT );
    wxDataViewTextRenderer *tr3 =
        new wxDataViewTextRenderer( "long", wxDATAVIEW_CELL_INERT );


    wxDataViewColumn *column1 =
        new wxDataViewColumn( "Duration", tr2, 1, 125, wxALIGN_RIGHT,
                wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *column2 =
        new wxDataViewColumn( "Msg. Size", tr3, 2, 125, wxALIGN_RIGHT,
                wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *column3 =
        new wxDataViewColumn( "HWC", tr1, 3, 125, wxALIGN_RIGHT,
                wxDATAVIEW_COL_RESIZABLE );

    dataviewtree_pseudocode->AppendColumn(column0);
    dataviewtree_pseudocode->AppendColumn(column1);
    dataviewtree_pseudocode->AppendColumn(column2);
    dataviewtree_pseudocode->AppendColumn(column3);
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
    if (name == wxT("../../../paraver/git/wxparaver/icons/file_browser.xpm"))
    {
        wxBitmap bitmap(file_browser_xpm);
        return bitmap;
    }
    else if (name == wxT("../../imgs/derived_add.xpm"))
    {
        wxBitmap bitmap(derived_add_xpm);
        return bitmap;
    }
    else if (name == wxT("../../../paraver/git/wxparaver/icons/derived_substract.xpm"))
    {
        wxBitmap bitmap(derived_substract_xpm);
        return bitmap;
    }
    else if (name == wxT("../../../paraver/git/wxparaver/icons/cut_trace.xpm"))
    {
        wxBitmap bitmap(cut_trace_xpm);
        return bitmap;
    }
    else if (name == wxT("../../../paraver/git/wxparaver/icons/histo_zoom.xpm"))
    {
        wxBitmap bitmap(histo_zoom_xpm);
        return bitmap;
    }
    else if (name == wxT("../../imgs/timeline.xpm"))
    {
        wxBitmap bitmap(timeline_xpm);
        return bitmap;
    }
    else if (name == wxT("../../../paraver/git/wxparaver/icons/new_window.xpm"))
    {
        wxBitmap bitmap(application_star_xpm);
        return bitmap;
    }
    else if (name == wxT("../../../paraver/git/wxparaver/icons/delete.xpm"))
    {
        wxBitmap bitmap(delete_xpm);
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
    wxDataViewItemArray new_last_level_items;
    for (auto it : last_level_items)
    {
        this->dataviewtree_pseudocode->Expand(it);
        wxDataViewItemArray tmp_array;
        this->dataviewtree_pseudocode->GetModel()
            ->GetChildren(it, tmp_array);
        for (auto it2 : tmp_array)
        {
            if (this->dataviewtree_pseudocode->GetModel()->IsContainer(it2))
                    new_last_level_items.Add(it2);
        }
    }
    if (new_last_level_items.size() > 0)
        this->last_level_items = new_last_level_items;
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL3
 */

void Structuredetection::OnFoldButtonClick( wxCommandEvent& event )
{
    wxDataViewItemArray new_last_level_items;
    for (auto it : last_level_items)
    {
        this->dataviewtree_pseudocode->Collapse(it);
        wxDataViewItem parent = this->dataviewtree_pseudocode->GetModel()
            ->GetParent(it);
        bool exits = std::find(
                new_last_level_items.begin(), new_last_level_items.end(), parent)
            != new_last_level_items.end();
        
        if (!exits)
        {
            new_last_level_items.Add(parent);
        }
    }
    if (new_last_level_items.size() > 0)
        this->last_level_items = new_last_level_items;
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
            this->info_panel, -140);
}


/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
 */

void Structuredetection::OnShowComputationsCheckboxClick( wxCommandEvent& event )
{
    this->burst_threshold_text->Enable(event.IsChecked());
}

void Structuredetection::addLegendItem(std::vector<unsigned int>* ranks, 
        std::string rgb)
{
    //unsigned int color = std::stoul(rgb, nullptr, 16);
    std::string red = rgb.substr(0,2);
    std::string green = rgb.substr(2,2);
    std::string blue = rgb.substr(4,2);

    unsigned int vred = (unsigned int) std::stoul(red, nullptr, 16);
    unsigned int vgreen = (unsigned int) std::stoul(green, nullptr, 16);
    unsigned int vblue = (unsigned int) std::stoul(blue, nullptr, 16);

    std::string legend_text("");
    std::sort(ranks->begin(), ranks->end());

    std::vector<std::pair<unsigned int, unsigned int>> ranges;
    ranges.push_back(std::make_pair(ranks->at(0), ranks->at(0)));
    
    for(int i=1; i<ranks->size(); ++i)
    {
        unsigned int rank = ranks->at(i);
        if (rank-1 == ranges.back().second)
            ranges.back().second = rank;
        else
            ranges.push_back(std::make_pair(rank, rank));
    }

    for (auto range : ranges)
    {
        if (range.first == range.second)
            legend_text += std::to_string(range.first);
        else
            legend_text += std::to_string(range.first)+"-"
                + std::to_string(range.second);
        legend_text += " ";
    }

    wxSize tmpSize( 12, 12 );
    wxPanel* item_color = new wxPanel(
            this->legend_panel,
            wxID_ANY, wxDefaultPosition, tmpSize);
    //wxColour tmpColor(color);
    wxColour tmpColor(vred,vgreen,vblue);
    item_color->SetBackgroundColour(tmpColor);

    wxStaticText* new_legend_item = new wxStaticText( 
            this->legend_panel, 
            wxID_STATIC, _(legend_text), 
            wxDefaultPosition, wxDefaultSize, 0 );

    legend_panel_sizer->Add(item_color, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    legend_panel_sizer->Add(new_legend_item, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
}

void Structuredetection::OnItemSelection(wxDataViewEvent& event)
{
    if (event.GetItem().IsOk())
    {
        wxPseudocodeItem *item = static_cast<wxPseudocodeItem*>(
                event.GetItem().GetID());
        GUIRepresentation* obj = item->getActualObject();

        
        selected_item_callpath->Clear(true);
        std::vector<Caller> callpath = obj->getCallpath();
        for (auto it : callpath)
        {
            unsigned int line = std::get<0>(it);
            std::string file = std::get<1>(it);
            std::string caller = std::get<2>(it);

            std::string caller_text = caller + " [" + file + ":" 
                + std::to_string(line) + "]";
            wxStaticText* new_callpath_item = new wxStaticText( 
                    this->callpath_panel, 
                    wxID_STATIC, _(caller_text), 
                    wxDefaultPosition, wxDefaultSize, 0 );

            new_callpath_item->SetFont(wxFont(10, wxFONTFAMILY_SWISS, 
                        wxFONTSTYLE_NORMAL, 
                        wxFONTWEIGHT_BOLD, false, wxT("Sans")));
            new_callpath_item->CenterOnParent();
            selected_item_callpath->Add(new_callpath_item, 0, wxALL, 5);
        }
        this->callpath_panel->Layout();
    }
}

void Structuredetection::setGeneralInfo(unsigned int nphases,
        std::vector<unsigned int> deltas)
{
    this->general_nphases_label->SetLabel(std::to_string(nphases));
    std::string deltas_str;
    for (auto it : deltas)
        deltas_str += std::to_string(it) + "% ";
    this->general_deltas_label->SetLabel(deltas_str);
}

/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
 */

void Structuredetection::OnFilterButtonClick( wxCommandEvent& event )
{
    std::string rank_filter(this->rank_filter_text->GetLineText(0));
    std::string delimiter(",");

    size_t pos = 0;
    size_t last_pos = 0;
    std::vector<unsigned int> ranks;
    if (rank_filter.size() > 0)
    {
        while (1) 
        {
            pos = rank_filter.find(delimiter, last_pos);
            std::string token = rank_filter.substr(last_pos, pos-last_pos);
            ranks.push_back(std::stoi(token));

            last_pos = pos+1;
            if (pos == std::string::npos)
                break;
        }
    }

    wxPseudocode* mm = static_cast<wxPseudocode*>(dataviewtree_pseudocode
            ->GetModel());

    if (this->burst_threshold_text->IsEnabled())
    {
        std::string th(this->burst_threshold_text->GetLineText(0));
        mm->showComputations(true);
        mm->setComputationsThresshold(std::atoll(th.c_str()));
    }
    else
        mm->showComputations(false);

    std::string hwc_type_name(std::string(this->hwc_combobox->GetString(
                    this->hwc_combobox->GetSelection())));
    mm->setHWCColumnType(hwc_type_name);

    // Configure model
    mm->setRanksFilter(ranks);
    mm->Cleared();
    dataviewtree_pseudocode->AssociateModel((wxDataViewModel*) mm);
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL6
 */

void Structuredetection::OnOpenTraceClick( wxCommandEvent& event )
{
    std::string path = this->tracefile.substr(0, 
            this->tracefile.find_last_of("/"));
    wxFileDialog dialog( this, _( "Load Trace" ), path, _( "" ), 
            _( "Paraver trace (*.prv;*.prv.gz)|*.prv;*.prv.gz|All files (*.*)|*.*" ),
            wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR );

    if( dialog.ShowModal() == wxID_OK )
    {
        wxString path = dialog.GetPath();
        this->run(std::string(path));
    }
}


void Structuredetection::SetAssociateModel(wxDataViewModel* model)
{
    dataviewtree_pseudocode->AssociateModel(model);
    this->dataviewtree_pseudocode->GetModel()->GetChildren(
            wxDataViewItem(0), 
            last_level_items);
}

void Structuredetection::run(std::string tracefile)
{
    std::string paraver_pcf;
    size_t start_pos = tracefile.find(".prv");
    if (start_pos != std::string::npos)
    {
        paraver_pcf = tracefile.substr(0, start_pos);
        paraver_pcf += ".pcf";
    }
    else
    {
        std::cout << "Incorrect tracefile: " << tracefile << std::endl;
        return;
    }
    
    std::cout << "Tracefile: " << tracefile << std::endl;
    std::cout << "Semantic file: " << paraver_pcf << std::endl;

    struct stat buffer;   
    if (stat (tracefile.c_str(), &buffer) != 0)
    {
        std::cout << "ERROR: Paraver tracefile does not exists." << std::endl;
        return;
    }
    if (stat (paraver_pcf.c_str(), &buffer) != 0)
    {
        std::cout << "ERROR: Paraver configuration file does not exists." 
            << std::endl;
        return;
    }

    this->run(tracefile, paraver_pcf,
            this->eps, this->minPts,
            this->eps_tl, this->minPts_tl,
            this->filter_lbound);
}

void Structuredetection::run(std::string tracefile, std::string paraver_pcf, 
        double eps, size_t minPts,
        double eps_tl, size_t minPts_tl,
        double filter_lbound)
{
    if (this->tracefile == tracefile)
        return;

    this->tracefile = tracefile;
    this->eps = eps;
    this->minPts = minPts;
    this->eps_tl = eps_tl;
    this->minPts_tl = minPts_tl;
    this->filter_lbound = filter_lbound;

    this->trace_file_path_text->SetValue(tracefile);

    trace_semantic.parse(paraver_pcf);

    std::vector<std::vector<unsigned int>> event_types = 
        trace_semantic.getGroupedEventTypes();

    for (auto group : event_types)
        for (unsigned int type : group)
            if (type >= 42000000 and type < 43000000)
            {
                std::string hwc_type_name = trace_semantic.getEventType(type);
                this->hwc_combobox->Append(hwc_type_name);
                // TODO : Insert to combobox
            }
    this->hwc_combobox->SetSelection(0);

    TraceHeader trace_info(tracefile);
    
    loadingDialog * loading_dialog = new loadingDialog(NULL);
    loading_dialog->setTraceName(tracefile);
    loading_dialog->Show(true);
    loading_dialog->Update();

    // Disable all windows unless loading_dialog. It reenable windows after
    // deletion of the object (at this function return).
    wxWindowDisabler disableall(loading_dialog);

    // Declare all phases
    NPTrace parser;
    Reducer reducer(trace_info.texe, filter_lbound, trace_info.ntasks);
    LoopsIdentification loops_id(eps, minPts);
    LoopsMerge loops_merge(eps_tl, minPts_tl, trace_info.texe);
    Pseudocode pseudocode(&trace_semantic);

    parser.setProgressReporter(loading_dialog);
    parser.setLogReporter(loading_dialog);
    reducer.setLogReporter(loading_dialog);
    loops_id.setLogReporter(loading_dialog);
    loops_merge.setLogReporter(loading_dialog);
    pseudocode.setLogReporter(loading_dialog);

    // Connect them
    parser.connect(&reducer);
    reducer.connect(&loops_id);
    loops_id.connect(&loops_merge);
    loops_merge.connect(&pseudocode);

    // Just run the first stage
    parser.setInput(&tracefile);
    parser.run();

    this->gnuplot_script = loops_merge.getGNUPlotScript();
    wxDataViewModel* model = (wxDataViewModel*)pseudocode.getResult();
    this->SetAssociateModel(model);
    static_cast<wxPseudocode*>(model)->setHWCColumnType(
            std::string(this->hwc_combobox->GetString(0)));

    std::vector<std::pair<std::vector<unsigned int>*, std::string>> color_map;
    color_map = pseudocode.getResult()->getColormap();

    this->legend_panel_sizer->Clear();
    for (auto legend_item : color_map )
        this->addLegendItem(legend_item.first, legend_item.second);

    this->setGeneralInfo(loops_merge.getNPhases(), loops_merge.getDeltas());

    loading_dialog->WellDone();
    loading_dialog->ShowModal(); // block the execution

    loading_dialog->Show(false);
    loading_dialog->Destroy();
}


/*
 * wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOCTRL
 */

void Structuredetection::OnHWCComboctrlSelected( wxCommandEvent& event )
{
    std::string hwc_name(event.GetString());

    wxPseudocode* mm = static_cast<wxPseudocode*>(dataviewtree_pseudocode
            ->GetModel());
    mm->setHWCColumnType(hwc_name);
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL5
 */

void Structuredetection::OnShowClusteringButtonClick( wxCommandEvent& event )
{
    pid_t pid = fork();
    if (pid == 0)
    {
        execl("/usr/bin/gnuplot", "gnuplot", 
                this->gnuplot_script.c_str(), (char*)0);
        std::cout << "Error on execl" << std::endl;
        exit(1);
    }
    std::cout << "gnuplot " << this->gnuplot_script << std::endl;
}

