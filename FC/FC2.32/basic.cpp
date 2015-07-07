#include "stdafx.h"
#include "FC.h"

#include "basic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString GetFileName(CString strFileTitle)
{
	CString strFileName;
	for(int i=strFileTitle.GetLength()-1;i>=0;i--)
		if(strFileTitle[i]=='.' || strFileTitle[i]=='\\' || strFileTitle[i]=='/')
			break;
	if(i<0 || strFileTitle[i]=='\\' || strFileTitle[i]=='/')
	{
		if(strFileTitle.GetLength()==0)
			strFileName="";
		else
			strFileName=strFileTitle;
	}
	else
		strFileName=strFileTitle.Left(i);
	return strFileName;
}

CString DesysString(CString string)
{
	if(string=="")
		return "\\0";
	CString desysstr;
	int length=string.GetLength();
	for(int i=0;i<length;i++)
	{
		switch(string[i])
		{
		case '\n':
			desysstr+="\\n";
			break;
		case '\t':
			desysstr+="\\t";
			break;
		case '\\':
			desysstr+="\\\\";
			break;
		case '\"':
			desysstr+="\\\"";
			break;
		case '\'':
			desysstr+="\\\'";
			break;
		default:
			desysstr+=string[i];
			break;
		}
	}
	return desysstr;
}

int SizeOf(ZIT it)
{
	switch(it)
	{
	case IT_NULL:
	case IT_FUNCTION:
	case IT_FUNCTIONUS:
	case IT_VOID:
		return 0;
	case IT_CHAR:
	case IT_INT:
	case IT_ECHAR:
	case IT_EINT:
		return 1;
	default:
		return 2;
	}
}

CString TransStringNewline(CString string)
{
	int i=string.GetLength()-1,j=0;
	for(;i>=0;i--,j++)
	{
		if(string[i]=='\n')
		{
			string=string.Left(i)+"\r\n"+string.Right(j);
			j++;
		}
	}
	return string;
}

CString GetIniFilePath()
{
	char buffer[1024];
	::GetWindowsDirectory(buffer, 1024);

	return CString(buffer)+"\\FC232.ini";
}

CString GetTempFCRFilePath()
{
	char buffer[1024];
	::GetTempPath(1024, buffer);

	return CString(buffer)+"\\fc232.fcr";
}

char *GetLabel(int nLine, char label[8])
{
	label[0]='@';
	label[7]='\0';

	for(int i=6; i>0; i--)
	{
		label[i] = nLine%10+'0';
		nLine/=10;
	}
	return label;
}

int DownCompareChar(const void *ch1,const void *ch2)
{
	return *((char*)ch2) - *((char*)ch1);
}

int DownCompareInt(const void *i1,const void *i2)
{
	return *((int*)i2) - *((int*)i1);
}

int DownCompareDouble(const void *d1,const void *d2)
{
	if( *((double*)d2) < *((double*)d1) )
		return -1;
	else if( *((double*)d2) == *((double*)d1) )
		return 0;
	else
		return 1;
}

int UpCompareChar(const void *ch1,const void *ch2)
{
	return *((char*)ch1) - *((char*)ch2);
}

int UpCompareInt(const void *i1,const void *i2)
{
	return *((int*)i1) - *((int*)i2);
}

int UpCompareDouble(const void *d1,const void *d2)
{
	if( *((double*)d1) < *((double*)d2) )
		return -1;
	else if( *((double*)d1) == *((double*)d2) )
		return 0;
	else
		return 1;
}
