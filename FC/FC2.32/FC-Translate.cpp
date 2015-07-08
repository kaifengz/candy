#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"
#include "FCView.h"

#include "basic.h"
#include <fstream.h>
#include <iomanip.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CFCView::Translate()
{
	m_fout.open(m_strCppFile,ios::out);
#ifndef _DEBUG
	tm t_current;
	_getsystime(&t_current);
	m_fout.fill('0');
	m_fout<<"// "<<m_strCppFile<<'\n';
	m_fout<<"// FC2.32 �� "<<t_current.tm_year+1900<<"��"<<setw(2)<<t_current.tm_mon+1;
	m_fout<<"��"<<setw(2)<<t_current.tm_mday<<"��"<<setw(2)<<t_current.tm_hour<<':';
	m_fout<<setw(2)<<t_current.tm_min<<"\n";
	m_fout<<"// ���� "<<GetDocument()->GetPathName()<<" ����\n";
	m_fout<<"// ��Ȩ���� �쿭�� 2004.02\n";
	m_fout<<"\n";
#endif
	if(m_bKeepComment)
	{
		m_fout<<"// �������ԵĲ��죬FC2.32 ��û�з���ȫ����FC�⺯����\n";
		m_fout<<"// ����abs,downsort֮�࣬������ʵ����Щ������\n";
		m_fout<<'\n';
	}
	m_fout<<"#include <iostream.h>\n";
	m_fout<<"#include <stdlib.h>\n";
	m_fout<<"#include <string.h>\n";
	m_fout<<"#include <math.h>\n";
	if(m_bRunTime)
	{
		m_fout<<"#include <time.h>\n";
		m_fout<<"\n";
		if(m_bKeepComment)
		{
			m_fout<<"// �±�����ȫ�ֱ�������ͳ�Ƴ�������ʱ��\n";
			m_fout<<"// ��main������ʹ����return��������е�����exit()���������޷�ͳ�Ƴ�������ʱ��\n";
			m_fout<<"// �������Ҫͳ�Ƴ�������ʱ�䣬��ɾ����һ�У���ɾ��������ʹ�����������������������\n";
		}
		m_fout<<"clock_t tStart,tElapse;\n";
	}
	m_fout<<"\n#define PI 3.14159265358979323846\n";
	if(!m_bEmptyLine)
		m_fout<<"\n";

	m_char='\0';			//��ǰ�ַ�
	m_iCharCount=0;			//Դ�����ַ�����
	m_int=0;				//��ǰ����
	m_double=0.0;			//��ǰʵ��
	m_ident="";				//��ǰ��ʶ��
	m_string="";			//��ǰ�ַ���
	m_comment="";			//��ǰע��
	m_element=E_NULL;		//��ǰ����
	m_nLevel=0;

	GetElement();
	while(1)
	{
		if(m_element==E_FILEEND)
			break;
		if(m_bEmptyLine)
			m_fout<<"\n";
		TrsFunction();
	}
	m_fout.close();
	return TRUE;
}

void CFCView::TrsAndCondition()
{
	while(1)
	{
		TrsSubCondition();
		if(m_element!=E_AND)
			return;
		m_fout<<" && ";
		GetElement();
	}
}

void CFCView::TrsCondition()
{
	while(1)
	{
		TrsAndCondition();
		if(m_element!=E_OR)
			return;
		m_fout<<" || ";
		GetElement();
	}
}

