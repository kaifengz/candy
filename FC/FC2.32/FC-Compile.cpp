#include "stdafx.h"
#include "FC.h"

#include "FCDoc.h"
#include "FCView.h"

#include "basic.h"
#include "FCErrorReportDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CFCView::ErrorReport(ZET et)
{
	m_iErrorCount++;
	if(m_bIgnoreError)//���������
		return TRUE;
	int i,iLine=1,start,end;
	for(i=0;i<m_iCharCount && i<m_nSourceLength;i++)
		if(m_source[i]=='\r')
			iLine++;
	for(i=m_iCharCount-1;i>=0;i--)
		if(m_source[i]=='\n')
			break;
	start=i+1;
	for(i=(m_iCharCount>0?m_iCharCount-1:0);i<m_nSourceLength;i++)
		if(m_source[i]=='\r')
			break;
	if(i==m_nSourceLength)
		end=i;
	else end=i-1;
	CFCErrorReportDlg dlg;
	dlg.m_bIgnoreAllError=m_bIgnoreError;
	BOOL result= (dlg.DoModal(iLine,m_source.Mid(start,end+1-start),et)==IDOK) ;
	m_bIgnoreError=dlg.m_bIgnoreAllError;
	return result;
}

BOOL CFCView::ExamUnsolvedFunctions()
{
	bool *bCalled=new bool[m_nFunction];
	int i;
	for(i=0;i<m_nFunction;i++)
		bCalled[i]=false;
	for(i=0;i<m_nCode;i++)
	{
		if(m_pCode[i].op==OP_CALL && m_pCode[i].sd.iNumber<m_nFunction)
			bCalled[m_pCode[i].sd.iNumber]=true;
	}
	for(i=0;i<m_nFunction;i++)
	{
		if( !m_pFunction[i]->bSolved && bCalled[i])
		{
			if(!ErrorReport(ET_FUNCTIONUS))
			{
				delete bCalled;
				return FALSE;
			}
			break;
		}
	}
	delete bCalled;
	return TRUE;
}

void CFCView::ClearFunction()
{
	if(m_pFunction!=NULL)
	{
		int i,j;
		for(i=0;i<m_nFunction;i++)
		{
			if(m_pFunction[i]->pParam)
			{
				for(j=0;j<m_pFunction[i]->nParam;j++)
					delete m_pFunction[i]->pParam[j];
				delete m_pFunction[i]->pParam;
			}
			if(m_pFunction[i]->pIdent)
			{
				for(j=0;j<m_pFunction[i]->nIdent;j++)
					delete m_pFunction[i]->pIdent[j];
				delete m_pFunction[i]->pIdent;
			}
		}
		delete m_pFunction;
		m_pFunction=NULL;
		m_nFunction=0;
		m_nMaxFunction=0;
	}
}

void CFCView::ClearCode()
{
	if(m_pCode!=NULL)
	{
		int i;
		for(i=0;i<m_nCode;i++)
		{
			switch(m_pCode[i].op)
			{
			case OP_OUTPUTS:
			case OP_STRLENS:
				delete m_pCode[i].sd.string;
				break;
			case OP_STRCPYS:
				delete m_pCode[i].sd.string2;
				break;
			default:
				break;
			}
		}
		delete m_pCode;
		m_pCode=NULL;
		m_nCode=0;
		m_nMaxCode=0;
	}
}

