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
#include "wx/propgrid/propgrid.h"
////@end includes
#include <wx/msgdlg.h>

#include "structuredetection.h"

////@begin XPM images
#include "file_browser.xpm"
#include "../../imgs/derived_add.xpm"
#include "../../imgs/derived_substract.xpm"
#include "app_edit.xpm"
#include "../../imgs/timeline.xpm"
#include "new_window.xpm"
#include "../../imgs/delete.xpm"
////@end XPM images


//
// Specialized functions for ConfigField
//

template<> void* ConfigField<float>::myGetManager(float df)
{
    wxFloatProperty* tmp_prop = new wxFloatProperty(this->getName(), 
            wxPG_LABEL, this->getDefault());
    tmp_prop->SetAttribute("config_field", this);
    tmp_prop->SetAttribute("config_type", PG_PROPERTY_FLOAT);

    return (void *)tmp_prop;
}
template<> void* ConfigField<int>::myGetManager(int df)
{
    wxIntProperty* tmp_prop = new wxIntProperty(this->getName(), 
            wxPG_LABEL, this->getDefault());
    tmp_prop->SetAttribute("config_field", this);
    tmp_prop->SetAttribute("config_type", PG_PROPERTY_INT);

    return (void *)tmp_prop;
}
template<> void* ConfigField<std::string>::myGetManager(std::string df)
{
    wxStringProperty* tmp_prop = new wxStringProperty(this->getName(), 
            wxPG_LABEL, this->getDefault());
    tmp_prop->SetAttribute("config_field", this);
    tmp_prop->SetAttribute("config_type", PG_PROPERTY_STRING);

    return (void *)tmp_prop;
}

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
    EVT_SLIDER( ID_SLIDER, Structuredetection::OnIterationSliderUpdated )
    EVT_SLIDER( ID_SLIDER1, Structuredetection::OnIterationMaxSliderUpdated )
    EVT_CHECKBOX( ID_CHECKBOX1, Structuredetection::OnSpecificIterationCheckboxClick )
    EVT_BUTTON( ID_BITMAPBUTTON, Structuredetection::OnParaverShowClick )