void CFCView::TrsDefine()
{
	BOOL bArray;
	ZIT it;
	int nLength;
	Indent();
	switch(m_element)
	{
	case E_CHAR:	m_fout<<"char ";	it=IT_CHAR;		break;
	case E_INT:		m_fout<<"int ";		it=IT_INT;		break;
	case E_DOUBLE:	m_fout<<"double ";	it=IT_DOUBLE;	break;
	}
	while(1)
	{
		GetElement();//��ȡ��ʶ����[
		if(m_element==E_LSUB)
		{
			bArray=TRUE;
			GetElement();//��ȡ���鳤��
			if(m_element==E_INUMBER)
				nLength=m_int;
			else
				nLength=int(m_char);
			GetElement();//��ȡ]
			GetElement();//��ȡ��ʶ��
		}
		else
			bArray=FALSE;
		m_fout<<m_ident;
		if(bArray)
		{
			if(it==IT_CHAR)
				m_fout<<"["<<nLength+1<<"]";
			else
				m_fout<<"["<<nLength<<"]";
		}
		GetElement();//��ȡ=,;
		OutOperator("=");
		if(m_element==E_BECOMES)
		{
			GetElement();
			if(m_element==E_STRING)
			{
				m_fout<<'\"'<<DesysString(m_string)<<'\"';
				GetElement();//��ȡ,;
			}
			else
				TrsExpression();
		}
		else
		{
			switch(it)
			{
			case IT_CHAR:
				if(bArray)
					m_fout<<"\"\"";
				else
					m_fout<<"\'\\0\'";
				break;
			case IT_INT:
				if(bArray)
					m_fout<<"{0}";
				else
					m_fout<<0;
				break;
			case IT_DOUBLE:
				if(bArray)
					m_fout<<"{0.0}";
				else
					m_fout<<"0.0";
				break;
			}
		}
		if(m_element!=E_COMMA)
			break;
		m_fout<<",";
	}
	m_fout<<";\n";
	GetElement();
}

void CFCView::TrsDoWhile()
{
	Indent();
	m_fout<<"do\n";
	GetElement();//��ȡdo�����ĵ�һ������
	if(m_element==E_BEGIN)
		TrsMultiStatement();
	else
	{
		m_nLevel++;
		TrsStatement();
		m_nLevel--;
	}
	Indent();
	m_fout<<"while(";
	GetElement();//��ȡ(
	GetElement();//��ȡ�����ĵ�һ������
	TrsCondition();
	m_fout<<");\n";
	GetElement();//��ȡ;
	GetElement();
}

void CFCView::TrsExpression()
{
	switch(m_element)
	{
	case E_MINUS:
		OutOperator("-");
		//no break here
	case E_PLUS:
		GetElement();
		break;
	}
	TrsTerm();
	while(m_element==E_MINUS || m_element==E_PLUS)
	{
		switch(m_element)
		{
		case E_MINUS:
			OutOperator("-");
			break;
		case E_PLUS:
			OutOperator("+");
			break;
		}
		GetElement();
		TrsTerm();
	}
}

void CFCView::TrsFactor()
{
	switch(m_element)
	{
	case E_CHARACTER://************�ַ�
		m_fout<<'\''<<DesysString(m_char)<<'\'';
		GetElement();
		break;
	case E_INUMBER://**************����
		m_fout<<m_int;
		GetElement();
		break;
	case E_DNUMBER://**************ʵ��
		m_fout<<m_double;
		GetElement();
		break;
	case E_CHAR://char
	case E_INT://int
	case E_DOUBLE://*******ǿ������ת��
		switch(m_element)
		{
		case E_CHAR:	m_fout<<"(char)";	break;
		case E_INT:		m_fout<<"(int)";	break;
		case E_DOUBLE:	m_fout<<"(double)";	break;
		}
		GetElement();//��ȡ(
		//no break here
	case E_LPAREN://*****�������ı��ʽ
		m_fout<<"(";
		GetElement();//��ȡ���ʽ�ĵ�һ������
		TrsExpression();
		m_fout<<")";
		GetElement();
		break;
	case E_IDENT://**************��ʶ��
		m_fout<<m_ident;
		GetElement();//��ȡ[(�������
		if(m_element==E_LSUB)
		{
			m_fout<<"[";
			GetElement();
			TrsExpression();
			m_fout<<"]";
			GetElement();
		}
		else if(m_element==E_LPAREN)
			TrsParameter();
		break;
	case E_ARCCOS:
	case E_ARCCTG:
	case E_ARCSIN:
	case E_ARCTG:
	case E_CH:
	case E_COS:
	case E_CTG:
	case E_CTH:
	case E_EXP:
	case E_LG:
	case E_LN:
	case E_SH:
	case E_SIN:
	case E_SQRT:
	case E_TG:
	case E_TH://*********��ѧ�⺯������
		TrsMathFunctionCall();
		break;
	case E_ABS://********** abs��������
		TrsAbsCall();
		break;
	case E_POW://********** pow��������
		TrsPowCall();
		break;
	case E_STRCMP://**** strcmp��������
		TrsStrcmpCall();
		break;
	case E_STRLEN://**** strlen��������
		TrsStrlenCall();
		break;
	case E_AVERAGE:
	case E_MAX:
	case E_MIN:
	case E_SUM://******* ����⺯������
		TrsArrayFunctionCall();
		break;
	}
}

