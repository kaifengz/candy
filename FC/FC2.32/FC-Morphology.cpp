#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"
#include "FCView.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CFCView::GetElement(BOOL bWantFileEnd)
{
	BOOL bReportWrongToken=TRUE;//此标记用于避免连续的报告“非法符号”
RestartGetElement:
	int &iCount=m_iCharCount;
	while(1)//跳过空格，tab，回车符，注释
	{
		if(iCount>=m_nSourceLength)//到达源代码尾
		{
			m_element=E_FILEEND;
			if(!bWantFileEnd)//源代码
			{
				ErrorReport(ET_SOURCENOEND);
				return FALSE;
			}
			return TRUE;
		}
		if( m_source[iCount]==' '  || m_source[iCount]=='\t' || //空白字符
			m_source[iCount]=='\r' || m_source[iCount]=='\n')	//换行字符
				iCount++;
		else if(m_source[iCount]=='/' && iCount+1<m_nSourceLength) //可能为注释
		{
			if(m_source[iCount+1]=='/')//单行注释
			{
				m_comment="//";
				for(iCount+=2;iCount<m_nSourceLength && m_source[iCount]!='\n';iCount++)
					m_comment+=m_source[iCount];
				if(iCount!=m_nSourceLength)//并没到达源文件尾
				{
					m_comment+="\n";
					iCount++;
				}
			}
			else if(m_source[iCount+1]=='*')//多行注释
			{
				m_comment="/*";
				iCount+=2;
				while(1)
				{
					if(iCount>=m_nSourceLength)//到达文件尾
					{
						m_element=E_FILEEND;
						if(!bWantFileEnd)//源代码
						{
							ErrorReport(ET_SOURCENOEND);
							return FALSE;
						}
						return TRUE;
					}
					m_comment+=m_source[iCount];
					if(m_source[iCount]=='*' && iCount+1<m_nSourceLength &&
						m_source[iCount+1]=='/') //到达注释尾
					{
						m_comment+="*/";
						iCount+=2;
						break;//多行注释结束
					}
					iCount++;
				}
			}
			else break;//不是注释
		}
		else break;//不是空白字符，且不可能是注释
	}
	if( (m_source[iCount]>='a' && m_source[iCount]<='z') ||
		(m_source[iCount]>='A' && m_source[iCount]<='Z') ||
		m_source[iCount]=='_' )
////标识符、关键字、库函数/////////////////////////////////////////////////////
	{
		m_ident="";
		//获取连续的字母、数字、下划线串
		while(iCount<m_nSourceLength &&
			  ( (m_source[iCount]>='a' && m_source[iCount]<='z') ||
				(m_source[iCount]>='A' && m_source[iCount]<='Z') ||
				(m_source[iCount]>='0' && m_source[iCount]<='9') ||
				m_source[iCount]=='_' ) )
		{
			m_ident+=m_source[iCount];
			iCount++;
		}
		if(m_ident.GetLength()>31) //只保留标识符的前31个字符
			m_ident=m_ident.Left(31);
		if(GetKeyword())
			return TRUE;
		if(GetLibraryFunction())
			return TRUE;
		m_element=E_IDENT;
		return TRUE;
	}
	if(m_source[iCount]>='0' && m_source[iCount]<='9')
////数/////////////////////////////////////////////////////////////////////////
	{
		m_int=0;
		while(iCount<m_nSourceLength && m_source[iCount]>='0' && m_source[iCount]<='9')
		{
			m_int*=10;
			m_int+=m_source[iCount]-'0';
			iCount++;
		}
		if(iCount>=m_nSourceLength || m_source[iCount]!='.')//到达源文件尾或不是小数点，即为整数
		{
			m_element=E_INUMBER;
			return TRUE;
		}
		int l=1;
		m_double=m_int;
		iCount++;//跳过小数点iCount<m_nSourceLength && 
		while(iCount<m_nSourceLength && m_source[iCount]>='0' && m_source[iCount]<='9')
		{
			m_double+=(m_source[iCount]-'0')/pow(10,l);
			l++;
			iCount++;
		}
		m_element=E_DNUMBER;
		return TRUE;
	}
	if(m_source[iCount]=='\'')
////字符///////////////////////////////////////////////////////////////////////
	{
		iCount++;//跳过单引号
		if(iCount>=m_nSourceLength || m_source[iCount]=='\r')//源代码结束或行结束
		{
			if(!ErrorReport(ET_WRONGCHARACTER))
				return FALSE;
			m_element=E_CHARACTER;
			m_char='\0';
			return TRUE;
		}
		if(m_source[iCount]=='\\')
		{
			iCount++;
			if(iCount>=m_nSourceLength || m_source[iCount]=='\r')//源代码结束或行结束
			{
				if(!ErrorReport(ET_WRONGCHARACTER))
					return FALSE;
				m_char='\0';
				return TRUE;
			}
			m_element=E_CHARACTER;
			if(!GetSysChar())
				return FALSE;
		}
		else
		{
			m_element=E_CHARACTER;
			m_char=m_source[iCount];
			iCount++;//跳过字符
		}
		if(iCount>=m_nSourceLength || m_source[iCount]!='\'')//源代码结束或不是单引号
		{
			if(!ErrorReport(ET_WRONGCHARACTER))
				return FALSE;
			m_char='\0';
			return TRUE;
		}
		iCount++;//跳过单引号
		return TRUE;
	}
	if(m_source[iCount]=='\"')
////字符串/////////////////////////////////////////////////////////////////////
	{
		iCount++;//跳过双引号
		m_element=E_STRING;
		m_string="";
		while(iCount<m_nSourceLength && m_source[iCount]!='\"')
		{
			if(m_source[iCount]=='\\')//转义字符
			{
				iCount++;
				if(!GetSysChar())
					return FALSE;
				m_string+=m_char;
			}
			else if(m_source[iCount]=='\r')//行结束
			{
				if(!ErrorReport(ET_STRINGNOEND))
					return FALSE;
				return TRUE;
			}
			else
			{
				m_string+=m_source[iCount];
				iCount++;
			}
		}
		iCount++;//跳过双引号
		return TRUE;
	}
	switch(m_source[iCount])
////运算符/////////////////////////////////////////////////////////////////////
	{
	case '+':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_PLUSBECOMES;
		}
		else m_element=E_PLUS;
		return TRUE;
	case '-':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_MINUSBECOMES;
		}
		else m_element=E_MINUS;
		return TRUE;
	case '*':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_TIMESBECOMES;
		}
		else m_element=E_TIMES;
		return TRUE;
	case '/':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_SLASHBECOMES;
		}
		else m_element=E_SLASH;
		return TRUE;
	case '%':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_MODBECOMES;
		}
		else m_element=E_MOD;
		return TRUE;
	case '(':
		iCount++;
		m_element=E_LPAREN;
		return TRUE;
	case ')':
		iCount++;
		m_element=E_RPAREN;
		return TRUE;
	case '[':
		iCount++;
		m_element=E_LSUB;
		return TRUE;
	case ']':
		iCount++;
		m_element=E_RSUB;
		return TRUE;
	case '{':
		iCount++;
		m_element=E_BEGIN;
		return TRUE;
	case '}':
		iCount++;
		m_element=E_END;
		return TRUE;
	case ',':
		iCount++;
		m_element=E_COMMA;
		return TRUE;
	case ':':
		iCount++;
		m_element=E_COLON;
		return TRUE;
	case ';':
		iCount++;
		m_element=E_SEMICOLON;
		return TRUE;
	case '!':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_NOTEQUAL;
		}
		else m_element=E_NOT;
		return TRUE;
	case '>':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_GREATEQUAL;
		}
		else m_element=E_GREAT;
		return TRUE;
	case '<':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_LESSEQUAL;
		}
		else m_element=E_LESS;
		return TRUE;
	case '=':
		iCount++;
		if(iCount<m_nSourceLength && m_source[iCount]=='=')
		{
			iCount++;
			m_element=E_EQUAL;
		}
		else m_element=E_BECOMES;
		return TRUE;
	case '&':
		iCount++;
		if(iCount>=m_nSourceLength || m_source[iCount]!='&')
		{
			if(!ErrorReport(ET_UNKNOWNTOKEN))
				return FALSE;
			m_element=E_AND;
			return TRUE;
		}
		iCount++;
		m_element=E_AND;
		return TRUE;
	case '|':
		iCount++;
		if(iCount>=m_nSourceLength || m_source[iCount]!='|')
		{
			if(!ErrorReport(ET_UNKNOWNTOKEN))
				return FALSE;
			m_element=E_OR;
			return TRUE;
		}
		iCount++;
		m_element=E_OR;
		return TRUE;
	}
	if(bReportWrongToken)
	{
		if(!ErrorReport(ET_UNKNOWNTOKEN))
			return FALSE;
		bReportWrongToken=FALSE;
	}
	//碰到无法识别的符号，跳过之，使用goto语句重新启动本函数
	iCount++;
	goto RestartGetElement;
}

