#if !defined(AFX_FCINTERPRETDLG_H__C198300F_F826_41B3_95E5_AEE83659912B__INCLUDED_)
#define AFX_FCINTERPRETDLG_H__C198300F_F826_41B3_95E5_AEE83659912B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FCInterpretDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFCInterpretDlg dialog

#include "FC-define.h"
#include <fstream>

class CFCInterpretDlg : public CDialog
{
// Construction
public:
	CFCInterpretDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFCInterpretDlg)
	enum { IDD = IDD_INTERPRET_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCInterpretDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFCInterpretDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnInterpret();
	afx_msg void OnSaveOutput();
	afx_msg void OnPresicion0();
	afx_msg void OnPrecision6();
	afx_msg void OnPrecision15();
	afx_msg void OnExitDlg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CString m_strFileTitle;		//�ļ���������չ����
	char *m_lpszOutput;			//
	int m_nOutputLength;		//
	int m_nOutputCapacity;		//
private://�м�������/////////////////////////////////////////////////////////
	const ZCode *m_pCode;
	int m_nCode;
private://����ִ�в���/////////////////////////////////////////////////////////
	std::ofstream m_fout;		//�ļ����
	BOOL m_bCalcRuntime;		//�Ƿ�ͳ������ʱ��
	BOOL m_bFileOutput;			//�Ƿ�������ļ�
	BOOL m_bFixedPrecision;		//�Ƿ�̶��������
	BOOL m_bIgnoreCircleNoend;	//�Ƿ���Կ��ܵ���ѭ��
	BOOL m_bOutputInput;		//�Ƿ��������
	CString m_strOutputFile;	//����ļ���
	int m_nPrecision;			//�������
	int m_nMemoryApply;			//��������ڴ���
private://����ִ�к���/////////////////////////////////////////////////////////
	BOOL Interpret();						//����ִ��������
	BOOL TestAddress(int);					//����ڴ��Ƿ����
	BOOL SlashModZero(char);				//��鱻������ģ�Ƿ�Ϊ��
	BOOL SlashModZero(int);					//��鱻������ģ�Ƿ�Ϊ��
	BOOL SlashModZero(double);				//��鱻������ģ�Ƿ�Ϊ��
	BOOL ArraySubscript(int,int);			//��������±����Ч��
	BOOL ValidArray(void*);					//����������Ч��
	BOOL ExamCircumscriptive(ZOP,double);	//�����ѧ���������Ƿ��ڶ�������
	void InitInterpretOutput();				//��ʼ������ִ�е����
	void EndInterpretOutput(int);			//��������ִ�е����
	void Output(char,BOOL=FALSE);			//����ַ�
	void Output(int,BOOL=FALSE);			//�������
	void Output(double,BOOL=FALSE);			//���ʵ��
	void Output(CString,BOOL=FALSE);		//����ַ���
	void Output(char*,BOOL=FALSE);			//����ַ�����
	void OutputToMemory(CString);			//
	void UpdateOutput();					//
	BOOL InputChar(char &);					//�����ַ�
	BOOL InputInt(int &);					//��������
	BOOL InputDouble(double &);				//����ʵ��
	BOOL InputString(CString &);			//�����ַ���
public://��Ϣ������/////////////////////////////////////////////////////////
	void DoModal(const ZCode *pCode,int nCode,BOOL bCalcRuntime,BOOL bFileOutput,
		BOOL bIgnoreCircleNoend,BOOL bOutputInput,CString strOutputFile,
		int &nPrecision,int nMemoryApply,CString strFileTitle);
private:
	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCINTERPRETDLG_H__C198300F_F826_41B3_95E5_AEE83659912B__INCLUDED_)