void CFCView::TrsFunction()
{
	switch(m_element)//��������
	{
	case E_VOID:	m_fout<<"void ";	break;
	case E_CHAR:	m_fout<<"char ";	break;
	case E_INT:		m_fout<<"int ";		break;
	case E_DOUBLE:	m_fout<<"double ";	break;
	}
	GetElement();//��ȡ������
	m_fout<<m_ident;
	m_bMainFunction=(m_ident=="main");
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ) void���һ������������
	if(m_element==E_VOID)
	{
		m_fout<<"void";
		GetElement();
	}
	else if(m_element!=E_RPAREN)
	{
		BOOL bArray;
		while(1)
		{
			switch(m_element)
			{
			case E_CHAR:	m_fout<<"char ";	break;
			case E_INT:		m_fout<<"int ";		break;
			case E_DOUBLE:	m_fout<<"double ";	break;
			}
			GetElement();//��ȡ��������[
			if(m_element==E_LSUB)
			{
				bArray=TRUE;
				GetElement();//��ȡ]
				GetElement();//��ȡ������
			}
			else
				bArray=FALSE;
			m_fout<<m_ident;
			if(bArray)
				m_fout<<"[]";
			GetElement();//��ȡ,)
			if(m_element!=E_COMMA)
				break;
			m_fout<<",";
			GetElement();//��ȡ��һ������������
		}
	}
	m_fout<<")";
	GetElement();//��ȡ;{
	if(m_element==E_SEMICOLON)
	{
		m_fout<<";\n";
		GetElement();
	}
	else
	{
		m_fout<<"\n";
		TrsMultiStatement();
	}
}

void CFCView::TrsIf()
{
	Indent();
	m_fout<<"if(";
	GetElement();//��ȡ(
	GetElement();//��ȡ�����ĵ�һ������
	TrsCondition();
	m_fout<<")\n";
	GetElement();//��ȡif�������ĵ�һ������
	if(m_element==E_BEGIN)
		TrsMultiStatement();
	else
	{
		m_nLevel++;
		TrsStatement();
		m_nLevel--;
	}
	if(m_element!=E_ELSE)
		return;
	Indent();
	m_fout<<"else\n";
	GetElement();//��ȡelse�������ĵ�һ������
	if(m_element==E_BEGIN)
		TrsMultiStatement();
	else
	{
		m_nLevel++;
		TrsStatement();
		m_nLevel--;
	}
}

void CFCView::TrsInput()
{
	if(m_bRunTime)
	{
		Indent();
		if(m_bInsertSpace)
			m_fout<<"::tElapse += clock() - ::tStart;\n";
		else
			m_fout<<"::tElapse+=clock()-::tStart;\n";
	}
	BOOL bNewLine=TRUE;
	while(1)
	{
		GetElement();//��ȡ��ʶ����[
		if(m_element==E_LSUB)//�����ַ���
		{
			GetElement();//��ȡ]
			GetElement();//��ȡ��ʶ��
			if(!bNewLine)
				m_fout<<";\n";
			Indent();
			m_fout<<"cin.getline("<<m_ident<<",sizeof("<<m_ident<<"));\n";
			bNewLine=TRUE;
			GetElement();
		}
		else
		{
			if(bNewLine)
			{
				bNewLine=FALSE;
				Indent();
				m_fout<<"cin";
			}
			OutOperator(">>");
			m_fout<<m_ident;
			GetElement();//��ȡ,;[
			if(m_element==E_LSUB)
			{
				m_fout<<"[";
				TrsExpression();
				m_fout<<"]";
				GetElement();//��ȡ,;
			}
		}
		if(m_element!=E_COMMA)
			break;
	}
	if(!bNewLine)
		m_fout<<";\n";
	if(m_bRunTime)
	{
		Indent();
		if(m_bInsertSpace)
			m_fout<<"::tStart = clock();\n";
		else
			m_fout<<"::tStart=clock();\n";
	}
	GetElement();
}

