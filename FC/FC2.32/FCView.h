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

private://其他函数/////////////////////////////////////////////////////////////
	void OnExample(CString,UINT);
	BOOL SaveResToFile(CString,UINT,LPCSTR);		//把资源输出到文件
	BOOL ReadResToView(CString,UINT,LPCSTR);		//把资源读到编辑窗口
private://选项读取与储存函数///////////////////////////////////////////////////
	void ReadOptions();		//选项读取
	void SaveOptions();		//选项储存
private://库函数包含与否标志///////////////////////////////////////////////////
	BOOL m_bIncludeMath;	//是否包含数学库函数
	BOOL m_bIncludeString;	//是否包含字符串库函数
	BOOL m_bIncludeArray;	//是否包含数组库函数
private://生成中间代码文件参数/////////////////////////////////////////////////
	BOOL m_bGenerateBinary;		//是否生成二进制可解释执行文件
	BOOL m_bGenerateText;		//是否生成供阅读的文本文件
private://生成中间代码文件函数/////////////////////////////////////////////////
	void GenerateBinaryFile(CString,CString);		//生成二进制可解释执行文件
	void GenerateTextFile(CString,CString);			//生成供阅读的文本文件
	void GenerateExeFile(CString,CString,CString);	//生成可执行文件
private://编译选项参数/////////////////////////////////////////////////////////
	BOOL m_bAutoChangeType;			//是否自动进行强制类型转换
	BOOL m_bDisableChangeType;		//是否禁止隐式类型转换
	BOOL m_bAllowEmptyStatement;	//是否允许空语句
	BOOL m_bJumpOptimize;			//是否优化跳转链
private://词法分析参数/////////////////////////////////////////////////////////
	char m_char;			//词法分析取得的 字符
	int m_iCharCount;		//字符计数
	int m_int;				//词法分析取得的 整数
	int m_nSourceLength;	//源代码长
	double m_double;		//词法分析取得的 实数
	CString m_source;		//源代码
	CString m_ident;		//词法分析取得的 标识符
	CString m_string;		//词法分析取得的 字符串
	CString m_comment;		//词法分析取得的 注释
	ZElement m_element;		//词法分析取得的 单词类型
private://词法分析函数/////////////////////////////////////////////////////////
	BOOL GetElement(BOOL=FALSE);//词法分析函数
	BOOL GetSysChar();			//获取转义字符
	BOOL GetLibraryFunction();	//获取库函数
	BOOL GetKeyword();			//获取关键字
private://语法分析参数/////////////////////////////////////////////////////////
	int m_iErrorCount;			//语法语义错误计数
	BOOL m_bIgnoreError;		//是否报告语法语义错误
	BOOL m_bCompiled;			//是否编译成功
	ZFunction **m_pFunction;	//指向函数数组的指针
	int m_nFunction;			//当前函数的个数
	int m_nMaxFunction;			//当前函数数组允许的最大函数数目
	int m_nCurFunction;			//当前处理的函数
	ZCode *m_pCode;				//中间代码序列
	int m_nCode;				//当前中间代码序列的长度
	int m_nMaxCode;				//当前中间代码序列允许的最大长度
	ZIT m_it;					//标识符类型，用于表达式的类型检查
	int m_nBreak;				//break的跳转地址
	int m_nContinue;			//continue的跳转地址
	int m_nTrue;				//条件的真出口
	int m_nFalse;				//条件的假出口
	int m_nCase;				//case语句继续的跳转起点
