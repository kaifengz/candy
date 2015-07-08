// FCErrorReportDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FC.h"
#include "FCErrorReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCErrorReportDlg dialog


CFCErrorReportDlg::CFCErrorReportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFCErrorReportDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFCErrorReportDlg)
	m_bIgnoreAllError = FALSE;
	m_strLineString = _T("");
	m_strLineNumber = _T("");
	m_msg = _T("");
	//}}AFX_DATA_INIT
}

void CFCErrorReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCErrorReportDlg)
	DDX_Check(pDX, IDC_IGNORE_ALL_ERROR, m_bIgnoreAllError);
	DDX_Text(pDX, IDC_LINE_STRING, m_strLineString);
	DDX_Text(pDX, IDC_STATIC_LINE_NUMBER, m_strLineNumber);
	DDX_Text(pDX, IDC_STATIC_ERROR_MESSAGE, m_msg);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFCErrorReportDlg, CDialog)
	//{{AFX_MSG_MAP(CFCErrorReportDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCErrorReportDlg message handlers
int CFCErrorReportDlg::DoModal(int iLine,CString strLine,ZET et)
{
	m_strLineNumber.Format("Դ����� %d �У�",iLine);
	m_strLineString=strLine;
	switch(et)
	{
	case ET_DEFINEDFUNCTION:
		m_msg="�����Ѿ����壡";
		break;
	case ET_DEFINEDIDENT:
		m_msg="��ʶ���Ѿ����壡";
		break;
	case ET_DIFPARAMNUM:
		m_msg="����������Ŀ��Ԥ����ʱ����ͬ��";
		break;
	case ET_DIFPARAMTYPE:
		m_msg="��������������Ԥ����ʱ����ͬ��";
		break;
	case ET_DIFRETURNTYPE:
		m_msg="��������������Ԥ����ʱ����ͬ��";
		break;
	case ET_DONOWHILE:
		m_msg="do-whileѭ�����ȱ��while��";
		break;
	case ET_DOUBLEMOD:
		m_msg="�������μ���ģ���㣡";
		break;
	case ET_DOUBLESUB:
		m_msg="ʵ������ʵ�����ʽ����Ϊ�����±꣡";
		break;
	case ET_DOUBLESWITCH:
		m_msg="����������ѡ����䣡";
		break;
	case ET_EMPTYSTATE:
		m_msg="�����п���䣡";
		break;
	case ET_FUNCTIONUS:
		m_msg="����û��ʵ�ֵĺ�����";
		break;
	case ET_INPUTNACARRAY:
		m_msg="�޷�������ַ����飡";
		break;
	case ET_LONGSTR2ARRAY:
		m_msg="���ڳ�ʼ���ַ�������ַ���̫����";
		break;
	case ET_MISSBEGIN:
		m_msg="ȱ�����š�{����";
		break;
	case ET_MISSCOLON:
		m_msg="ȱ��ð�š�:����";
		break;
	case ET_MISSCOMMA:
		m_msg="ȱ�ٶ��š�,����";
		break;
	case ET_MISSEND:
		m_msg="ȱ�����š�}����";
		break;
	case ET_MISSIDENT:
		m_msg="ȱ�ٱ�ʶ����";
		break;
	case ET_MISSLPAREN:
		m_msg="ȱ�����š�(����";
		break;
	case ET_MISSLSUB:
		m_msg="ȱ�������š�[����";
		break;
	case ET_MISSRPAREN:
		m_msg="ȱ�������š�)����";
		break;
	case ET_MISSRSUB:
		m_msg="ȱ�������š�]����";
		break;
	case ET_MISSSEMICOLON:
		m_msg="ȱ�ٷֺš�;����";
		break;
	case ET_MUSTCHANGETYPE:
		m_msg="�������ǿ������ת����";
		break;
	case ET_MUSTRETURNVALUE:
		m_msg="�������ͷ�void���������з���ֵ��";
		break;
	case ET_NOARRIDENTARR:
		m_msg="�Ѳ�������ı�ʶ����Ϊ���������";
		break;
	case ET_NOMAIN:
		m_msg="�Ҳ���main������";
		break;
	case ET_NOVOIDMAIN:
		m_msg="main�����������Ͳ���void��";
		break;
	case ET_PARAMMAIN:
		m_msg="main�������в�����";
		break;
	case ET_OUTPUTNCARRAY:
		m_msg="��������鲻���ַ����飡";
		break;
	case ET_SOURCENOEND:
		m_msg="Դ�����쳣������";
		break;
	case ET_STRINGNOEND:
		m_msg="�ַ����쳣������";
		break;
	case ET_UNKNOWNARRAYSIZE:
		m_msg="δ֪�����鳤�ȣ�";
		break;
	case ET_UNKNOWNERROR:
		m_msg="δ֪���﷨�������";
		break;
	case ET_UNKNOWNFUNCDEF:
		m_msg="δ֪�ĺ���������ʽ��";
		break;
	case ET_UNKNOWNIDENT:
		m_msg="δ����ı�ʶ����";
		break;
	case ET_UNKNOWNOP:
		m_msg="δ֪�Ĳ�����";
		break;
	case ET_UNKNOWNSTATE:
		m_msg="δ֪�������ʽ��";
		break;
	case ET_UNKNOWNSYSCHAR:
		m_msg="δ֪��ת���ַ���";
		break;
	case ET_UNKNOWNTOKEN:
		m_msg="�Ƿ����ţ�";
		break;
	case ET_VOIDIDENT:
		m_msg="��ʶ�����Ͳ���Ϊ��void����";
		break;
	case ET_VOIDINEXPRESSION:
		m_msg="���ʽ�е����˷�������Ϊvoid�ĺ�����";
		break;
	case ET_VOIDRETURNVALUE:
		m_msg="��������Ϊvoid�ĺ��������з���ֵ��";
		break;
	case ET_WRONGBREAK:
		m_msg="�����break��䣡";
		break;
	case ET_WRONGCHARACTER:
		m_msg="������ַ���ʾ��";
		break;
	case ET_WRONGCONTINUE:
		m_msg="�����continue��䣡";
		break;
	case ET_WRONGIDENTTYPE:
		m_msg="����ı������ͣ�";
		break;
	case ET_WRONGINIT:
		m_msg="����ı����������ʼ����";
		break;
	case ET_WRONGPARAMNUM:
		m_msg="��������ʱ������Ŀ����ȷ��";
		break;
	case ET_WRONGPARAMTYPE:
		m_msg="��������ʱ�������Ͳ���ȷ��";
		break;
	case ET_WRONGRETURNTYPE:
		m_msg="����ĺ����������ͣ�";
		break;
	default:
		m_msg="δ֪���﷨���������";
		break;
	}
	MessageBeep(1);
	return CDialog::DoModal();
}
