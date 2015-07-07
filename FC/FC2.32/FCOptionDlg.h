#if !defined(AFX_FCOPTIONDLG_H__A88A5636_5796_4507_B57A_95799185994D__INCLUDED_)
#define AFX_FCOPTIONDLG_H__A88A5636_5796_4507_B57A_95799185994D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FCOptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFCOptionDlg dialog

class CFCOptionDlg : public CDialog
{
// Construction
public:
	CFCOptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFCOptionDlg)
	enum { IDD = IDD_OPTION_DLG };
	BOOL	m_bAllowEmptyStatement;
	BOOL	m_bAutoChangeType;
	BOOL	m_bDisableChangeType;
	BOOL	m_bFileOutput;
	BOOL	m_bGenerateBinary;
	BOOL	m_bGenerateText;
	BOOL	m_bIncludeArray;
	BOOL	m_bIncludeMath;
	BOOL	m_bIncludeString;
	BOOL	m_bJumpOptimize;
	BOOL	m_bOutRuntime;
	UINT	m_nPrecision;
	BOOL	m_bReportError;
	CString	m_strOutputFile;
	CString	m_strMemoryApply;
	BOOL	m_bIgnoreCircleNoend;
	BOOL	m_bOutputInput;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCOptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFCOptionDlg)
	afx_msg void OnDefaultOption();
	afx_msg void OnFileOutput();
	afx_msg void OnAutoChangeType();
	afx_msg void OnDisableChangeType();
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusPrecision();
	virtual void OnOK();
	afx_msg void OnKillfocusMemoryApply();
	afx_msg void OnIgnoreCircleNoend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCOPTIONDLG_H__A88A5636_5796_4507_B57A_95799185994D__INCLUDED_)
