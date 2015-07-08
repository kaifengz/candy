#include "stdafx.h"
#include "FC.h"

#include "basic.h"
#include "FCInterpretDlg.h"
#include "FCInputDlg.h"
#include <math.h>
#include <string.h>
#include <stdlib.h>

#define PI						3.14159265358979323846
#define INIT_OUTPUT_BUFFER_SIZE	1024

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CFCInterpretDlg::TestAddress(int iAddress)
{
	if(iAddress>=m_nMemoryApply*256)
	{
		MessageBox("�ڴ��������ֹ����ִ�У�","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::SlashModZero(char ch)
{
	if(ch=='\0')
	{
		MessageBox("��/����%�����Ҳ�����Ϊ0����ֹ����ִ�У�",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::SlashModZero(int i)
{
	if(i==0)
	{
		MessageBox("��/����%�����Ҳ�����Ϊ0����ֹ����ִ�У�",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::SlashModZero(double d)
{
	if(d==0.0)
	{
		MessageBox("��/����%�����Ҳ�����Ϊ0����ֹ����ִ�У�",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::ArraySubscript(int index,int length)
{
	if(index<0 || index>=length)
	{
		MessageBox("�����±��������ֹ����ִ�У�","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::ValidArray(void *pArray)
{
	if(pArray==NULL)
	{
		MessageBox("�޷���������ռ䣬��ֹ����ִ�У�","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::ExamCircumscriptive(ZOP op,double value)
{
	CString msg="";
	switch(op)
	{
	case OP_ARCCOS:
		if(value<-1 || value>1)
			msg="arccos";
		break;
	case OP_ARCSIN:
		if(value<-1 || value>1)
			msg="arcsin";
		break;
	case OP_CTH:
		if(value==0)
			msg="cth";
		break;
	case OP_LG:
		if(value<=0)
			msg="lg";
		break;
	case OP_LN:
		if(value<=0)
			msg="ln";
		break;
	case OP_POW:
		if(value<0)
			msg="pow";
		break;
	case OP_SQRT:
		if(value<0)
			msg="sqrt";
		break;
	}
	if(msg!="")
	{
		msg+=" ��������ʱ����������������ֹ����ִ�У�";
		MessageBox(msg,"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::Interpret()
{
	const double m_nCode=0;//����˱���ֻ��Ϊ�������ⲿ��Ա����

	const ZCode *pCode=m_pCode;

	char cTemp;
	int iTemp,iTemp2;
	double dTemp;
	CString sTemp;

	int i;

	register int iBase=-1;		//��������ַ��ָ������һ��������ַ
	register int iCode=0;		//����ָ�룬ָ��ǰ����
	register int iAddress=0;	//��ַָ�룬ָ���һ��δ�õĵ�ַ��Ԫ
	int *memory=new int[m_nMemoryApply*256];	//���ڴ�

	register int iOp;			//ָ����������
	BOOL bWarning=!m_bIgnoreCircleNoend;		//�Ƿ�Կ��ܵ���ѭ�����о���

	clock_t tStart,tElapse=0;
	tStart=clock();

	InitInterpretOutput();

	for(iOp=0;;iOp++)
	{
		if(iOp==173330 && bWarning)//17333ֻ�Ǳ�ʾ173��33�ţ�����������
		{
			UpdateOutput();
			tElapse+=clock()-tStart;
			switch(MessageBox("\
������ܽ�����ѭ����Ҫ��ֹ����ִ����\n\
�����ǡ���ֹ����ִ�У������񡱼�������ִ��\n\
����ȡ������������ִ�в�������ʾ����Ϣ��",
				"FC 2.2",MB_YESNOCANCEL|MB_ICONQUESTION))
			{
			case IDYES:
				tStart=clock();
				goto EndInterpret;
				//�˴�����break���
			case IDCANCEL:
				bWarning=FALSE;
				//�˴�������break���
			case IDNO:
				tStart=clock();
				iOp=0;
				break;
			}
		}
		switch(pCode[iCode].op)
		{
		case OP_C2D:				//����ת�� char => double	��sd.iNumberΪ��ַƫ������
			if(!TestAddress(iAddress))
				return FALSE;
			if(pCode[iCode++].sd.iNumber==0)
				*((double*)&memory[iAddress-1])=*((char*)&memory[iAddress-1]);
			else//sd.iNumber==-1
			{
				*((double*)&memory[iAddress-1])=*((double*)&memory[iAddress-2]);
				*((double*)&memory[iAddress-3])=*((char*)&memory[iAddress-3]);
			}
			iAddress++;
			break;
		case OP_C2I:				//����ת�� char => int		��sd.iNumberΪ��ַƫ������
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-1]=*((char*)&memory[iAddress-1]);
			else
				memory[iAddress-2]=*((char*)&memory[iAddress-1]);
			break;
		case OP_CALL:				//���ú�����sdΪ������ַ��
			if(!TestAddress(iAddress+3))// +3 ����Ϊ�±���Ҫ3��Ԫ���ڴ�
				return FALSE;
			memory[iAddress  ]=iBase;	//����ԭ����ַ
			memory[iAddress+1]=iAddress;//����ԭջ��ָ��
			memory[iAddress+2]=iCode;	//����ԭ�м����ָ��
			iBase=iAddress+3;
			iCode=pCode[iCode].sd.iNumber;
			iAddress+=3;
			break;
		case OP_CHANGESIGNALC:		//�ı��ַ�����
			*((char*)(&memory[iAddress-1]))= - *((char*)(&memory[iAddress-1]));
			iCode++;
			break;
		case OP_CHANGESIGNALD:		//�ı�ʵ������
			*((double*)(&memory[iAddress-2]))= - *((double*)(&memory[iAddress-2]));
			iCode++;
			break;
		case OP_CHANGESIGNALI:		//�ı���������
			memory[iAddress-1]= -memory[iAddress-1];
			iCode++;
			break;
		case OP_COPYI:				//����ջ����ֵ��������
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress]=memory[iAddress-1];
			iAddress++;
			iCode++;
			break;
		case OP_D2C:				//����ת�� double => char	��sd.iNumberΪ��ַƫ������
			if(pCode[iCode++].sd.iNumber==0)
				*((char*)&memory[iAddress-2])=char(*((double*)&memory[iAddress-2]));
			else
			{
				*((char*)&memory[iAddress-3])=char(*((double*)&memory[iAddress-3]));
				*((char*)&memory[iAddress-2])=*((char*)&memory[iAddress-1]);
			}
			iAddress--;
			break;
		case OP_D2I:				//����ת�� double => int	��sd.iNumberΪ��ַƫ������
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-2]=int(*((double*)&memory[iAddress-2]));
			else
			{
				memory[iAddress-3]=int(*((double*)&memory[iAddress-3]));
				memory[iAddress-2]=memory[iAddress-1];
			}
			iAddress--;
			break;
		case OP_DELARRAYC:			//ɾ��һ���ַ����飨sd.iNumberΪ�����ַ��
			delete (char*)(memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_DELARRAYD:			//ɾ��һ��ʵ�����飨sd.iNumberΪ�����ַ��
			delete (double*)(memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_DELARRAYI:			//ɾ��һ���������飨sd.iNumberΪ�����ַ��
			delete (int*)(memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_DELETEC:			//ɾ��ջ���ַ�
			iAddress--;
			iCode++;
			break;
		case OP_DELETED:			//ɾ��ջ��ʵ��
			iAddress-=2;
			iCode++;
			break;
		case OP_DELETEI:			//ɾ��ջ������
			iAddress--;
			iCode++;
			break;
		case OP_EQUALC:				//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((char*)&memory[iAddress-2]) == *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_EQUALD:				//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((double*)&memory[iAddress-4]) == *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_EQUALI:				//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if(memory[iAddress-2]==memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_EXIT:				//�������
			goto EndInterpret;
		case OP_FUNCTIONSTART:		//������ʼ��־��sd.iNumberΪ�����������������ڴ棩
			iAddress+=pCode[iCode++].sd.iNumber;
			break;
		case OP_GREATC:				//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((char*)&memory[iAddress-2]) > *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_GREATD:				//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((double*)&memory[iAddress-4]) > *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_GREATEQUALC:		//���ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((char*)&memory[iAddress-2]) >= *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_GREATEQUALD:		//���ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((double*)&memory[iAddress-4]) >= *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_GREATEQUALI:		//���ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if(memory[iAddress-2]>=memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_GREATI:				//���ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if(memory[iAddress-2]>memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_I2C:				//����ת�� int => char	��sd.iNumberΪ��ַƫ������
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-1]= int(*((char*)&memory[iAddress-1]));
			else
				memory[iAddress-2]= int(*((char*)&memory[iAddress-2]));
			break;
		case OP_I2D:				//����ת�� int => double��sd.iNumberΪ��ַƫ������
			if(!TestAddress(iAddress))
				return FALSE;
			if(pCode[iCode++].sd.iNumber==0)
				*((double*)&memory[iAddress-1])=double(memory[iAddress-1]);
			else
			{
				*((double*)&memory[iAddress-1])=*((double*)&memory[iAddress-2]);
				*((double*)&memory[iAddress-3])=double(memory[iAddress-3]);
			}
			iAddress++;
			break;
		case OP_INPUTARRAYC:		//�����ַ�����ֵ��sd.iNumberΪ�����ַ���±���ջ����
			iTemp =memory[--iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			tElapse+=clock()-tStart;
			if(!InputChar(((char*)memory[iTemp2])[iTemp]))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTARRAYD:		//����ʵ������ֵ��sd.iNumberΪ�����ַ���±���ջ����
			iTemp =memory[--iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			tElapse+=clock()-tStart;
			if(!InputDouble(((double*)memory[iTemp2])[iTemp]))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTARRAYI:		//������������ֵ��sd.iNumberΪ�����ַ���±���ջ����
			iTemp =memory[--iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			tElapse+=clock()-tStart;
			if(!InputInt(((int*)memory[iTemp2])[iTemp]))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTC:				//�����ַ���sd.iNumberΪ��ַ��
			tElapse+=clock()-tStart;
			if(!InputChar(*((char*)&memory[iBase+pCode[iCode++].sd.iNumber])))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTCA:			//�����ַ����飨sd.iNumberΪ�����ַ��
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iTemp+1];//���鳤��
			tElapse+=clock()-tStart;
			if(!InputString(sTemp))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			if(sTemp.GetLength()>iTemp2)
				strcpy( (char*)memory[iTemp],sTemp.Left(iTemp2) );
			else
				strcpy( (char*)memory[iTemp],sTemp);
			break;
		case OP_INPUTD:				//����ʵ����sd.iNumberΪ��ַ��
			tElapse+=clock()-tStart;
			if(!InputDouble(*((double*)&memory[iBase+pCode[iCode++].sd.iNumber])))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTI:				//����������sd.iNumberΪ��ַ��
			tElapse+=clock()-tStart;
			if(!InputInt(memory[iBase+pCode[iCode++].sd.iNumber]))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_JUMP:				//��������ת��sdΪ��ת��ַ��
			iCode=pCode[iCode].sd.iNumber;
			break;
		case OP_LESSC:				//С�ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((char*)&memory[iAddress-2]) < *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LESSD:				//С�ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((double*)&memory[iAddress-4]) < *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_LESSEQUALC:			//С�ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((char*)&memory[iAddress-2]) <= *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LESSEQUALD:			//С�ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((double*)&memory[iAddress-4]) <= *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_LESSEQUALI:			//С�ڵ��ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if(memory[iAddress-2]<=memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LESSI:				//С�ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if(memory[iAddress-2]<memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LOADARRAY:			//���������ַ��sd.iNumberΪ����ͷ��ַ��
			memory[iAddress++]=memory[iBase+pCode[iCode].sd.iNumber];
			memory[iAddress++]=memory[iBase+pCode[iCode++].sd.iNumber+1];
			break;
		case OP_LOADARRAYC:			//�����ַ�����ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
			iTemp =memory[iAddress-1];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			*((char*)&memory[iAddress-1])=((char*)memory[iTemp2])[iTemp];
			break;
		case OP_LOADARRAYD:			//����ʵ������ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =memory[iAddress-1];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			*((double*)&memory[iAddress-1])=
				((double*)memory[iTemp2])[iTemp];
			iAddress++;
			break;
		case OP_LOADARRAYI:			//������������ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
			iTemp =memory[iAddress-1];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			memory[iAddress-1]=
				((int*)memory[iTemp2])[iTemp];
			break;
		case OP_LOADCONSTC:			//�����ַ�����	��sdΪ������
			if(!TestAddress(iAddress))
				return FALSE;
			*((char*)&memory[iAddress++])=pCode[iCode++].sd.cNumber;
			break;
		case OP_LOADCONSTD:			//����ʵ����	��sdΪ������
			if(!TestAddress(iAddress+1))
				return FALSE;
			*((double*)&memory[iAddress])=pCode[iCode++].sd.dNumber;
			iAddress+=2;
			break;
		case OP_LOADCONSTI:			//����������	��sdΪ������
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++]=pCode[iCode++].sd.iNumber;
			break;
		case OP_LOADVARC:			//�����ַ���sd.iNumberΪ��ַ��
			if(!TestAddress(iAddress))
				return FALSE;
			*((char*)&memory[iAddress++])=*((char*)&memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_LOADVARD:			//����ʵ����sd.iNumberΪ��ַ��
			if(!TestAddress(iAddress+1))
				return FALSE;
			*((double*)&memory[iAddress])=
				*((double*)&memory[iBase+pCode[iCode++].sd.iNumber]);
			iAddress+=2;
			break;
		case OP_LOADVARI:			//����������sd.iNumberΪ��ַ��
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++]=memory[iBase+pCode[iCode++].sd.iNumber];
			break;
		case OP_MINUSC:				//�����ַ���
			iAddress--;
			*((char*)&memory[iAddress-1]) -= *((char*)&memory[iAddress]);
			iCode++;
			break;
		case OP_MINUSD:				//����ʵ����
			iAddress-=2;
			*((double*)&memory[iAddress-2]) -= *((double*)&memory[iAddress]);
			iCode++;
			break;
		case OP_MINUSI:				//����������
			iAddress--;
			memory[iAddress-1]-=memory[iAddress];
			iCode++;
			break;
		case OP_MODC:				//�ࣨ�ַ���
			cTemp= *((char*)&memory[--iAddress]);
			if(!SlashModZero(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1]) %= cTemp;
			iCode++;
			break;
		case OP_MODI:				//�ࣨ������
			iTemp=memory[--iAddress];
			if(!SlashModZero(iTemp))
				return FALSE;
			memory[iAddress-1]%=iTemp;
			iCode++;
			break;
		case OP_NEWARRAYC:			//����һ���ַ����飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
			iTemp =iBase+pCode[iCode].sd.iNumber;//�����ַ
			iTemp2=pCode[iCode++].sd.iNumber2;//���鳤��
			memory[iTemp]=int(new char[iTemp2+1]);//�ַ����鳤�����һ
			if(!ValidArray((char*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<=iTemp2;i++)
				((char*)memory[iTemp])[i]='\0';
			break;
		case OP_NEWARRAYD:			//����һ��ʵ�����飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
			iTemp =iBase+pCode[iCode].sd.iNumber;//�����ַ
			iTemp2=pCode[iCode++].sd.iNumber2;//���鳤��
			memory[iTemp]=int(new double[iTemp2]);
			if(!ValidArray((double*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<iTemp2;i++)
				((double*)memory[iTemp])[i]=0.0;
			break;
		case OP_NEWARRAYI:			//����һ���������飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
			iTemp =iBase+pCode[iCode].sd.iNumber;//�����ַ
			iTemp2=pCode[iCode++].sd.iNumber2;//���鳤��
			memory[iTemp]=int(new int[iTemp2]);
			if(!ValidArray((int*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<iTemp2;i++)
				((int*)memory[iTemp])[i]=0;
			break;
		case OP_NOTEQUALC:			//�����ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((char*)&memory[iAddress-2]) != *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_NOTEQUALD:			//�����ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if( *((double*)&memory[iAddress-4]) != *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_NOTEQUALI:			//�����ڣ�sd.iNumberΪ��������ʱ����ת��ַ��
			if(memory[iAddress-2]!=memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_NULL:				//�ղ���
			iCode++;
			break;
		case OP_OUTPUTC:			//����ַ�
			Output( *((char*)&memory[--iAddress]));
			iCode++;
			break;
		case OP_OUTPUTCA:			//����ַ����飨sdΪ�ַ������ַ��
			Output( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		case OP_OUTPUTD:			//���ʵ��
			iAddress-=2;
			Output(*((double*)&memory[iAddress]));
			iCode++;
			break;
		case OP_OUTPUTI:			//�������
			Output(memory[--iAddress]);
			iCode++;
			break;
		case OP_OUTPUTS:			//����ַ�����sdΪ�ַ���ʵ��ַ��
			Output(pCode[iCode].sd.string);
			iCode++;
			break;
		case OP_PARAMTRANSA:		//����������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
			iTemp=iBase+pCode[iCode].sd.iNumber;
			memory[iTemp]=memory[iTemp-pCode[iCode].sd.iNumber2];
			memory[iTemp+1]=memory[iTemp+1-pCode[iCode++].sd.iNumber2];
			break;
		case OP_PARAMTRANSC:		//�ַ��������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
			iTemp=iBase+pCode[iCode].sd.iNumber;
			*((char*)&memory[iTemp])=
				*((char*)&memory[iTemp-pCode[iCode++].sd.iNumber2]);
			break;
		case OP_PARAMTRANSD:		//ʵ���������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
			iTemp=iBase+pCode[iCode].sd.iNumber;
			*((double*)&memory[iTemp])=
				*((double*)&memory[iTemp-pCode[iCode++].sd.iNumber2]);
			break;
		case OP_PARAMTRANSI:		//�����������ݣ�sd.iNumberΪ��ַ��sd.iNumber2Ϊƫ������
			iTemp=iBase+pCode[iCode].sd.iNumber;
			memory[iTemp]=memory[iTemp-pCode[iCode++].sd.iNumber2];
			break;
		case OP_PLUSC:				//�ӣ��ַ���
			iAddress--;
			*((char*)&memory[iAddress-1]) += *((char*)&memory[iAddress]);
			iCode++;
			break;
		case OP_PLUSD:				//�ӣ�ʵ����
			iAddress-=2;
			*((double*)&memory[iAddress-2]) += *((double*)&memory[iAddress]);
			iCode++;
			break;
		case OP_PLUSI:				//�ӣ�������
			iAddress--;
			memory[iAddress-1]+=memory[iAddress];
			iCode++;
			break;
		case OP_RETURN:				//void	�������أ�sd.iNumberΪƫ������
			iAddress=memory[iBase-2]	//�ָ��м����ָ��
				+3-pCode[iCode].sd.iNumber;//���ղ�������ʱ����ռ�õ����ڴ�
			iCode	=memory[iBase-1]+1;	//�ָ�ջ��ָ��
			iBase	=memory[iBase-3];	//�ָ�ԭ����ַ
			if(iBase<0)
				goto EndInterpret;
			break;
		case OP_RETURNC:			//char	�������أ�sd.iNumberΪƫ������
			cTemp= *((char*)&memory[iAddress-1]);
			iAddress=memory[iBase-2]	//�ָ��м����ָ��
				+3-pCode[iCode].sd.iNumber;//���ղ�������ʱ����ռ�õ����ڴ�
			iCode	=memory[iBase-1]+1;	//�ָ�ջ��ָ��
			iBase	=memory[iBase-3];	//�ָ�ԭ����ַ
			*((char*)&memory[iAddress++])=cTemp;
			break;
		case OP_RETURND:			//double�������أ�sd.iNumberΪƫ������
			dTemp=*((double*)&memory[iAddress-2]);
			iAddress=memory[iBase-2]	//�ָ��м����ָ��
				+3-pCode[iCode].sd.iNumber;//���ղ�������ʱ����ռ�õ����ڴ�
			iCode	=memory[iBase-1]+1;	//�ָ�ջ��ָ��
			iBase	=memory[iBase-3];	//�ָ�ԭ����ַ
			*((double*)&memory[iAddress])=dTemp;
			iAddress+=2;
			break;
		case OP_RETURNI:			//int	�������أ�sd.iNumberΪƫ������
			iTemp=memory[iAddress-1];
			iAddress=memory[iBase-2]	//�ָ��м����ָ��
				+3-pCode[iCode].sd.iNumber;//���ղ�������ʱ����ռ�õ����ڴ�
			iCode	=memory[iBase-1]+1;	//�ָ�ջ��ָ��
			iBase	=memory[iBase-3];	//�ָ�ԭ����ַ
			memory[iAddress++]=iTemp;
			break;
		case OP_SAVEARRAYC:			//�洢�ַ�����ֵ��sd.iNumberΪ�����ַ����ֵ��ջ�����±���ջ�ζ���
			iAddress-=2;
			iTemp =memory[iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			((char*)memory[iTemp2])[iTemp]= *((char*)&memory[iAddress+1]);
			break;
		case OP_SAVEARRAYD:			//�洢ʵ������ֵ��sd.iNumberΪ�����ַ����ֵ��ջ�����±���ջ�ζ���
			iAddress-=3;
			iTemp =memory[iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			((double*)memory[iTemp2])[iTemp]= *((double*)&memory[iAddress+1]);
			break;
		case OP_SAVEARRAYI:			//�洢��������ֵ��sd.iNumberΪ�����ַ����ֵ��ջ�����±���ջ�ζ���
			iAddress-=2;
			iTemp =memory[iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			((int*)memory[iTemp2])[iTemp]=memory[iAddress+1];
			break;
		case OP_SAVEC:				//�����ַ�������sdΪ������ַ��
			*((char*)&memory[iBase+pCode[iCode++].sd.iNumber])=
				*((char*)&memory[--iAddress]);
			break;
		case OP_SAVED:				//����ʵ��������sdΪ������ַ��
			iAddress-=2;
			*((double*)&memory[iBase+pCode[iCode++].sd.iNumber])=
				*((double*)&memory[iAddress]);
			break;
		case OP_SAVEI:				//��������������sdΪ������ַ��
			memory[iBase+pCode[iCode++].sd.iNumber]=memory[--iAddress];
			break;
		case OP_SLASHC:				//�����ַ���
			cTemp= *((char*)&memory[--iAddress]);
			if(!SlashModZero(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1])/=cTemp;
			iCode++;
			break;
		case OP_SLASHD:				//����ʵ����
			iAddress-=2;
			dTemp=*((double*)&memory[iAddress]);
			if(!SlashModZero(dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) /= dTemp;
			iCode++;
			break;
		case OP_SLASHI:				//����������
			iTemp=memory[--iAddress];
			if(!SlashModZero(iTemp))
				return FALSE;
			memory[iAddress-1]/=iTemp;
			iCode++;
			break;
		case OP_TIMESC:				//�ˣ��ַ���
			iAddress--;
			*((char*)&memory[iAddress-1]) *= *((char*)&memory[iAddress]);
			iCode++;
			break;
		case OP_TIMESD:				//�ˣ�ʵ����
			iAddress-=2;
			*((double*)&memory[iAddress-2]) *= *((double*)&memory[iAddress]);
			iCode++;
			break;
		case OP_TIMESI:				//�ˣ�������
			iAddress--;
			memory[iAddress-1]*=memory[iAddress];
			iCode++;
			break;
//***************************************************************************//
		// math
		case OP_ABSC:
			if( *((char*)&memory[iAddress-1]) < 0)
				*((char*)&memory[iAddress-1]) = -(*((char*)&memory[iAddress-1]));
			iCode++;
			break;
		case OP_ABSD:
			if( *((double*)&memory[iAddress-2]) < 0)
				*((double*)&memory[iAddress-2]) = -(*((double*)&memory[iAddress-2]));
			iCode++;
			break;
		case OP_ABSI:
			if( memory[iAddress-1] < 0)
				memory[iAddress-1] = -memory[iAddress-1];
			iCode++;
			break;
		case OP_ARCCOS:
			dTemp= *((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_ARCCOS,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) = acos(dTemp);
			iCode++;
			break;
		case OP_ARCCTG:
			*((double*)&memory[iAddress-2]) =
				PI/2.0-atan( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_ARCSIN:
			dTemp= *((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_ARCSIN,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) = acos(dTemp);
			iCode++;
			break;
		case OP_ARCTG:
			*((double*)&memory[iAddress-2]) =
				atan( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_CH:
			*((double*)&memory[iAddress-2]) =
				cosh( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_COS:
			*((double*)&memory[iAddress-2]) =
				cos( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_CTG:
			*((double*)&memory[iAddress-2]) =
				tan( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_CTH:
			dTemp=*((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_CTH,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) =
				1.0/tanh( dTemp );
			iCode++;
			break;
		case OP_EXP:
			*((double*)&memory[iAddress-2]) =
				exp( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_LG:
			dTemp=*((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_LG,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) =
				log10( dTemp );
			iCode++;
			break;
		case OP_LN:
			dTemp=*((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_LN,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) =
				log( dTemp );
			iCode++;
			break;
		case OP_POW:
			iAddress-=2;
			dTemp=*((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_POW,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) =
				pow( dTemp , *((double*)&memory[iAddress]) );
			iCode++;
			break;
		case OP_SH:
			*((double*)&memory[iAddress-2]) =
				sinh( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_SIN:
			*((double*)&memory[iAddress-2]) =
				sin( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_SQRT:
			dTemp=*((double*)&memory[iAddress-2]);
			if(!ExamCircumscriptive(OP_SQRT,dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) =
				sqrt( dTemp );
			iCode++;
			break;
		case OP_TG:
			*((double*)&memory[iAddress-2]) =
				tan( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		case OP_TH:
			*((double*)&memory[iAddress-2]) =
				tanh( *((double*)&memory[iAddress-2]) );
			iCode++;
			break;
		//string
		case OP_STRCMP:				//int strcmp([]char,[]char)
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++] =
				strcmp( ((char*)memory[iBase+pCode[iCode].sd.iNumber ]),
						((char*)memory[iBase+pCode[iCode].sd.iNumber2]) );
			iCode++;
			break;
		case OP_STRCPYCA:			//void strcpy([]char,[]char)
			sTemp.Format("%s",((char*)memory[iBase+pCode[iCode].sd.iNumber2]));//ԭ�ַ�����
			iTemp =iBase+pCode[iCode++].sd.iNumber;//Ŀ���ַ������ַ
			iTemp2=memory[iTemp+1];//Ŀ���ַ����鳤
			if(sTemp.GetLength()>iTemp2)//ԭ�ַ����鳤�ȴ���Ŀ���ַ�����
				sTemp=sTemp.Left(iTemp2);
			strcpy( ((char*)memory[iTemp]),sTemp );
			break;
		case OP_STRCPYS:			//�ַ������ʼ����sd.iNumberΪ�����ַ��sd.string2Ϊ�ַ���ʵ��ַ��
			if(pCode[iCode].sd.string2==NULL)
				strcpy((char*)(memory[iBase+pCode[iCode].sd.iNumber]),"");
			else
			{
				sTemp.Format("%s",pCode[iCode].sd.string2);//ԭ�ַ�����
				iTemp =iBase+pCode[iCode].sd.iNumber;//Ŀ���ַ������ַ
				iTemp2=memory[iTemp+1];//Ŀ���ַ����鳤
				if( (int)strlen(pCode[iCode].sd.string2)>iTemp2 )
					sTemp=sTemp.Left(iTemp2);
				strcpy((char*)(memory[iTemp]),sTemp);
			}
			iCode++;
			break;
		case OP_STRLENCA:			//int strlen([]char)
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++] =
				strlen( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		case OP_STRLENS:			//int strlen("")
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++] =
				strlen( pCode[iCode++].sd.string );
			break;
		case OP_STRLWR:				//void strlwr([]char)
			strlwr( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		case OP_STRUPR:				//void strupr([]char)
			strupr( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		//array
		case OP_AVERAGEC:			//double average([]char,int)
			dTemp=0;
			iTemp=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			for(i=iTemp2-1;i>=0;i--)
				dTemp+= double( ((char*)memory[iTemp])[i] );
			dTemp/=memory[iAddress-1];
			*((double*)&memory[iAddress-1]) = dTemp;
			iAddress++;
			break;
		case OP_AVERAGED:			//double average([]double,int)
			if(!TestAddress(iAddress))
				return FALSE;
			dTemp=0;
			iTemp=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			for(i=iTemp2-1;i>=0;i--)
				dTemp+= ((double*)memory[iTemp])[i];
			dTemp/=memory[iAddress-1];
			*((double*)&memory[iAddress-1]) = dTemp;
			iAddress++;
			break;
		case OP_AVERAGEI:			//double arerage([]int,int)
			dTemp=0;
			iTemp=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			for(i=iTemp2-1;i>=0;i--)
				dTemp+= double( ((int*)memory[iTemp])[i] );
			dTemp/=memory[iAddress-1];
			*((double*)&memory[iAddress-1]) = dTemp;
			iAddress++;
			break;
		case OP_DOWNSORTC:			//void downsort([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (char*)memory[iTemp],iTemp2,sizeof(char),DownCompareChar );
			break;
		case OP_DOWNSORTD:			//void downsort([]double,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (double*)memory[iTemp],iTemp2,sizeof(double),DownCompareDouble );
			break;
		case OP_DOWNSORTI:			//void downsort([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (int*)memory[iTemp],iTemp2,sizeof(int),DownCompareInt );
			break;
		case OP_MAXC:				//char max([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			cTemp= ((char*)memory[iTemp])[0];
			for(i=1;i<iTemp2;i++)
				if(cTemp<((char*)memory[iTemp])[i])
					cTemp=((char*)memory[iTemp])[i];
			*((char*)&memory[iAddress-1]) = cTemp;
			break;
		case OP_MAXD:				//double max([]double,int)
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			dTemp= ((double*)memory[iTemp])[0];
			for(i=1;i<iTemp2;i++)
				if(dTemp<((double*)memory[iTemp])[i])
					dTemp=((double*)memory[iTemp])[i];
			*((double*)&memory[iAddress-1]) = dTemp;
			iAddress++;
			break;
		case OP_MAXI:				//int max([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			iTemp2= ((int*)memory[iTemp])[0];
			for(i--;i>0;i--)
				if(iTemp2<((int*)memory[iTemp])[i])
					iTemp2=((int*)memory[iTemp])[i];
			*((int*)&memory[iAddress-1]) = iTemp2;
			break;
		case OP_MINC:				//char min([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			cTemp= ((char*)memory[iTemp])[0];
			for(i=1;i<iTemp2;i++)
				if(cTemp>((char*)memory[iTemp])[i])
					cTemp=((char*)memory[iTemp])[i];
			*((char*)&memory[iAddress-1]) = cTemp;
			break;
		case OP_MIND:				//double min([]double,int)
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[iAddress-1];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			dTemp= ((double*)memory[iTemp])[0];
			for(i=1;i<iTemp2;i++)
				if(dTemp>((double*)memory[iTemp])[i])
					dTemp=((double*)memory[iTemp])[i];
			*((double*)&memory[iAddress-1]) = dTemp;
			iAddress++;
			break;
		case OP_MINI:				//int min([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			iTemp2= ((int*)memory[iTemp])[0];
			for(i--;i>0;i--)
				if(iTemp2>((int*)memory[iTemp])[i])
					iTemp2=((int*)memory[iTemp])[i];
			*((int*)&memory[iAddress-1]) = iTemp2;
			break;
		case OP_SUMC:				//int sum([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			iTemp2=0;
			for(i--;i>=0;i--)
				iTemp2+= int(((char*)memory[iTemp])[i]);
			memory[iAddress-1]=iTemp2;
			break;
		case OP_SUMD:				//double sum([]double,int)
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			dTemp=0.0;
			for(i--;i>0;i--)
				dTemp+= ((double*)memory[iTemp])[i];
			*((double*)&memory[iAddress-1]) = dTemp;
			break;
		case OP_SUMI:				//int sum([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			iTemp2=0;
			for(i--;i>=0;i--)
				iTemp2+= ((int*)memory[iTemp])[i];
			memory[iAddress-1]=iTemp2;
			break;
		case OP_UPSORTC:			//void upsort([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (char*)memory[iTemp],iTemp2,sizeof(char),UpCompareChar );
			break;
		case OP_UPSORTD:			//void upsort([]double,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (double*)memory[iTemp],iTemp2,sizeof(double),DownCompareDouble );
			break;
		case OP_UPSORTI:			//void upsort([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//�����ַ
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (int*)memory[iTemp],iTemp2,sizeof(int),DownCompareInt );
			break;
		default:
			MessageBox("����ִ��ʱ�����˷Ƿ��Ĳ��������޷���������ִ�У���������ṩ����ϵ��",
				"FC",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
	}
EndInterpret:
	delete memory;
	EndInterpretOutput(tElapse+clock()-tStart);
	MessageBeep(0);
	return TRUE;
}

void CFCInterpretDlg::InitInterpretOutput()
{
	if(m_lpszOutput != NULL)
		free(m_lpszOutput);

	m_lpszOutput = (char *)malloc(INIT_OUTPUT_BUFFER_SIZE);
	m_nOutputLength = 0;
	m_nOutputCapacity = INIT_OUTPUT_BUFFER_SIZE;

	if(m_bFileOutput)
	{
		m_fout.open(m_strOutputFile,ios::out);
		if(m_fout)
		{
			m_fout.precision(m_nPrecision);
			m_fout.setf(ios::fixed);
		}
		else
		{
			MessageBox("�޷��򿪻򴴽���"+m_strOutputFile+"��������Ϊ�������Ļ��",
				"FC",MB_OK|MB_ICONINFORMATION);
			m_bFileOutput=FALSE;
		}
	}
}

void CFCInterpretDlg::EndInterpretOutput(int tElapse)
{
	if(m_bFileOutput)
		m_fout.close();
	if(m_bCalcRuntime)
	{
		CString string;
		string.Format("\n����ִ����ʱ %.3f ��\n",tElapse/1000.0);
		Output(string,TRUE);
	}
	UpdateOutput();
}

void CFCInterpretDlg::Output(char ch,BOOL bToScreen)
{
	if(ch=='\0')
		ch = ' ';

	if(bToScreen || !m_bFileOutput)//to screen
	{
		OutputToMemory( TransStringNewline(ch) );
	}
	else
	{
		m_fout<<ch;
	}
}

void CFCInterpretDlg::Output(int i,BOOL bToScreen)
{
	if(bToScreen || !m_bFileOutput)//to screen
	{
		CString temp;
		temp.Format("%d",i);
		OutputToMemory( temp );
	}
	else
	{
		m_fout<<i;
	}
}

void CFCInterpretDlg::Output(double d,BOOL bToScreen)
{
	if(bToScreen || !m_bFileOutput)//to screen
	{
		CString temp;
		temp.Format("%%.%df",m_nPrecision);
		temp.Format(temp,d);
		OutputToMemory( temp );
	}
	else
	{
		m_fout<<d;
	}
}

void CFCInterpretDlg::Output(CString string,BOOL bToScreen)
{
	if(bToScreen || !m_bFileOutput)//to screen
	{
		OutputToMemory( TransStringNewline(string) );
	}
	else
	{
		m_fout<<string;
	}
}

void CFCInterpretDlg::Output(char *string,BOOL bToScreen)
{
	if(bToScreen || !m_bFileOutput)//to screen
	{
		OutputToMemory( TransStringNewline(CString(string)) );
	}
	else
	{
		m_fout<<string;
	}
}

void CFCInterpretDlg::OutputToMemory(CString string)
{
	int increment = string.GetLength();
	BOOL bUpdateData = FALSE;

	if(m_nOutputLength+increment >= m_nOutputCapacity)
	{
		m_nOutputCapacity = max(4*m_nOutputCapacity, m_nOutputLength+increment);
		m_nOutputCapacity = max(m_nOutputCapacity, INIT_OUTPUT_BUFFER_SIZE);

		m_lpszOutput = (char *)realloc((void *)m_lpszOutput, m_nOutputCapacity);

		bUpdateData = TRUE;
	}
	strcpy(m_lpszOutput+m_nOutputLength, string.GetBuffer(string.GetLength()));
	m_nOutputLength += increment;

	if(bUpdateData)
		UpdateOutput();
}

void CFCInterpretDlg::UpdateOutput() 
{
	HWND hWndThis = GetSafeHwnd();

	::SetDlgItemText(
		hWndThis,
		IDC_OUTPUT,
		m_lpszOutput);
	::SendDlgItemMessage(	// �ѹ��ŵ����������
		hWndThis,
		IDC_OUTPUT,
		EM_SETSEL,
		m_nOutputLength,
		m_nOutputLength);
	::SendDlgItemMessage( // ���������������
		hWndThis,
		IDC_OUTPUT,
		EM_SCROLLCARET,
		0,
		0);
}

BOOL CFCInterpretDlg::InputChar(char &c)//ֻ����ɼ��ַ�
{
	if(!m_bFileOutput)
		UpdateOutput();
	CFCInputDlg dlg;
	if(IDOK!=dlg.InputChar(c))
		return FALSE;
	if(m_bOutputInput)
	{
		Output(c);
		Output("\n");
	}
	return TRUE;
}

BOOL CFCInterpretDlg::InputInt(int &i)
{
	if(!m_bFileOutput)
		UpdateOutput();
	CFCInputDlg dlg;
	if(IDOK!=dlg.InputInt(i))
		return FALSE;
	if(m_bOutputInput)
	{
		Output(i);
		Output("\n");
	}
	return TRUE;
}

BOOL CFCInterpretDlg::InputDouble(double &d)
{
	if(!m_bFileOutput)
		UpdateOutput();
	CFCInputDlg dlg;
	if(IDOK!=dlg.InputDouble(d))
		return FALSE;
	if(m_bOutputInput)
	{
		Output(d);
		Output("\n");
	}
	return TRUE;
}

BOOL CFCInterpretDlg::InputString(CString &string)
{
	if(!m_bFileOutput)
		UpdateOutput();
	CFCInputDlg dlg;
	if(IDOK!=dlg.InputString(string))
		return FALSE;
	if(m_bOutputInput)
	{
		OutputToMemory( string );
	}
	return TRUE;
}
