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
	CString m_strFileTitle;		//文件名（无扩展名）
	char *m_lpszOutput;			//
	int m_nOutputLength;		//
	int m_nOutputCapacity;		//
private://中间代码参数/////////////////////////////////////////////////////////
	const ZCode *m_pCode;
	int m_nCode;
private://解释执行参数/////////////////////////////////////////////////////////
	std::ofstream m_fout;		//文件输出
	BOOL m_bCalcRuntime;		//是否统计运行时间
	BOOL m_bFileOutput;			//是否输出到文件
	BOOL m_bFixedPrecision;		//是否固定输出精度
	BOOL m_bIgnoreCircleNoend;	//是否忽略可能的死循环
	BOOL m_bOutputInput;		//是否输入回显
	CString m_strOutputFile;	//输出文件名
	int m_nPrecision;			//输出精度
	int m_nMemoryApply;			//申请的虚内存量
private://解释执行函数/////////////////////////////////////////////////////////
	BOOL Interpret();						//解释执行主函数
	BOOL TestAddress(int);					//检查内存是否溢出
	BOOL SlashModZero(char);				//检查被除数、模是否为零
	BOOL SlashModZero(int);					//检查被除数、模是否为零
	BOOL SlashModZero(double);				//检查被除数、模是否为零
	BOOL ArraySubscript(int,int);			//检查数组下标的有效性
	BOOL ValidArray(void*);					//检查数组的有效性
	BOOL ExamCircumscriptive(ZOP,double);	//检查数学函数变量是否在定义域内
	void InitInterpretOutput();				//初始化解释执行的输出
	void EndInterpretOutput(int);			//结束解释执行的输出
	void Output(char,BOOL=FALSE);			//输出字符
	void Output(int,BOOL=FALSE);			//输出整数
	void Output(double,BOOL=FALSE);			//输出实数
	void Output(CString,BOOL=FALSE);		//输出字符串
	void Output(char*,BOOL=FALSE);			//输出字符数组
	void OutputToMemory(CString);			//
	void UpdateOutput();					//
	BOOL InputChar(char &);					//输入字符
	BOOL InputInt(int &);					//输入整数
	BOOL InputDouble(double &);				//输入实数
	BOOL InputString(CString &);			//输入字符串
public://消息处理函数/////////////////////////////////////////////////////////
	void DoModal(const ZCode *pCode,int nCode,BOOL bCalcRuntime,BOOL bFileOutput,
		BOOL bIgnoreCircleNoend,BOOL bOutputInput,CString strOutputFile,
		int &nPrecision,int nMemoryApply,CString strFileTitle);
private:
	int DoModal();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCINTERPRETDLG_H__C198300F_F826_41B3_95E5_AEE83659912B__INCLUDED_)
