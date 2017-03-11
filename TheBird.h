// TheBird.h : main header file for the THEBIRD application
//

#if !defined(AFX_THEBIRD_H__87254E82_553D_41F2_BBA9_38ED2C8E301A__INCLUDED_)
#define AFX_THEBIRD_H__87254E82_553D_41F2_BBA9_38ED2C8E301A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTheBirdApp:
// See TheBird.cpp for the implementation of this class
//

class CTheBirdApp : public CWinApp
{
public:
	CTheBirdApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTheBirdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTheBirdApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THEBIRD_H__87254E82_553D_41F2_BBA9_38ED2C8E301A__INCLUDED_)