void CFCView::TrsJump()
{
	Indent();
	switch(m_element)
	{
	case E_BREAK:
		m_fout<<"break;\n";
		GetElement();//��ȡ;
		GetElement();
		break;
	case E_CONTINUE:
		m_fout<<"continue;\n";
		GetElement();//��ȡ;
		GetElement();
		break;
	case E_EXIT:
		m_fout<<"exit(0);\n";
		GetElement();
		GetElement();
		break;
	case E_RETURN:
		GetElement();
		if(m_element==E_SEMICOLON)
			m_fout<<"return;\n";
		else
		{
			m_fout<<"return ";
			TrsExpression();
			m_fout<<";\n";
		}
		GetElement();
		break;
	}
}

void CFCView::TrsMultiStatement()
{
	Indent();
	m_fout<<"{\n";
	m_nLevel++;
	if(m_bMainFunction)
	{
		m_bMainFunction=FALSE;
		if(m_bOutPrecision)
		{
			if(m_bKeepComment)
			{
				Indent();
				m_fout<<"// ��һ������������ȣ������Ҫ�����Խ������ڵ�";
				m_fout<<m_nPrecision<<"��Ϊ0��15����������ֵ\n";
			}
			Indent();
			m_fout<<"cout.precision("<<m_nPrecision<<");\n";
			m_fout<<"\n";
		}
		if(m_bRunTime)
		{
			if(m_bKeepComment)
			{
				Indent();
				m_fout<<"// �����г�ʼ���������м�ʱ\n";
			}
			Indent();
			if(m_bInsertSpace)
				m_fout<<"::tStart = clock();\n";
			else
				m_fout<<"::tStart=clock();\n";
			Indent();
			if(m_bInsertSpace)
				m_fout<<"::tElapse = 0;\n";
			else
				m_fout<<"::tElapse=0;\n";
			m_fout<<"\n";
		}
		GetElement();//��ȡ{�ڵ�һ�����ĵ�һ������
		while(m_element!=E_END)
			TrsStatement();
		if(m_bRunTime)
		{
			m_fout<<"\n";
			if(m_bKeepComment)
			{
				Indent();
				m_fout<<"// ���������ʱͳ�ƽ��\n";
			}
			Indent();
			if(m_bInsertSpace)
				m_fout<<"cout << \"\\n����ִ����ʱ \" << clock() + ::tElapse - ::tStart << \" ����\\n\";\n";
			else
				m_fout<<"cout<<\"\\n����ִ����ʱ \"<<clock()+::tElapse-::tStart<<\" ����\\n\";\n";
		}
		if(m_bWaitEnd)
		{
			m_fout<<"\n";
			if(m_bKeepComment)
			{
				Indent();
				m_fout<<"// �����е��������ڳ������ǰ�ȴ��û����������ַ���\n";
				Indent();
				m_fout<<"// �Է����û��鿴�������н�����粻��Ҫ����ɾ��֮\n";
			}
			Indent();
			if(m_bInsertSpace)
				m_fout<<"cout << \"\\n\";\n";
			else
				m_fout<<"cout<<\"\\n\";\n";
			Indent();
			m_fout<<"cout.flush();\n";
			Indent();
			m_fout<<"system(\"pause\");\n";
		}
		m_nLevel--;
		Indent();
		m_fout<<"}\n";
	}
	else// !m_bMainFunction
	{
		GetElement();
		while(m_element!=E_END)
			TrsStatement();
		m_nLevel--;
		Indent();
		m_fout<<"}\n";
	}
	GetElement(TRUE);
}

