#if !defined(AFX_FCERRORREPORTDLG_H__8FAC5931_BFC1_4255_A048_9153CC96D3EA__INCLUDED_)
#define AFX_FCERRORREPORTDLG_H__8FAC5931_BFC1_4255_A048_9153CC96D3EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FC-define.h"

class CFCErrorReportDlg : public CDialog
{
// Construction
public:
	CFCErrorReportDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFCErrorReportDlg)
	enum { IDD = IDD_ERROR_REPORT_DLG };
	BOOL	m_bIgnoreAllError;
	CString	m_strLineString;
	CString	m_strLineNumber;
	CString	m_msg;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCErrorReportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFCErrorReportDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int DoModal(int iLine,CString strLine,ZET et);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCERRORREPORTDLG_H__8FAC5931_BFC1_4255_A048_9153CC96D3EA__INCLUDED_)
