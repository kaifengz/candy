// FCView.h : interface of the CFCView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_FCVIEW_H__6E7AA227_F3C3_41BD_8256_0F116FC0C188__INCLUDED_)
#define AFX_FCVIEW_H__6E7AA227_F3C3_41BD_8256_0F116FC0C188__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FC-define.h"
#include <fstream.h>

class CFCView : public CEditView
{
protected: // create from serialization only
	CFCView();
	DECLARE_DYNCREATE(CFCView)
public:
	CFCDoc* GetDocument();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFCView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~CFCView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(CFCView)
	afx_msg void OnCompileCompile();
	afx_msg void OnCompileInterpret();
	afx_msg void OnCompileTranslate();
	afx_msg void OnCompileReportError();
	afx_msg void OnUpdateCompileReportError(CCmdUI* pCmdUI);
	afx_msg void OnOption();
	afx_msg void OnHelpGrammar();
	afx_msg void OnHelpGlobal();
	afx_msg void OnCompileExecute();
	afx_msg void OnCompileRebuild();
	afx_msg void OnExampleHelloworld();
	afx_msg void OnExamplePermutation();
	afx_msg void OnExamplePrime();
	afx_msg void OnExamplePi();
	afx_msg void OnExampleHanio();
	afx_msg void OnExampleFibonacci();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private://��������/////////////////////////////////////////////////////////////
	void OnExample(CString,UINT);
	BOOL SaveResToFile(CString,UINT,LPCSTR);		//����Դ������ļ�
	BOOL ReadResToView(CString,UINT,LPCSTR);		//����Դ�����༭����
private://ѡ���ȡ�봢�溯��///////////////////////////////////////////////////
	void ReadOptions();		//ѡ���ȡ
	void SaveOptions();		//ѡ���
private://�⺯����������־///////////////////////////////////////////////////
	BOOL m_bIncludeMath;	//�Ƿ������ѧ�⺯��
	BOOL m_bIncludeString;	//�Ƿ�����ַ����⺯��
	BOOL m_bIncludeArray;	//�Ƿ��������⺯��
private://�����м�����ļ�����/////////////////////////////////////////////////
	BOOL m_bGenerateBinary;		//�Ƿ����ɶ����ƿɽ���ִ���ļ�
	BOOL m_bGenerateText;		//�Ƿ����ɹ��Ķ����ı��ļ�
private://�����м�����ļ�����/////////////////////////////////////////////////
	void GenerateBinaryFile(CString,CString);		//���ɶ����ƿɽ���ִ���ļ�
	void GenerateTextFile(CString,CString);			//���ɹ��Ķ����ı��ļ�
	void GenerateExeFile(CString,CString,CString);	//���ɿ�ִ���ļ�
private://����ѡ�����/////////////////////////////////////////////////////////
	BOOL m_bAutoChangeType;			//�Ƿ��Զ�����ǿ������ת��
	BOOL m_bDisableChangeType;		//�Ƿ��ֹ��ʽ����ת��
	BOOL m_bAllowEmptyStatement;	//�Ƿ���������
	BOOL m_bJumpOptimize;			//�Ƿ��Ż���ת��
private://�ʷ���������/////////////////////////////////////////////////////////
	char m_char;			//�ʷ�����ȡ�õ� �ַ�
	int m_iCharCount;		//�ַ�����
	int m_int;				//�ʷ�����ȡ�õ� ����
	int m_nSourceLength;	//Դ���볤
	double m_double;		//�ʷ�����ȡ�õ� ʵ��
	CString m_source;		//Դ����
	CString m_ident;		//�ʷ�����ȡ�õ� ��ʶ��
	CString m_string;		//�ʷ�����ȡ�õ� �ַ���
	CString m_comment;		//�ʷ�����ȡ�õ� ע��
	ZElement m_element;		//�ʷ�����ȡ�õ� ��������
private://�ʷ���������/////////////////////////////////////////////////////////
	BOOL GetElement(BOOL=FALSE);//�ʷ���������
	BOOL GetSysChar();			//��ȡת���ַ�
	BOOL GetLibraryFunction();	//��ȡ�⺯��
	BOOL GetKeyword();			//��ȡ�ؼ���
private://�﷨��������/////////////////////////////////////////////////////////
	int m_iErrorCount;			//�﷨����������
	BOOL m_bIgnoreError;		//�Ƿ񱨸��﷨�������
	BOOL m_bCompiled;			//�Ƿ����ɹ�
	ZFunction **m_pFunction;	//ָ���������ָ��
	int m_nFunction;			//��ǰ�����ĸ���
	int m_nMaxFunction;			//��ǰ��������������������Ŀ
	int m_nCurFunction;			//��ǰ����ĺ���
	ZCode *m_pCode;				//�м��������
	int m_nCode;				//��ǰ�м�������еĳ���
	int m_nMaxCode;				//��ǰ�м���������������󳤶�
	ZIT m_it;					//��ʶ�����ͣ����ڱ��ʽ�����ͼ��
	int m_nBreak;				//break����ת��ַ
	int m_nContinue;			//continue����ת��ַ
	int m_nTrue;				//�����������
	int m_nFalse;				//�����ļٳ���
	int m_nCase;				//case����������ת���
private://�﷨��������/////////////////////////////////////////////////////////
	BOOL ErrorReport(ZET et);			//�﷨������󱨸�
	BOOL ExamUnsolvedFunctions();		//����Ƿ��������˵�ȴû��ʵ�ֵĺ���
	void ClearFunction();				//���պ����ռ�
	void ClearCode();					//�����м����ռ�
	BOOL NewFunction();					//���һ������
	BOOL NewIdent();					//�ڵ�ǰ���������һ������
	BOOL NewParam();					//�ڵ�ǰ���������һ������
	BOOL NewCode();						//���һ���м����
	int GetFunctionIndex(CString);		//��ȡһ���������±�
	int GetIdentAddress(CString);		//��ȡһ����ʶ���ĵ�ַ
	ZIT GetIdentType(CString);			//��ȡһ����ʶ��������
	ZIT GetFunctionIdentType(CString);	//��ȡ������ʶ��������
	BOOL GenerateCode(ZOP);				//�����޲��������м����
	BOOL GenerateCode(ZOP,char);		//�������ַ�Ϊ���������м����
	BOOL GenerateCode(ZOP,char*);		//�������ַ�ָ��Ϊ���������м����
	BOOL GenerateCode(ZOP,int);			//����������Ϊ���������м����
	BOOL GenerateCode(ZOP,double);		//������ʵ��Ϊ���������м����
	BOOL GenerateCode(ZOP,int,int);		//����������������Ϊ�������м����
	BOOL GenerateCode(ZOP,int,char*);	//���������������ַ�ָ��Ϊ���������м����
	BOOL RecycleMemory();				//���վֲ�������������ռ�õ��ڴ�
	BOOL RecycleArrayMemory();			//��������������ռ�õ��ڴ�
	BOOL Compile();						//����������
	BOOL DoFunctionHead();				//���� ����ͷ
	BOOL DoFunction();					//���� ����
	BOOL DoStatement();					//���� ���
	BOOL DoDefine();					//���� �������
	BOOL DoInput();						//���� �������
	BOOL DoOutput();					//���� ������
	BOOL DoIf();						//���� ѡ�����
	BOOL DoSwitch();					//���� �������
	BOOL DoMultiStatement();			//���� �������
	BOOL DoJump();						//���� ��ת���
	BOOL DoBecomes();					//���� ��ֵ���
	BOOL DoWhile();						//���� whileѭ��
	BOOL DoCall();						//���� ��������
	BOOL DoDoWhile();					//���� do-whileѭ��
	BOOL DoExpression();				//���� ���ʽ
	BOOL DoTerm();						//���� ��
	BOOL DoFactor();					//���� ����
	BOOL DoSubscript();					//���� �����±�
	BOOL DoCondition();					//���� ����
	BOOL DoAndCondition();				//���� ��������
	BOOL DoSubCondition();				//���� ��������
	BOOL DoCase();						//���� ��������case��֧
	BOOL DoDefault();					//���� ��������default��֧
	BOOL DoEndProgram();				//���� ����ĺ��ڹ���
	void DoOptimize();					//���д����Ż�����
	BOOL DoMathFunctionCall();			//���� ��abs,pow�����ѧ�⺯������
	BOOL DoAbsCall();					//���� abs��������
	BOOL DoPowCall();					//���� pow��������
	BOOL DoStrcmpCall();				//���� strcmp��������
	BOOL DoStrcpyCall();				//���� strcpy��������
	BOOL DoStrlenCall();				//���� strlen��������
	BOOL DoStrlwruprCall();				//���� strlwr,strupr��������
	BOOL DoArrayFunctionCall();			//���� ��downsort,upsort�������⺯������
	BOOL DoSortCall();					//���� downsort,upsort��������
private://����ִ�в���/////////////////////////////////////////////////////////
	BOOL m_bCalcRuntime;		//�Ƿ�ͳ������ʱ��
	BOOL m_bFileOutput;			//�Ƿ�������ļ�
	BOOL m_bIgnoreCircleNoend;	//�Ƿ���Կ��ܵ���ѭ��
	BOOL m_bOutputInput;		//�Ƿ��������
	CString m_strOutputFile;	//����ļ���
	int m_nPrecision;			//�������
	int m_nMemoryApply;			//��������ڴ���
private://����ѡ�����/////////////////////////////////////////////////////////
	ofstream m_fout;		//�ļ����
	int m_nLevel;			//��������
	BOOL m_bMainFunction;	//�Ƿ���������

