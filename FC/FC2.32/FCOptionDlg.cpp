// FCOptionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FC.h"
#include "FCOptionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCOptionDlg dialog


CFCOptionDlg::CFCOptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFCOptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFCOptionDlg)
	m_bAllowEmptyStatement = FALSE;
	m_bAutoChangeType = FALSE;
	m_bDisableChangeType = FALSE;
	m_bFileOutput = FALSE;
	m_bGenerateBinary = FALSE;
	m_bGenerateText = FALSE;
	m_bIncludeArray = FALSE;
	m_bIncludeMath = FALSE;
	m_bIncludeString = FALSE;
	m_bJumpOptimize = FALSE;
	m_bOutRuntime = FALSE;
	m_nPrecision = 0;
	m_bReportError = FALSE;
	m_strOutputFile = "";
	m_strMemoryApply = "8";
	m_bIgnoreCircleNoend = FALSE;
	m_bOutputInput = FALSE;
	//}}AFX_DATA_INIT
}

void CFCOptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCOptionDlg)
	DDX_Check(pDX, IDC_ALLOW_EMPTY_STATEMENT, m_bAllowEmptyStatement);
	DDX_Check(pDX, IDC_AUTO_CHANGE_TYPE, m_bAutoChangeType);
	DDX_Check(pDX, IDC_DISABLE_CHANGE_TYPE, m_bDisableChangeType);
	DDX_Check(pDX, IDC_FILE_OUTPUT, m_bFileOutput);
	DDX_Check(pDX, IDC_GENERATE_BINARY, m_bGenerateBinary);
	DDX_Check(pDX, IDC_GENERATE_TEXT, m_bGenerateText);
	DDX_Check(pDX, IDC_INCLUDE_ARRAY, m_bIncludeArray);
	DDX_Check(pDX, IDC_INCLUDE_MATH, m_bIncludeMath);
	DDX_Check(pDX, IDC_INCLUDE_STRING, m_bIncludeString);
	DDX_Check(pDX, IDC_JUMP_OPTIMIZE, m_bJumpOptimize);
	DDX_Check(pDX, IDC_OUTPUT_RUNTIME, m_bOutRuntime);
	DDX_Text(pDX, IDC_PRECISION, m_nPrecision);
	DDX_Check(pDX, IDC_REPORT_ERROR, m_bReportError);
	DDX_Text(pDX, IDC_OUTPUT_FILE, m_strOutputFile);
	DDX_CBString(pDX, IDC_MEMORY_APPLY, m_strMemoryApply);
	DDX_Check(pDX, IDC_IGNORE_CIRCLE_NOEND, m_bIgnoreCircleNoend);
	DDX_Check(pDX, IDC_OUTPUT_INPUT, m_bOutputInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFCOptionDlg, CDialog)
	//{{AFX_MSG_MAP(CFCOptionDlg)
	ON_BN_CLICKED(IDC_DEFAULT_OPTION, OnDefaultOption)
	ON_BN_CLICKED(IDC_FILE_OUTPUT, OnFileOutput)
	ON_BN_CLICKED(IDC_AUTO_CHANGE_TYPE, OnAutoChangeType)
	ON_BN_CLICKED(IDC_DISABLE_CHANGE_TYPE, OnDisableChangeType)
	ON_EN_KILLFOCUS(IDC_PRECISION, OnKillfocusPrecision)
	ON_CBN_KILLFOCUS(IDC_MEMORY_APPLY, OnKillfocusMemoryApply)
	ON_BN_CLICKED(IDC_IGNORE_CIRCLE_NOEND, OnIgnoreCircleNoend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCOptionDlg message handlers

BOOL CFCOptionDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	GetDlgItem(IDC_AUTO_CHANGE_TYPE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DISABLE_CHANGE_TYPE)->EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFCOptionDlg::OnDefaultOption() 
{
	m_bAllowEmptyStatement = FALSE;
	m_bAutoChangeType = FALSE;
	m_bDisableChangeType = FALSE;
	m_bFileOutput = FALSE;
	m_bGenerateBinary = TRUE;
#ifdef _DEBUG //debug�汾ȱʡ�����м�����ı��ļ�
	m_bGenerateText = TRUE;
#else
	m_bGenerateText = FALSE;
#endif
	m_bIgnoreCircleNoend = FALSE;
	m_bIncludeArray = TRUE;
	m_bIncludeMath = TRUE;
	m_bIncludeString = TRUE;
	m_bJumpOptimize = TRUE;
	m_bOutRuntime = TRUE;
	m_bOutputInput =TRUE;
	m_bReportError = TRUE;
	m_nPrecision = 6;
	m_strMemoryApply = "16";
	m_strOutputFile = "";

	UpdateData(FALSE);
}

void CFCOptionDlg::OnFileOutput() 
{
	UpdateData(TRUE);
	if(m_bFileOutput)
	{
		CFileDialog dlg(FALSE,"txt","",
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			"�ı��ļ� (*.txt)|*.txt|�����ļ� (*.*)|*.*||");
		if(dlg.DoModal()==IDOK)
			m_strOutputFile = dlg.GetPathName();
		else
			m_bFileOutput=FALSE;
	}
	else
		m_strOutputFile="";
	UpdateData(FALSE);
}

void CFCOptionDlg::OnAutoChangeType() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_DISABLE_CHANGE_TYPE)->EnableWindow(!m_bAutoChangeType);
}

