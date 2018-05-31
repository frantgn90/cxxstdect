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
#include "wx/splitter.h"
#include "wx/statline.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxStatusBar;
class wxSplitterWindow;
class wxDataViewCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_STRUCTUREDETECTION 10000
#define ID_STATUSBAR 10005
#define ID_TOOLBAR 10006
#define ID_TOOL2 10012
#define ID_TOOL3 10013
#define ID_TOOL 10010
#define ID_TOOL1 10011
#define ID_SASHWINDOW 10014
#define ID_SPLITTERWINDOW 10002
#define ID_TREECTRL 10001
#define ID_PANEL 10003
#define ID_TEXTCTRL 10004
#define ID_CHECKBOX 10007
#define ID_TEXTCTRL1 10008
#define ID_BUTTON 10009
#define ID_PANEL1 10015
#define SYMBOL_STRUCTUREDETECTION_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_STRUCTUREDETECTION_TITLE _("Structure detection")
#define SYMBOL_STRUCTUREDETECTION_IDNAME ID_STRUCTUREDETECTION
#define SYMBOL_STRUCTUREDETECTION_SIZE wxSize(600, 700)
#define SYMBOL_STRUCTUREDETECTION_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * Structuredetection class declaration
 */

class Structuredetection: public wxFrame
{    
    DECLARE_CLASS( Structuredetection )
    DECLARE_EVENT_TABLE()

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
    void OnCheckboxClick( wxCommandEvent& event );

////@end Structuredetection event handler declarations

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
    wxSplitterWindow* main_hsplit;
    wxSplitterWindow* main_split;
    wxDataViewCtrl* dataviewtree_pseudocode;
    wxPanel* config_panel;
    wxPanel* info_panel;
////@end Structuredetection member variables
//
//
    wxDataViewCtrl* itemControl3;
    void SetAssociateModel(wxDataViewModel* model)
    {
        dataviewtree_pseudocode->AssociateModel(model);
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
            new wxDataViewColumn( "Duration", tr2, 1, 120, wxALIGN_RIGHT,
                    wxDATAVIEW_COL_RESIZABLE );
        wxDataViewColumn *column2 =
            new wxDataViewColumn( "Msg. Size", tr3, 2, 120, wxALIGN_RIGHT,
                    wxDATAVIEW_COL_RESIZABLE );
        wxDataViewColumn *column3 =
            new wxDataViewColumn( "IPC", tr1, 3, 120, wxALIGN_RIGHT,
                    wxDATAVIEW_COL_RESIZABLE );

        dataviewtree_pseudocode->AppendColumn(column0);
        dataviewtree_pseudocode->AppendColumn(column1);
        dataviewtree_pseudocode->AppendColumn(column2);
        dataviewtree_pseudocode->AppendColumn(column3);
    }
};

#endif
    // _STRUCTUREDETECTION_H_
