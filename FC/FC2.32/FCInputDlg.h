#if !defined(AFX_FCINPUTDLG_H__A0824469_84E8_4D4C_9BC0_3F72F901EDFD__INCLUDED_)
#define AFX_FCINPUTDLG_H__A0824469_84E8_4D4C_9BC0_3F72F901EDFD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FCInputDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFCInputDlg dialog

class CFCInputDlg : public CDialog
{
// Construction
public:
	CFCInputDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFCInputDlg)
	enum { IDD = IDD_INPUT_DLG };
	CString	m_strInformation;
	CString	m_strInput;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCInputDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFCInputDlg)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	enum ZInputMode
	{
		IM_CHAR,
		IM_INT,
		IM_DOUBLE,
		IM_STRING,
	};
	ZInputMode m_im;
	char m_cInput;
	int m_iInput;
	double m_dInput;
public:
	int InputChar(char &c);
	int InputInt(int &i);
	int InputDouble(double &d);
	int InputString(CString &string);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCINPUTDLG_H__A0824469_84E8_4D4C_9BC0_3F72F901EDFD__INCLUDED_)
