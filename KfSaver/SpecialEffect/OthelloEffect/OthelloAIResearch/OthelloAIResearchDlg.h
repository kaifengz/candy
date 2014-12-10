
// OthelloAIResearchDlg.h : header file
//

#pragma once

#include "OthelloBoard.h"
#include "afxwin.h"

// COthelloAIResearchDlg dialog
class COthelloAIResearchDlg : public CDialogEx
{
// Construction
public:
	COthelloAIResearchDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_OTHELLOAIRESEARCH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	void FormatStatistic();

public:
	CComboBox m_black;
	CComboBox m_white;
	CEdit m_rounds;
	CButton m_start;
	CStatic m_progress;
	CStatic m_major;
	CStatic m_minor;

private:
	COthelloBoard m_board;
	int m_total_rounds;
	int m_curr_rounds;
	int m_black_major;
	int m_white_major;
	int m_black_minor;
	int m_white_minor;
};
