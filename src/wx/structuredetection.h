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
#include "wx/splitter.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxDataViewCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_STRUCTUREDETECTION 10000
#define ID_SPLITTERWINDOW 10002
#define ID_TREECTRL 10001
#define ID_SPLITTERWINDOW1 10003
#define ID_TEXTCTRL 10004
#define SYMBOL_STRUCTUREDETECTION_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_STRUCTUREDETECTION_TITLE _("Structure detection")
#define SYMBOL_STRUCTUREDETECTION_IDNAME ID_STRUCTUREDETECTION
#define SYMBOL_STRUCTUREDETECTION_SIZE wxSize(500, 300)
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

    /// wxEVT_UPDATE_UI event handler for ID_TREECTRL
    void OnTreectrlUpdate( wxUpdateUIEvent& event );

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
    wxDataViewCtrl* dataviewtree_pseudocode;
////@end Structuredetection member variables
//
//
    wxDataViewCtrl* itemControl3;
    void SetAssociateModel(wxDataViewModel* model)
    {
        dataviewtree_pseudocode->AssociateModel(model);
        wxDataViewTextRenderer *tr =
            new wxDataViewTextRenderer( "string", wxDATAVIEW_CELL_INERT );
        wxDataViewColumn *column0 =
            new wxDataViewColumn( "Pseudocode", tr, 0, 200, wxALIGN_LEFT,
                    wxDATAVIEW_COL_RESIZABLE );
        dataviewtree_pseudocode->AppendColumn( column0 );

    }
};

#endif
    // _STRUCTUREDETECTION_H_