void CFCView::TrsOutput()
{
	Indent();
	m_fout<<"cout";
	while(1)
	{
		OutOperator("<<");
		GetElement();//��ȡ���ʽ�ĵ�һ�����ʻ��ַ�����[
		if(m_element==E_LSUB)
		{
			GetElement();//��ȡ]
			GetElement();//��ȡ��ʶ��
			m_fout<<m_ident;
			GetElement();//��ȡ,;
		}
		else if(m_element==E_STRING)
		{
			m_fout<<'\"'<<DesysString(m_string)<<'\"';
			GetElement();//��ȡ,;
		}
		else
			TrsExpression();
		if(m_element!=E_COMMA)
			break;
	}
	m_fout<<";\n";
	GetElement();
}

void CFCView::TrsParameter()
{
	m_fout<<"(";
	GetElement();//��ȡ��һ�������ĵ�һ������
	while(1)
	{
		if(m_element==E_LSUB)//����Ϊ����
		{
			GetElement();//��ȡ]
			GetElement();//��ȡ�����ʶ��
			m_fout<<m_ident;
			GetElement();//��ȡ,)
		}
		else
			TrsExpression();
		if(m_element==E_RPAREN)
			break;
		OutOperator(",");
		GetElement();//��ȡ��һ�������ĵ�һ������
	}
	m_fout<<")";
	GetElement();
}

void CFCView::TrsStatement()
{
	switch(m_element)
	{
	case E_OUTPUT:
		TrsOutput();
		break;
	case E_INPUT:
		TrsInput();
		break;
	case E_CHAR:
	case E_INT:
	case E_DOUBLE:
		TrsDefine();
		break;
	case E_BREAK:
	case E_CONTINUE:
	case E_EXIT:
	case E_RETURN:
		TrsJump();
		break;
	case E_IF:
		TrsIf();
		break;
	case E_WHILE:
		TrsWhile();
		break;
	case E_DO:
		TrsDoWhile();
		break;
	case E_SWITCH:
		TrsSwitch();
		break;
	case E_BEGIN:
		TrsMultiStatement();
		break;
	case E_IDENT:
		Indent();
		m_fout<<m_ident;
		GetElement();//��ȡ([=
		if(m_element==E_LPAREN)
		{
			TrsParameter();
			m_fout<<";\n";
			GetElement();
			break;
		}
		if(m_element==E_LSUB)
		{
			m_fout<<"[";
			GetElement();//��ȡ�±���ʽ�ĵ�һ������
			TrsExpression();
			m_fout<<"]";
			GetElement();
		}
		switch(m_element)
		{
		case E_BECOMES:			OutOperator("=");	break;
		case E_PLUSBECOMES:		OutOperator("+=");	break;
		case E_MINUSBECOMES:	OutOperator("-=");	break;
		case E_TIMESBECOMES:	OutOperator("*=");	break;
		case E_SLASHBECOMES:	OutOperator("/=");	break;
		case E_MODBECOMES:		OutOperator("%=");	break;
		}
		GetElement();
		TrsExpression();
		m_fout<<";\n";
		GetElement();
		break;
	case E_STRCPY://**************************** strcpy����
		TrsStrcpyCall();
		break;
	case E_STRLWR:
	case E_STRUPR://********************* strlwr strupr����
		TrsStrlwruprCall();
		break;
	case E_DOWNSORT:
	case E_UPSORT://******************* downsort upsort����
		TrsSortCall();
		break;
	case E_SEMICOLON://************************�ֺţ������
		GetElement();
		break;
	}
}

void CFCView::TrsSubCondition()
{
	if(m_element==E_NOT)
	{
		OutOperator("!");
		GetElement();
		TrsFactor();
	}
	else
	{
		TrsExpression();
		switch(m_element)
		{
		case E_EQUAL:		OutOperator("==");	break;
		case E_NOTEQUAL:	OutOperator("!=");	break;
		case E_LESS:		OutOperator("<");	break;
		case E_LESSEQUAL:	OutOperator("<=");	break;
		case E_GREAT:		OutOperator(">");	break;
		case E_GREATEQUAL:	OutOperator(">=");	break;
		default:return;
		}
		GetElement();//��ȡ�ڶ������ʽ�ĵ�һ������
		TrsExpression();
	}
}