void CFCOptionDlg::OnDisableChangeType() 
{
	UpdateData(TRUE);
	GetDlgItem(IDC_AUTO_CHANGE_TYPE)->EnableWindow(!m_bDisableChangeType);
}

void CFCOptionDlg::OnKillfocusPrecision() 
{
	UpdateData(TRUE);
	if(m_nPrecision>15)
		m_nPrecision=15;
	UpdateData(FALSE);
}

void CFCOptionDlg::OnKillfocusMemoryApply() 
{
	CString old=m_strMemoryApply;
	UpdateData(TRUE);
	for(int i=m_strMemoryApply.GetLength()-1;i>=0;i--)
		if(m_strMemoryApply[i]<'0' || m_strMemoryApply>'9')
		{
			MessageBox("��������ȷ�������ڴ棡","FC",MB_OK|MB_ICONEXCLAMATION);
			GetDlgItem(IDC_MEMORY_APPLY)->SetFocus();
			((CComboBox*)(GetDlgItem(IDC_MEMORY_APPLY)))->SetEditSel(0,-1);
			return;
		}
	if(old==m_strMemoryApply)
		return;
	i=atoi(m_strMemoryApply);
	if(i==0)
	{
		MessageBox("��������ȷ�������ڴ棡","FC",MB_OK|MB_ICONEXCLAMATION);
		GetDlgItem(IDC_MEMORY_APPLY)->SetFocus();
		((CComboBox*)(GetDlgItem(IDC_MEMORY_APPLY)))->SetEditSel(0,-1);
		return;
	}
	if(i<4 || i>=1024)
	{
		CString message;
		if(i<4)
			message="���������ֵ̫С�������޷�����ִ�У�";
		else
			message="���������ֵ̫�󣬿����޷����뵽�㹻���ڴ棡";
		if(MessageBox(message+"\nҪ������","FC",MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			GetDlgItem(IDC_MEMORY_APPLY)->SetFocus();
			((CComboBox*)(GetDlgItem(IDC_MEMORY_APPLY)))->SetEditSel(0,-1);
			return;
		}
	}
//	QUESTION: ����������������룬������ֵ��3��֮������������Ի���ġ��ǡ���ť��
//	��ֵ���Զ���ɡ�32�����������ȥ���룬������֡�0016���Ȳ��淶����ֵ��ʾ
//	m_strMemoryApply.Format("%d",i);
//	UpdateData(FALSE);
}

void CFCOptionDlg::OnOK() 
{
	CDialog::OnOK();
}


void CFCOptionDlg::OnIgnoreCircleNoend() 
{
	UpdateData(TRUE);
	if(m_bIgnoreCircleNoend)
	{
		if(MessageBox("������Կ��ܵ���ѭ��������棬�������ִ��ʱ��������ѭ����ֹͣ��Ӧ��\n\
\n\
ȷ�ϲ��Կ��ܵ���ѭ�����������","FC",MB_YESNO|MB_ICONQUESTION)==IDNO)
		{
			m_bIgnoreCircleNoend=FALSE;
			UpdateData(FALSE);
		}
	}
}
