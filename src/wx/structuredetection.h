/////////////////////////////////////////////////////////////////////////////
// Name:        structuredetection.h
// Purpose:     
// Author:      Juan Francisco Martínez Vera
// Modified by: 
// Created:     Wed 23 May 2018 11:58:08 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _STRUCTUREDETECTION_H_
#define _STRUCTUREDETECTION_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/statusbr.h"
#include "wx/toolbar.h"
#include "wx/statline.h"
#include "wx/splitter.h"
////@end includes
#include "wx/dataview.h"
#include "wx/listctrl.h"
#include <UIParaverTraceConfig.h>

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxStatusBar;
class wxSplitterWindow;
class wxDataViewCtrl;
class wxBoxSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_STRUCTUREDETECTION 10000
#define ID_STATUSBAR 10005
#define ID_TOOLBAR 10006
#define ID_TOOL6 10021
#define ID_TOOL2 10012
#define ID_TOOL3 10013
#define ID_TOOL 10010
#define ID_TOOL1 10011
#define ID_TOOL4 10018
#define ID_TOOL5 10019
#define ID_TEXTCTRL2 10020
#define ID_SASHWINDOW 10014
#define ID_SPLITTERWINDOW 10002
#define ID_TREECTRL 10001
#define ID_PANEL 10003
#define ID_CHECKBOX 10007
#define ID_TEXTCTRL1 10008
#define ID_COMBOCTRL 10022
#define ID_TEXTCTRL 10004
#define ID_BUTTON 10009
#define ID_PANEL1 10015
#define ID_PANEL3 10017
#define ID_PANEL2 10016
#define SYMBOL_STRUCTUREDETECTION_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_STRUCTUREDETECTION_TITLE _("Structure detection")
#define SYMBOL_STRUCTUREDETECTION_IDNAME ID_STRUCTUREDETECTION
#define SYMBOL_STRUCTUREDETECTION_SIZE wxSize(650, 500)
#define SYMBOL_STRUCTUREDETECTION_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * Structuredetection class declaration
 */

class Structuredetection: public wxFrame
{    
    DECLARE_CLASS( Structuredetection )
    DECLARE_EVENT_TABLE()
private:
    wxDataViewItemArray last_level_items;
    std::string tracefile;
    double filter_lbound = 0.1;
    double eps = 0.1;
    size_t minPts = 1;
    double eps_tl = 0.1;
    size_t minPts_tl = 1;

public:
    /// Constructors
    Structuredetection();
    Structuredetection( wxWindow* parent, wxWindowID id = SYMBOL_STRUCTUREDETECTION_IDNAME, const wxString& caption = SYMBOL_STRUCTUREDETECTION_TITLE, const wxPoint& pos = SYMBOL_STRUCTUREDETECTION_POSITION, const wxSize& size = SYMBOL_STRUCTUREDETECTION_SIZE, long style = SYMBOL_STRUCTUREDETECTION_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_STRUCTUREDETECTION_IDNAME, const wxString& caption = SYMBOL_STRUCTUREDETECTION_TITLE, const wxPoint& pos = SYMBOL_STRUCTUREDETECTION_POSITION, const wxSize& size = SYMBOL_STRUCTUREDETECTION_SIZE, long style = SYMBOL_STRUCTUREDETECTION_STYLE );

    /// Destructor
    ~Structuredetection();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin Structuredetection event handler declarations

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL6
    void OnOpenTraceClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL2
    void OnUnfoldButtonClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL3
    void OnFoldButtonClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL
    void OnToolClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_TOOL1
    void OnInfoButtonClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_EXIT
    void OnExitClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_TREECTRL
    void OnTreectrlUpdate( wxUpdateUIEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX
    void OnShowComputationsCheckboxClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOCTRL
    void OnHWCComboctrlSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON
    void OnFilterButtonClick( wxCommandEvent& event );

////@end Structuredetection event handler declarations
    void OnItemSelection(wxDataViewEvent& event);
////@begin Structuredetection member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Structuredetection member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Structuredetection member variables
    wxStatusBar* status_bar;
    wxTextCtrl* trace_file_path_text;
    wxSplitterWindow* main_hsplit;
    wxSplitterWindow* main_split;
    wxDataViewCtrl* dataviewtree_pseudocode;
    wxScrolledWindow* config_panel;
    wxTextCtrl* burst_threshold_text;
    wxComboBox* hwc_combobox;
    wxTextCtrl* rank_filter_text;
    wxPanel* info_panel;
    wxStaticText* general_nphases_label;
    wxStaticText* general_deltas_label;
    wxPanel* callpath_panel;
    wxBoxSizer* selected_item_callpath;
    wxPanel* legend_panel;
    wxBoxSizer* legend_panel_sizer;
////@end Structuredetection member variables
//
//

    libparaver::UIParaverTraceConfig trace_semantic;
    wxDataViewCtrl* itemControl3;
    void SetAssociateModel(wxDataViewModel* model);
    void setGeneralInfo(unsigned int nphases, std::vector<unsigned int> deltas);
    void addLegendItem(std::vector<unsigned int>* ranks, std::string rgb);
    void run(std::string tracefile, std::string paraver_pcf, 
        double eps, size_t minPts,
        double eps_tl, size_t minPts_tl,
        double filter_lbound);
    void run(std::string tracefile);
};

#endif
    // _STRUCTUREDETECTION_H_