void CFCView::TrsSwitch()
{
	Indent();
	m_fout<<"switch(";
	GetElement();//��ȡ(
	GetElement();//��ȡ�����ĵ�һ������
	TrsExpression();
	m_fout<<")\n";
	GetElement();//��ȡ{
	Indent();
	m_fout<<"{\n";
	m_nLevel++;
	GetElement();//��ȡcase
	BOOL bFirst=TRUE;
	while(1)
	{
		if(m_element==E_CASE)
		{
			if(bFirst)
				bFirst=FALSE;
			else
			{
				m_nLevel--;
				Indent();
				m_fout<<"}\n";
			}
			m_nLevel--;
			Indent();
			m_fout<<"case ";
			GetElement();//��ȡcase����������ַ�
			if(m_element==E_INUMBER)
				m_fout<<m_int;
			else
				m_fout<<'\''<<DesysString(m_char)<<'\'';
			GetElement();//��ȡ:
			m_fout<<":\n";
			m_nLevel++;
			Indent();
			m_fout<<"{\n";
			m_nLevel++;
			GetElement();
		}
		else if(m_element==E_DEFAULT)
		{
			if(bFirst)
				bFirst=FALSE;
			else
			{
				m_nLevel--;
				Indent();
				m_fout<<"}\n";
			}
			m_nLevel--;
			Indent();
			GetElement();//��ȡ:
			m_fout<<"default:\n";
			m_nLevel++;
			Indent();
			m_fout<<"{\n";
			m_nLevel++;
			GetElement();
		}
		else if(m_element==E_END)
		{
			if(bFirst)
				bFirst=FALSE;
			else
			{
				m_nLevel--;
				Indent();
				m_fout<<"}\n";
			}
			m_nLevel--;
			Indent();
			m_fout<<"}\n";
			GetElement();
			return;
		}
		else
			TrsStatement();
	}
}

void CFCView::TrsTerm()
{
	while(1)
	{
		TrsFactor();
		switch(m_element)
		{
		case E_TIMES:
			OutOperator("*");
			break;
		case E_SLASH:
			OutOperator("/");
			break;
		case E_MOD:
			OutOperator("%");
			break;
		default:return;
		}
		GetElement();
	}
}

void CFCView::TrsWhile()
{
	Indent();
	m_fout<<"while(";
	GetElement();//��ȡ(
	GetElement();//��ȡ�����ĵ�һ������
	TrsCondition();
	m_fout<<")\n";
	GetElement();
	if(m_element==E_BEGIN)
		TrsMultiStatement();
	else
	{
		m_nLevel++;
		TrsStatement();
		m_nLevel--;
	}
}

void CFCView::OutOperator(char op[])
{
	if(m_bInsertSpace)
		m_fout<<' ';
	m_fout<<op;
	if(m_bInsertSpace)
		m_fout<<' ';
}

void CFCView::Indent()
{
	int i;
	if(m_iBackChoice==0)
		for(i=m_nLevel;i>0;i--)
			m_fout<<'\t';
	else
		for(i=m_iBackNumber*m_nLevel;i>0;i--)
			m_fout<<' ';
}

void CFCView::TrsMathFunctionCall()
{
	switch(m_element)
	{
	case E_ARCCOS:
		m_fout<<"acos";
		break;
	case E_ARCCTG:
		if(m_bInsertSpace)
			m_fout<<"PI - atan";
		else
			m_fout<<"PI-atan";
		break;
	case E_ARCSIN:
		m_fout<<"asin";
		break;
	case E_ARCTG:
		m_fout<<"atan";
		break;
	case E_CH:
		m_fout<<"cosh";
		break;
	case E_COS:
		m_fout<<"cos";
		break;
	case E_CTG:
		if(m_bInsertSpace)
			m_fout<<"1 / tan";
		else
			m_fout<<"1/tan";
		break;
	case E_CTH:
		if(m_bInsertSpace)
			m_fout<<"1 / tanh";
		else
			m_fout<<"1/tanh";
		break;
	case E_EXP:
		m_fout<<"exp";
		break;
	case E_LG:
		m_fout<<"log10";
		break;
	case E_LN:
		m_fout<<"log";
		break;
	case E_SH:
		m_fout<<"sinh";
		break;
	case E_SIN:
		m_fout<<"sin";
		break;
	case E_SQRT:
		m_fout<<"sqrt";
		break;
	case E_TG:
		m_fout<<"tan";
		break;
	case E_TH:
		m_fout<<"tanh";
		break;
	}
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ�������ʽ�ĵ�һ������
	TrsExpression();
	m_fout<<")";
	GetElement();
}