private://语法分析函数/////////////////////////////////////////////////////////
	BOOL ErrorReport(ZET et);			//语法语义错误报告
	BOOL ExamUnsolvedFunctions();		//检查是否有引用了的却没有实现的函数
	void ClearFunction();				//回收函数空间
	void ClearCode();					//回首中间代码空间
	BOOL NewFunction();					//添加一个函数
	BOOL NewIdent();					//在当前函数中添加一个变量
	BOOL NewParam();					//在当前函数中添加一个参数
	BOOL NewCode();						//添加一个中间代码
	int GetFunctionIndex(CString);		//获取一个函数的下标
	int GetIdentAddress(CString);		//获取一个标识符的地址
	ZIT GetIdentType(CString);			//获取一个标识符的类型
	ZIT GetFunctionIdentType(CString);	//获取函数标识符的类型
	BOOL GenerateCode(ZOP);				//产生无操作数的中间代码
	BOOL GenerateCode(ZOP,char);		//产生以字符为操作数的中间代码
	BOOL GenerateCode(ZOP,char*);		//产生以字符指针为操作数的中间代码
	BOOL GenerateCode(ZOP,int);			//产生以整数为操作数的中间代码
	BOOL GenerateCode(ZOP,double);		//产生以实数为操作数的中间代码
	BOOL GenerateCode(ZOP,int,int);		//产生有两个整型数为参数的中间代码
	BOOL GenerateCode(ZOP,int,char*);	//产生以以整数、字符指针为操作数的中间代码
	BOOL RecycleMemory();				//回收局部变量和数组所占用的内存
	BOOL RecycleArrayMemory();			//回收所有数组所占用的内存
	BOOL Compile();						//编译主函数
	BOOL DoFunctionHead();				//处理 函数头
	BOOL DoFunction();					//处理 函数
	BOOL DoStatement();					//处理 语句
	BOOL DoDefine();					//处理 定义语句
	BOOL DoInput();						//处理 输入语句
	BOOL DoOutput();					//处理 输出语句
	BOOL DoIf();						//处理 选择语句
	BOOL DoSwitch();					//处理 开关语句
	BOOL DoMultiStatement();			//处理 复合语句
	BOOL DoJump();						//处理 跳转语句
	BOOL DoBecomes();					//处理 赋值语句
	BOOL DoWhile();						//处理 while循环
	BOOL DoCall();						//处理 函数调用
	BOOL DoDoWhile();					//处理 do-while循环
	BOOL DoExpression();				//处理 表达式
	BOOL DoTerm();						//处理 项
	BOOL DoFactor();					//处理 因子
	BOOL DoSubscript();					//处理 数组下标
	BOOL DoCondition();					//处理 条件
	BOOL DoAndCondition();				//处理 条件与项
	BOOL DoSubCondition();				//处理 条件子项
	BOOL DoCase();						//处理 开关语句的case分支
	BOOL DoDefault();					//处理 开关语句的default分支
	BOOL DoEndProgram();				//处理 编译的后期工作
	void DoOptimize();					//进行代码优化工作
	BOOL DoMathFunctionCall();			//处理 除abs,pow外的数学库函数调用
	BOOL DoAbsCall();					//处理 abs函数调用
	BOOL DoPowCall();					//处理 pow函数调用
	BOOL DoStrcmpCall();				//处理 strcmp函数调用
	BOOL DoStrcpyCall();				//处理 strcpy函数调用
	BOOL DoStrlenCall();				//处理 strlen函数调用
	BOOL DoStrlwruprCall();				//处理 strlwr,strupr函数调用
	BOOL DoArrayFunctionCall();			//处理 除downsort,upsort外的数组库函数调用
	BOOL DoSortCall();					//处理 downsort,upsort函数调用
private://解释执行参数/////////////////////////////////////////////////////////
	BOOL m_bCalcRuntime;		//是否统计运行时间
	BOOL m_bFileOutput;			//是否输出到文件
	BOOL m_bIgnoreCircleNoend;	//是否忽略可能的死循环
	BOOL m_bOutputInput;		//是否输入回显
	CString m_strOutputFile;	//输出文件名
	int m_nPrecision;			//输出精度
	int m_nMemoryApply;			//申请的虚内存量
private://翻译选项参数/////////////////////////////////////////////////////////
	ofstream m_fout;		//文件输出
	int m_nLevel;			//缩进级别
	BOOL m_bMainFunction;	//是否是主函数

	int m_iBackChoice;		//缩进方式
	int m_iBackNumber;		//缩进数目
	BOOL m_bKeepComment;	//是否保留注释
	BOOL m_bEmptyLine;		//是否函数间空行
	BOOL m_bInsertSpace;	//是否运算符前后加空格
	BOOL m_bOutPrecision;	//是否输出精度控制
	BOOL m_bRunTime;		//是否统计运行时间
	BOOL m_bWaitEnd;		//是否等待用户响应结束
	CString m_strCppFile;	//翻译目标文件名
private://翻译函数/////////////////////////////////////////////////////////////
	BOOL Translate();			//翻译主函数
	void TrsAndCondition();		//翻译 条件与项
	void TrsCondition();		//翻译 条件
	void TrsExpression();
	void TrsDefine();
	void TrsDoWhile();
	void TrsFactor();
	void TrsFunction();			//翻译 函数
	void TrsIf();
	void TrsInput();
	void TrsJump();
	void TrsMultiStatement();	//翻译 复合语句
	void TrsOutput();
	void TrsParameter();		//翻译 函数调用的参数部分
	void TrsStatement();		//翻译 语句
	void TrsSubCondition();		//翻译 条件子项
	void TrsSwitch();
	void TrsTerm();
	void TrsWhile();
	void OutOperator(char[]);
	void Indent();				//缩进
	void TrsMathFunctionCall();		//处理 除abs,pow外的数学库函数调用
	void TrsAbsCall();				//处理 abs函数调用
	void TrsPowCall();				//处理 pow函数调用
	void TrsStrcmpCall();			//处理 strcmp函数调用
	void TrsStrcpyCall();			//处理 strcpy函数调用
	void TrsStrlenCall();			//处理 strlen函数调用
	void TrsStrlwruprCall();		//处理 strlwr,strupr函数调用
	void TrsArrayFunctionCall();	//处理 除downsort,upsort外的数组库函数调用
	void TrsSortCall();				//处理 downsort,upsort函数调用
};

#ifndef _DEBUG  // debug version in FCView.cpp
inline CFCDoc* CFCView::GetDocument()
   { return (CFCDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FCVIEW_H__6E7AA227_F3C3_41BD_8256_0F116FC0C188__INCLUDED_)