	int m_iBackChoice;		//������ʽ
	int m_iBackNumber;		//������Ŀ
	BOOL m_bKeepComment;	//�Ƿ���ע��
	BOOL m_bEmptyLine;		//�Ƿ��������
	BOOL m_bInsertSpace;	//�Ƿ������ǰ��ӿո�
	BOOL m_bOutPrecision;	//�Ƿ�������ȿ���
	BOOL m_bRunTime;		//�Ƿ�ͳ������ʱ��
	BOOL m_bWaitEnd;		//�Ƿ�ȴ��û���Ӧ����
	CString m_strCppFile;	//����Ŀ���ļ���
private://���뺯��/////////////////////////////////////////////////////////////
	BOOL Translate();			//����������
	void TrsAndCondition();		//���� ��������
	void TrsCondition();		//���� ����
	void TrsExpression();
	void TrsDefine();
	void TrsDoWhile();
	void TrsFactor();
	void TrsFunction();			//���� ����
	void TrsIf();
	void TrsInput();
	void TrsJump();
	void TrsMultiStatement();	//���� �������
	void TrsOutput();
	void TrsParameter();		//���� �������õĲ�������
	void TrsStatement();		//���� ���
	void TrsSubCondition();		//���� ��������
	void TrsSwitch();
	void TrsTerm();
	void TrsWhile();
	void OutOperator(char[]);
	void Indent();				//����
	void TrsMathFunctionCall();		//���� ��abs,pow�����ѧ�⺯������
	void TrsAbsCall();				//���� abs��������
	void TrsPowCall();				//���� pow��������
	void TrsStrcmpCall();			//���� strcmp��������
	void TrsStrcpyCall();			//���� strcpy��������
	void TrsStrlenCall();			//���� strlen��������
	void TrsStrlwruprCall();		//���� strlwr,strupr��������
	void TrsArrayFunctionCall();	//���� ��downsort,upsort�������⺯������
	void TrsSortCall();				//���� downsort,upsort��������
};

#ifndef _DEBUG  // debug version in FCView.cpp
inline CFCDoc* CFCView::GetDocument()
   { return (CFCDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCVIEW_H__6E7AA227_F3C3_41BD_8256_0F116FC0C188__INCLUDED_)
