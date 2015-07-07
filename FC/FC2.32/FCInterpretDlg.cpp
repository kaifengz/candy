// FCInterpretDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FC.h"
#include "FCInterpretDlg.h"

#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCInterpretDlg dialog


CFCInterpretDlg::CFCInterpretDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFCInterpretDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFCInterpretDlg)
	//}}AFX_DATA_INIT
}


void CFCInterpretDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCInterpretDlg)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFCInterpretDlg, CDialog)
	//{{AFX_MSG_MAP(CFCInterpretDlg)
	ON_COMMAND(ID_INTERPRET, OnInterpret)
	ON_COMMAND(ID_SAVE_OUTPUT, OnSaveOutput)
	ON_COMMAND(ID_PRESICION_0, OnPresicion0)
	ON_COMMAND(ID_PRECISION_6, OnPrecision6)
	ON_COMMAND(ID_PRECISION_15, OnPrecision15)
	ON_COMMAND(ID_EXIT_DLG, OnExitDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCInterpretDlg message handlers

void CFCInterpretDlg::DoModal(const ZCode *pCode,int nCode,BOOL bCalcRuntime,
		BOOL bFileOutput,BOOL bIgnoreCircleNoend,BOOL bOutputInput,
		CString strOutputFile,int &nPrecision,int nMemoryApply,CString strFileTitle)
{
	m_pCode				= pCode;
	m_nCode				= nCode;
	m_bCalcRuntime		= bCalcRuntime;
	m_bFileOutput		= bFileOutput;
	m_bIgnoreCircleNoend= bIgnoreCircleNoend;
	m_bOutputInput		= bOutputInput;
	m_strOutputFile		= strOutputFile;
	m_nPrecision		= nPrecision;
	m_nMemoryApply		= nMemoryApply;
	m_strFileTitle		= strFileTitle;

	m_lpszOutput = NULL;

	CDialog::DoModal();

	nPrecision			= m_nPrecision;
}

int CFCInterpretDlg::DoModal()
{
	return CDialog::DoModal();
}

BOOL CFCInterpretDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// 使窗口居中
	CRect rcParentWnd, rcThisDlg;
	GetParent()->GetWindowRect(&rcParentWnd);
	GetWindowRect(&rcThisDlg);
	MoveWindow(
		(rcParentWnd.Width()-rcThisDlg.Width())/2,
		(rcParentWnd.Height()-rcThisDlg.Height())/2,
		rcThisDlg.Width(),
		rcThisDlg.Height(),
		FALSE);

	ShowWindow(SW_SHOW);
	PostMessage(WM_COMMAND, ID_INTERPRET, 0);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFCInterpretDlg::OnInterpret() 
{
	Interpret();
}

void CFCInterpretDlg::OnSaveOutput() 
{
	CFileDialog dlg(FALSE,"txt",m_strFileTitle+" 运行结果",
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"文本文件 (*.txt)|*.txt|所有文件 (*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	std::ofstream fout(dlg.GetFileName(),std::ios::out);
	fout.write(m_lpszOutput, m_nOutputLength);
	fout.close();
}

void CFCInterpretDlg::OnPresicion0() 
{
	if(m_nPrecision!=0)
	{
		m_nPrecision=0;
		if(MessageBox("是否要重新执行程序？","FC",MB_YESNO|MB_ICONQUESTION)==IDYES)
			Interpret();
	}
}

void CFCInterpretDlg::OnPrecision6() 
{
	if(m_nPrecision!=6)
	{
		m_nPrecision=6;
		if(MessageBox("是否要重新执行程序？","FC",MB_YESNO|MB_ICONQUESTION)==IDYES)
			Interpret();
	}
}

void CFCInterpretDlg::OnPrecision15() 
{
	if(m_nPrecision!=15)
	{
		m_nPrecision=15;
		if(MessageBox("是否要重新执行程序？","FC",MB_YESNO|MB_ICONQUESTION)==IDYES)
			Interpret();
	}
}

void CFCInterpretDlg::OnExitDlg() 
{
	CDialog::OnCancel();
}
