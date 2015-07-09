#if !defined(AFX_SCOREDLG_H__3A0F6D32_A45F_435B_930F_7196FE550646__INCLUDED_)
#define AFX_SCOREDLG_H__3A0F6D32_A45F_435B_930F_7196FE550646__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScoreDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CScoreDlg dialog

class CScoreDlg : public CDialog
{
// Construction
public:
	CScoreDlg(CWnd* pParent = NULL);   // standard constructor
	CScoreDlg(int *pScoreTotal, int *pScoreCur, CWnd *pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CScoreDlg)
	enum { IDD = IDD_SCORE_DLG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScoreDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CScoreDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnRescore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	int *m_pScoreTotal;
	int *m_pScoreCur;

public:
	static CString GetRankString(const int nScoreTotal[]);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOREDLG_H__3A0F6D32_A45F_435B_930F_7196FE550646__INCLUDED_)