BOOL CFCView::GetSysChar()
{
	switch(m_source[m_iCharCount])
	{
	case '0':
		m_iCharCount++;
		m_char='\0';
		return TRUE;
	case 'n':
		m_iCharCount++;
		m_char='\n';
		return TRUE;
	case 't':
		m_iCharCount++;
		m_char='\t';
		return TRUE;
	case '\'':
		m_iCharCount++;
		m_char='\'';
		return TRUE;
	case '\"':
		m_iCharCount++;
		m_char='\"';
		return TRUE;
	case '\\':
		m_iCharCount++;
		m_char='\\';
		return TRUE;
	default:
		if(!ErrorReport(ET_UNKNOWNSYSCHAR))
			return FALSE;
		m_char=m_source[m_iCharCount];
		m_iCharCount++;
		return TRUE;
	}
}

static char keyword[18][10]={
	"break",
	"case",
	"char",
	"continue",
	"default",
	"do",
	"double",
	"else",
	"exit",
	"for",
	"if",
	"input",
	"int",
	"output",
	"return",
	"switch",
	"void",
	"while"
};

BOOL CFCView::GetKeyword(void)
{
	int low=0,high=17,mid;
	int compare;
	while(low<=high)
	{
		mid=(low+high)/2;
		compare=strcmp(m_ident,keyword[mid]);
		if(compare==0)
		{
			m_element=ZElement(E_BREAK+mid);
			return TRUE;
		}
		if(compare<0)
			high=mid-1;
		else
			low=mid+1;
	}
	return FALSE;
}

