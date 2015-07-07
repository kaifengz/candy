#if !defined(AFX_FCTRANSLATEDLG_H__00B40C6B_B2B2_452B_BD57_CFE77CF98A4F__INCLUDED_)
#define AFX_FCTRANSLATEDLG_H__00B40C6B_B2B2_452B_BD57_CFE77CF98A4F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FCTranslateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFCTranslateDlg dialog

class CFCTranslateDlg : public CDialog
{
// Construction
public:
	CFCTranslateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFCTranslateDlg)
	enum { IDD = IDD_TRANSLATE_DLG };
	int		m_iBackNumber;
	BOOL	m_bEmptyLine;
	BOOL	m_bInsertSpace;
	CString	m_strObjectFile;
	BOOL	m_bOutPrecision;
	BOOL	m_bRunTime;
	BOOL	m_bWaitEnd;
	int		m_iBackChoice;
	BOOL	m_bKeepComment;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCTranslateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFCTranslateDlg)
	afx_msg void OnBrowse();
	afx_msg void OnDefaultSet();
	virtual void OnOK();
	afx_msg void OnTabBack();
	afx_msg void OnSpaceBack();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int DoModal(CString strFCFileName,BOOL bInsertSpace,BOOL bEmptyLine,
		int iBackChoice,int iBackNumber,BOOL bKeepComment,
		BOOL bOutPrecision,BOOL bRunTime,BOOL bWaitEnd);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCTRANSLATEDLG_H__00B40C6B_B2B2_452B_BD57_CFE77CF98A4F__INCLUDED_)