////@end Structuredetection event table entries
    EVT_DATAVIEW_SELECTION_CHANGED( ID_TREECTRL, Structuredetection::OnItemSelection)
    EVT_PG_CHANGED(ID_PROPGRID, Structuredetection::OnPGValueChanged)
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
        ((wxSplitterWindow*) FindWindow(ID_SASHWINDOW))->SetSashPosition(-110);
    if (FindWindow(ID_SPLITTERWINDOW))
        ((wxSplitterWindow*) FindWindow(ID_SPLITTERWINDOW))->SetSashPosition(-250);
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
    configuration_propgrid = NULL;
    info_panel = NULL;
    loop_iterations_slider = NULL;
    loop_iterations_slider_max = NULL;
    loop_iterations_min = NULL;
    loop_iterations_max = NULL;
    loop_specific_iteration = NULL;
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
    wxBitmap itemtool4Bitmap(itemFrame1->GetBitmapResource(wxT("file_browser.xpm")));
    wxBitmap itemtool4BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL6, wxEmptyString, itemtool4Bitmap, itemtool4BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    wxStaticLine* itemStaticLine5 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine5);
    wxBitmap itemtool6Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/derived_add.xpm")));
    wxBitmap itemtool6BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL2, wxEmptyString, itemtool6Bitmap, itemtool6BitmapDisabled, wxITEM_NORMAL, _("Unfold more"), _("Unfold more"));
    wxBitmap itemtool7Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/derived_substract.xpm")));
    wxBitmap itemtool7BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL3, wxEmptyString, itemtool7Bitmap, itemtool7BitmapDisabled, wxITEM_NORMAL, _("Unfold less"), _("Unfold less"));
    wxStaticLine* itemStaticLine8 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine8);
    wxBitmap itemtool9Bitmap(itemFrame1->GetBitmapResource(wxT("app_edit.xpm")));
    wxBitmap itemtool9BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL, wxEmptyString, itemtool9Bitmap, itemtool9BitmapDisabled, wxITEM_NORMAL, _("Show configuration panel"), _("Show configuration panel"));
    wxBitmap itemtool10Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/timeline.xpm")));
    wxBitmap itemtool10BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL1, wxEmptyString, itemtool10Bitmap, itemtool10BitmapDisabled, wxITEM_NORMAL, _("Show information panel"), _("Show information panel"));
    wxBitmap itemtool11Bitmap(itemFrame1->GetBitmapResource(wxT("new_window.xpm")));
    wxBitmap itemtool11BitmapDisabled;
    itemToolBar3->AddTool(ID_TOOL5, _("Show clustering"), itemtool11Bitmap, itemtool11BitmapDisabled, wxITEM_NORMAL, _("Show clustering"), _("Show clustering"));
    wxStaticLine* itemStaticLine12 = new wxStaticLine( itemToolBar3, wxID_STATIC, wxDefaultPosition, wxSize(-1, 20), wxLI_HORIZONTAL );
    itemToolBar3->AddControl(itemStaticLine12);
    wxBitmap itemtool13Bitmap(itemFrame1->GetBitmapResource(wxT("../../imgs/delete.xpm")));
    wxBitmap itemtool13BitmapDisabled;
    itemToolBar3->AddTool(wxID_EXIT, wxEmptyString, itemtool13Bitmap, itemtool13BitmapDisabled, wxITEM_NORMAL, _("Exit program"), _("Exit program"));
    itemToolBar3->Realize();
    itemFrame1->SetToolBar(itemToolBar3);

    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer14);

    trace_file_path_text = new wxTextCtrl( itemFrame1, ID_TEXTCTRL2, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer14->Add(trace_file_path_text, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 3);

    main_hsplit = new wxSplitterWindow( itemFrame1, ID_SASHWINDOW, wxDefaultPosition, wxSize(100, 100), wxNO_BORDER );
    main_hsplit->SetMinimumPaneSize(0);

    main_split = new wxSplitterWindow( main_hsplit, ID_SPLITTERWINDOW, wxDefaultPosition, wxSize(100, 100), wxSP_3DBORDER|wxSP_3DSASH|wxNO_BORDER );
    main_split->SetMinimumPaneSize(0);
    main_split->SetSashGravity(1);

    dataviewtree_pseudocode = new wxDataViewCtrl( main_split,ID_TREECTRL,wxDefaultPosition,wxDefaultSize,wxNO_BORDER|wxHSCROLL|wxDV_ROW_LINES );
    dataviewtree_pseudocode->SetName(wxT("code_structure"));
    dataviewtree_pseudocode->SetBackgroundColour(wxColour(255, 255, 255));

    configuration_propgrid = new wxPropertyGrid( main_split, ID_PROPGRID, wxDefaultPosition, wxSize(100, 100), wxNO_BORDER );
    configuration_propgrid->SetName(wxT("configurations_propgrid"));

    main_split->SplitVertically(dataviewtree_pseudocode, configuration_propgrid, -250);
    info_panel = new wxPanel( main_hsplit, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxHSCROLL|wxTAB_TRAVERSAL );
    info_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    info_panel->SetSizer(itemBoxSizer21);

    wxBoxSizer* itemBoxSizer22 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer21->Add(itemBoxSizer22, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer22->Add(itemBoxSizer23, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    loop_iterations_slider = new wxSlider( info_panel, ID_SLIDER, 0, 0, 100, wxDefaultPosition, wxSize(200, -1), wxSL_LEFT|wxNO_BORDER );
    loop_iterations_slider->Enable(false);
    itemBoxSizer23->Add(loop_iterations_slider, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    loop_iterations_slider_max = new wxSlider( info_panel, ID_SLIDER1, 0, 0, 100, wxDefaultPosition, wxSize(200, -1), wxSL_AUTOTICKS|wxSL_LABELS|wxSL_LEFT|wxNO_BORDER );
    loop_iterations_slider_max->Enable(false);
    itemBoxSizer23->Add(loop_iterations_slider_max, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    loop_iterations_min = new wxButton( info_panel, ID_BUTTON1, _("Min"), wxDefaultPosition, wxDefaultSize, 0 );
    loop_iterations_min->Enable(false);
    itemBoxSizer22->Add(loop_iterations_min, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    loop_iterations_max = new wxButton( info_panel, ID_BUTTON2, _("Max"), wxDefaultPosition, wxDefaultSize, 0 );
    loop_iterations_max->Enable(false);
    itemBoxSizer22->Add(loop_iterations_max, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticLine* itemStaticLine28 = new wxStaticLine( info_panel, wxID_STATIC, wxDefaultPosition, wxSize(20, -1), wxLI_VERTICAL );
    itemBoxSizer22->Add(itemStaticLine28, 0, wxGROW|wxALL, 5);

    loop_specific_iteration = new wxCheckBox( info_panel, ID_CHECKBOX1, _("Specific iteration"), wxDefaultPosition, wxDefaultSize, 0 );
    loop_specific_iteration->SetValue(false);
    itemBoxSizer22->Add(loop_specific_iteration, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton30 = new wxButton( info_panel, ID_BITMAPBUTTON, _("Show in Paraver"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer22->Add(itemButton30, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer31 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer21->Add(itemBoxSizer31, 1, wxGROW|wxALL, 5);
    wxStaticBox* itemStaticBoxSizer32Static = new wxStaticBox(info_panel, wxID_ANY, _("General info"));
    wxStaticBoxSizer* itemStaticBoxSizer32 = new wxStaticBoxSizer(itemStaticBoxSizer32Static, wxHORIZONTAL);
    itemStaticBoxSizer32Static->Show(false);
    itemBoxSizer31->Add(itemStaticBoxSizer32, 0, wxALIGN_TOP|wxALL, 5);
    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer32->Add(itemBoxSizer33, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText34 = new wxStaticText( itemStaticBoxSizer32->GetStaticBox(), wxID_STATIC, _("N. Phases:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer33->Add(itemStaticText34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    general_nphases_label = new wxStaticText( itemStaticBoxSizer32->GetStaticBox(), wxID_STATIC, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    general_nphases_label->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer33->Add(general_nphases_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer36 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer32->Add(itemBoxSizer36, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxStaticText* itemStaticText37 = new wxStaticText( itemStaticBoxSizer32->GetStaticBox(), wxID_STATIC, _("Deltas:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer36->Add(itemStaticText37, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    general_deltas_label = new wxStaticText( itemStaticBoxSizer32->GetStaticBox(), wxID_STATIC, _("0,32; 0,4"), wxDefaultPosition, wxDefaultSize, 0 );
    general_deltas_label->SetFont(wxFont(10, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("Sans")));
    itemBoxSizer36->Add(general_deltas_label, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer39Static = new wxStaticBox(info_panel, wxID_ANY, _("Selection callstack"));
    wxStaticBoxSizer* itemStaticBoxSizer39 = new wxStaticBoxSizer(itemStaticBoxSizer39Static, wxVERTICAL);
    itemStaticBoxSizer39Static->Show(false);
    itemBoxSizer31->Add(itemStaticBoxSizer39, 1, wxGROW|wxALL, 5);
    callpath_panel = new wxPanel( itemStaticBoxSizer39->GetStaticBox(), ID_PANEL3, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
    callpath_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    callpath_panel->Show(false);
    itemStaticBoxSizer39->Add(callpath_panel, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);
    selected_item_callpath = new wxBoxSizer(wxVERTICAL);
    callpath_panel->SetSizer(selected_item_callpath);

    main_hsplit->SplitHorizontally(main_split, info_panel, -110);
    itemBoxSizer14->Add(main_hsplit, 1, wxGROW|wxALL, 0);

    legend_panel = new wxPanel( itemFrame1, ID_PANEL2, wxDefaultPosition, wxSize(-1, 25), wxTAB_TRAVERSAL );
    legend_panel->SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
    itemBoxSizer14->Add(legend_panel, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 0);

    legend_panel_sizer = new wxBoxSizer(wxHORIZONTAL);
    legend_panel->SetSizer(legend_panel_sizer);

////@end Structuredetection content construction

    //status_bar->SetStatusText("Executing analysis...", 0);
    //status_bar->SetStatusText("Done", 1);
    
    // Initally filter panel should not be shown
    this->main_hsplit->Unsplit(NULL);
    //this->main_split->Unsplit(NULL);

    // Adding columns to dataviewctrl
    wxDataViewTextRenderer *tr = new wxDataViewTextRenderer();
    tr->EnableMarkup();
    wxDataViewTextRenderer *tr1 = new wxDataViewTextRenderer( 
            "string", wxDATAVIEW_CELL_INERT );
    wxDataViewTextRenderer *tr2 = new wxDataViewTextRenderer( 
            "string", wxDATAVIEW_CELL_INERT );
    wxDataViewTextRenderer *tr3 = new wxDataViewTextRenderer( 
            "string", wxDATAVIEW_CELL_INERT );


    wxDataViewColumn *column0 = new wxDataViewColumn( 
            DATAVIEW_COLUMN_0_NAME, tr, 0, 
            DATAVIEW_COLUMN_0_SIZE, wxALIGN_LEFT,
            wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *column1 = new wxDataViewColumn( 
            DATAVIEW_COLUMN_1_NAME, tr2, 1, 
            DATAVIEW_COLUMN_1_SIZE, wxALIGN_RIGHT,
            wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *column2 = new wxDataViewColumn( 
            DATAVIEW_COLUMN_2_NAME, tr3, 2, 
            DATAVIEW_COLUMN_2_SIZE, wxALIGN_RIGHT,
            wxDATAVIEW_COL_RESIZABLE );
    wxDataViewColumn *column3 = new wxDataViewColumn( 
            DATAVIEW_COLUMN_3_NAME, tr1, 3,
            DATAVIEW_COLUMN_3_SIZE, wxALIGN_RIGHT,
            wxDATAVIEW_COL_RESIZABLE );

    dataviewtree_pseudocode->AppendColumn(column0);
    dataviewtree_pseudocode->AppendColumn(column1);
    dataviewtree_pseudocode->AppendColumn(column2);
    dataviewtree_pseudocode->AppendColumn(column3);

    // Fill the configuration wxPropertyGrid with general configs
    wxArrayString time_units;
    time_units.Add(TIME_UNITS_NS);
    time_units.Add(TIME_UNITS_US);
    time_units.Add(TIME_UNITS_MS);
    time_units.Add(TIME_UNITS_S);
    
    wxArrayString size_units;
    size_units.Add(SIZE_UNITS_B);
    size_units.Add(SIZE_UNITS_KB);
    size_units.Add(SIZE_UNITS_MB);
    size_units.Add(SIZE_UNITS_GB);

    configuration_propgrid->Append( new wxPropertyCategory("Main") );
    this->configuration_propgrid->Append(
            new wxStringProperty(CONFIG_LABEL_MPI_FILTER, wxPG_LABEL, ""));
    this->configuration_propgrid->Append(
            new wxEnumProperty(CONFIG_LABEL_TIME_UNITS, wxPG_LABEL, time_units));
    this->configuration_propgrid->Append(
            new wxEnumProperty(CONFIG_LABEL_SIZE_UNITS, wxPG_LABEL, size_units));
    this->configuration_propgrid->Append(
            new wxBoolProperty(CONFIG_LABEL_CPU_BURSTS, wxPG_LABEL, false));
    this->configuration_propgrid->Append(
            new wxIntProperty(CONFIG_LABEL_CPU_LOWBND, wxPG_LABEL, 0));
    this->loading_dialog = new loadingDialog(NULL);
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
    if (name == wxT("file_browser.xpm"))
    {
        wxBitmap bitmap(file_browser_xpm);
        return bitmap;
    }
    else if (name == wxT("../../imgs/derived_add.xpm"))
    {
        wxBitmap bitmap(derived_add_xpm);
        return bitmap;
    }
    else if (name == wxT("../../imgs/derived_substract.xpm"))
    {
        wxBitmap bitmap(derived_substract_xpm);
        return bitmap;
    }
    else if (name == wxT("app_edit.xpm"))
    {
        wxBitmap bitmap(app_edit_xpm);
        return bitmap;
    }
    else if (name == wxT("../../imgs/timeline.xpm"))
    {
        wxBitmap bitmap(timeline_xpm);
        return bitmap;
    }
    else if (name == wxT("new_window.xpm"))
    {
        wxBitmap bitmap(application_star_xpm);
        return bitmap;
    }
    else if (name == wxT("../../imgs/delete.xpm"))
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
        this->main_split->SplitVertically(
            this->dataviewtree_pseudocode, 
            this->configuration_propgrid, -200);
        //this->main_split->SplitHorizontally(
        //    this->dataviewtree_pseudocode, 
        //    this->config_panel, -70);
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
            this->info_panel, -100);
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

        if (obj->isLoop())
        {
            GUILoop* l = static_cast<GUILoop*>(obj);
            this->loop_iterations_slider->SetRange(1,l->getNIterations());
            this->loop_iterations_slider_max->SetRange(1,l->getNIterations());
            this->selected_loop = static_cast<void*>(l);
        }
        else
        {
            this->selected_loop = NULL;
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
    //this->dataviewtree_pseudocode->GetModel()->GetChildren(
    //        wxDataViewItem(0), 
    //        last_level_items);
    model->GetChildren(
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
    else
    {
        // TODO: Free all data of previous analysis
    }

    this->tracefile = std::string(tracefile);
    this->eps = eps;
    this->minPts = minPts;
    this->eps_tl = eps_tl;
    this->minPts_tl = minPts_tl;
    this->filter_lbound = filter_lbound;

    this->trace_file_path_text->SetValue(this->tracefile);

    trace_semantic.parse(paraver_pcf);

    std::vector<std::vector<unsigned int>> event_types = 
        trace_semantic.getGroupedEventTypes();

    for (auto group : event_types)
        for (unsigned int type : group)
            if (type >= 42000000 and type < 43000000)
            {
                std::string hwc_type_name = trace_semantic.getEventType(type);
                hwc_types.Add(hwc_type_name);
            }
    this->configuration_propgrid->Append(
            new wxEnumProperty(CONFIG_LABEL_CPU_HWC, wxPG_LABEL, hwc_types));

    TraceHeader trace_info(this->tracefile);
    paraver_interface = new ParaverInterface(this->tracefile, trace_info.texe);
    
    loading_dialog->setTraceName(this->tracefile);
    loading_dialog->Show(true);
    loading_dialog->Update();

    // Disable all windows unless loading_dialog. It reenable windows after
    // deletion of the object (at this function return).
    wxWindowDisabler disableall(loading_dialog);

    // Declare all phases
    pipeline = { 
        new NPTrace(), 
        new Reducer(trace_info.texe, filter_lbound, trace_info.ntasks), 
        new LoopsIdentification(eps, minPts), 
        new LoopsMerge(eps_tl, minPts_tl, trace_info.texe), 
        new Pseudocode(&trace_semantic) 
    };

    // Connecting to reporters
    pipeline[0]->setProgressReporter(loading_dialog);
    std::for_each(pipeline.begin(), pipeline.end(), 
            [=](PipelineStageGeneral* p){ p->setLogReporter(loading_dialog); });


    wxPGProperty* fine_tunning_root = configuration_propgrid->Append(
            new wxPropertyCategory(CONFIG_LABEL_PIPELINE));

    // Filling configuration panel
    std::for_each(pipeline.begin(), pipeline.end(),
            [=](PipelineStageGeneral* p){
                configuration_propgrid->AppendIn(fine_tunning_root,
                    new wxPropertyCategory(p->getPhaseName().c_str()));
                for (auto it : p->getConfFields())
                {
                    configuration_propgrid->Append(
                            static_cast<wxPGProperty*>(it->getManager()));
                }
            });
            
    // Connect them
    for (int i=0; i<pipeline.size()-1; ++i)
        pipeline[i]->connect(pipeline[i+1]);

    // Just run the first stage
    pipeline[0]->setInput(&(this->tracefile));
    pipeline[0]->run();

    /* Already done */

    // Showing mpi ranks legend
    std::vector<std::pair<std::vector<unsigned int>*, std::string>> color_map;
    color_map = static_cast<Pseudocode*>(pipeline[4])->getResult()->getColormap();
    this->legend_panel_sizer->Clear();
    for (auto legend_item : color_map )
        this->addLegendItem(legend_item.first, legend_item.second);

    this->postRunActions();

    loading_dialog->WellDone();
    //loading_dialog->ShowModal(); // block the execution
    //loading_dialog->Show(false);
    this->Layout();
}

void Structuredetection::partial_run(unsigned int from)
{
    assert(from < this->pipeline.size());
    
    loading_dialog->resetProgress();
    loading_dialog->Show(true);
    loading_dialog->Update();
    
    //wxWindowDisabler disableall(loading_dialog);
    this->dataviewtree_pseudocode->AssociateModel(NULL);

    // If a phase have multi-recv enabled, previous stage also must be 
    // reexecuted
    unsigned int first_stage = from;
    while (this->pipeline[first_stage]->withMR())
        first_stage--;

    loading_dialog->addMessage("Re-starting execution from ["
            + this->pipeline[first_stage]->getPhaseName() + "]");

    for(unsigned int i=first_stage; i<this->pipeline.size(); ++i)
        this->pipeline[i]->clear();
    this->pipeline[first_stage]->run();
    
    this->postRunActions();

    loading_dialog->WellDone();
    //loading_dialog->ShowModal(); // block the execution
    //loading_dialog->Show(false);
}

void Structuredetection::postRunActions()
{
    // GNUPlot script
    this->gnuplot_script = static_cast<LoopsMerge*>(pipeline[3])->getGNUPlotScript();

    // Setting model for dataview control
    wxDataViewModel* model = (wxDataViewModel*)
        static_cast<Pseudocode*>(pipeline[4])->getResult();
    this->SetAssociateModel(model);

    // Show hwc by default
    static_cast<wxPseudocode*>(model)->setHWCColumnType(
            std::string(hwc_types[0]));

    // Setting up general info
    this->setGeneralInfo(static_cast<LoopsMerge*>(pipeline[3])->getNPhases(), 
            static_cast<LoopsMerge*>(pipeline[3])->getDeltas());
    auto deltas = static_cast<LoopsMerge*>(pipeline[3])->getDeltas();

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

/*
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX1
 */

void Structuredetection::OnSpecificIterationCheckboxClick( wxCommandEvent& event )
{
    this->loop_iterations_slider->Enable(event.IsChecked());
    this->loop_iterations_slider_max->Enable(event.IsChecked());
    this->loop_iterations_max->Enable(event.IsChecked());
    this->loop_iterations_min->Enable(event.IsChecked());
}


/*
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON
 */

void Structuredetection::OnParaverShowClick( wxCommandEvent& event )
{
    if (this->loop_specific_iteration->IsChecked())
    {
        unsigned int iteration = this->loop_iterations_slider->GetValue();
        unsigned int iteration_max = this->loop_iterations_slider_max->GetValue();
        if (this->selected_loop != NULL)
        {
            GUILoop* l = static_cast<GUILoop*>(this->selected_loop);
            std::pair<unsigned int, unsigned int> it_bounds = 
                l->getLoopObj()->getIterationsBounds(iteration-1);
            std::pair<unsigned int, unsigned int> it_bounds_max = 
                l->getLoopObj()->getIterationsBounds(iteration_max-1);
            this->paraver_interface->Zoom(it_bounds.first, it_bounds_max.second, 
                    !this->timeline_already_opened);
            this->timeline_already_opened = true;
        }
    }
    else
    {
        this->paraver_interface->OpenTrace();
        this->timeline_already_opened = true;
    }
}

void Structuredetection::OnPGValueChanged(wxPropertyGridEvent& event)
{
    void* cf = event.GetProperty()->GetAttribute("config_field")
        .GetVoidPtr();

    if (cf) // Pipeline options
    {
        GeneralConfigField* config_field = static_cast<GeneralConfigField*>(cf);
        void *new_value;
        unsigned int ct = event.GetProperty()->GetAttribute("config_type")
            .GetLong();
        PipelineStageGeneral* stage = config_field->getStage();
        unsigned int stage_position = stage->getStagePosition();

        switch(ct)
        {
            case PG_PROPERTY_FLOAT:
            {
                float val = event.GetPropertyValue().GetReal();
                new_value = (void*)&val;
                break;
            }
            case PG_PROPERTY_INT:
            {
                int val = event.GetPropertyValue().GetLong();
                new_value = (void*)&val;
                break;
            }
            case PG_PROPERTY_STRING:
            {
                std::string val(event.GetPropertyValue().GetString());
                new_value = (void*)&val;
                break;
            }
            default:
                assert(false);
        }
        config_field->setValuep(new_value);
        wxMessageDialog msgd(this, "Do you want the analysis to be re-executed"\
                " with these new parameters?", "Message", wxYES_NO|wxNO_DEFAULT);
        if (msgd.ShowModal() == wxID_YES )
        {
            this->partial_run(stage_position); 
        }
        msgd.Destroy();
    }
    else // Main options (added by hand)
    {
        wxPseudocode* mm = static_cast<wxPseudocode*>(
                dataviewtree_pseudocode->GetModel());
        if (mm == NULL) return;

        wxString propName = event.GetPropertyName();
        if(propName == CONFIG_LABEL_MPI_FILTER)
        {
            std::string rank_filter(event.GetPropertyValue());
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
            mm->setRanksFilter(ranks);
        }
        else if (propName == CONFIG_LABEL_CPU_BURSTS)
        {
            mm->showComputations(event.GetPropertyValue());
        }
        else if (propName == CONFIG_LABEL_CPU_LOWBND)
        {
            mm->setComputationsThresshold(event.GetPropertyValue().GetLong());
        }
        else if (propName == CONFIG_LABEL_CPU_HWC)
        {
            auto index = event.GetPropertyValue();
            std::string hwc_t(event.GetProperty()->ValueToString(index));
            mm->setHWCColumnType(hwc_t);
        }
        else if (propName == CONFIG_LABEL_SIZE_UNITS)
        {

            auto index = event.GetPropertyValue();
            std::string size_units(event.GetProperty()->ValueToString(index));
            mm->setSizeUnits(size_units);
        }
        else if (propName == CONFIG_LABEL_TIME_UNITS)
        {
            auto index = event.GetPropertyValue();
            std::string time_units(event.GetProperty()->ValueToString(index));
            mm->setTimeUnits(time_units);
        }

        // TODO : Whenever the model is updated, the expanded/collapsed items state
        // should be gathered in order to restore the current state when model is
        // showed again through the control.
        //
        dataviewtree_pseudocode->AssociateModel((wxDataViewModel*) mm);
    }
}


/*
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER
 */

void Structuredetection::OnIterationSliderUpdated( wxCommandEvent& event )
{
    if (this->loop_iterations_slider_max->GetValue() < event.GetInt())
        this->loop_iterations_slider_max->SetValue(event.GetInt());
}


/*
 * wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER1
 */

void Structuredetection::OnIterationMaxSliderUpdated( wxCommandEvent& event )
{
    if (this->loop_iterations_slider->GetValue() > event.GetInt())
        this->loop_iterations_slider->SetValue(event.GetInt());
}

