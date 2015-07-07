// FCView.cpp : implementation of the CFCView class
//

#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"
#include "FCView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCView

IMPLEMENT_DYNCREATE(CFCView, CEditView)

BEGIN_MESSAGE_MAP(CFCView, CEditView)
	//{{AFX_MSG_MAP(CFCView)
	ON_COMMAND(ID_COMPILE_COMPILE, OnCompileCompile)
	ON_COMMAND(ID_COMPILE_INTERPRET, OnCompileInterpret)
	ON_COMMAND(ID_COMPILE_TRANSLATE, OnCompileTranslate)
	ON_COMMAND(ID_COMPILE_REPORT_ERROR, OnCompileReportError)
	ON_UPDATE_COMMAND_UI(ID_COMPILE_REPORT_ERROR, OnUpdateCompileReportError)
	ON_COMMAND(ID_OPTION, OnOption)
	ON_COMMAND(ID_HELP_GRAMMAR, OnHelpGrammar)
	ON_COMMAND(ID_HELP_GLOBAL, OnHelpGlobal)
	ON_COMMAND(ID_COMPILE_EXECUTE, OnCompileExecute)
	ON_COMMAND(ID_COMPILE_REBUILD, OnCompileRebuild)
	ON_COMMAND(ID_EXAMPLE_HELLOWORLD, OnExampleHelloworld)
	ON_COMMAND(ID_EXAMPLE_PERMUTATION, OnExamplePermutation)
	ON_COMMAND(ID_EXAMPLE_PRIME, OnExamplePrime)
	ON_COMMAND(ID_EXAMPLE_PI, OnExamplePi)
	ON_COMMAND(ID_EXAMPLE_HANIO, OnExampleHanio)
	ON_COMMAND(ID_EXAMPLE_FIBONACCI, OnExampleFibonacci)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCView construction/destruction

CFCView::CFCView()
{
	//库函数包含与否标志
	m_bIncludeMath=TRUE;
	m_bIncludeString=TRUE;
	m_bIncludeArray=TRUE;
	//生成中间代码文件参数
	m_bGenerateBinary=TRUE;
#ifdef _DEBUG
	m_bGenerateText=TRUE;
#else
	m_bGenerateText=FALSE;
#endif
	//编译选项参数
	m_bAutoChangeType=FALSE;
	m_bDisableChangeType=FALSE;
	m_bAllowEmptyStatement=FALSE;
	m_bJumpOptimize=TRUE;
	//词法分析参数
	m_char='\0';
	m_iCharCount=0;
	m_int=0;
	m_nSourceLength=0;
	m_double=0;
	m_source="";
	m_ident="";
	m_string="";
	m_comment="";
	m_element=E_NULL;
	//语法分析参数
	m_iErrorCount=0;
	m_bIgnoreError=FALSE;
	m_bCompiled=FALSE;
	m_pFunction=NULL;
	m_nFunction=0;
	m_nMaxFunction=0;
	m_nCurFunction=0;
	m_pCode=NULL;
	m_nCode=0;
	m_nMaxCode=0;
	m_it=IT_NULL;
	m_nBreak=-1;
	m_nContinue=-1;
	m_nTrue=-1;
	m_nFalse=-1;
	m_nCase=-1;
	//解释执行参数
	m_bCalcRuntime=TRUE;
	m_bFileOutput=FALSE;
	m_bIgnoreCircleNoend=FALSE;
	m_bOutputInput=TRUE;
	m_strOutputFile="";
	m_nPrecision=6;
	m_nMemoryApply=16;
	//翻译选项参数
//	m_fout=NULL;//不能初始化为NULL，否则该变量后边将无法使用
	m_nLevel=0;
	m_bMainFunction=FALSE;
	m_iBackChoice=0;
	m_iBackNumber=4;
	m_bEmptyLine=TRUE;
#ifdef _DEBUG
	m_bKeepComment=FALSE;
	m_bWaitEnd=FALSE;
#else
	m_bKeepComment=TRUE;
	m_bWaitEnd=TRUE;
#endif
	m_bInsertSpace=FALSE;
	m_bOutPrecision=FALSE;
	m_bRunTime=FALSE;
	m_strCppFile="";

#ifndef _DEBUG
	ReadOptions();
#endif
}

CFCView::~CFCView()
{
#ifndef _DEBUG
	SaveOptions();
#endif
}

BOOL CFCView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
//	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// Enable word-wrapping

	return bPreCreated;
}

/////////////////////////////////////////////////////////////////////////////
// CFCView drawing

void CFCView::OnDraw(CDC* pDC)
{
	CFCDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CFCView diagnostics

#ifdef _DEBUG
void CFCView::AssertValid() const
{
	CEditView::AssertValid();
}

void CFCView::Dump(CDumpContext& dc) const
{
	CEditView::Dump(dc);
}

CFCDoc* CFCView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CFCDoc)));
	return (CFCDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CFCView message handlers

void CFCView::OnInitialUpdate() 
{
	CEditView::OnInitialUpdate();

	//设置字体
	CFont fixedsys;
	fixedsys.CreateStockObject(SYSTEM_FIXED_FONT);
	SetFont(&fixedsys,FALSE);
	//设置tab
	LOGFONT lf;
	fixedsys.GetLogFont(&lf);
	SetTabStops(2*lf.lfWidth);
	//设置边界空白
	GetEditCtrl().SetMargins(2,2);
}

BOOL CFCView::SaveResToFile(CString strFile, UINT nIdResource, LPCSTR lpszResType)
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	HRSRC hResourceInfo = ::FindResource(hInstance, (LPCSTR)nIdResource, lpszResType);
	if(hResourceInfo)
	{
		int size = ::SizeofResource(hInstance, hResourceInfo);
		HGLOBAL hResource = ::LoadResource(hInstance, hResourceInfo);
		if(hResource)
		{
			LPSTR lpRes = (LPSTR)LockResource(hResource);
			if(lpRes)
			{
				CFile file(strFile, CFile::modeCreate|CFile::modeWrite|CFile::typeBinary);
				file.Write(lpRes, size);
				file.Close();

				UnlockResource(hResource);
				return TRUE;
			}
		}
	}
	return FALSE;
}

void CFCView::OnExample(CString strTitle, UINT nIDExample)
{
	if(GetDocument()->IsModified())
	{
		if(IDYES==MessageBox("将改动保存到“" + GetDocument()->GetTitle() + "” ？", "FC", MB_YESNO|MB_ICONQUESTION))
		{
			SendMessage(WM_COMMAND, ID_FILE_SAVE, 0);
			if(GetDocument()->IsModified())
				return;
		}
	}
	ReadResToView(strTitle, nIDExample, "FC");
}

BOOL CFCView::ReadResToView(CString strTitle, UINT nIdResource, LPCSTR lpszResType)
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	HRSRC hResourceInfo = ::FindResource(hInstance, (LPCSTR)nIdResource, lpszResType);
	if(hResourceInfo)
	{
		int size = ::SizeofResource(hInstance, hResourceInfo);
		HGLOBAL hResource = ::LoadResource(hInstance, hResourceInfo);
		if(hResource)
		{
			LPSTR lpRes = (LPSTR)LockResource(hResource);
			if(lpRes)
			{
				CFCDoc *pDoc = GetDocument();
				pDoc->OnNewDocument();
				pDoc->SetTitle(strTitle);

				LPSTR lpString = new char[size+1];
				memcpy(lpString, lpRes, size);
				lpString[size] = 0;
				SetWindowText(lpString);

				UnlockResource(hResource);
				return TRUE;
			}
		}
	}
	return FALSE;
}
