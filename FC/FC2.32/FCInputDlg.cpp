// FCInputDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FC.h"
#include "FCInputDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFCInputDlg dialog


CFCInputDlg::CFCInputDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFCInputDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFCInputDlg)
	m_strInformation = _T("");
	m_strInput = _T("");
	//}}AFX_DATA_INIT
}


void CFCInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFCInputDlg)
	DDX_Text(pDX, IDC_INFORMATION, m_strInformation);
	DDX_Text(pDX, IDC_INPUT, m_strInput);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFCInputDlg, CDialog)
	//{{AFX_MSG_MAP(CFCInputDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFCInputDlg message handlers

void CFCInputDlg::OnOK() 
{
	UpdateData(TRUE);

	int i,length=m_strInput.GetLength();
	int sign=1;

	switch(m_im)
	{
	case IM_CHAR:
		for(i=0;i<length && (m_strInput[i]==' ' || m_strInput[i]=='\t');i++)
			;
		if(i<length)
		{
			m_cInput=m_strInput[i];
			for(i++;i<length && (m_strInput[i]==' ' || m_strInput[i]=='\t');i++)
				;
			if(i==length)
				break;
		}
		MessageBox("请输入一个字符！","FC",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_INPUT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_INPUT))->SetSel(0,-1);
		return;
	case IM_INT:
		for(i=0;i<length && (m_strInput[i]==' ' || m_strInput[i]=='\t');i++)
			;
		if(i<length)
		{
			if(m_strInput[i]=='+')
				i++;
			else if(m_strInput[i]=='-')
			{
				i++;
				sign=-1;
			}
		}
		if(i<length)
		{
			m_iInput=0;
			for(;i<length && m_strInput[i]>='0' && m_strInput[i]<='9';i++)
			{
				m_iInput*=10;
				m_iInput+=m_strInput[i]-'0';
			}
			for(;i<length && (m_strInput[i]==' ' || m_strInput[i]=='\t');i++)
				;
			if(i==length)
			{
				m_iInput*=sign;
				break;
			}
		}
		MessageBox("请输入一个整数！","FC",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_INPUT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_INPUT))->SetSel(0,-1);
		return;
	case IM_DOUBLE:
		for(i=0;i<length && (m_strInput[i]==' ' || m_strInput[i]=='\t');i++)
			;
		if(i<length)
		{
			if(m_strInput[i]=='+')
				i++;
			else if(m_strInput[i]=='-')
			{
				i++;
				sign=-1;
			}
		}
		if(i<length)
		{
			m_iInput=0;
			for(;i<length && m_strInput[i]>='0' && m_strInput[i]<='9';i++)
			{
				m_iInput*=10;
				m_iInput+=m_strInput[i]-'0';
			}
			if(m_strInput[i]=='.')
			{
				double l=1;
				m_dInput=0;
				for(i++;i<length && m_strInput[i]>='0' && m_strInput[i]<='9';i++)
				{
					m_dInput*=10;
					m_dInput+=m_strInput[i]-'0';
					l*=10;
				}
				m_dInput/=l;
				m_dInput+=m_iInput;
			}
			else
				m_dInput=m_iInput;
			for(;i<length && (m_strInput[i]==' ' || m_strInput[i]=='\t');i++)
				;
			if(i==length)
			{
				m_dInput*=sign;
				break;
			}
		}
		MessageBox("请输入一个实数！","FC",MB_OK|MB_ICONINFORMATION);
		GetDlgItem(IDC_INPUT)->SetFocus();
		((CEdit*)GetDlgItem(IDC_INPUT))->SetSel(0,-1);
		return;
	case IM_STRING:
		break;
	}

	CDialog::OnOK();
}

int CFCInputDlg::InputChar(char &c)
{
	m_im=IM_CHAR;
	m_strInformation="请输入一个字符：";
	int result=CDialog::DoModal();
	c=m_cInput;
	return result;
}

int CFCInputDlg::InputInt(int &i)
{
	m_im=IM_INT;
	m_strInformation="请输入一个整数：";
	int result=CDialog::DoModal();
	i=m_iInput;
	return result;
}

int CFCInputDlg::InputDouble(double &d)
{
	m_im=IM_DOUBLE;
	m_strInformation="请输入一个实数：";
	int result=CDialog::DoModal();
	d=m_dInput;
	return result;
}

int CFCInputDlg::InputString(CString &string)
{
	m_im=IM_STRING;
	m_strInformation="请输入一行字符串：";
	int result=CDialog::DoModal();
	string=m_strInput;
	return result;
}
