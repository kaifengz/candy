// FCTranslateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FC.h"
#include "FCTranslateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCTranslateDlg dialog


CFCTranslateDlg::CFCTranslateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFCTranslateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFCTranslateDlg)
	m_iBackNumber = 4;
	m_bEmptyLine = TRUE;
	m_bInsertSpace = FALSE;
	m_strObjectFile = "";
	m_bOutPrecision = FALSE;
	m_bRunTime = FALSE;
	m_bWaitEnd = TRUE;
	m_iBackChoice = 0;
	m_bKeepComment = TRUE;
	//}}AFX_DATA_INIT
}

void CFCTranslateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCTranslateDlg)
	DDX_CBIndex(pDX, IDC_BACK_NUMBER, m_iBackNumber);
	DDX_Check(pDX, IDC_EMPTY_LINE, m_bEmptyLine);
	DDX_Check(pDX, IDC_INSERT_SPACE, m_bInsertSpace);
	DDX_Text(pDX, IDC_OBJECT_FILE, m_strObjectFile);
	DDX_Check(pDX, IDC_OUT_PRECISION, m_bOutPrecision);
	DDX_Check(pDX, IDC_RUN_TIME, m_bRunTime);
	DDX_Check(pDX, IDC_WAIT_END, m_bWaitEnd);
	DDX_Radio(pDX, IDC_TAB_BACK, m_iBackChoice);
	DDX_Check(pDX, IDC_KEEP_COMMENT, m_bKeepComment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFCTranslateDlg, CDialog)
	//{{AFX_MSG_MAP(CFCTranslateDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_DEFAULT_SET, OnDefaultSet)
	ON_BN_CLICKED(IDC_TAB_BACK, OnTabBack)
	ON_BN_CLICKED(IDC_SPACE_BACK, OnSpaceBack)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCTranslateDlg message handlers

void CFCTranslateDlg::OnBrowse() 
{
	UpdateData(TRUE);
	CFileDialog dlg(FALSE,"cpp",m_strObjectFile,
		OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"CPP Source File (*.cpp)|*.cpp|All Files (*.*)|*.*||");
	if(dlg.DoModal()!=IDOK)
		return;
	m_strObjectFile = dlg.m_ofn.lpstrFile;
	UpdateData(FALSE);
}

void CFCTranslateDlg::OnDefaultSet() 
{
	//翻译选项参数
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
	GetDlgItem(IDC_BACK_NUMBER)->EnableWindow(FALSE);
	UpdateData(FALSE);
}

void CFCTranslateDlg::OnOK() 
{
	UpdateData(TRUE);
	CFile fout;//(m_strObjectFile,CFile::modeWrite);
	if(!fout.Open(m_strObjectFile,CFile::modeWrite|CFile::modeCreate))
	{
		MessageBox("无法打开或创建文件！","FC",MB_OK|MB_ICONEXCLAMATION);
		return;
	}
	fout.Close();
	CDialog::OnOK();
}

void CFCTranslateDlg::OnTabBack() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BACK_NUMBER)->EnableWindow(m_iBackChoice==1);
}

void CFCTranslateDlg::OnSpaceBack() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_BACK_NUMBER)->EnableWindow(m_iBackChoice==1);
}

BOOL CFCTranslateDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_BACK_NUMBER)->EnableWindow(m_iBackChoice==1);
	return TRUE;
}

int CFCTranslateDlg::DoModal(CString strFCFileName,BOOL bInsertSpace,
	BOOL bEmptyLine,int iBackChoice,int iBackNumber,
	BOOL bKeepComment,BOOL bOutPrecision,BOOL bRunTime,BOOL bWaitEnd)
{
	m_strObjectFile=strFCFileName+".cpp";
	m_bInsertSpace=bInsertSpace;
	m_bEmptyLine=bEmptyLine;
	m_iBackChoice=iBackChoice;
	m_iBackNumber=iBackNumber;
	m_bKeepComment=bKeepComment;
	m_bOutPrecision=bOutPrecision;
	m_bRunTime=bRunTime;
	m_bWaitEnd=bWaitEnd;
	return CDialog::DoModal();
}
