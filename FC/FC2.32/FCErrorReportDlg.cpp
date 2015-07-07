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
	m_strLineNumber.Format("源代码第 %d 行：",iLine);
	m_strLineString=strLine;
	switch(et)
	{
	case ET_DEFINEDFUNCTION:
		m_msg="函数已经定义！";
		break;
	case ET_DEFINEDIDENT:
		m_msg="标识符已经定义！";
		break;
	case ET_DIFPARAMNUM:
		m_msg="函数参数数目与预定义时不相同！";
		break;
	case ET_DIFPARAMTYPE:
		m_msg="函数参数类型与预定义时不相同！";
		break;
	case ET_DIFRETURNTYPE:
		m_msg="函数返回类型与预定义时不相同！";
		break;
	case ET_DONOWHILE:
		m_msg="do-while循环语句缺少while！";
		break;
	case ET_DOUBLEMOD:
		m_msg="浮点数参加求模运算！";
		break;
	case ET_DOUBLESUB:
		m_msg="实数（或实数表达式）作为数组下标！";
		break;
	case ET_DOUBLESWITCH:
		m_msg="浮点数用于选择语句！";
		break;
	case ET_EMPTYSTATE:
		m_msg="不能有空语句！";
		break;
	case ET_FUNCTIONUS:
		m_msg="存在没有实现的函数！";
		break;
	case ET_INPUTNACARRAY:
		m_msg="无法输入非字符数组！";
		break;
	case ET_LONGSTR2ARRAY:
		m_msg="用于初始化字符数组的字符串太长！";
		break;
	case ET_MISSBEGIN:
		m_msg="缺少括号“{”！";
		break;
	case ET_MISSCOLON:
		m_msg="缺少冒号“:”！";
		break;
	case ET_MISSCOMMA:
		m_msg="缺少逗号“,”！";
		break;
	case ET_MISSEND:
		m_msg="缺少括号“}”！";
		break;
	case ET_MISSIDENT:
		m_msg="缺少标识符！";
		break;
	case ET_MISSLPAREN:
		m_msg="缺少括号“(”！";
		break;
	case ET_MISSLSUB:
		m_msg="缺少左括号“[”！";
		break;
	case ET_MISSRPAREN:
		m_msg="缺少右括号“)”！";
		break;
	case ET_MISSRSUB:
		m_msg="缺少右括号“]”！";
		break;
	case ET_MISSSEMICOLON:
		m_msg="缺少分号“;”！";
		break;
	case ET_MUSTCHANGETYPE:
		m_msg="必须进行强制类型转换！";
		break;
	case ET_MUSTRETURNVALUE:
		m_msg="返回类型非void函数必须有返回值！";
		break;
	case ET_NOARRIDENTARR:
		m_msg="把不是数组的标识符作为数组操作！";
		break;
	case ET_NOMAIN:
		m_msg="找不到main函数！";
		break;
	case ET_NOVOIDMAIN:
		m_msg="main函数返回类型不是void！";
		break;
	case ET_PARAMMAIN:
		m_msg="main函数带有参数！";
		break;
	case ET_OUTPUTNCARRAY:
		m_msg="输出的数组不是字符数组！";
		break;
	case ET_SOURCENOEND:
		m_msg="源代码异常结束！";
		break;
	case ET_STRINGNOEND:
		m_msg="字符串异常结束！";
		break;
	case ET_UNKNOWNARRAYSIZE:
		m_msg="未知的数组长度！";
		break;
	case ET_UNKNOWNERROR:
		m_msg="未知的语法语义错误！";
		break;
	case ET_UNKNOWNFUNCDEF:
		m_msg="未知的函数定义形式！";
		break;
	case ET_UNKNOWNIDENT:
		m_msg="未定义的标识符！";
		break;
	case ET_UNKNOWNOP:
		m_msg="未知的操作！";
		break;
	case ET_UNKNOWNSTATE:
		m_msg="未知的语句形式！";
		break;
	case ET_UNKNOWNSYSCHAR:
		m_msg="未知的转义字符！";
		break;
	case ET_UNKNOWNTOKEN:
		m_msg="非法符号！";
		break;
	case ET_VOIDIDENT:
		m_msg="标识符类型不能为“void”！";
		break;
	case ET_VOIDINEXPRESSION:
		m_msg="表达式中调用了返回类型为void的函数！";
		break;
	case ET_VOIDRETURNVALUE:
		m_msg="返回类型为void的函数不能有返回值！";
		break;
	case ET_WRONGBREAK:
		m_msg="错误的break语句！";
		break;
	case ET_WRONGCHARACTER:
		m_msg="错误的字符表示！";
		break;
	case ET_WRONGCONTINUE:
		m_msg="错误的continue语句！";
		break;
	case ET_WRONGIDENTTYPE:
		m_msg="错误的变量类型！";
		break;
	case ET_WRONGINIT:
		m_msg="错误的变量或数组初始化！";
		break;
	case ET_WRONGPARAMNUM:
		m_msg="函数调用时参数数目不正确！";
		break;
	case ET_WRONGPARAMTYPE:
		m_msg="函数调用时参数类型不正确！";
		break;
	case ET_WRONGRETURNTYPE:
		m_msg="错误的函数返回类型！";
		break;
	default:
		m_msg="未知的语法或语义错误！";
		break;
	}
	MessageBeep(1);
	return CDialog::DoModal();
}