static char MathFunction[18][10]={
	"abs",		//char(char),int(int),double(double)
	"arccos",	//double(double)
	"arcctg",	//double(double)
	"arcsin",	//double(double)
	"arctg",	//double(double)
	"ch",		//double(double)
	"cos",		//double(double)
	"ctg",		//double(double)
	"cth",		//double(double)
	"exp",		//double(double)
	"lg",		//double(double)
	"ln",		//double(double)
	"pow",		//double(double,double)
	"sh",		//double(double)
	"sin",		//double(double)
	"sqrt",		//double(double)
	"tg",		//double(double)
	"th",		//double(double)
};

static char StringFunction[5][10]={
	"strcmp",	//int(char[],char[])
	"strcpy",	//void(char[],char[]),void(char[],"")
	"strlen",	//int(char[]),int("")
	"strlwr",	//void(char[])
	"strupr",	//void(char[])
};

static char ArrayFunction[6][10]={
	"average",	//double(char[],int),double(int[],int),double(double[],int)
	"downsort",	//void(char[],int),void(int[],int),void(double[],int)
	"max",		//char(char[],int),int(int[],int),double(double[],int)
	"min",		//char(char[],int),int(int[],int),double(double[],int)
	"sum",		//int(char[],int),int(int[],int),double(double[],int)
	"upsort",	//void(char[],int),void(int[],int),void(double[],int)
};

BOOL CFCView::GetLibraryFunction()
{
	int low,high,mid;
	int compare;
	if(m_bIncludeMath)
	{
		low=0;
		high=17;
		while(low<=high)
		{
			mid=(low+high)/2;
			compare=strcmp(m_ident,MathFunction[mid]);
			if(compare==0)
			{
				m_element=ZElement(E_ABS+mid);
				return TRUE;
			}
			if(compare<0)
				high=mid-1;
			else
				low=mid+1;
		}
	}
	if(m_bIncludeString)
	{
		low=0;
		high=4;
		while(low<=high)
		{
			mid=(low+high)/2;
			compare=strcmp(m_ident,StringFunction[mid]);
			if(compare==0)
			{
				m_element=ZElement(E_STRCMP+mid);
				return TRUE;
			}
			if(compare<0)
				high=mid-1;
			else
				low=mid+1;
		}
	}
	if(m_bIncludeArray)
	{
		low=0;
		high=5;
		while(low<=high)
		{
			mid=(low+high)/2;
			compare=strcmp(m_ident,ArrayFunction[mid]);
			if(compare==0)
			{
				m_element=ZElement(E_AVERAGE+mid);
				return TRUE;
			}
			if(compare<0)
				high=mid-1;
			else
				low=mid+1;
		}
	}
	return FALSE;
}
