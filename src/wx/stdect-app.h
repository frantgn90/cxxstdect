/////////////////////////////////////////////////////////////////////////////
// Name:        stdect-app.h
// Purpose:     
// Author:      Juan Francisco Martínez Vera
// Modified by: 
// Created:     Wed 23 May 2018 11:56:29 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _STDECT-APP_H_
#define _STDECT-APP_H_


/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "structuredetection.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * StdectApp class declaration
 */

class StdectApp: public wxApp
{    
    DECLARE_CLASS( StdectApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    StdectApp();

    void Init();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin StdectApp event handler declarations

////@end StdectApp event handler declarations

////@begin StdectApp member function declarations

////@end StdectApp member function declarations

////@begin StdectApp member variables
////@end StdectApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(StdectApp)
////@end declare app

#endif
    // _STDECT-APP_H_
