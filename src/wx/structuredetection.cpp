/////////////////////////////////////////////////////////////////////////////
// Name:        structuredetection.cpp
// Purpose:     
// Author:      Juan Francisco Mart�nez Vera
// Modified by: 
// Created:     Wed 23 May 2018 11:58:08 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#include "pseudocode.h"
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
    EVT_MENU( ID_TOOL2, Structuredetection::OnUnfoldButtonClick )
    EVT_MENU( ID_TOOL3, Structuredetection::OnFoldButtonClick )
    EVT_MENU( ID_TOOL, Structuredetection::OnToolClick )
    EVT_MENU( ID_TOOL1, Structuredetection::OnInfoButtonClick )
    EVT_MENU( wxID_EXIT, Structuredetection::OnExitClick )
    EVT_UPDATE_UI( ID_TREECTRL, Structuredetection::OnTreectrlUpdate )
    EVT_CHECKBOX( ID_CHECKBOX, Structuredetection::OnCheckboxClick )
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
        ((wxSplitterWindow*) FindWindow(ID_SPLITTERWINDOW))->SetSashPosition(-35);
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
    rank_filter_text = NULL;
    burst_threshold_text = NULL;
    info_panel = NULL;
    legend_panel = NULL;
    legend_panel_sizer = NULL;
    general_nphases_label = NULL;
    general_deltas_label = NULL;
    callpath_panel = NULL;
    selected_item_callpath = NULL;
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
    status_bar->SetFieldsCount(2);
    itemFrame1->SetStatusBar(status_bar);

    wxToolBar* itemToolBar3 = CreateToolBar( wxTB_FLAT|wxTB_VERTICAL, ID_TOOLBAR );
    wxBitmap itemtool4Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/derived_add.xpm")));
    wxBitmap itemtool4BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL2, wxEmptyString, itemtool4Bitmap, itemtool4BitmapDisabled, wxITEM_NORMAL, _("Unfold more"), _("Unfold more"));
    wxBitmap itemtool5Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/derived_substract.xpm")));
    wxBitmap itemtool5BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL3, wxEmptyString, itemtool5Bitmap, itemtool5BitmapDisabled, wxITEM_NORMAL, _("Unfold less"), _("Unfold less"));
    wxBitmap itemtool6Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/cut_trace.xpm")));
    wxBitmap itemtool6BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL, wxEmptyString, itemtool6Bitmap, itemtool6BitmapDisabled, wxITEM_NORMAL, _("Show configuration panel"), _("Show configuration panel"));
    wxBitmap itemtool7Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/histo_zoom.xpm")));
    wxBitmap itemtool7BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL1, wxEmptyString, itemtool7Bitmap, itemtool7BitmapDisabled, wxITEM_NORMAL, _("Show information panel"), _("Show information panel"));
    wxStaticLine* itemStaticLine8 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine8);
    wxBitmap itemtool9Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/timeline.xpm")));
    wxBitmap itemtool9BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL4, _("Launch paraver"), itemtool9Bitmap, itemtool9BitmapDisabled, wxITEM_NORMAL, wxEmptyString, _("Launch paraver"));
    wxBitmap itemtool10Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/new_window.xpm")));
    wxBitmap itemtool10BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL5, _("Show clustering"), itemtool10Bitmap, itemtool10BitmapDisabled, wxITEM_NORMAL, wxEmptyString, _("Show clustering"));
    wxStaticLine* itemStaticLine11 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine11);
    wxBitmap itemtool12Bitmap(itemFrame1->GetBitmapResource(wxT("../../../paraver/git/wxparaver/icons/delete.xpm")));
    wxBitmap itemtool12BitmapDisabled;
    itemToolBar3->AddTool(wxID_EXIT, wxEmptyString, itemtool12Bitmap, itemtool12BitmapDisabled, wxITEM_NORMAL, _("Exit program"), _("Exit program"));
    itemToolBar3->Realize();
    itemFrame1->SetToolBar(itemToolBar3);

    main_hsplit = new wxSplitterWindow( itemFrame1, ID_SASHWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_BORDER|wxSP_3DSASH|wxDOUBLE_BORDER );
    main_hsplit->SetMinimumPaneSize(0);

    main_split = new wxSplitterWindow( main_hsplit, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    main_split->SetMinimumPaneSize(0);
    main_split->SetSashGravity(1);

    dataviewtree_pseudocode = new wxDataViewCtrl( main_split,ID_TREECTRL,wxDefaultPosition,wxDefaultSize,wxHSCROLL|wxDV_ROW_LINES );
    dataviewtree_pseudocode->SetName(wxT("code_structure"));
    dataviewtree_pseudocode->SetBackgroundColour(wxColour(255, 255, 255));

    config_panel = new wxPanel( main_split, ID_PANEL, wxDefaultPosition, wxSize(-1, 20), wxTAB_TRAVERSAL );
    config_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxBoxSizer* itemBoxSizer17 = new wxBoxSizer(wxHORIZONTAL);
    config_panel->SetSizer(itemBoxSizer17);

    wxStaticText* itemStaticText18 = new wxStaticText( config_panel, wxID_STATIC, _("Rank filter"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(itemStaticText18, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    rank_filter_text = new wxTextCtrl( config_panel, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(rank_filter_text, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine20 = new wxStaticLine( config_panel, wxID_STATIC, wxDefaultPosition, wxSize(10, -1), wxLI_VERTICAL );
    itemBoxSizer17->Add(itemStaticLine20, 0, wxGROW|wxALL, 5);

    wxCheckBox* itemCheckBox21 = new wxCheckBox( config_panel, ID_CHECKBOX, _("Show CPU bursts"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
    itemCheckBox21->SetValue(false);
    itemBoxSizer17->Add(itemCheckBox21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    burst_threshold_text = new wxTextCtrl( config_panel, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    burst_threshold_text->Enable(false);
    itemBoxSizer17->Add(burst_threshold_text, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine23 = new wxStaticLine( config_panel, wxID_STATIC, wxDefaultPosition, wxSize(10, -1), wxLI_VERTICAL );
    itemBoxSizer17->Add(itemStaticLine23, 0, wxGROW|wxALL, 5);

    wxButton* itemButton24 = new wxButton( config_panel, ID_BUTTON, _("Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer17->Add(itemButton24, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    main_split->SplitHorizontally(dataviewtree_pseudocode, config_panel, -35);
    info_panel = new wxPanel( main_hsplit, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxHSCROLL|wxTAB_TRAVERSAL );
    info_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxBoxSizer* itemBoxSizer26 = new wxBoxSizer(wxVERTICAL);
    info_panel->SetSizer(itemBoxSizer26);

    legend_panel = new wxPanel( info_panel, ID_PANEL2, wxDefaultPosition, wxSize(-1, 50), wxTAB_TRAVERSAL );
    legend_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    itemBoxSizer26->Add(legend_panel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
    legend_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    legend_panel->SetSizer(legend_panel_sizer);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer26->Add(itemBoxSizer29, 1, wxGROW|wxALL, 5);
    wxStaticBox* itemStaticBoxSizer30Static = new wxStaticBox(info_panel, wxID_ANY, _("General info"));
    wxStaticBoxSizer* itemStaticBoxSizer30 = new wxStaticBoxSizer(itemStaticBoxSizer30Static, wxHORIZONTAL);
    itemBoxSizer29->Add(itemStaticBoxSizer30, 0, wxGROW|wxALL, 5);
    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer30->Add(itemBoxSizer31, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText32 = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("N. Phases:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer31->Add(itemStaticText32, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    general_nphases_label = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    general_nphases_label->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer31->Add(general_nphases_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer34 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer30->Add(itemBoxSizer34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText35 = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("Deltas:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer34->Add(itemStaticText35, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    general_deltas_label = new wxStaticText( itemStaticBoxSizer30->GetStaticBox(), wxID_STATIC, _("0,32; 0,4"), wxDefaultPosition, wxDefaultSize, 0 );
    general_deltas_label->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer34->Add(general_deltas_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer37Static = new wxStaticBox(info_panel, wxID_ANY, _("Selection callstack"));
    wxStaticBoxSizer* itemStaticBoxSizer37 = new wxStaticBoxSizer(itemStaticBoxSizer37Static, wxHORIZONTAL);
    itemBoxSizer29->Add(itemStaticBoxSizer37, 1, wxGROW|wxALL, 5);
    callpath_panel = new wxPanel( itemStaticBoxSizer37->GetStaticBox(), ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    callpath_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    itemStaticBoxSizer37->Add(callpath_panel, 1, wxGROW|wxALL, 0);
    selected_item_callpath = new wxBoxSizer(wxVERTICAL);
    callpath_panel->SetSizer(selected_item_callpath);

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
    if (name == wxT("../../imgs/derived_add.xpm"))
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
            this->config_panel, -50);
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

void Structuredetection::OnCheckboxClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX in Structuredetection.
    // Before editing this code, remove the block markers.
    event.Skip();
////@end wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX in Structuredetection. 
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

    std::string legend_text;
    std::sort(ranks->begin(), ranks->end());
    for (auto rank : *ranks)
        legend_text += std::to_string(rank) + " ";

    wxSize tmpSize( 20, 20 );
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
        wxPseudocodeItem *item = static_cast<wxPseudocodeItem*>(event.GetItem().GetID());
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

void Structuredetection::setSelectionInfo(unsigned int repetitions, 
        std::vector<unsigned int> ranks)
{
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
        //for (auto it : ranks)
        //    std::cout << it << std::endl;
    }

    // Configure model
    wxPseudocode* mm = static_cast<wxPseudocode*>(dataviewtree_pseudocode
            ->GetModel());
    mm->setRanksFilter(ranks);
    mm->Cleared();
    dataviewtree_pseudocode->AssociateModel((wxDataViewModel*) mm);
}

