// RJ.h : main header file for the RJ application
//

#if !defined(AFX_RJ_H__4163BDA4_20EF_451E_9E43_4D62278BDFB4__INCLUDED_)
#define AFX_RJ_H__4163BDA4_20EF_451E_9E43_4D62278BDFB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRJApp:
// See RJ.cpp for the implementation of this class
//

class CRJApp : public CWinApp
{
public:
	CRJApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRJApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CRJApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RJ_H__4163BDA4_20EF_451E_9E43_4D62278BDFB4__INCLUDED_)