BOOL CFCView::NewFunction()
{
	if(m_nFunction==m_nMaxFunction)
	{
		ZFunction **pFunction=m_pFunction;
		m_nMaxFunction+=FUNCTION_NUM_INC;
		m_pFunction=new ZFunction* [m_nMaxFunction];
		if(m_pFunction==NULL)
		{
			MessageBox("�����������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
		for(int i=0;i<m_nFunction;i++)
			m_pFunction[i]=pFunction[i];
		if(m_nFunction!=0)
			delete pFunction;
	}
	m_pFunction[m_nFunction]=new ZFunction;
	if(m_pFunction[m_nFunction]==NULL)
	{
		MessageBox("�����������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	m_pFunction[m_nFunction]->bSolved=FALSE;
	m_pFunction[m_nFunction]->CurLevel=0;
	m_pFunction[m_nFunction]->name="";
	m_pFunction[m_nFunction]->nCurAddress=0;
	m_pFunction[m_nFunction]->nIdent=0;
	m_pFunction[m_nFunction]->nMaxIdent=0;
	m_pFunction[m_nFunction]->nMaxParam=0;
	m_pFunction[m_nFunction]->nMaxTotalMemory=0;
	m_pFunction[m_nFunction]->nStartAddress=0;
	m_pFunction[m_nFunction]->nParam=0;
	m_pFunction[m_nFunction]->pIdent=0;
	m_pFunction[m_nFunction]->pParam=0;
	m_pFunction[m_nFunction]->ReturnType=IT_VOID;
	return TRUE;
}

BOOL CFCView::NewIdent()
{
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	if(pFunction->nIdent==pFunction->nMaxIdent)
	{
		ZIdent **pIdent=pFunction->pIdent;
		pFunction->nMaxIdent+=IDENT_NUM_INC;
		pFunction->pIdent=new ZIdent* [pFunction->nMaxIdent];
		if(pFunction->pIdent==NULL)
		{
			MessageBox("�����������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
		for(int i=0;i < pFunction->nIdent;i++)
			pFunction->pIdent[i]=pIdent[i];
		if(pFunction->nIdent!=0)
			delete pIdent;
	}
	pFunction->pIdent[pFunction->nIdent]=new ZIdent;
	if(pFunction->pIdent[pFunction->nIdent]==NULL)
	{
		MessageBox("�����������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	pFunction->pIdent[pFunction->nIdent]->level=0;
	pFunction->pIdent[pFunction->nIdent]->name="";
	pFunction->pIdent[pFunction->nIdent]->nAddress=0;
	pFunction->pIdent[pFunction->nIdent]->type=IT_VOID;
	return TRUE;
}

BOOL CFCView::NewParam()
{
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	if(pFunction->nParam==pFunction->nMaxParam)//�������������������µĿռ�
	{
		ZIdent **pParam=pFunction->pParam;
		pFunction->nMaxParam+=IDENT_NUM_INC;
		pFunction->pParam=new ZIdent* [pFunction->nMaxParam];
		if(pFunction->pParam==NULL)
		{
			MessageBox("�����������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
		for(int i=0;i < pFunction->nParam;i++)
			pFunction->pParam[i]=pParam[i];
		if(pFunction->nParam!=0)
			delete pParam;
	}
	pFunction->pParam[pFunction->nParam]=new ZIdent;
	if(pFunction->pParam[pFunction->nParam]==NULL)
	{
		MessageBox("�����������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	pFunction->pParam[pFunction->nParam]->level=0;
	pFunction->pParam[pFunction->nParam]->name="";
	pFunction->pParam[pFunction->nParam]->nAddress=0;
	pFunction->pParam[pFunction->nParam]->type=IT_VOID;
	return TRUE;
}

BOOL CFCView::NewCode()
{
	if(m_nCode==m_nMaxCode)
	{
		ZCode *pCode=m_pCode;
		m_nMaxCode+=CODE_NUM_INC;
		m_pCode=new ZCode[m_nMaxCode];
		if(m_pCode==NULL)
		{
			MessageBox("���м��������ڴ�ռ�ʱ��������","FC",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
		memcpy(m_pCode,pCode,sizeof(ZCode)*m_nCode);
	}
	return TRUE;
}

int CFCView::GetFunctionIndex(CString ident)
{
	for(int i=0;i<m_nFunction;i++)
		if(ident==m_pFunction[i]->name)
			return i;
	return -1;
}

ZIT CFCView::GetIdentType(CString ident)
{
	if(ident=="")
		return IT_NULL;
	const ZFunction *pFunction=m_pFunction[m_nCurFunction];
	for(int i=pFunction->nIdent-1;i>=0;i--)
		if(ident==pFunction->pIdent[i]->name)
		{
			if(pFunction->pIdent[i]->level==pFunction->CurLevel)
				return pFunction->pIdent[i]->type;
			else if(pFunction->pIdent[i]->level>=0)
				return ZIT(pFunction->pIdent[i]->type+IT_ECHAR-IT_CHAR);
		}
	for(i=0;i<pFunction->nParam;i++)
		if(ident==pFunction->pParam[i]->name)
		{
			if(pFunction->CurLevel == 0)
				return pFunction->pParam[i]->type;
			else 
				return ZIT(pFunction->pParam[i]->type+IT_ECHAR-IT_CHAR);
		}
	return GetFunctionIdentType(ident);
}

ZIT CFCView::GetFunctionIdentType(CString ident)
{
	if(ident=="")
		return IT_NULL;
	for(int i=0;i<m_nFunction;i++)
		if(ident==m_pFunction[i]->name)
		{
			if(m_pFunction[i]->bSolved)
				return IT_FUNCTION;
			else
				return IT_FUNCTIONUS;
		}
	return IT_NULL;
}

int CFCView::GetIdentAddress(CString ident)
{
	if(ident=="")
		return 0;
	const ZFunction *pFunction=m_pFunction[m_nCurFunction];
	for(int i=pFunction->nIdent-1;i>=0;i--)
		if(ident==pFunction->pIdent[i]->name)
			return pFunction->pIdent[i]->nAddress;
	for(i=0;i<pFunction->nParam;i++)
		if(ident==pFunction->pParam[i]->name)
			return pFunction->pParam[i]->nAddress;
	return -1;
}

BOOL CFCView::GenerateCode(ZOperator op)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::GenerateCode(ZOperator op,char cNumber)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_pCode[m_nCode].sd.cNumber=cNumber;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::GenerateCode(ZOperator op,int iNumber)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_pCode[m_nCode].sd.iNumber=iNumber;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::GenerateCode(ZOperator op,double dNumber)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_pCode[m_nCode].sd.dNumber=dNumber;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::GenerateCode(ZOperator op,char *string)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_pCode[m_nCode].sd.string=string;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::GenerateCode(ZOP op,int iNumber,int iNumber2)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_pCode[m_nCode].sd.iNumber=iNumber;
	m_pCode[m_nCode].sd.iNumber2=iNumber2;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::GenerateCode(ZOP op,int iNumber,char *string2)
{
	if(!NewCode())
		return FALSE;
	m_pCode[m_nCode].op=op;
	m_pCode[m_nCode].sd.iNumber=iNumber;
	m_pCode[m_nCode].sd.string2=string2;
	m_nCode++;
	return TRUE;
}

BOOL CFCView::RecycleMemory()
{
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	ZIdent **pIdent=pFunction->pIdent;
	int i;
	//����������ڴ�
	if(pFunction->nMaxTotalMemory < pFunction->nCurAddress)
		pFunction->nMaxTotalMemory = pFunction->nCurAddress;
	//���������ڴ���� -- ����Ҫ���ղ���������ڴ�
	for(i=pFunction->nIdent-1;i>=0 && pIdent[i]->level > pFunction->CurLevel;i--)
	{
		switch(pFunction->pIdent[i]->type)
		{
		case IT_CHARARRAY:
			if(!GenerateCode(OP_DELARRAYC,pFunction->pIdent[i]->nAddress))
				return FALSE;
			break;
		case IT_INTARRAY:
			if(!GenerateCode(OP_DELARRAYI,pFunction->pIdent[i]->nAddress))
				return FALSE;
			break;
		case IT_DOUBLEARRAY:
			if(!GenerateCode(OP_DELARRAYD,pFunction->pIdent[i]->nAddress))
				return FALSE;
			break;
		default:break;
		}
		delete pIdent[i];
		pFunction->nCurAddress -= SizeOf(pFunction->pIdent[i]->type);
	}
	pFunction->nIdent=i+1;
	return TRUE;
}

BOOL CFCView::RecycleArrayMemory()
{	//�˺�������return���ǰ���ڴ���չ��������������������ĸ���
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	for(int i=pFunction->nIdent-1;i>=0;i--)
	{
		if(pFunction->pIdent[i]->level>=0)
		{
			switch(pFunction->pIdent[i]->type)
			{
			case IT_CHARARRAY:
				if(!GenerateCode(OP_DELARRAYC,pFunction->pIdent[i]->nAddress))
					return FALSE;
				break;
			case IT_INTARRAY:
				if(!GenerateCode(OP_DELARRAYI,pFunction->pIdent[i]->nAddress))
					return FALSE;
				break;
			case IT_DOUBLEARRAY:
				if(!GenerateCode(OP_DELARRAYD,pFunction->pIdent[i]->nAddress))
					return FALSE;
				break;
			}
		}
	}
	return TRUE;
}

BOOL CFCView::Compile()
{
	CString m_OldSource=m_source;
	GetWindowText(m_source);
	m_nSourceLength=m_source.GetLength();
	if(m_OldSource==m_source && m_bCompiled)
		return TRUE;

	m_char='\0';			//��ǰ�ַ�
	m_iCharCount=0;			//Դ�����ַ�����
	m_int=0;				//��ǰ����
	m_double=0.0;			//��ǰʵ��
	m_ident="";				//��ǰ��ʶ��
	m_string="";			//��ǰ�ַ���
	m_comment="";			//��ǰע��
	m_element=E_NULL;		//��ǰ����
	ClearFunction();
	m_nCurFunction=0;		//��ǰ����ĺ���
	m_iErrorCount=0;		//Դ����������
	ClearCode();
	m_nBreak=-1;			//
	m_nContinue=-1;			//
	m_nTrue=-1;				//
	m_nFalse=-1;			//
	m_nCase=-1;				//

	//�м�����ײ�Ϊcall main�����е�ַ����DoEndProgram()�����
	if(!GenerateCode(OP_CALL,0))
		return FALSE;

	if(!GetElement(TRUE))	//��ȡ��һ������
		return FALSE;
	while(1)
	{
		if(m_element==E_FILEEND)
			break;
		if(!DoFunctionHead())
			return FALSE;
	}
	if(!DoEndProgram())		//�������������
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoFunctionHead()
{
	ZIT ReturnType,it;
	switch(m_element)//��麯����������
	{
	case E_VOID:	ReturnType=IT_VOID;		break;
	case E_CHAR:	ReturnType=IT_CHAR;		break;
	case E_INT:		ReturnType=IT_INT;		break;
	case E_DOUBLE:	ReturnType=IT_DOUBLE;	break;
	default:
		if(!ErrorReport(ET_WRONGRETURNTYPE))
			return FALSE;
		ReturnType=IT_VOID;
		break;
	}
	if(!GetElement())//��ȡ������
		return FALSE;
	if(m_element!=E_IDENT)//���Ǳ�ʶ��������
	{
		if(!ErrorReport(ET_MISSIDENT))
			return FALSE;
		m_ident="";
	}
	it=GetFunctionIdentType(m_ident);
	switch(it)
	{
	case IT_FUNCTION://�Ѿ�������ĺ���
		if(!ErrorReport(ET_DEFINEDFUNCTION))
			return FALSE;
		m_ident="";
		m_nCurFunction=m_nFunction;
		break;
	case IT_FUNCTIONUS://�Ѿ�Ԥ������ĺ���
		m_nCurFunction=GetFunctionIndex(m_ident);
		break;
	case IT_NULL://û�ж�����ı�ʶ��
		m_nCurFunction=m_nFunction;
		break;
	}
	if(it==IT_FUNCTIONUS)//�Ѿ�Ԥ������ĺ���
	{
		if(ReturnType!=m_pFunction[m_nCurFunction]->ReturnType)//���ζ���ķ������Ͳ�ͬ������
		{
			if(!ErrorReport(ET_DIFRETURNTYPE))
				return FALSE;
			//�����ζ���ĺ����������Ͳ�ͬ��ȡ���һ�ζ����Ϊ�淵������
			m_pFunction[m_nCurFunction]->ReturnType=ReturnType;
		}
		if(!GetElement())//��ȡ(
			return FALSE;
		if(m_element!=E_LPAREN)//����(������
		{
			if(!ErrorReport(ET_MISSLPAREN))
				return FALSE;
		}
		if(!GetElement())//��ȡvoid��)��������ֵĵ�һ����������
			return FALSE;
		if(m_element==E_VOID || m_element==E_RPAREN)//void��)���޲κ���
		{
			if(m_pFunction[m_nCurFunction]->nParam!=0)//���ζ���Ĳ�����Ŀ��ͬ������
			{
				if(!ErrorReport(ET_DIFPARAMNUM))
					return FALSE;
				//�����ζ���Ĳ�����Ŀ��ͬ����ȡ���һ�ζ����Ϊ��
				m_pFunction[m_nCurFunction]->nParam=0;
			}
			if(m_element==E_VOID)
			{
				if(!GetElement())
					return FALSE;
				if(m_element!=E_RPAREN)//void��߲���)������
				{
					if(!ErrorReport(ET_MISSRPAREN))
						return FALSE;
				}
			}
		}
		else//�вκ���
		{
			const int nOldParam=m_pFunction[m_nCurFunction]->nParam;
			int i,&nParam=m_pFunction[m_nCurFunction]->nParam;
			nParam=0;
			for(i=0;i<nOldParam;i++)//���Ԥ����ʱ�ǼǵĲ�����
				m_pFunction[m_nCurFunction]->pParam[i]->name="";
			while(1)
			{
				if(nParam == nOldParam)
				{	//�ٴζ���ʱ������Ŀ��ǰ�ζ����һ����������֤��ֻ����һ�Σ�������
					if(!ErrorReport(ET_DIFPARAMNUM))
						return FALSE;
					//�����κ�������ʱ������Ŀ��ͬ��ȡ���һ��Ϊ�棬�˴����趯������������nParamֵ
				}
				switch(m_element)
				{
				case E_CHAR:	it=IT_CHAR;		break;
				case E_INT:		it=IT_INT;		break;
				case E_DOUBLE:	it=IT_DOUBLE;	break;
				default://����Ĳ�������
					if(!ErrorReport(ET_WRONGIDENTTYPE))
						return FALSE;
					it=IT_INT;
					break;
				}
				if(!GetElement())//��ȡ������ʶ����[
					return FALSE;
				if(m_element==E_LSUB)//[������
				{
					it=ZIT(it+3);
					if(!GetElement())//��ȡ]
						return FALSE;
					if(m_element!=E_RSUB)//����]������
					{
						if(!ErrorReport(ET_MISSRSUB))
							return FALSE;
					}
					if(!GetElement())//��ȡ��ʶ��
						return FALSE;
				}
				if(m_element!=E_IDENT)//���Ǳ�ʶ��,����
				{
					if(!ErrorReport(ET_MISSIDENT))
						return FALSE;
					m_ident="";
				}
				if(GetIdentType(m_ident)!=IT_NULL)//������־���Ѿ����壬����
				{
					if(!ErrorReport(ET_DEFINEDIDENT))
						return FALSE;
					m_ident="";
				}
				if(nParam < nOldParam)
				{
					if(it!=m_pFunction[m_nCurFunction]->pParam[nParam]->type)//���ζ���Ĳ������Ͳ�ͬ������
					{
						if(!ErrorReport(ET_DIFPARAMTYPE))
							return FALSE;
						//�����ζ���Ĳ������Ͳ�ͬ��ȡ��һ�ζ����Ϊ���������
						m_pFunction[m_nCurFunction]->pParam[nParam]->type=it;
					}
				}
				else
				{
					if(!NewParam())
						return FALSE;
					m_pFunction[m_nCurFunction]->pParam[nParam]->type=it;
				}
				m_pFunction[m_nCurFunction]->pParam[nParam]->name=m_ident;
				nParam++;
				if(!GetElement())//��ȡ, )
					return FALSE;
				if(m_element==E_RPAREN)//�����ţ�������������
					break;
				if(m_element!=E_COMMA)//���Ƕ��ţ�����
				{
					if(!ErrorReport(ET_MISSCOMMA))
						return FALSE;
				}
				if(!GetElement())//��ȡ��һ������������
					return FALSE;
			}//end while
			if(nParam<nOldParam)//������Ŀ��һ�£�����
			{
				if(!ErrorReport(ET_DIFPARAMNUM))
					return FALSE;
				//�����κ�������ʱ������Ŀ��ͬ��ȡ���һ��Ϊ�棬�˴����趯��
			}
		}
		if(!GetElement())//��ȡ{ ;
			return FALSE;
		if(m_element==E_BEGIN)
		{
			if(!DoFunction())
				return FALSE;
		}
		else if(m_element==E_SEMICOLON)//����Ԥ����
		{
			if(!GetElement(TRUE))
				return FALSE;
		}
		else//�ȷ�{�ַ�;������
		{
			if(!ErrorReport(ET_UNKNOWNFUNCDEF))
				return FALSE;
		}
	}
	else//û��Ԥ����ĺ���
	{
		if(!NewFunction())
			return FALSE;
		m_nCurFunction=m_nFunction;
		m_pFunction[m_nCurFunction]->ReturnType=ReturnType;
		m_pFunction[m_nCurFunction]->name=m_ident;
		m_nFunction++;
		if(!GetElement())//��ȡ(
			return FALSE;
		if(m_element!=E_LPAREN)//����(������
		{
			if(!ErrorReport(ET_MISSLPAREN))
				return FALSE;
		}
		if(!GetElement())//��ȡvoid��)���һ������������
			return FALSE;
		if(m_element==E_VOID)//void���޲κ���
		{
			if(!GetElement())
				return FALSE;
			if(m_element!=E_RPAREN)//void��߲���)������
			{
				if(!ErrorReport(ET_MISSRPAREN))
					return FALSE;
			}
		}
		else if(m_element!=E_RPAREN)//����voidҲ����)���вκ���
		{
			int &nParam=m_pFunction[m_nCurFunction]->nParam;
			while(1)
			{
				switch(m_element)
				{
				case E_CHAR:	it=IT_CHAR;		break;
				case E_INT:		it=IT_INT;		break;
				case E_DOUBLE:	it=IT_DOUBLE;	break;
				default://����Ĳ�������
					if(!ErrorReport(ET_WRONGIDENTTYPE))
						return FALSE;
					it=IT_INT;
					break;
				}
				if(!NewParam())
					return FALSE;
				m_pFunction[m_nCurFunction]->pParam[nParam]->type=it;
				if(!GetElement())//��ȡ������ʶ����[
					return FALSE;
				if(m_element==E_LSUB)//[������
				{
					m_pFunction[m_nCurFunction]->pParam[nParam]->type=
						ZIT(m_pFunction[m_nCurFunction]->pParam[nParam]->type+3);
					if(!GetElement())//��ȡ]
						return FALSE;
					if(m_element!=E_RSUB)//����]������
					{
						if(!ErrorReport(ET_MISSRSUB))
							return FALSE;
					}
					if(!GetElement())//��ȡ��ʶ��
						return FALSE;
				}
				if(m_element!=E_IDENT)//���Ǳ�ʶ��,����
				{
					if(!ErrorReport(ET_MISSIDENT))
						return FALSE;
					m_ident="";
				}
				if(GetIdentType(m_ident)!=IT_NULL)//������־���Ѿ����壬����
				{
					if(!ErrorReport(ET_DEFINEDIDENT))
						return FALSE;
					m_ident="";
				}
				m_pFunction[m_nCurFunction]->pParam[nParam]->name=m_ident;
				nParam++;
				if(!GetElement())//��ȡ, )
					return FALSE;
				if(m_element==E_RPAREN)//�����ţ�������������
					break;
				if(m_element!=E_COMMA)//���Ƕ��ţ�����
				{
					if(!ErrorReport(ET_MISSCOMMA))
						return FALSE;
				}
				if(!GetElement())//��ȡ��һ������������
					return FALSE;
			}//end while
		}
		if(!GetElement())//��ȡ{ ;
			return FALSE;
		if(m_element==E_BEGIN)
		{
			if(!DoFunction())
				return FALSE;
		}
		else if(m_element==E_SEMICOLON)//����Ԥ����
		{
			if(!GetElement(TRUE))
				return FALSE;
		}
		else//�ȷ�{�ַ�;������
		{
			if(!ErrorReport(ET_UNKNOWNFUNCDEF))
				return FALSE;
		}
	}
	return TRUE;
}

BOOL CFCView::DoFunction()
{
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	pFunction->nStartAddress=m_nCode;//�ǼǺ�����ʼ��ַ
	if(!GenerateCode(OP_FUNCTIONSTART))
		return FALSE;
	int i,offset;
	if(pFunction->nParam>0)
	{
		//��ʼ�Ǽǲ�����ַ
		pFunction->pParam[0]->nAddress=0;
		for(i=1;i<pFunction->nParam;i++)
		{
			pFunction->pParam[i]->nAddress =
				pFunction->pParam[i-1]->nAddress+SizeOf(pFunction->pParam[i-1]->type);
		}
		//�����Ǽǲ�����ַ
		//��ʼ��������
		pFunction->nCurAddress=pFunction->pParam[pFunction->nParam-1]->nAddress +
			SizeOf(pFunction->pParam[pFunction->nParam-1]->type);//�Ǽǵ�ǰ�ڴ�ʹ����
		offset=pFunction->nCurAddress+3;
		ZOperator op;
		for(i=0;i<pFunction->nParam;i++)
		{
			switch(pFunction->pParam[i]->type)
			{
			case IT_CHARARRAY:
			case IT_INTARRAY:
			case IT_DOUBLEARRAY:
				op=OP_PARAMTRANSA;
				break;
			case IT_CHAR:
				op=OP_PARAMTRANSC;
				break;
			case IT_DOUBLE:
				op=OP_PARAMTRANSD;
				break;
			case IT_INT:
				op=OP_PARAMTRANSI;
				break;
			}
			if(!GenerateCode(op,pFunction->pParam[i]->nAddress,offset))
				return FALSE;
		}
		//������������
	}
	else
		offset=3;
	m_it=IT_NULL;
	m_nBreak= -1;
	m_nContinue= -1;
	if(!GetElement())//��ȡ�������ĵĵ�һ������
		return FALSE;
	while(m_element!=E_END)//ѭ���������
	{
		if(!DoStatement())
			return FALSE;
	}
	m_pFunction[m_nCurFunction]->CurLevel=-1;//�����Ѿ��������������б���ռ�õ��ڴ�
	if(!RecycleMemory())//���������ڴ�Ĵ���
		return FALSE;
	m_pCode[pFunction->nStartAddress].sd.iNumber=pFunction->nMaxTotalMemory;//�ǼǺ�����С�ڴ���
	//��ʼ����ȱʡ���ش���
	ZOP op;
	switch(pFunction->ReturnType)
	{
	case IT_VOID:
		op=OP_RETURN;
		break;
	case IT_CHAR:
		if(!GenerateCode(OP_LOADCONSTC,'\0'))
			return FALSE;
		op=OP_RETURNC;
		break;
	case IT_INT:
		if(!GenerateCode(OP_LOADCONSTI,0))
			return FALSE;
		op=OP_RETURNI;
		break;
	case IT_DOUBLE:
		if(!GenerateCode(OP_LOADCONSTD,0.0))
			return FALSE;
		op=OP_RETURND;
		break;
	}
	if(!GenerateCode(op,offset))
		return FALSE;
	m_pFunction[m_nCurFunction]->bSolved=TRUE;//�����Ѿ����׶���
	if(!GetElement(TRUE))//��ȡ�¸������ķ������ͻ�Դ���������
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoStatement()
{
	ZIT it;
	ZOP op;
	switch(m_element)
	{
	case E_CHAR:
	case E_INT:
	case E_DOUBLE://*******************************��������
		if(!DoDefine())
			return FALSE;
		break;
	case E_INPUT://********************************�������
		if(!DoInput())
			return FALSE;
		break;
	case E_OUTPUT://*******************************������
		if(!DoOutput())
			return FALSE;
		break;
	case E_IF://***********************************ѡ�����
		if(!DoIf())
			return FALSE;
		break;
	case E_SWITCH://*******************************�������
		if(!DoSwitch())
			return FALSE;
		break;
	case E_BEGIN://********************************�������
		if(!DoMultiStatement())
			return FALSE;
		break;
	case E_BREAK:
	case E_CONTINUE:
	case E_EXIT:
	case E_RETURN://*******************************ת�����
		if(!DoJump())
			return FALSE;
		break;
	case E_WHILE://*******************************whileѭ��
		if(!DoWhile())
			return FALSE;
		break;
	case E_DO://*******************************do-whileѭ��
		if(!DoDoWhile())
			return FALSE;
		break;
	case E_IDENT://**********************��ֵ��䡢��������
		switch(GetIdentType(m_ident))
		{
		case IT_NULL://δ֪�ı�ʶ��
			if(!ErrorReport(ET_UNKNOWNIDENT))
				return FALSE;
			if(!GetElement())
				return FALSE;
			break;
		case IT_FUNCTION:
		case IT_FUNCTIONUS://��������
			it=m_pFunction[GetFunctionIndex(m_ident)]->ReturnType;
			if(!DoCall())
				return FALSE;
			if(m_element!=E_SEMICOLON)//����;������
			{
				if(!ErrorReport(ET_MISSSEMICOLON))
					return FALSE;
			}
			switch(it)
			{
			case IT_VOID:	op=OP_NULL;		break;
			case IT_CHAR:	op=OP_DELETEC;	break;
			case IT_INT:	op=OP_DELETEI;	break;
			case IT_DOUBLE:	op=OP_DELETED;	break;
			}
			if(op!=OP_NULL)
			{
				if(!GenerateCode(op))
					return FALSE;
			}
			if(!GetElement())
				return FALSE;
			break;
		default://������ֵ
			if(!DoBecomes())
				return FALSE;
		}
		break;
	case E_STRCPY://**************************** strcpy����
		if(!DoStrcpyCall())
			return FALSE;
		if(m_element!=E_SEMICOLON)
		{
			if(!ErrorReport(ET_MISSSEMICOLON))
				return FALSE;
		}
		if(!GetElement())
			return FALSE;
		break;
	case E_STRLWR:
	case E_STRUPR://********************* strlwr strupr����
		if(!DoStrlwruprCall())
			return FALSE;
		if(m_element!=E_SEMICOLON)
		{
			if(!ErrorReport(ET_MISSSEMICOLON))
				return FALSE;
		}
		if(!GetElement())
			return FALSE;
		break;
	case E_DOWNSORT:
	case E_UPSORT://******************* downsort upsort����
		if(!DoSortCall())
			return FALSE;
		if(m_element!=E_SEMICOLON)
		{
			if(!ErrorReport(ET_MISSSEMICOLON))
				return FALSE;
		}
		if(!GetElement())
			return FALSE;
		break;
	case E_SEMICOLON://************************�ֺţ������
		if(!m_bAllowEmptyStatement)
		{
			if(!ErrorReport(ET_EMPTYSTATE))
				return FALSE;
		}
		if(!GetElement())
			return FALSE;
		break;
	default:
		if(!ErrorReport(ET_UNKNOWNSTATE))
			return FALSE;
		if(!GetElement())
			return FALSE;
		break;
	}
	return TRUE;
}

BOOL CFCView::DoDefine()
{
	ZIdentType it;
	BOOL bArray;
	int nArrayLength;
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	switch(m_element)
	{
	case E_CHAR:	it=IT_CHAR;		break;
	case E_INT:		it=IT_INT;		break;
	case E_DOUBLE:	it=IT_DOUBLE;	break;
	}
	do
	{
		if(!GetElement())//��ȡ��һ���������ı�ʶ����[
			return FALSE;
		if(m_element==E_LSUB)
		{
			bArray=TRUE;
			if(!GetElement())//��ȡ��ʾ���鳤���������ַ�
				return FALSE;
			nArrayLength=0;
			switch(m_element)//���ݲ�ͬ�Ķ��������ͬ�Ĵ�����ʾ
			{
			case E_CHARACTER:	nArrayLength=m_char;break;
			case E_INUMBER:		nArrayLength=m_int;	break;
			case E_DOUBLE:
				if(!ErrorReport(ET_DOUBLESUB))
					return FALSE;
				break;
			default:
				if(!ErrorReport(ET_UNKNOWNARRAYSIZE))
					return FALSE;
			}
			if(!GetElement())//��ȡ]
				return FALSE;
			if(m_element!=E_RSUB)//����]������
			{
				if(!ErrorReport(ET_MISSRSUB))
					return FALSE;
			}
			if(!GetElement())//��ȡ�������ı�ʶ��
				return FALSE;
		}
		else bArray=FALSE;
		if(m_element!=E_IDENT)//��õĲ��Ǳ�ʶ��������
		{
			if(!ErrorReport(ET_MISSIDENT))
				return FALSE;
			m_ident="";
		}
		switch(GetIdentType(m_ident))//����ʶ������
		{
		case IT_CHAR:
		case IT_INT:
		case IT_DOUBLE:
		case IT_CHARARRAY:
		case IT_INTARRAY:
		case IT_DOUBLEARRAY:
		case IT_FUNCTION:
		case IT_FUNCTIONUS://��ʶ���Ѿ����壬����
			if(!ErrorReport(ET_DEFINEDIDENT))
				return FALSE;
			m_ident="";
		default:break;
		}
		if(!NewIdent())
			return FALSE;
		pFunction->pIdent[pFunction->nIdent]->level=pFunction->CurLevel;//�ǼǱ������
		pFunction->pIdent[pFunction->nIdent]->name=m_ident;//�ǼǱ�����
		pFunction->pIdent[pFunction->nIdent]->nAddress=pFunction->nCurAddress;//�ǼǱ�����ַ
		if(!bArray)//�ǼǱ������� -- ��������
			pFunction->pIdent[pFunction->nIdent]->type=it;
		else//�ǼǱ������� -- ������
		{
			pFunction->pIdent[pFunction->nIdent]->type=ZIT(it+IT_CHARARRAY-IT_CHAR);
			ZOP op;
			switch(pFunction->pIdent[pFunction->nIdent]->type)
			{
			case IT_CHARARRAY:	op=OP_NEWARRAYC;break;
			case IT_INTARRAY:	op=OP_NEWARRAYI;break;
			case IT_DOUBLEARRAY:op=OP_NEWARRAYD;break;
			}
			if(!GenerateCode(op,pFunction->pIdent[pFunction->nIdent]->nAddress,nArrayLength))
				return FALSE;
		}
		pFunction->nCurAddress += SizeOf(pFunction->pIdent[pFunction->nIdent]->type);//���º����ڴ�ʹ����
		if(!GetElement())//��ȡ= , ;
			return FALSE;
		if(m_element==E_BECOMES)
		{
			if(!GetElement())//��ȡ�ַ�������ʽ�ĵ�һ������
				return FALSE;
			switch(pFunction->pIdent[pFunction->nIdent]->type)
			{
			case IT_CHARARRAY://�ַ�����
				if(m_element!=E_STRING)//�����ַ���
				{
					if(!ErrorReport(ET_WRONGINIT))
						return FALSE;
				}
				else if(m_string.GetLength()>nArrayLength)
				{
					if(!ErrorReport(ET_LONGSTR2ARRAY))
						return FALSE;
				}
				else
				{
					char *string=new char[m_string.GetLength()+1];
					strcpy(string,m_string);
					if(!GenerateCode(OP_STRCPYS,pFunction->pIdent[pFunction->nIdent]->nAddress,string))
						return FALSE;
				}
				if(!GetElement())//��ȡ, ;
					return FALSE;
				break;
			case IT_CHAR:
			case IT_INT:
			case IT_DOUBLE:
				if(!DoExpression())
					return FALSE;
				if(m_it != pFunction->pIdent[pFunction->nIdent]->type)
				{	//���Ͳ�ͬ��������Ҫǿ������ת����������Ҫ��ʽ����ת��
					if(int(m_it) > int(pFunction->pIdent[pFunction->nIdent]->type))//
					{	//double => int,char; int => char;�޷���ֵ����Ҫǿ������ת��
						if(!ErrorReport(ET_MUSTCHANGETYPE))
							return FALSE;
					}
					else//��ʽ����ת��
					{
						ZOP op;
						switch(pFunction->pIdent[pFunction->nIdent]->type)
						{
						case IT_INT:
							op= OP_C2I;
							break;
						case IT_DOUBLE:
							op= (m_it==IT_INT ? OP_I2D : OP_C2D);
							break;
						}
						if(!GenerateCode(op,0))
							return FALSE;
					}
				}
				ZOP op;
				switch(pFunction->pIdent[pFunction->nIdent]->type)
				{
				case IT_CHAR:	op=OP_SAVEC;break;
				case IT_INT:	op=OP_SAVEI;break;
				case IT_DOUBLE:	op=OP_SAVED;break;
				}
				if(!GenerateCode(op,pFunction->pIdent[pFunction->nIdent]->nAddress))
					return FALSE;
				break;
			case IT_INTARRAY:	//��������
			case IT_DOUBLEARRAY://ʵ������
				if(!ErrorReport(ET_WRONGINIT))
					return FALSE;
				if(!GetElement())//��ȡ, ;
					return FALSE;
				break;
			}
		}
		else//û�еȺ�
		{
			switch(pFunction->pIdent[pFunction->nIdent]->type)
			{
			case IT_CHAR:
				if(!GenerateCode(OP_LOADCONSTC,'\0'))
					return FALSE;
				if(!GenerateCode(OP_SAVEC,pFunction->pIdent[pFunction->nIdent]->nAddress))
					return FALSE;
				break;
			case IT_INT:
				if(!GenerateCode(OP_LOADCONSTI,0))
					return FALSE;
				if(!GenerateCode(OP_SAVEI,pFunction->pIdent[pFunction->nIdent]->nAddress))
					return FALSE;
				break;
			case IT_DOUBLE:
				if(!GenerateCode(OP_LOADCONSTD,0.0))
					return FALSE;
				if(!GenerateCode(OP_SAVED,pFunction->pIdent[pFunction->nIdent]->nAddress))
					return FALSE;
				break;
			}
		}
		pFunction->nIdent++;//���º���������Ŀ����������ȫ������һ�������Ķ���ʱ����
	}
	while(m_element==E_COMMA);
	if(m_element!=E_SEMICOLON)
	{
		if(!ErrorReport(ET_MISSSEMICOLON))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoInput()
{
	while(1)
	{
		if(!GetElement())//��ȡ�������ĵ�һ������
			return FALSE;
		if(m_element==E_LSUB)//[���ַ���������
		{
			if(!GetElement())//��ȡ]
				return FALSE;
			if(m_element!=E_RSUB)//����]������
			{
				if(!ErrorReport(ET_MISSRSUB))
					return FALSE;
			}
			if(!GetElement())//��ȡ�ַ������ʶ��
				return FALSE;
			if(m_element!=E_IDENT)//���Ǳ�ʶ��������
			{
				if(!ErrorReport(ET_MISSIDENT))
					return FALSE;
				m_ident="";
			}
			switch(GetIdentType(m_ident))//���ݲ�ͬ�ı�ʶ�����͸�����ͬ����ʾ
			{
			case IT_CHARARRAY:
			case IT_ECHARARRAY:
				if(!GenerateCode(OP_INPUTCA,GetIdentAddress(m_ident)))
					return FALSE;
				break;
			case IT_INTARRAY:
			case IT_EINTARRAY:
			case IT_DOUBLEARRAY:
			case IT_EDOUBLEARRAY:
				if(!ErrorReport(ET_INPUTNACARRAY))
					return FALSE;
				break;
			default:
				if(!ErrorReport(ET_NOARRIDENTARR))
					return FALSE;
				break;
			}
			if(!GetElement())//��ȡ, ;
				return FALSE;
		}
		else//��ʶ��
		{
			if(m_element!=E_IDENT)//���Ǳ�ʶ��������
			{
				if(!ErrorReport(ET_MISSIDENT))
					return FALSE;
				m_ident="";
			}
			switch(GetIdentType(m_ident))//���ݲ�ͬ�ı�ʶ�����͸�����ͬ����ʾ
			{
			case IT_CHAR:
			case IT_ECHAR:
				if(!GenerateCode(OP_INPUTC,GetIdentAddress(m_ident)))
					return FALSE;
				if(!GetElement())//��ȡ, ;
					return FALSE;
				break;
			case IT_INT:
			case IT_EINT:
				if(!GenerateCode(OP_INPUTI,GetIdentAddress(m_ident)))
					return FALSE;
				if(!GetElement())//��ȡ, ;
					return FALSE;
				break;
			case IT_DOUBLE:
			case IT_EDOUBLE:
				if(!GenerateCode(OP_INPUTD,GetIdentAddress(m_ident)))
					return FALSE;
				if(!GetElement())//��ȡ, ;
					return FALSE;
				break;
			case IT_CHARARRAY:
			case IT_ECHARARRAY:
				if(!GetElement())//��ȡ[
					return FALSE;
				if(!DoSubscript())
					return FALSE;
				if(!GenerateCode(OP_INPUTARRAYC,GetIdentAddress(m_ident)))
					return FALSE;
				break;
			case IT_INTARRAY:
			case IT_EINTARRAY:
				if(!GetElement())//��ȡ[
					return FALSE;
				if(!DoSubscript())
					return FALSE;
				if(!GenerateCode(OP_INPUTARRAYI,GetIdentAddress(m_ident)))
					return FALSE;
				break;
			case IT_DOUBLEARRAY:
			case IT_EDOUBLEARRAY:
				if(!GetElement())//��ȡ[
					return FALSE;
				if(!DoSubscript())
					return FALSE;
				if(!GenerateCode(OP_INPUTARRAYD,GetIdentAddress(m_ident)))
					return FALSE;
				break;
			}
		}
		if(m_element!=E_COMMA)
			break;
	}
	if(m_element!=E_SEMICOLON)
	{
		if(!ErrorReport(ET_MISSSEMICOLON))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoOutput()
{
	do
	{
		if(!GetElement())//��ȡ�������ĵ�һ������
			return FALSE;
		if(m_element==E_STRING)//����ַ���
		{
			char *string=new char[m_string.GetLength()+1];
			strcpy(string,m_string);
			if(!GenerateCode(OP_OUTPUTS,string))
				return FALSE;
			if(!GetElement())//��ȡ, ;
				return FALSE;
		}
		else if(m_element==E_LSUB)//����ַ�����
		{
			if(!GetElement())//��ȡ]
				return FALSE;
			if(m_element!=E_RSUB)//����]������
			{
				if(!ErrorReport(ET_MISSRSUB))
					return FALSE;
			}
			if(!GetElement())//��ȡ�ַ������ʶ��
				return FALSE;
			if(m_element!=E_IDENT)//���Ǳ�ʶ��������
			{
				if(!ErrorReport(ET_MISSIDENT))
					return FALSE;
				m_ident="";
			}
			switch(GetIdentType(m_ident))//����ʶ������
			{
			case IT_CHARARRAY:
			case IT_ECHARARRAY:
				break;
			case IT_INTARRAY:
			case IT_EINTARRAY:
			case IT_DOUBLEARRAY:
			case IT_EDOUBLEARRAY://��ʶ�����Ͳ����ַ����飬����
				if(!ErrorReport(ET_OUTPUTNCARRAY))
					return FALSE;
				m_ident="";
				break;
			default://��ʶ���������飬����
				if(!ErrorReport(ET_NOARRIDENTARR))
					return FALSE;
				m_ident="";
				break;
			}
			if(!GenerateCode(OP_OUTPUTCA,GetIdentAddress(m_ident)))
				return FALSE;
			if(!GetElement())//��ȡ, ;
				return FALSE;
		}
		else//���ʽ
		{
			if(!DoExpression())
				return FALSE;
			ZOP op;
			switch(m_it)//�������
			{
			case IT_CHAR:	op=OP_OUTPUTC;break;
			case IT_INT:	op=OP_OUTPUTI;break;
			case IT_DOUBLE:	op=OP_OUTPUTD;break;
			}
			if(!GenerateCode(op))
				return FALSE;
		}
	}
	while(m_element==E_COMMA);
	if(m_element!=E_SEMICOLON)//���Ƿֺţ�����
	{
		if(!ErrorReport(ET_MISSSEMICOLON))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoIf()
{
	const int nTrue=m_nTrue,nFalse=m_nFalse;//������һ������ٳ���
	if(!GenerateCode(OP_JUMP,m_nCode+3))
		return FALSE;
	m_nTrue=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	m_nFalse=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)//����(������
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ�����ĵ�һ������
		return FALSE;
	if(!DoCondition())
		return FALSE;
	if(m_element!=E_RPAREN)//����)������
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	m_pCode[m_nTrue].sd.iNumber=m_nCode;//�������ڵ���ʵ��ַ
	m_pFunction[m_nCurFunction]->CurLevel++;//Ƕ�ײ�μ�һ
	if(!GetElement())//��ȡif�����ĵ�һ������
		return FALSE;
	if(!DoStatement())
		return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel--;//Ƕ�ײ�μ�һ
	if(!RecycleMemory())//�����ڴ�
		return FALSE;
	if(m_element!=E_ELSE)//if��else���
		m_pCode[m_nFalse].sd.iNumber=m_nCode;//���ٳ��ڵ���ʵ��ַ
	else//if-else���
	{
		int over=m_nCode;
		if(!GenerateCode(OP_JUMP,0))
			return FALSE;
		m_pCode[m_nFalse].sd.iNumber=m_nCode;//���ٳ��ڵ���ʵ��ַ
		m_pFunction[m_nCurFunction]->CurLevel++;//Ƕ�ײ�μ�һ
		if(!GetElement())//��ȡelse������ĵ�һ������
			return FALSE;
		if(!DoStatement())
			return FALSE;
		m_pFunction[m_nCurFunction]->CurLevel--;//Ƕ�ײ�μ�һ
		if(!RecycleMemory())//�����ڴ�
			return FALSE;
		m_pCode[over].sd.iNumber=m_nCode;
	}
	m_nTrue=nTrue;//�ָ���һ������ٳ���
	m_nFalse=nFalse;
	return TRUE;
}

BOOL CFCView::DoSwitch()
{
	const int nBreak=m_nBreak;	//������һ����break��ת��ַ
	const int nCase=m_nCase;	//������һ����case�������
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)//����(������
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡswitch����ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	if(m_it==IT_DOUBLE)//��switch����ʽ������Ϊʵ��������
	{
		if(!ErrorReport(ET_DOUBLESWITCH))
			return FALSE;
	}
	else if(m_it==IT_CHAR)
	{
		if(!GenerateCode(OP_C2I,0))
			return FALSE;
	}
	if(m_element!=E_RPAREN)//���ʱ����)������
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ{
		return FALSE;
	if(m_element!=E_BEGIN)//����{������
	{
		if(!ErrorReport(ET_MISSBEGIN))
			return FALSE;
	}
	if(!GenerateCode(OP_JUMP,m_nCode+2))
		return FALSE;
	m_nBreak=m_nCode;//����break��ת��ַ
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	m_nCase=-1;
	if(!GetElement())//��ȡcase�ؼ���
		return FALSE;
	while(m_element==E_CASE)//ѭ������case��֧
	{
		if(!DoCase())
			return FALSE;
	}
	if(m_element==E_DEFAULT)//��default������default��֧
	{
		if(!DoDefault())
			return FALSE;
	}
	m_pCode[m_nBreak].sd.iNumber=m_nCode;//����break����ʵ��ת�յ�
	if(!GenerateCode(OP_DELETEI))//ɾ��ջ����
		return FALSE;
	if(m_element!=E_END)
	{
		if(!ErrorReport(ET_MISSEND))
			return FALSE;
	}
	m_nBreak=nBreak;//�ָ���һ����break��תλ��
	m_nCase=nCase;	//�ָ���һ����case������ת���
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoMultiStatement()
{
	if(!GetElement())
		return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel++;
	while(m_element!=E_END)
		if(!DoStatement())
			return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel--;
	if(!RecycleMemory())
		return FALSE;
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoJump()
{
	ZFunction *pFunction=m_pFunction[m_nCurFunction];
	switch(m_element)
	{
	case E_BREAK:
		if(m_nBreak<0)//�����break���
		{
			if(!ErrorReport(ET_WRONGBREAK))
				return FALSE;
		}
		if(!GenerateCode(OP_JUMP,m_nBreak))
			return FALSE;
		if(!GetElement())//��ȡ�ֺ�
			return FALSE;
		break;
	case E_CONTINUE:
		if(m_nContinue<0)
		{
			if(!ErrorReport(ET_WRONGCONTINUE))
				return FALSE;
		}
		if(!GenerateCode(OP_JUMP,m_nContinue))
			return FALSE;
		if(!GetElement())//��ȡ�ֺ�
			return FALSE;
		break;
	case E_EXIT:
		if(!GenerateCode(OP_EXIT))
			return FALSE;
		if(!GetElement())//��ȡ�ֺ�
			return FALSE;
		break;
	case E_RETURN:
		if(!GetElement())//��ȡ�ֺŻ�return����ʽ�ĵ�һ������
			return FALSE;
		if(m_element!=E_SEMICOLON)//���Ƿֺţ��з���ֵ
		{
			if(pFunction->ReturnType==IT_VOID)
			{	//������������Ϊvoid��return����б��ʽ
				if(!ErrorReport(ET_VOIDRETURNVALUE))
					return FALSE;
			}
			if(!DoExpression())
				return FALSE;
			if(pFunction->ReturnType!=IT_VOID && m_it != pFunction->ReturnType)
			{	//���Ͳ�ͬ��������Ҫǿ������ת����������Ҫ��ʽ����ת��
				if(int(m_it) > int(pFunction->ReturnType))//
				{	//double => int,char; int => char;�޷���ֵ����Ҫǿ������ת��
					if(!ErrorReport(ET_MUSTCHANGETYPE))
						return FALSE;
				}
				else//��ʽ����ת��
				{
					ZOP op;
					switch(pFunction->ReturnType)
					{
					case IT_CHAR:
						op= (m_it==IT_INT ? OP_I2C : OP_D2C);
						break;
					case IT_INT:
						op= OP_D2I;
						break;
					}
					if(!GenerateCode(op,0))
						return FALSE;
				}
			}
			if(!RecycleArrayMemory())//��������ǰҪ���������ڴ�
				return FALSE;
			ZOP op;
			int offset=3;
			switch(pFunction->ReturnType)
			{
			case IT_CHAR:	op=OP_RETURNC;break;
			case IT_INT:	op=OP_RETURNI;break;
			case IT_DOUBLE:	op=OP_RETURND;break;
			}
			if(pFunction->nParam>0)
				offset= 3+pFunction->pParam[pFunction->nParam-1]->nAddress +
					SizeOf(pFunction->pParam[pFunction->nParam-1]->type);
			if(!GenerateCode(op,offset))
				return FALSE;
		}
		else//û�з���ֵ
		{
			if(pFunction->ReturnType!=IT_VOID)//����void������Ҳû�з���ֵ
			{
				if(!ErrorReport(ET_MUSTRETURNVALUE))
					return FALSE;
			}
			else
			{
				if(!RecycleArrayMemory())//��������ǰҪ���������ڴ�
					return FALSE;
				int offset=3;
				if(pFunction->nParam>0)
					offset= 3+pFunction->pParam[pFunction->nParam-1]->nAddress +
						SizeOf(pFunction->pParam[pFunction->nParam-1]->type);
				if(!GenerateCode(OP_RETURN,offset))
					return FALSE;
			}
		}
		break;//end case E_RETURN
	}//end switch
	if(m_element!=E_SEMICOLON)//���Ƿֺţ�����
	{
		if(!ErrorReport(ET_MISSSEMICOLON))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoBecomes()
{
	ZIT it;//����ֵ���������ͣ����ڸ�ֵʱ�����ͼ��
	BOOL bArray=FALSE;
	int address;//������ַ
	ZElement elmt;//��ֵ��
	ZOP op;
	switch(GetIdentType(m_ident))
	{
	case IT_CHARARRAY:
	case IT_ECHARARRAY:
		bArray=TRUE;
		//�˴�����Ҫbreak;
	case IT_CHAR:
	case IT_ECHAR:
		it=IT_CHAR;
		break;
	case IT_INTARRAY:
	case IT_EINTARRAY:
		bArray=TRUE;
		//�˴�����Ҫbreak;
	case IT_INT:
	case IT_EINT:
		it=IT_INT;
		break;
	case IT_DOUBLEARRAY:
	case IT_EDOUBLEARRAY:
		bArray=TRUE;
		//�˴�����Ҫbreak;
	case IT_DOUBLE:
	case IT_EDOUBLE:
		it=IT_DOUBLE;
		break;
	}
	address=GetIdentAddress(m_ident);
	if(!GetElement())//��ȡ��ֵ�Ż�[
		return FALSE;
	if(bArray)
	{
		if(m_element!=E_LSUB)
		{
			if(!ErrorReport(ET_MISSLSUB))
				return FALSE;
		}
		else
		{
			if(!DoSubscript())
				return FALSE;
		}
	}
	switch(m_element)
	{
	case E_PLUSBECOMES:
	case E_MINUSBECOMES:
	case E_TIMESBECOMES:
	case E_SLASHBECOMES:
	case E_MODBECOMES:
		if(bArray)
		{
			if(!GenerateCode(OP_COPYI))
				return FALSE;
		}
	case E_BECOMES:
		elmt=m_element;
		break;
	default://���Ǹ�ֵ�ţ�����
		if(!ErrorReport(ET_UNKNOWNOP))
			return FALSE;
		break;
	}
	if(elmt!=E_BECOMES)
	{
		switch(it)
		{
		case IT_CHAR:	op=(bArray ? OP_LOADARRAYC : OP_LOADVARC);	break;
		case IT_INT:	op=(bArray ? OP_LOADARRAYI : OP_LOADVARI);	break;
		case IT_DOUBLE:	op=(bArray ? OP_LOADARRAYD : OP_LOADVARD);	break;
		}
		if(!GenerateCode(op,address))//�ѱ�������ջ��
			return FALSE;
	}
	if(!GetElement())//��ȡ��ֵ�ź�ߵĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	if(it!=m_it)//���Ͳ��ȣ�������Ҫǿ������ת��������Ҫ��ʽ����ת��
	{
		if(int(it) < int(m_it))//��ǿ������ת��������
		{
			if(!ErrorReport(ET_MUSTCHANGETYPE))
				return FALSE;
		}
		else//������ʽ����ת��
		{
			ZOP op;
			switch(it)
			{
			case IT_DOUBLE:
				op= (m_it==IT_INT ? OP_I2D : OP_C2D);
				break;
			case IT_INT:
				op= OP_C2I;
				break;
			}
			if(!GenerateCode(op,0))
				return FALSE;
		}
	}
	if(elmt!=E_BECOMES)
	{
		switch(elmt)
		{
		case E_PLUSBECOMES:
			op= (it==IT_CHAR ? OP_PLUSC : (it==IT_INT ? OP_PLUSI : OP_PLUSD));
			break;
		case E_MINUSBECOMES:
			op= (it==IT_CHAR ? OP_MINUSC : (it==IT_INT ? OP_MINUSI : OP_MINUSD));
			break;
		case E_TIMESBECOMES:
			op= (it==IT_CHAR ? OP_TIMESC : (it==IT_INT ? OP_TIMESI : OP_TIMESD));
			break;
		case E_SLASHBECOMES:
			op= (it==IT_CHAR ? OP_SLASHC : (it==IT_INT ? OP_SLASHI : OP_SLASHD));
			break;
		case E_MODBECOMES:
			if(it==IT_DOUBLE)//���������࣬����
			{
				if(!ErrorReport(ET_DOUBLEMOD))
					return FALSE;
			}
			op= (it==IT_CHAR ? OP_MODC : OP_MODI);
			break;
		}
		if(!GenerateCode(op))//��ֵ
			return FALSE;
	}
	switch(it)
	{
	case IT_CHAR:	op=(bArray ? OP_SAVEARRAYC : OP_SAVEC);	break;
	case IT_INT:	op=(bArray ? OP_SAVEARRAYI : OP_SAVEI);	break;
	case IT_DOUBLE:	op=(bArray ? OP_SAVEARRAYD : OP_SAVED);	break;
	}
	if(!GenerateCode(op,address))//��ֵ
		return FALSE;
	if(m_element!=E_SEMICOLON)//����;������
	{
		if(!ErrorReport(ET_MISSSEMICOLON))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoCall()
{
	const int nDestFunc=GetFunctionIndex(m_ident);
	const ZFunction *pDestFunc=m_pFunction[nDestFunc];
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)//����(������
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ[ ) ���һ���������ʽ�ĵ�һ������
		return FALSE;
	if(m_element==E_RPAREN)
	{
		if(pDestFunc->nParam!=0)//�����в���������ʱû�в����б�����
		{
			if(!ErrorReport(ET_WRONGPARAMNUM))
				return FALSE;
		}
	}
	else//�в����б�
	{
		const int nParam=pDestFunc->nParam;
		int iParam=0;
		while(1)
		{
			if(iParam==nParam)//�����ú�����������Ŀ�Ⱥ���������Ŀ��һ������
			{
				if(!ErrorReport(ET_WRONGPARAMNUM))
					return FALSE;
			}
			if(m_element==E_LSUB)//[��������Ϊ����
			{
				if(!GetElement())//��ȡ]
					return FALSE;
				if(m_element!=E_RSUB)
				{
					if(!ErrorReport(ET_MISSRSUB))
						return FALSE;
				}
				if(!GetElement())//��ȡ�����ʶ��
					return FALSE;
				if(m_element!=E_IDENT)//���Ǳ�ʶ��
				{
					if(!ErrorReport(ET_MISSIDENT))
						return FALSE;
					m_ident="";
				}
				switch(GetIdentType(m_ident))
				{
				case IT_CHARARRAY:
				case IT_ECHARARRAY:
					m_it=IT_CHARARRAY;
					break;
				case IT_INTARRAY:
				case IT_EINTARRAY:
					m_it=IT_INTARRAY;
					break;
				case IT_DOUBLEARRAY:
				case IT_EDOUBLEARRAY:
					m_it=IT_DOUBLEARRAY;
					break;
				default://���������ʶ��
					if(!ErrorReport(ET_NOARRIDENTARR))
						return FALSE;
					break;
				}
				if(iParam<nParam)
				{
					if(m_it != pDestFunc->pParam[iParam]->type)//�������Ͳ�ͬ
					{
						if(!ErrorReport(ET_WRONGPARAMTYPE))
							return FALSE;
					}
					else
					{
						if(!GenerateCode(OP_LOADARRAY,GetIdentAddress(m_ident)))
							return FALSE;
					}
				}
				if(!GetElement())
					return FALSE;
			}
			else//���ʽ
			{
				if(!DoExpression())
					return FALSE;
				ZIT it;
				if(iParam<nParam)
				{
					switch(pDestFunc->pParam[iParam]->type)
					{
					case IT_INT:
					case IT_CHAR:
					case IT_DOUBLE:
						it=pDestFunc->pParam[iParam]->type;
						break;
					default://��ʽ�������Ǳ�������Ϊ���飩
						it=IT_NULL;
						if(!ErrorReport(ET_WRONGPARAMTYPE))
							return FALSE;
						break;
					}
					if(it!=IT_NULL && it != m_it)//���Ͳ�ͬ
					{
						if(int(it) > int(pDestFunc->pParam[iParam]->type))//Ҫ����ǿ������ת��
						{
							if(!ErrorReport(ET_MUSTCHANGETYPE))
								return FALSE;
						}
						else//������ʽ����ת��
						{
							ZOP op;
							switch(pDestFunc->pParam[iParam]->type)
							{
							case IT_DOUBLE:
								op= (it==IT_CHAR ? OP_C2D : OP_I2D);
								break;
							case IT_INT:
								op= OP_C2I;
								break;
							}
							if(!GenerateCode(op,0))
								return FALSE;
						}
					}
					else;//������ͬ�����趯��
				}
			}
			iParam++;
			if(m_element!=E_COMMA)//���Ƕ��ţ���������
				break;
			if(!GetElement())
				return FALSE;
		}
		if(m_element!=E_RPAREN)//����)������
		{
			if(!ErrorReport(ET_MISSRPAREN))
				return FALSE;
		}
		if(iParam<nParam)//������Ŀ���ԣ�����
		{
			if(!ErrorReport(ET_WRONGPARAMNUM))
				return FALSE;
		}
	}
	if(!GenerateCode(OP_CALL,nDestFunc))
		return FALSE;
	m_it=pDestFunc->ReturnType;
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoWhile()
{
	const int nTrue=m_nTrue,nFalse=m_nFalse;
	const int nBreak=m_nBreak,nContinue=m_nContinue;
	if(!GenerateCode(OP_JUMP,m_nCode+3))
		return FALSE;
	m_nTrue=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	m_nFalse=m_nBreak=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	m_nContinue=m_nCode;
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)//����(������
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡwhile�������ĵ�һ������
		return FALSE;
	if(!DoCondition())
		return FALSE;
	if(m_element!=E_RPAREN)//����)������
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡwhile�����ĵ�һ������
		return FALSE;
	m_pCode[m_nTrue].sd.iNumber=m_nCode;//����������յ��ַ
	m_pFunction[m_nCurFunction]->CurLevel++;
	if(!DoStatement())
		return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel--;
	if(!RecycleMemory())
		return FALSE;
	if(!GenerateCode(OP_JUMP,m_nContinue))
		return FALSE;
	m_pCode[m_nBreak].sd.iNumber=m_nCode;//����ٳ����ص��ַ
	m_nTrue=nTrue;
	m_nFalse=nFalse;
	m_nBreak=nBreak;
	m_nContinue=nContinue;
	return TRUE;
}

BOOL CFCView::DoDoWhile()
{
	const int nTrue=m_nTrue,nFalse=m_nFalse;
	const int nBreak=m_nBreak,nContinue=m_nContinue;
	if(!GenerateCode(OP_JUMP,m_nCode+2))
		return FALSE;
	m_nBreak=m_nFalse=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	m_nContinue=m_nTrue=m_nCode;
	if(!GetElement())
		return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel++;
	if(!DoStatement())
		return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel--;
	if(!RecycleMemory())
		return FALSE;
	if(m_element!=E_WHILE)//����while������
	{
		if(!ErrorReport(ET_DONOWHILE))
			return FALSE;
	}
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)//����(������
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡwhile�������ĵ�һ������
		return FALSE;
	if(!DoCondition())
		return FALSE;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	if(m_element!=E_SEMICOLON)
	{
		if(!ErrorReport(ET_MISSSEMICOLON))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	m_pCode[m_nBreak].sd.iNumber=m_nCode;
	return TRUE;
}

BOOL CFCView::DoExpression()
{
	m_it=IT_CHAR;
	ZIT it;
	BOOL bMinus;
	if(m_element==E_PLUS || m_element==E_MINUS)//ǰ�� + -
	{
		bMinus= (m_element==E_MINUS);
		if(!GetElement())//��ȡ��ĵ�һ������
			return FALSE;
		if(!DoTerm())
			return FALSE;
		if(bMinus)//ǰ�и��ţ�����Ŵ���
		{
			ZOP op;
			switch(m_it)
			{
			case IT_CHAR:	op=OP_CHANGESIGNALC;break;
			case IT_INT:	op=OP_CHANGESIGNALI;break;
			case IT_DOUBLE:	op=OP_CHANGESIGNALD;break;
			}
			if(!GenerateCode(op))
				return FALSE;
		}
	}
	else//ǰ��+ -
	{
		if(!DoTerm())
			return FALSE;
	}
	while(m_element==E_PLUS || m_element==E_MINUS)//ѭ��������
	{
		bMinus= (m_element==E_MINUS);
		if(!GetElement())//��ȡ��ĵ�һ������
			return FALSE;
		it=m_it;//����ǰ���������
		if(!DoTerm())
			return FALSE;
		if(it!=m_it)//ǰ��������Ͳ�ͬ
		{
			ZOP op;
			int offset;
			switch(it)//�������
			{
			case IT_CHAR:
				if(m_it==IT_INT)
					op=OP_C2I;
				else
					op=OP_C2D;
				offset= -1;
				break;
			case IT_INT:
				if(m_it==IT_CHAR)
				{
					m_it=it;
					op=OP_C2I;
					offset=0;
				}
				else
				{
					op=OP_I2D;
					offset= -1;
				}
				break;
			case IT_DOUBLE:
				if(m_it==IT_CHAR)
					op=OP_C2D;
				else
					op=OP_I2D;
				m_it=it;
				offset=0;
				break;
			}
			if(!GenerateCode(op,offset))//������ʽ����ת������
				return FALSE;
		}
		else;//ǰ�����������ͬ�����趯��
		ZOP op;
		switch(m_it)
		{
		case IT_CHAR:
			op= (bMinus ? OP_MINUSC : OP_PLUSC);
			break;
		case IT_INT:
			op= (bMinus ? OP_MINUSI : OP_PLUSI);
			break;
		case IT_DOUBLE:
			op= (bMinus ? OP_MINUSD : OP_PLUSD);
			break;
		}
		if(!GenerateCode(op))
			return FALSE;
	}
	return TRUE;
}

BOOL CFCView::DoTerm()
{
	m_it=IT_CHAR;
	ZIT it;
	ZElement elmt;
	if(!DoFactor())
		return FALSE;
	while(m_element==E_TIMES || m_element==E_SLASH || m_element==E_MOD)
	{
		elmt=m_element;
		if(!GetElement())//��ȡ�¸����ӵĵ�һ������
			return FALSE;
		it=m_it;//������һ�����ӵ�����
		if(!DoFactor())
			return FALSE;
		if(it!=m_it)
		{
			ZOP op;
			int offset;
			switch(it)//�������
			{
			case IT_CHAR:
				if(m_it==IT_INT)
					op=OP_C2I;
				else
					op=OP_C2D;
				offset= -1;
				break;
			case IT_INT:
				if(m_it==IT_CHAR)
				{
					m_it=it;
					op=OP_C2I;
					offset=0;
				}
				else
				{
					op=OP_I2D;
					offset= -1;
				}
				break;
			case IT_DOUBLE:
				if(m_it==IT_CHAR)
					op=OP_C2D;
				else
					op=OP_I2D;
				m_it=it;
				offset=0;
				break;
			}
			if(!GenerateCode(op,offset))//������ʽ����ת������
				return FALSE;
		}
		else;//ǰ�����������ͬ�����趯��
		if(elmt==E_MOD && m_it==IT_DOUBLE)//ʵ����������������
		{
			if(!ErrorReport(ET_DOUBLEMOD))
				return FALSE;
			elmt=E_SLASH;
		}
		ZOP op;
		switch(m_it)
		{
		case IT_CHAR:
			op= (elmt==E_TIMES ? OP_TIMESC : (elmt==E_SLASH ? OP_SLASHC : OP_MODC));
			break;
		case IT_INT:
			op= (elmt==E_TIMES ? OP_TIMESI : (elmt==E_SLASH ? OP_SLASHI : OP_MODI));
			break;
		case IT_DOUBLE:
			op= (elmt==E_TIMES ? OP_TIMESD : OP_SLASHD);
			break;
		}
		if(!GenerateCode(op))
			return FALSE;
	}
	return TRUE;
}

BOOL CFCView::DoFactor()
{
	m_it=IT_CHAR;
	ZIT it=IT_VOID;//����ǿ������ת��
	int address;//�洢��ʶ���ĵ�ַ
	switch(m_element)
	{
	case E_CHARACTER://************�ַ�
		m_it=IT_CHAR;
		if(!GenerateCode(OP_LOADCONSTC,m_char))
			return FALSE;
		if(!GetElement())
			return FALSE;
		break;
	case E_INUMBER://**************����
		m_it=IT_INT;
		if(!GenerateCode(OP_LOADCONSTI,m_int))
			return FALSE;
		if(!GetElement())
			return FALSE;
		break;
	case E_DNUMBER://**************ʵ��
		m_it=IT_DOUBLE;
		if(!GenerateCode(OP_LOADCONSTD,m_double))
			return FALSE;
		if(!GetElement())
			return FALSE;
		break;
	case E_CHAR://char
	case E_INT://int
	case E_DOUBLE://*******ǿ������ת��
		it= (m_element==E_CHAR ? IT_CHAR : (m_element==E_INT ? IT_INT : IT_DOUBLE));
		if(!GetElement())
			return FALSE;
		if(m_element!=E_LPAREN)
		{
			if(!ErrorReport(ET_MISSLPAREN))
				return FALSE;
		}
		//�˴�������break���
	case E_LPAREN://*****�������ı��ʽ
		if(!GetElement())
			return FALSE;
		if(!DoExpression())
			return FALSE;
		if(m_element!=E_RPAREN)
		{
			if(!ErrorReport(ET_MISSRPAREN))
				return FALSE;
			//�������룬Ȩ���˴��и���С����
		}
		if(it!=IT_VOID && it!=m_it)
		{
			ZOP op;
			switch(it)
			{
			case IT_CHAR:
				op= (m_it==IT_INT ? OP_I2C : OP_D2C);
				break;
			case IT_INT:
				op= (m_it==IT_CHAR ? OP_C2I : OP_D2I);
				break;
			case IT_DOUBLE:
				op= (m_it==IT_CHAR ? OP_C2D : OP_I2D);
				break;
			}
			if(!GenerateCode(op,0))
				return FALSE;
			m_it=it;
		}
		if(!GetElement())
			return FALSE;
		break;
	case E_IDENT://**************��ʶ��
		address=GetIdentAddress(m_ident);
		switch(GetIdentType(m_ident))//���ݱ�ʶ�����ʹ���
		{
		case IT_NULL://δ����ı�ʶ��
			if(!ErrorReport(ET_UNKNOWNIDENT))
				return FALSE;
			if(!GetElement())
				return FALSE;
			m_it=IT_CHAR;
			break;
		case IT_FUNCTION:
		case IT_FUNCTIONUS://������
			m_it = m_pFunction[GetFunctionIndex(m_ident)]->ReturnType;
			if(m_it==IT_VOID)//void����
			{
				if(!ErrorReport(ET_VOIDINEXPRESSION))
					return FALSE;
			}
			if(!DoCall())
				return FALSE;
			break;
		case IT_CHAR:
		case IT_ECHAR://�ַ�����
			if(!GenerateCode(OP_LOADVARC,address))
				return FALSE;
			if(!GetElement())
				return FALSE;
			m_it=IT_CHAR;
			break;
		case IT_INT:
		case IT_EINT://��������
			if(!GenerateCode(OP_LOADVARI,address))
				return FALSE;
			if(!GetElement())
				return FALSE;
			m_it=IT_INT;
			break;
		case IT_DOUBLE:
		case IT_EDOUBLE://ʵ������
			if(!GenerateCode(OP_LOADVARD,address))
				return FALSE;
			if(!GetElement())
				return FALSE;
			m_it=IT_DOUBLE;
			break;
		case IT_CHARARRAY:
		case IT_ECHARARRAY://�ַ�����
			if(!GetElement())
				return FALSE;
			if(!DoSubscript())
				return FALSE;
			if(!GenerateCode(OP_LOADARRAYC,address))
				return FALSE;
			m_it=IT_CHAR;
			break;
		case IT_INTARRAY:
		case IT_EINTARRAY://��������
			if(!GetElement())
				return FALSE;
			if(!DoSubscript())
				return FALSE;
			if(!GenerateCode(OP_LOADARRAYI,address))
				return FALSE;
			m_it=IT_INT;
			break;
		case IT_DOUBLEARRAY:
		case IT_EDOUBLEARRAY://ʵ������
			if(!GetElement())
				return FALSE;
			if(!DoSubscript())
				return FALSE;
			if(!GenerateCode(OP_LOADARRAYD,address))
				return FALSE;
			m_it=IT_DOUBLE;
			break;
		}//end inner switch
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
		if(!DoMathFunctionCall())
			return FALSE;
		break;
	case E_ABS://********** abs��������
		if(!DoAbsCall())
			return FALSE;
		break;
	case E_POW://********** pow��������
		if(!DoPowCall())
			return FALSE;
		break;
	case E_STRCMP://**** strcmp��������
		if(!DoStrcmpCall())
			return FALSE;
		break;
	case E_STRLEN://**** strlen��������
		if(!DoStrlenCall())
			return FALSE;
		break;
	case E_AVERAGE:
	case E_SUM:
	case E_MAX:
	case E_MIN://******* ����⺯������
		if(!DoArrayFunctionCall())
			return FALSE;
		break;
	default:
		if(!ErrorReport(ET_UNKNOWNERROR))
			return FALSE;
		if(!GetElement())
			return FALSE;
		break;
	}//end outer switch
	return TRUE;
}

BOOL CFCView::DoSubscript()
{
	if(m_element!=E_LSUB)//����[������
	{
		if(!ErrorReport(ET_MISSLSUB))
			return FALSE;
	}
	if(!GetElement())//��ȡ�±���ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	if(m_it==IT_DOUBLE)//ʵ�����ʽ��Ϊ�����±꣬����
	{
		if(!ErrorReport(ET_DOUBLESUB))
			return FALSE;
	}
	if(m_it==IT_CHAR)//�ַ����ʽ��Ϊ�����±꣬������ʽ����ת��
	{
		if(!GenerateCode(OP_C2I,0))
			return FALSE;
	}
	if(m_element!=E_RSUB)//����]������
	{
		if(!ErrorReport(ET_MISSRSUB))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoCondition()
{
	if(!DoAndCondition())
		return FALSE;
	while(m_element==E_OR)
	{
		if(!GetElement())
			return FALSE;
		if(!DoAndCondition())
			return FALSE;
	}
	if(!GenerateCode(OP_JUMP,m_nFalse))
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoAndCondition()
{
	const int nFalse=m_nFalse;
	if(!GenerateCode(OP_JUMP,m_nCode+2))
		return FALSE;
	m_nFalse=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	if(!DoSubCondition())
		return FALSE;
	while(m_element==E_AND)
	{
		if(!GetElement())
			return FALSE;
		if(!DoSubCondition())
			return FALSE;
	}
	if(!GenerateCode(OP_JUMP,m_nTrue))
		return FALSE;
	m_pCode[m_nFalse].sd.iNumber=m_nCode;
	m_nFalse=nFalse;
	return TRUE;
}

BOOL CFCView::DoSubCondition()
{
	if(m_element==E_NOT)
	{
		if(!GetElement())
			return FALSE;
		if(!DoFactor())
			return FALSE;
		switch(m_it)
		{
		case IT_CHAR:
			if(!GenerateCode(OP_LOADCONSTC,'\0'))
				return FALSE;
			if(!GenerateCode(OP_NOTEQUALC,m_nFalse))
				return FALSE;
			break;
		case IT_INT:
			if(!GenerateCode(OP_LOADCONSTI,0))
				return FALSE;
			if(!GenerateCode(OP_NOTEQUALI,m_nFalse))
				return FALSE;
			break;
		case IT_DOUBLE:
			if(!GenerateCode(OP_LOADCONSTD,0.0))
				return FALSE;
			if(!GenerateCode(OP_NOTEQUALD,m_nFalse))
				return FALSE;
			break;
		}
	}
	else
	{
		ZIT it;
		ZElement elmt;
		if(!DoExpression())
			return FALSE;
		it=m_it;
		switch(m_element)
		{
		case E_EQUAL:
		case E_NOTEQUAL:
		case E_LESS:
		case E_LESSEQUAL:
		case E_GREAT:
		case E_GREATEQUAL:
			elmt=m_element;
			break;
		default:
			switch(it)
			{
			case IT_CHAR:
				if(!GenerateCode(OP_LOADCONSTC,'\0'))
					return FALSE;
				if(!GenerateCode(OP_EQUALC,m_nFalse))
					return FALSE;
				break;
			case IT_INT:
				if(!GenerateCode(OP_LOADCONSTI,0))
					return FALSE;
				if(!GenerateCode(OP_EQUALI,m_nFalse))
					return FALSE;
				break;
			case IT_DOUBLE:
				if(!GenerateCode(OP_LOADCONSTD,0.0))
					return FALSE;
				if(!GenerateCode(OP_EQUALD,m_nFalse))
					return FALSE;
				break;
			}
			return TRUE;
		}
		if(!GetElement())
			return FALSE;
		if(!DoExpression())
			return FALSE;
		if(m_it != it)//���Ͳ�ͬ
		{
			ZOP op;
			int offset;
			switch(it)
			{
			case IT_CHAR:
				if(m_it==IT_INT)
					op=OP_C2I;
				else
					op=OP_C2D;
				offset=-1;
				break;
			case IT_INT:
				if(m_it==IT_CHAR)
				{
					op=OP_C2I;
					offset=0;
				}
				else
				{
					op=OP_I2D;
					offset=-1;
				}
				break;
			case IT_DOUBLE:
				if(m_it==IT_CHAR)
					op=OP_C2D;
				else
					op=OP_I2D;
				offset=0;
				break;
			}
			m_it= (int(m_it)>int(it) ? m_it : it);
			if(!GenerateCode(op,offset))
				return FALSE;
		}
		ZOP op;
		switch(m_it)
		{
		case IT_CHAR:
			switch(elmt)
			{
			case E_EQUAL:		op=OP_NOTEQUALC;	break;
			case E_NOTEQUAL:	op=OP_EQUALC;		break;
			case E_LESS:		op=OP_GREATEQUALC;	break;
			case E_LESSEQUAL:	op=OP_GREATC;		break;
			case E_GREAT:		op=OP_LESSEQUALC;	break;
			case E_GREATEQUAL:	op=OP_LESSC;		break;
			}
			break;
		case IT_INT:
			switch(elmt)
			{
			case E_EQUAL:		op=OP_NOTEQUALI;	break;
			case E_NOTEQUAL:	op=OP_EQUALI;		break;
			case E_LESS:		op=OP_GREATEQUALI;	break;
			case E_LESSEQUAL:	op=OP_GREATI;		break;
			case E_GREAT:		op=OP_LESSEQUALI;	break;
			case E_GREATEQUAL:	op=OP_LESSI;		break;
			}
			break;
		case IT_DOUBLE:
			switch(elmt)
			{
			case E_EQUAL:		op=OP_NOTEQUALD;	break;
			case E_NOTEQUAL:	op=OP_EQUALD;		break;
			case E_LESS:		op=OP_GREATEQUALD;	break;
			case E_LESSEQUAL:	op=OP_GREATD;		break;
			case E_GREAT:		op=OP_LESSEQUALD;	break;
			case E_GREATEQUAL:	op=OP_LESSD;		break;
			}
			break;
		}//end switch
		if(!GenerateCode(op,m_nFalse))
			return FALSE;
	}
	return TRUE;
}

BOOL CFCView::DoCase()
{
	int nCase;
	if(!GenerateCode(OP_COPYI))//����ջ����
		return FALSE;
	if(!GetElement())//��ȡcase����������ַ�
		return FALSE;
	if(m_element==E_CHARACTER)
	{
		if(!GenerateCode(OP_LOADCONSTI,int(m_char)))
			return FALSE;
	}
	else if(m_element==E_INUMBER)
	{
		if(!GenerateCode(OP_LOADCONSTI,m_int))
			return FALSE;
	}
	else//�ȷ��ַ��ַ�����������
	{
		if(!ErrorReport(ET_DOUBLESWITCH))
			return FALSE;
	}
	if(!GetElement())//��ȡ:
		return FALSE;
	if(m_element!=E_COLON)//����:������
	{
		if(!ErrorReport(ET_MISSCOLON))
			return FALSE;
	}
	nCase=m_nCode;
	if(!GenerateCode(OP_NOTEQUALI,0))
		return FALSE;
	if(m_nCase>0)//����case������ת��ַ
		m_pCode[m_nCase].sd.iNumber=m_nCode;
	m_pFunction[m_nCurFunction]->CurLevel++;//Ƕ�ײ�μ�һ
	if(!GetElement())//��ȡcase��֧�е�һ�����ĵ�һ������
		return FALSE;
	while(m_element!=E_CASE && m_element!=E_DEFAULT && m_element!=E_END)
	{
		if(!DoStatement())
			return FALSE;
	}
	m_pFunction[m_nCurFunction]->CurLevel--;//Ƕ�ײ�μ�һ
	if(!RecycleMemory())
		return FALSE;
	m_nCase=m_nCode;
	if(!GenerateCode(OP_JUMP,0))
		return FALSE;
	m_pCode[nCase].sd.iNumber=m_nCode;//��������case��֧����תĿ�ĵ�ַ
	return TRUE;
}

BOOL CFCView::DoDefault()
{
	if(!GetElement())//��ȡdefault���ð��
		return FALSE;
	if(m_element!=E_COLON)//����ð�ţ�����
	{
		if(!ErrorReport(ET_MISSCOLON))
			return FALSE;
	}
	if(m_nCase>0)
		m_pCode[m_nCase].sd.iNumber=m_nCode;
	if(!GetElement())//��ȡð�ź��һ�����ĵ�һ������
		return FALSE;
	m_pFunction[m_nCurFunction]->CurLevel++;
	while(m_element!=E_END)
	{
		if(!DoStatement())
			return FALSE;
	}
	m_pFunction[m_nCurFunction]->CurLevel--;
	if(!RecycleMemory())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoEndProgram()
{
	int i;
	for(i=m_nFunction-1;i>=0;i--)//Ѱ��main����
		if("main"==m_pFunction[i]->name)
			break;
	if(i<0)//�Ҳ���main����
	{
		if(!ErrorReport(ET_NOMAIN))
			return FALSE;
	}
	else
	{
		if(m_pFunction[i]->ReturnType!=IT_VOID)//���main������������
		{
			if(!ErrorReport(ET_NOVOIDMAIN))
				return FALSE;
		}
		if(m_pFunction[i]->nParam>0)//���main�����Ĳ�����Ŀ
		{
			if(!ErrorReport(ET_PARAMMAIN))
				return FALSE;
		}
		m_pCode[0].sd.iNumber=i;//����call mainָ��ĵ�ַ
	}
	if(!ExamUnsolvedFunctions())//����Ƿ��б������˵�ȴû��ʵ�ֵĺ���
		return FALSE;
	if(m_iErrorCount>0)//�������﷨�����򲻽����±ߵ��Ż�
		return TRUE;
	for(i=0;i<m_nCode;i++)//����callָ��ĵ�ַ
	{
		if(m_pCode[i].op==OP_CALL)
			m_pCode[i].sd.iNumber=m_pFunction[m_pCode[i].sd.iNumber]->nStartAddress;
	}
	if(m_bJumpOptimize)
		DoOptimize();
	return TRUE;
}

void CFCView::DoOptimize()
{
	int i;
	//��ת���Ż�
	int dest;
	for(i=0;i<m_nCode;i++)
	{
		switch(m_pCode[i].op)
		{
		case OP_JUMP:
		case OP_EQUALC:
		case OP_EQUALD:
		case OP_EQUALI:
		case OP_NOTEQUALC:
		case OP_NOTEQUALD:
		case OP_NOTEQUALI:
		case OP_LESSC:
		case OP_LESSD:
		case OP_LESSI:
		case OP_LESSEQUALC:
		case OP_LESSEQUALD:
		case OP_LESSEQUALI:
		case OP_GREATC:
		case OP_GREATD:
		case OP_GREATI:
		case OP_GREATEQUALC:
		case OP_GREATEQUALD:
		case OP_GREATEQUALI:
			dest=i;
			do
				dest=m_pCode[dest].sd.iNumber;
			while(m_pCode[dest].op==OP_JUMP);
			m_pCode[i].sd.iNumber=dest;
			break;
		default:
			break;
		}
	}
	//ȥ�����ɵ������
	bool *bReach=new bool[m_nCode];
	BOOL b=TRUE;
	for(i=0;i<m_nCode;i++)
		bReach[i]=false;
	bReach[0]=true;
	for(i=1;i<m_nCode;i++)//��ʼ�����⺯���ĵ�һ�����붼�ǿɵ����
	{
		if(m_pCode[i].op==OP_FUNCTIONSTART)
			bReach[i]=true;
	}
	while(b)
	{
		b=FALSE;
		for(i=1;i<m_nCode;i++)
		{
			if(!bReach[i])
				continue;
			switch(m_pCode[i].op)
			{
			case OP_CALL:
			case OP_EQUALC:
			case OP_EQUALD:
			case OP_EQUALI:
			case OP_NOTEQUALC:
			case OP_NOTEQUALD:
			case OP_NOTEQUALI:
			case OP_LESSC:
			case OP_LESSD:
			case OP_LESSI:
			case OP_LESSEQUALC:
			case OP_LESSEQUALD:
			case OP_LESSEQUALI:
			case OP_GREATC:
			case OP_GREATD:
			case OP_GREATI:
			case OP_GREATEQUALC:
			case OP_GREATEQUALD:
			case OP_GREATEQUALI:
				if(!bReach[i+1])
				{
					b=TRUE;
					bReach[i+1]=true;
				}
				if(!bReach[m_pCode[i].sd.iNumber])
				{
					b=TRUE;
					bReach[m_pCode[i].sd.iNumber]=true;
				}
				break;
			case OP_JUMP:
				if(!bReach[m_pCode[i].sd.iNumber])
				{
					b=TRUE;
					bReach[m_pCode[i].sd.iNumber]=true;
				}
				break;
			case OP_NULL:
			case OP_RETURN:
			case OP_RETURNC:
			case OP_RETURND:
			case OP_RETURNI:
				break;
			default:
				if(i+1<m_nCode && !bReach[i+1])
				{
					b=TRUE;
					bReach[i+1]=true;
				}
				break;
			}
		}
	}
	for(i=1;i<m_nCode;i++)
		if(!bReach[i])
			m_pCode[i].op=OP_NULL;
	delete bReach;
	//ȥ����Ч����������ת
	int j;
	for(i=0;i<m_nCode;i++)
	{
		if(m_pCode[i].op==OP_JUMP)
		{
			for(j=i+1;m_pCode[j].op==OP_NULL;j++);
			if(j==m_pCode[i].sd.iNumber)
				m_pCode[i].op=OP_NULL;
		}
	}
	//��OP_NULL�Ӵ������޳�������С�м����洢�ռ�
	int nCode=0;//��Ч�������
	for(i=0;i<m_nCode;i++)
		if(m_pCode[i].op!=OP_NULL)
			nCode++;
	int nMaxCode=nCode/CODE_NUM_INC*CODE_NUM_INC+CODE_NUM_INC;//��nCode���CODE_NUM_INC����С����
	ZCode *pCode=new ZCode[nMaxCode];
	int k;
	for(i=j=0;i<m_nCode;i++)
	{
		if(m_pCode[i].op==OP_NULL)
			continue;
		memcpy(&pCode[j],&m_pCode[i],sizeof(ZCode));
		if(pCode[j].op==OP_FUNCTIONSTART)
		{
			for(k=0;k<m_nFunction;k++)
				if(m_pFunction[k]->nStartAddress==i)
					m_pFunction[k]->nStartAddress=j;
		}
		for(k=0;k<j;k++)
		{
			switch(pCode[k].op)
			{
			case OP_JUMP:
			case OP_CALL:
			case OP_EQUALC:
			case OP_EQUALD:
			case OP_EQUALI:
			case OP_NOTEQUALC:
			case OP_NOTEQUALD:
			case OP_NOTEQUALI:
			case OP_LESSC:
			case OP_LESSD:
			case OP_LESSI:
			case OP_LESSEQUALC:
			case OP_LESSEQUALD:
			case OP_LESSEQUALI:
			case OP_GREATC:
			case OP_GREATD:
			case OP_GREATI:
			case OP_GREATEQUALC:
			case OP_GREATEQUALD:
			case OP_GREATEQUALI:
				if(pCode[k].sd.iNumber==i)
					pCode[k].sd.iNumber=j;
				break;
			default:
				break;
			}
		}
		for(k=i+1;k<m_nCode;k++)
		{
			switch(m_pCode[k].op)
			{
			case OP_JUMP:
			case OP_CALL:
			case OP_EQUALC:
			case OP_EQUALD:
			case OP_EQUALI:
			case OP_NOTEQUALC:
			case OP_NOTEQUALD:
			case OP_NOTEQUALI:
			case OP_LESSC:
			case OP_LESSD:
			case OP_LESSI:
			case OP_LESSEQUALC:
			case OP_LESSEQUALD:
			case OP_LESSEQUALI:
			case OP_GREATC:
			case OP_GREATD:
			case OP_GREATI:
			case OP_GREATEQUALC:
			case OP_GREATEQUALD:
			case OP_GREATEQUALI:
				if(m_pCode[k].sd.iNumber==i)
					m_pCode[k].sd.iNumber=j;
				break;
			default:
				break;
			}
		}
		j++;
	}
	m_nMaxCode=nMaxCode;
	m_nCode=nCode;
	delete m_pCode;
	m_pCode=pCode;
}

BOOL CFCView::DoMathFunctionCall()
{
	const ZOP op=ZOP(OP_ARCCOS+m_element-E_ARCCOS);
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ�������ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	switch(m_it)//�������
	{
	case IT_CHAR:
		if(!GenerateCode(OP_C2D,0))
			return FALSE;
		break;
	case IT_INT:
		if(!GenerateCode(OP_I2D,0))
			return FALSE;
		break;
	}
	if(!GenerateCode(op))
		return FALSE;
	m_it=IT_DOUBLE;
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoAbsCall()
{
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ�������ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	ZOP op;
	switch(m_it)//�������
	{
	case IT_CHAR:	op=OP_ABSC;break;
	case IT_INT:	op=OP_ABSI;break;
	case IT_DOUBLE:	op=OP_ABSD;break;
	}
	if(!GenerateCode(op))
		return FALSE;
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoPowCall()
{
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ��һ���������ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	switch(m_it)//�������
	{
	case IT_CHAR:
		if(!GenerateCode(OP_C2D,0))
			return FALSE;
		break;
	case IT_INT:
		if(!GenerateCode(OP_I2D,0))
			return FALSE;
		break;
	}
	if(m_element!=E_COMMA)
	{
		if(!ErrorReport(ET_MISSCOMMA))
			return FALSE;
	}
	if(!GetElement())//��ȡ�ڶ����������ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	switch(m_it)//�������
	{
	case IT_CHAR:
		if(!GenerateCode(OP_C2D,0))
			return FALSE;
		break;
	case IT_INT:
		if(!GenerateCode(OP_I2D,0))
			return FALSE;
		break;
	}
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GenerateCode(OP_POW))
		return FALSE;
	m_it=IT_DOUBLE;
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoStrcmpCall()
{
	int i;
	int address[2];
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	for(i=0;i<2;i++)
	{
		if(!GetElement())//��ȡ�ַ������[
			return FALSE;
		if(m_element!=E_LSUB)
		{
			if(!ErrorReport(ET_MISSLSUB))
				return FALSE;
		}
		if(!GetElement())//��ȡ�ַ������]
			return FALSE;
		if(m_element!=E_RSUB)
		{
			if(!ErrorReport(ET_MISSRSUB))
				return FALSE;
		}
		if(!GetElement())//��ȡ�ַ������ʶ��
			return FALSE;
		if(m_element!=E_IDENT)
		{
			if(!ErrorReport(ET_MISSIDENT))
				return FALSE;
			m_ident="";
		}
		switch(GetIdentType(m_ident))
		{
		case IT_CHARARRAY:
		case IT_ECHARARRAY:
			address[i]=GetIdentAddress(m_ident);
			break;
		default:
			if(!ErrorReport(ET_WRONGPARAMTYPE))
				return FALSE;
			address[i]=0;
			break;
		}
		if(!GetElement())//��ȡ,)
			return FALSE;
		if(i==0 && m_element!=E_COMMA)
		{
			if(!ErrorReport(ET_MISSCOMMA))
				return FALSE;
		}
	}
	if(!GenerateCode(OP_STRCMP,address[0],address[1]))
		return FALSE;
	m_it=IT_INT;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoStrcpyCall()
{
	int i;
	int address[2];
	BOOL bStringToCA=FALSE;
	char *string;

	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	for(i=0;i<2;i++)
	{
		if(!GetElement())//��ȡ�ַ������[����Ϊ�ڶ��������ַ���
			return FALSE;
		if(i==1 && m_element==E_STRING)
		{
			bStringToCA=TRUE;
			string=new char[m_string.GetLength()+1];
			strcpy(string,m_string);
			if(!GetElement())
				return FALSE;
		}
		else
		{
			if(m_element!=E_LSUB)
			{
				if(!ErrorReport(ET_MISSLSUB))
					return FALSE;
			}
			if(!GetElement())//��ȡ�ַ������]
				return FALSE;
			if(m_element!=E_RSUB)
			{
				if(!ErrorReport(ET_MISSRSUB))
					return FALSE;
			}
			if(!GetElement())//��ȡ�ַ������ʶ��
				return FALSE;
			if(m_element!=E_IDENT)
			{
				if(!ErrorReport(ET_MISSIDENT))
					return FALSE;
				m_ident="";
			}
			switch(GetIdentType(m_ident))
			{
			case IT_CHARARRAY:
			case IT_ECHARARRAY:
				address[i]=GetIdentAddress(m_ident);
				break;
			default:
				if(!ErrorReport(ET_WRONGPARAMTYPE))
					return FALSE;
				address[i]=0;
				break;
			}
			if(!GetElement())//��ȡ,)
				return FALSE;
			if(i==0 && m_element!=E_COMMA)
			{
				if(!ErrorReport(ET_MISSCOMMA))
					return FALSE;
			}
		}
	}
	if(bStringToCA)
	{
		if(!GenerateCode(OP_STRCPYS,address[0],string))
			return FALSE;
	}
	else
	{
		if(!GenerateCode(OP_STRCPYCA,address[0],address[1]))
			return FALSE;
	}
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoStrlenCall()
{
	BOOL bString;
	char *string;
	int address;

	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ[���ַ���
		return FALSE;
	if(m_element==E_STRING)//���ַ����ĳ���
	{
		bString=TRUE;
		string=new char[m_string.GetLength()+1];
		strcpy(string,m_string);
	}
	else//���ַ�����ĳ���
	{
		bString=FALSE;
		if(m_element!=E_LSUB)
		{
			if(!ErrorReport(ET_MISSLPAREN))
				return FALSE;
		}
		if(!GetElement())//��ȡ]
			return FALSE;
		if(m_element!=E_RSUB)
		{
			if(!ErrorReport(ET_MISSRPAREN))
				return FALSE;
		}
		if(!GetElement())//��ȡ�ַ������ʶ��
			return FALSE;
		if(m_element!=E_IDENT)
		{
			if(!ErrorReport(ET_MISSIDENT))
				return FALSE;
			m_ident="";
		}
		switch(GetIdentType(m_ident))
		{
		case IT_CHARARRAY:
		case IT_ECHARARRAY:
			address=GetIdentAddress(m_ident);
			break;
		default:
			if(!ErrorReport(ET_WRONGPARAMTYPE))
				return FALSE;
			address=0;
			break;
		}
	}
	if(!GetElement())//��ȡ)
		return FALSE;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(bString)
	{
		if(!GenerateCode(OP_STRLENS,string))
			return FALSE;
	}
	else
	{
		if(!GenerateCode(OP_STRLENCA,address))
			return FALSE;
	}
	m_it=IT_INT;
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoStrlwruprCall()
{
	ZOP op;
	if(m_element==E_STRLWR)
		op=OP_STRLWR;
	else
		op=OP_STRUPR;
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ[
		return FALSE;
	if(m_element!=E_LSUB)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ]
		return FALSE;
	if(m_element!=E_RSUB)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ�ַ������ʶ��
		return FALSE;
	if(m_element!=E_IDENT)
	{
		if(!ErrorReport(ET_MISSIDENT))
			return FALSE;
		m_ident="";
	}
	switch(GetIdentType(m_ident))
	{
	case IT_CHARARRAY:
	case IT_ECHARARRAY:
		if(!GenerateCode(op,GetIdentAddress(m_ident)))
			return FALSE;
		break;
	default:
		if(!ErrorReport(ET_WRONGPARAMTYPE))
			return FALSE;
		break;
	}
	if(!GetElement())//��ȡ)
		return FALSE;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoArrayFunctionCall()
{
	ZOP op;
	ZIT it;
	int address;

	switch(m_element)
	{
	case E_AVERAGE:	op=OP_AVERAGEC;	break;
	case E_MAX:		op=OP_MAXC;		break;
	case E_MIN:		op=OP_MINC;		break;
	case E_SUM:		op=OP_SUMC;		break;
	}
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ[
		return FALSE;
	if(m_element!=E_LSUB)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ]
		return FALSE;
	if(m_element!=E_RSUB)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ�����ʶ��
		return FALSE;
	if(m_element!=E_IDENT)
	{
		if(!ErrorReport(ET_MISSIDENT))
			return FALSE;
		m_ident="";
	}
	switch(GetIdentType(m_ident))
	{
	case IT_CHARARRAY:
	case IT_ECHARARRAY:
		if(op==OP_AVERAGEC)
			it=IT_DOUBLE;
		else if(op==OP_SUMC)
			it=IT_INT;
		else
			it=IT_CHAR;
		break;
	case IT_DOUBLEARRAY:
	case IT_EDOUBLEARRAY:
		it=IT_DOUBLE;
		op=ZOP(op+1);
		break;
	case IT_INTARRAY:
	case IT_EINTARRAY:
		if(op==OP_AVERAGEC)
			it=IT_DOUBLE;
		else
			it=IT_INT;
		op=ZOP(op+2);
		break;
	default:
		if(!ErrorReport(ET_WRONGPARAMTYPE))
			return FALSE;
		it=IT_INT;
	}
	address=GetIdentAddress(m_ident);
	if(!GetElement())//��ȡ,
		return FALSE;
	if(m_element!=E_COMMA)
	{
		if(!ErrorReport(ET_MISSCOMMA))
			return FALSE;
	}
	if(!GetElement())//��ȡ���鳤�ȱ��ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	switch(m_it)//�������
	{
	case IT_CHAR:
		if(!GenerateCode(OP_C2I,0))
			return FALSE;
		break;
	case IT_DOUBLE:
		if(!ErrorReport(ET_MUSTCHANGETYPE))
			return FALSE;
		break;
	}
	if(!GenerateCode(op,address))
		return FALSE;
	m_it=it;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}

BOOL CFCView::DoSortCall()
{
	ZOP op;
	int address;

	if(m_element==E_DOWNSORT)
		op=OP_DOWNSORTC;
	else
		op=OP_UPSORTC;
//	return FALSE;
	if(!GetElement())//��ȡ(
		return FALSE;
	if(m_element!=E_LPAREN)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ[
		return FALSE;
	if(m_element!=E_LSUB)
	{
		if(!ErrorReport(ET_MISSLPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ]
		return FALSE;
	if(m_element!=E_RSUB)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())//��ȡ�����ʶ��
		return FALSE;
	if(m_element!=E_IDENT)
	{
		if(!ErrorReport(ET_MISSIDENT))
			return FALSE;
		m_ident="";
	}
	switch(GetIdentType(m_ident))
	{
	case IT_CHARARRAY:
	case IT_ECHARARRAY:
		break;
	case IT_DOUBLEARRAY:
	case IT_EDOUBLEARRAY:
		op=ZOP(op+1);
		break;
	case IT_INTARRAY:
	case IT_EINTARRAY:
		op=ZOP(op+2);
		break;
	default:
		if(!ErrorReport(ET_WRONGPARAMTYPE))
			return FALSE;
	}
	address=GetIdentAddress(m_ident);
	if(!GetElement())//��ȡ,
		return FALSE;
	if(m_element!=E_COMMA)
	{
		if(!ErrorReport(ET_MISSCOMMA))
			return FALSE;
	}
	if(!GetElement())//��ȡ���鳤�ȱ��ʽ�ĵ�һ������
		return FALSE;
	if(!DoExpression())
		return FALSE;
	switch(m_it)//�������
	{
	case IT_CHAR:
		if(!GenerateCode(OP_C2I,0))
			return FALSE;
		break;
	case IT_DOUBLE:
		if(!ErrorReport(ET_MUSTCHANGETYPE))
			return FALSE;
		break;
	}
	if(!GenerateCode(op,address))
		return FALSE;
	if(m_element!=E_RPAREN)
	{
		if(!ErrorReport(ET_MISSRPAREN))
			return FALSE;
	}
	if(!GetElement())
		return FALSE;
	return TRUE;
}