void CFCView::TrsAbsCall()
{
	GetElement();//��ȡ(
	m_fout<<"abs(";
	TrsExpression();
	m_fout<<")";
	GetElement();
}

void CFCView::TrsPowCall()
{
	GetElement();//��ȡ(
	m_fout<<"pow(";
	TrsExpression();
	m_fout<<",";
	GetElement();
	TrsExpression();
	m_fout<<")";
	GetElement();
}

void CFCView::TrsArrayFunctionCall()
{
	switch(m_element)
	{
	case E_AVERAGE:	m_fout<<"average";	break;
	case E_MAX:		m_fout<<"max";		break;
	case E_MIN:		m_fout<<"min";		break;
	case E_SUM:		m_fout<<"sum";		break;
	}
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ[
	GetElement();//��ȡ]
	GetElement();//��ȡ�����ʶ��
	m_fout<<m_ident;
	GetElement();//��ȡ,
	m_fout<<",";
	GetElement();//��ȡ���鳤���ʽ�ĵ�һ������
	TrsExpression();
	m_fout<<")";
	GetElement();
}

void CFCView::TrsStrcmpCall()
{
	m_fout<<"strcmp";
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ[
	GetElement();//��ȡ]
	GetElement();//��ȡ�����ʶ��
	m_fout<<m_ident;
	GetElement();//��ȡ,
	m_fout<<",";
	GetElement();//��ȡ[
	GetElement();//��ȡ]
	GetElement();//��ȡ�����ʶ��
	m_fout<<m_ident;
	GetElement();//��ȡ)
	m_fout<<")";
	GetElement();
}

void CFCView::TrsStrlenCall()
{
	m_fout<<"strlen";
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ[���ַ���
	if(m_element==E_LSUB)
	{
		GetElement();//��ȡ]
		GetElement();//��ȡ�����ʶ��
		m_fout<<m_ident;
	}
	else
		m_fout<<'\"'<<DesysString(m_string)<<'\"';
	GetElement();//��ȡ)
	m_fout<<")";
	GetElement();
}

void CFCView::TrsStrcpyCall()
{
	Indent();
	m_fout<<"strcpy";
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ[
	GetElement();//��ȡ]
	GetElement();//��ȡ�����ʶ��
	m_fout<<m_ident;
	GetElement();//��ȡ,
	m_fout<<",";
	GetElement();//��ȡ[���ַ���
	if(m_element==E_LSUB)
	{
		GetElement();//��ȡ]
		GetElement();//��ȡ�����ʶ��
		m_fout<<m_ident;
	}
	else
		m_fout<<'\"'<<DesysString(m_string)<<'\"';
	GetElement();//��ȡ)
	m_fout<<")";
	GetElement();//��ȡ;
	m_fout<<";\n";
	GetElement();
}

void CFCView::TrsStrlwruprCall()
{
	Indent();
	if(m_element==E_STRLWR)
		m_fout<<"strlwr";
	else
		m_fout<<"strupr";
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ[
	GetElement();//��ȡ]
	GetElement();//��ȡ�����ʶ��
	m_fout<<m_ident;
	GetElement();//��ȡ)
	m_fout<<")";
	GetElement();//��ȡ;
	m_fout<<";\n";
	GetElement();
}

void CFCView::TrsSortCall()
{
	Indent();
	if(m_element==E_DOWNSORT)
		m_fout<<"downsort";
	else
		m_fout<<"upsort";
	GetElement();//��ȡ(
	m_fout<<"(";
	GetElement();//��ȡ[
	GetElement();//��ȡ]
	GetElement();//��ȡ�����ʶ��
	m_fout<<m_ident;
	GetElement();//��ȡ,
	m_fout<<",";
	GetElement();//��ȡ���鳤���ʽ�ĵ�һ������
	TrsExpression();
	m_fout<<")";
	GetElement();//��ȡ;
	m_fout<<";\n";
	GetElement();
}
