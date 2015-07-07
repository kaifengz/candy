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
		MessageBox("内存溢出，中止解释执行！","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::SlashModZero(char ch)
{
	if(ch=='\0')
	{
		MessageBox("“/”或“%”的右操作数为0，中止解释执行！",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::SlashModZero(int i)
{
	if(i==0)
	{
		MessageBox("“/”或“%”的右操作数为0，中止解释执行！",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::SlashModZero(double d)
{
	if(d==0.0)
	{
		MessageBox("“/”或“%”的右操作数为0，中止解释执行！",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::ArraySubscript(int index,int length)
{
	if(index<0 || index>=length)
	{
		MessageBox("数组下标溢出，中止解释执行！","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::ValidArray(void *pArray)
{
	if(pArray==NULL)
	{
		MessageBox("无法分配数组空间，中止解释执行！","FC",MB_OK|MB_ICONINFORMATION);
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
		msg+=" 函数调用时变量超出定义域，中止解释执行！";
		MessageBox(msg,"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL CFCInterpretDlg::Interpret()
{
	const double m_nCode=0;//定义此变量只是为了屏蔽外部成员变量

	const ZCode *pCode=m_pCode;

	char cTemp;
	int iTemp,iTemp2;
	double dTemp;
	CString sTemp;

	int i;

	register int iBase=-1;		//函数基地址，指向函数第一个参数地址
	register int iCode=0;		//代码指针，指向当前代码
	register int iAddress=0;	//地址指针，指向第一个未用的地址单元
	int *memory=new int[m_nMemoryApply*256];	//虚内存

	register int iOp;			//指令条数计数
	BOOL bWarning=!m_bIgnoreCircleNoend;		//是否对可能的死循环进行警告

	clock_t tStart,tElapse=0;
	tStart=clock();

	InitInterpretOutput();

	for(iOp=0;;iOp++)
	{
		if(iOp==173330 && bWarning)//17333只是表示173班33号，无其他意义
		{
			UpdateOutput();
			tElapse+=clock()-tStart;
			switch(MessageBox("\
程序可能进入死循环，要中止解释执行吗？\n\
按“是”中止解释执行，按“否”继续解释执行\n\
按“取消”继续解释执行并不再显示此消息框",
				"FC 2.2",MB_YESNOCANCEL|MB_ICONQUESTION))
			{
			case IDYES:
				tStart=clock();
				goto EndInterpret;
				//此处无需break语句
			case IDCANCEL:
				bWarning=FALSE;
				//此处不填入break语句
			case IDNO:
				tStart=clock();
				iOp=0;
				break;
			}
		}
		switch(pCode[iCode].op)
		{
		case OP_C2D:				//类型转换 char => double	（sd.iNumber为地址偏移量）
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
		case OP_C2I:				//类型转换 char => int		（sd.iNumber为地址偏移量）
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-1]=*((char*)&memory[iAddress-1]);
			else
				memory[iAddress-2]=*((char*)&memory[iAddress-1]);
			break;
		case OP_CALL:				//调用函数（sd为函数地址）
			if(!TestAddress(iAddress+3))// +3 是因为下边需要3单元虚内存
				return FALSE;
			memory[iAddress  ]=iBase;	//保留原基地址
			memory[iAddress+1]=iAddress;//保留原栈顶指针
			memory[iAddress+2]=iCode;	//保留原中间代码指针
			iBase=iAddress+3;
			iCode=pCode[iCode].sd.iNumber;
			iAddress+=3;
			break;
		case OP_CHANGESIGNALC:		//改变字符符号
			*((char*)(&memory[iAddress-1]))= - *((char*)(&memory[iAddress-1]));
			iCode++;
			break;
		case OP_CHANGESIGNALD:		//改变实数符号
			*((double*)(&memory[iAddress-2]))= - *((double*)(&memory[iAddress-2]));
			iCode++;
			break;
		case OP_CHANGESIGNALI:		//改变整数符号
			memory[iAddress-1]= -memory[iAddress-1];
			iCode++;
			break;
		case OP_COPYI:				//复制栈顶数值（整数）
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress]=memory[iAddress-1];
			iAddress++;
			iCode++;
			break;
		case OP_D2C:				//类型转换 double => char	（sd.iNumber为地址偏移量）
			if(pCode[iCode++].sd.iNumber==0)
				*((char*)&memory[iAddress-2])=char(*((double*)&memory[iAddress-2]));
			else
			{
				*((char*)&memory[iAddress-3])=char(*((double*)&memory[iAddress-3]));
				*((char*)&memory[iAddress-2])=*((char*)&memory[iAddress-1]);
			}
			iAddress--;
			break;
		case OP_D2I:				//类型转换 double => int	（sd.iNumber为地址偏移量）
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-2]=int(*((double*)&memory[iAddress-2]));
			else
			{
				memory[iAddress-3]=int(*((double*)&memory[iAddress-3]));
				memory[iAddress-2]=memory[iAddress-1];
			}
			iAddress--;
			break;
		case OP_DELARRAYC:			//删除一个字符数组（sd.iNumber为数组地址）
			delete (char*)(memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_DELARRAYD:			//删除一个实数数组（sd.iNumber为数组地址）
			delete (double*)(memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_DELARRAYI:			//删除一个整数数组（sd.iNumber为数组地址）
			delete (int*)(memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_DELETEC:			//删除栈顶字符
			iAddress--;
			iCode++;
			break;
		case OP_DELETED:			//删除栈顶实数
			iAddress-=2;
			iCode++;
			break;
		case OP_DELETEI:			//删除栈顶整数
			iAddress--;
			iCode++;
			break;
		case OP_EQUALC:				//等于（sd.iNumber为条件满足时的跳转地址）
			if( *((char*)&memory[iAddress-2]) == *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_EQUALD:				//等于（sd.iNumber为条件满足时的跳转地址）
			if( *((double*)&memory[iAddress-4]) == *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_EQUALI:				//等于（sd.iNumber为条件满足时的跳转地址）
			if(memory[iAddress-2]==memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_EXIT:				//程序结束
			goto EndInterpret;
		case OP_FUNCTIONSTART:		//函数开始标志（sd.iNumber为变量参数所需的最大内存）
			iAddress+=pCode[iCode++].sd.iNumber;
			break;
		case OP_GREATC:				//大于（sd.iNumber为条件满足时的跳转地址）
			if( *((char*)&memory[iAddress-2]) > *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_GREATD:				//大于（sd.iNumber为条件满足时的跳转地址）
			if( *((double*)&memory[iAddress-4]) > *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_GREATEQUALC:		//大于等于（sd.iNumber为条件满足时的跳转地址）
			if( *((char*)&memory[iAddress-2]) >= *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_GREATEQUALD:		//大于等于（sd.iNumber为条件满足时的跳转地址）
			if( *((double*)&memory[iAddress-4]) >= *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_GREATEQUALI:		//大于等于（sd.iNumber为条件满足时的跳转地址）
			if(memory[iAddress-2]>=memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_GREATI:				//大于（sd.iNumber为条件满足时的跳转地址）
			if(memory[iAddress-2]>memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_I2C:				//类型转换 int => char	（sd.iNumber为地址偏移量）
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-1]= int(*((char*)&memory[iAddress-1]));
			else
				memory[iAddress-2]= int(*((char*)&memory[iAddress-2]));
			break;
		case OP_I2D:				//类型转换 int => double（sd.iNumber为地址偏移量）
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
		case OP_INPUTARRAYC:		//输入字符数组值（sd.iNumber为数组地址，下标在栈顶）
			iTemp =memory[--iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
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
		case OP_INPUTARRAYD:		//输入实型数组值（sd.iNumber为数组地址，下标在栈顶）
			iTemp =memory[--iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
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
		case OP_INPUTARRAYI:		//输入整型数组值（sd.iNumber为数组地址，下标在栈顶）
			iTemp =memory[--iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
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
		case OP_INPUTC:				//输入字符（sd.iNumber为地址）
			tElapse+=clock()-tStart;
			if(!InputChar(*((char*)&memory[iBase+pCode[iCode++].sd.iNumber])))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTCA:			//输入字符数组（sd.iNumber为数组地址）
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[iTemp+1];//数组长度
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
		case OP_INPUTD:				//输入实数（sd.iNumber为地址）
			tElapse+=clock()-tStart;
			if(!InputDouble(*((double*)&memory[iBase+pCode[iCode++].sd.iNumber])))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTI:				//输入整数（sd.iNumber为地址）
			tElapse+=clock()-tStart;
			if(!InputInt(memory[iBase+pCode[iCode++].sd.iNumber]))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_JUMP:				//无条件跳转（sd为跳转地址）
			iCode=pCode[iCode].sd.iNumber;
			break;
		case OP_LESSC:				//小于（sd.iNumber为条件满足时的跳转地址）
			if( *((char*)&memory[iAddress-2]) < *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LESSD:				//小于（sd.iNumber为条件满足时的跳转地址）
			if( *((double*)&memory[iAddress-4]) < *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_LESSEQUALC:			//小于等于（sd.iNumber为条件满足时的跳转地址）
			if( *((char*)&memory[iAddress-2]) <= *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LESSEQUALD:			//小于等于（sd.iNumber为条件满足时的跳转地址）
			if( *((double*)&memory[iAddress-4]) <= *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_LESSEQUALI:			//小于等于（sd.iNumber为条件满足时的跳转地址）
			if(memory[iAddress-2]<=memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LESSI:				//小于（sd.iNumber为条件满足时的跳转地址）
			if(memory[iAddress-2]<memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_LOADARRAY:			//导入数组地址（sd.iNumber为数组头地址）
			memory[iAddress++]=memory[iBase+pCode[iCode].sd.iNumber];
			memory[iAddress++]=memory[iBase+pCode[iCode++].sd.iNumber+1];
			break;
		case OP_LOADARRAYC:			//导入字符数组值（sd.iNumber为数组头地址，下标在栈顶）
			iTemp =memory[iAddress-1];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			*((char*)&memory[iAddress-1])=((char*)memory[iTemp2])[iTemp];
			break;
		case OP_LOADARRAYD:			//导入实数数组值（sd.iNumber为数组头地址，下标在栈顶）
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =memory[iAddress-1];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			*((double*)&memory[iAddress-1])=
				((double*)memory[iTemp2])[iTemp];
			iAddress++;
			break;
		case OP_LOADARRAYI:			//导入整数数组值（sd.iNumber为数组头地址，下标在栈顶）
			iTemp =memory[iAddress-1];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			memory[iAddress-1]=
				((int*)memory[iTemp2])[iTemp];
			break;
		case OP_LOADCONSTC:			//导入字符常数	（sd为参数）
			if(!TestAddress(iAddress))
				return FALSE;
			*((char*)&memory[iAddress++])=pCode[iCode++].sd.cNumber;
			break;
		case OP_LOADCONSTD:			//导入实常数	（sd为参数）
			if(!TestAddress(iAddress+1))
				return FALSE;
			*((double*)&memory[iAddress])=pCode[iCode++].sd.dNumber;
			iAddress+=2;
			break;
		case OP_LOADCONSTI:			//导入整常数	（sd为参数）
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++]=pCode[iCode++].sd.iNumber;
			break;
		case OP_LOADVARC:			//导入字符（sd.iNumber为地址）
			if(!TestAddress(iAddress))
				return FALSE;
			*((char*)&memory[iAddress++])=*((char*)&memory[iBase+pCode[iCode++].sd.iNumber]);
			break;
		case OP_LOADVARD:			//导入实数（sd.iNumber为地址）
			if(!TestAddress(iAddress+1))
				return FALSE;
			*((double*)&memory[iAddress])=
				*((double*)&memory[iBase+pCode[iCode++].sd.iNumber]);
			iAddress+=2;
			break;
		case OP_LOADVARI:			//导入整数（sd.iNumber为地址）
			if(!TestAddress(iAddress))
				return FALSE;
			memory[iAddress++]=memory[iBase+pCode[iCode++].sd.iNumber];
			break;
		case OP_MINUSC:				//减（字符）
			iAddress--;
			*((char*)&memory[iAddress-1]) -= *((char*)&memory[iAddress]);
			iCode++;
			break;
		case OP_MINUSD:				//减（实数）
			iAddress-=2;
			*((double*)&memory[iAddress-2]) -= *((double*)&memory[iAddress]);
			iCode++;
			break;
		case OP_MINUSI:				//减（整数）
			iAddress--;
			memory[iAddress-1]-=memory[iAddress];
			iCode++;
			break;
		case OP_MODC:				//余（字符）
			cTemp= *((char*)&memory[--iAddress]);
			if(!SlashModZero(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1]) %= cTemp;
			iCode++;
			break;
		case OP_MODI:				//余（整数）
			iTemp=memory[--iAddress];
			if(!SlashModZero(iTemp))
				return FALSE;
			memory[iAddress-1]%=iTemp;
			iCode++;
			break;
		case OP_NEWARRAYC:			//申请一个字符数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
			iTemp =iBase+pCode[iCode].sd.iNumber;//数组地址
			iTemp2=pCode[iCode++].sd.iNumber2;//数组长度
			memory[iTemp]=int(new char[iTemp2+1]);//字符数组长度须加一
			if(!ValidArray((char*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<=iTemp2;i++)
				((char*)memory[iTemp])[i]='\0';
			break;
		case OP_NEWARRAYD:			//申请一个实数数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
			iTemp =iBase+pCode[iCode].sd.iNumber;//数组地址
			iTemp2=pCode[iCode++].sd.iNumber2;//数组长度
			memory[iTemp]=int(new double[iTemp2]);
			if(!ValidArray((double*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<iTemp2;i++)
				((double*)memory[iTemp])[i]=0.0;
			break;
		case OP_NEWARRAYI:			//申请一个整数数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
			iTemp =iBase+pCode[iCode].sd.iNumber;//数组地址
			iTemp2=pCode[iCode++].sd.iNumber2;//数组长度
			memory[iTemp]=int(new int[iTemp2]);
			if(!ValidArray((int*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<iTemp2;i++)
				((int*)memory[iTemp])[i]=0;
			break;
		case OP_NOTEQUALC:			//不等于（sd.iNumber为条件满足时的跳转地址）
			if( *((char*)&memory[iAddress-2]) != *((char*)&memory[iAddress-1]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_NOTEQUALD:			//不等于（sd.iNumber为条件满足时的跳转地址）
			if( *((double*)&memory[iAddress-4]) != *((double*)&memory[iAddress-2]) )
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=4;
			break;
		case OP_NOTEQUALI:			//不等于（sd.iNumber为条件满足时的跳转地址）
			if(memory[iAddress-2]!=memory[iAddress-1])
				iCode=pCode[iCode].sd.iNumber;
			else
				iCode++;
			iAddress-=2;
			break;
		case OP_NULL:				//空操作
			iCode++;
			break;
		case OP_OUTPUTC:			//输出字符
			Output( *((char*)&memory[--iAddress]));
			iCode++;
			break;
		case OP_OUTPUTCA:			//输出字符数组（sd为字符数组地址）
			Output( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		case OP_OUTPUTD:			//输出实数
			iAddress-=2;
			Output(*((double*)&memory[iAddress]));
			iCode++;
			break;
		case OP_OUTPUTI:			//输出整数
			Output(memory[--iAddress]);
			iCode++;
			break;
		case OP_OUTPUTS:			//输出字符串（sd为字符串实地址）
			Output(pCode[iCode].sd.string);
			iCode++;
			break;
		case OP_PARAMTRANSA:		//数组参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
			iTemp=iBase+pCode[iCode].sd.iNumber;
			memory[iTemp]=memory[iTemp-pCode[iCode].sd.iNumber2];
			memory[iTemp+1]=memory[iTemp+1-pCode[iCode++].sd.iNumber2];
			break;
		case OP_PARAMTRANSC:		//字符参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
			iTemp=iBase+pCode[iCode].sd.iNumber;
			*((char*)&memory[iTemp])=
				*((char*)&memory[iTemp-pCode[iCode++].sd.iNumber2]);
			break;
		case OP_PARAMTRANSD:		//实数参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
			iTemp=iBase+pCode[iCode].sd.iNumber;
			*((double*)&memory[iTemp])=
				*((double*)&memory[iTemp-pCode[iCode++].sd.iNumber2]);
			break;
		case OP_PARAMTRANSI:		//整数参数传递（sd.iNumber为地址，sd.iNumber2为偏移量）
			iTemp=iBase+pCode[iCode].sd.iNumber;
			memory[iTemp]=memory[iTemp-pCode[iCode++].sd.iNumber2];
			break;
		case OP_PLUSC:				//加（字符）
			iAddress--;
			*((char*)&memory[iAddress-1]) += *((char*)&memory[iAddress]);
			iCode++;
			break;
		case OP_PLUSD:				//加（实数）
			iAddress-=2;
			*((double*)&memory[iAddress-2]) += *((double*)&memory[iAddress]);
			iCode++;
			break;
		case OP_PLUSI:				//加（整数）
			iAddress--;
			memory[iAddress-1]+=memory[iAddress];
			iCode++;
			break;
		case OP_RETURN:				//void	函数返回（sd.iNumber为偏移量）
			iAddress=memory[iBase-2]	//恢复中间代码指针
				+3-pCode[iCode].sd.iNumber;//回收参数传递时参数占用的虚内存
			iCode	=memory[iBase-1]+1;	//恢复栈顶指针
			iBase	=memory[iBase-3];	//恢复原基地址
			if(iBase<0)
				goto EndInterpret;
			break;
		case OP_RETURNC:			//char	函数返回（sd.iNumber为偏移量）
			cTemp= *((char*)&memory[iAddress-1]);
			iAddress=memory[iBase-2]	//恢复中间代码指针
				+3-pCode[iCode].sd.iNumber;//回收参数传递时参数占用的虚内存
			iCode	=memory[iBase-1]+1;	//恢复栈顶指针
			iBase	=memory[iBase-3];	//恢复原基地址
			*((char*)&memory[iAddress++])=cTemp;
			break;
		case OP_RETURND:			//double函数返回（sd.iNumber为偏移量）
			dTemp=*((double*)&memory[iAddress-2]);
			iAddress=memory[iBase-2]	//恢复中间代码指针
				+3-pCode[iCode].sd.iNumber;//回收参数传递时参数占用的虚内存
			iCode	=memory[iBase-1]+1;	//恢复栈顶指针
			iBase	=memory[iBase-3];	//恢复原基地址
			*((double*)&memory[iAddress])=dTemp;
			iAddress+=2;
			break;
		case OP_RETURNI:			//int	函数返回（sd.iNumber为偏移量）
			iTemp=memory[iAddress-1];
			iAddress=memory[iBase-2]	//恢复中间代码指针
				+3-pCode[iCode].sd.iNumber;//回收参数传递时参数占用的虚内存
			iCode	=memory[iBase-1]+1;	//恢复栈顶指针
			iBase	=memory[iBase-3];	//恢复原基地址
			memory[iAddress++]=iTemp;
			break;
		case OP_SAVEARRAYC:			//存储字符数组值（sd.iNumber为数组地址，数值在栈顶，下标在栈次顶）
			iAddress-=2;
			iTemp =memory[iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			((char*)memory[iTemp2])[iTemp]= *((char*)&memory[iAddress+1]);
			break;
		case OP_SAVEARRAYD:			//存储实数数组值（sd.iNumber为数组地址，数值在栈顶，下标在栈次顶）
			iAddress-=3;
			iTemp =memory[iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			((double*)memory[iTemp2])[iTemp]= *((double*)&memory[iAddress+1]);
			break;
		case OP_SAVEARRAYI:			//存储整数数组值（sd.iNumber为数组地址，数值在栈顶，下标在栈次顶）
			iAddress-=2;
			iTemp =memory[iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			((int*)memory[iTemp2])[iTemp]=memory[iAddress+1];
			break;
		case OP_SAVEC:				//保存字符变量（sd为变量地址）
			*((char*)&memory[iBase+pCode[iCode++].sd.iNumber])=
				*((char*)&memory[--iAddress]);
			break;
		case OP_SAVED:				//保存实数变量（sd为变量地址）
			iAddress-=2;
			*((double*)&memory[iBase+pCode[iCode++].sd.iNumber])=
				*((double*)&memory[iAddress]);
			break;
		case OP_SAVEI:				//保存整数变量（sd为变量地址）
			memory[iBase+pCode[iCode++].sd.iNumber]=memory[--iAddress];
			break;
		case OP_SLASHC:				//除（字符）
			cTemp= *((char*)&memory[--iAddress]);
			if(!SlashModZero(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1])/=cTemp;
			iCode++;
			break;
		case OP_SLASHD:				//除（实数）
			iAddress-=2;
			dTemp=*((double*)&memory[iAddress]);
			if(!SlashModZero(dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) /= dTemp;
			iCode++;
			break;
		case OP_SLASHI:				//除（整数）
			iTemp=memory[--iAddress];
			if(!SlashModZero(iTemp))
				return FALSE;
			memory[iAddress-1]/=iTemp;
			iCode++;
			break;
		case OP_TIMESC:				//乘（字符）
			iAddress--;
			*((char*)&memory[iAddress-1]) *= *((char*)&memory[iAddress]);
			iCode++;
			break;
		case OP_TIMESD:				//乘（实数）
			iAddress-=2;
			*((double*)&memory[iAddress-2]) *= *((double*)&memory[iAddress]);
			iCode++;
			break;
		case OP_TIMESI:				//乘（整数）
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
			sTemp.Format("%s",((char*)memory[iBase+pCode[iCode].sd.iNumber2]));//原字符数组
			iTemp =iBase+pCode[iCode++].sd.iNumber;//目标字符数组地址
			iTemp2=memory[iTemp+1];//目标字符数组长
			if(sTemp.GetLength()>iTemp2)//原字符数组长度大于目标字符数组
				sTemp=sTemp.Left(iTemp2);
			strcpy( ((char*)memory[iTemp]),sTemp );
			break;
		case OP_STRCPYS:			//字符数组初始化（sd.iNumber为数组地址，sd.string2为字符串实地址）
			if(pCode[iCode].sd.string2==NULL)
				strcpy((char*)(memory[iBase+pCode[iCode].sd.iNumber]),"");
			else
			{
				sTemp.Format("%s",pCode[iCode].sd.string2);//原字符数组
				iTemp =iBase+pCode[iCode].sd.iNumber;//目标字符数组地址
				iTemp2=memory[iTemp+1];//目标字符数组长
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
			iTemp=iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp=iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp=iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (char*)memory[iTemp],iTemp2,sizeof(char),DownCompareChar );
			break;
		case OP_DOWNSORTD:			//void downsort([]double,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (double*)memory[iTemp],iTemp2,sizeof(double),DownCompareDouble );
			break;
		case OP_DOWNSORTI:			//void downsort([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (int*)memory[iTemp],iTemp2,sizeof(int),DownCompareInt );
			break;
		case OP_MAXC:				//char max([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
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
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			dTemp=0.0;
			for(i--;i>0;i--)
				dTemp+= ((double*)memory[iTemp])[i];
			*((double*)&memory[iAddress-1]) = dTemp;
			break;
		case OP_SUMI:				//int sum([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			i=memory[iAddress-1];
			if(i>memory[iTemp+1])
				i=memory[iTemp+1];
			iTemp2=0;
			for(i--;i>=0;i--)
				iTemp2+= ((int*)memory[iTemp])[i];
			memory[iAddress-1]=iTemp2;
			break;
		case OP_UPSORTC:			//void upsort([]char,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (char*)memory[iTemp],iTemp2,sizeof(char),UpCompareChar );
			break;
		case OP_UPSORTD:			//void upsort([]double,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (double*)memory[iTemp],iTemp2,sizeof(double),DownCompareDouble );
			break;
		case OP_UPSORTI:			//void upsort([]int,int)
			iTemp =iBase+pCode[iCode++].sd.iNumber;//数组地址
			iTemp2=memory[--iAddress];
			if(iTemp2>memory[iTemp+1])
				iTemp2=memory[iTemp+1];
			qsort( (int*)memory[iTemp],iTemp2,sizeof(int),DownCompareInt );
			break;
		default:
			MessageBox("解释执行时碰到了非法的操作符，无法继续解释执行，请与程序提供者联系！",
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
			MessageBox("无法打开或创建“"+m_strOutputFile+"”，将改为输出到屏幕！",
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
		string.Format("\n程序执行用时 %.3f 秒\n",tElapse/1000.0);
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
	::SendDlgItemMessage(	// 把光标放到输出的最后边
		hWndThis,
		IDC_OUTPUT,
		EM_SETSEL,
		m_nOutputLength,
		m_nOutputLength);
	::SendDlgItemMessage( // 滚动滚动条到最后
		hWndThis,
		IDC_OUTPUT,
		EM_SCROLLCARET,
		0,
		0);
}

BOOL CFCInterpretDlg::InputChar(char &c)//只输入可见字符
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
