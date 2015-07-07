
#include <windows.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "FC-define.h"

#define PI						3.14159265358979323846
#define MEMORY_SIZE				16		// 16KB
#define INIT_OUTPUT_BUFFER_SIZE	1024

///////////////////////////////////////////////////////////////////////////////
// 全局变量
///////////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;

HWND g_hWndFcr;

// 伪代码
ZCode *g_pCode;
int g_nCode;

// 输出精度
int g_nPrecision;

// 输出内容
LPSTR g_lpszOutput;
int g_nOutputCapacity;
int g_nOutputLength;

// 程序标题
LPSTR g_lpszTitle;

///////////////////////////////////////////////////////////////////////////////
// 函数预定义
///////////////////////////////////////////////////////////////////////////////

// 外部函数定义
extern BOOL CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CallInputDialog(HWND, ZIM, void *);

// 消息处理
void OnFcrExecute();
void OnFcrSaveOutput();
void OnFcrPrecision0();
void OnFcrPrecision6();
void OnFcrPrecision15();
void OnFcrAbout();
void OnFcrExit();
void InitFcrDialog();

// 读取伪代码
BOOL ReadCodesFromRes();
BOOL ReadCodes(LPCTSTR lpCodes);

// 解释执行时的数据有效性检查
BOOL TestAddress(int iAddress);
BOOL SlashModZeroChar(char ch);
BOOL SlashModZeroInt(int i);
BOOL SlashModZeroDouble(double d);
BOOL ArraySubscript(int index,int length);
BOOL ValidArray(void *pArray);
BOOL ExamCircumscriptive(ZOP op,double value);

// 解释执行
BOOL Interpret();

// 输出控制
void InitInterpretOutput();
void UpdateOutput();
void EndInterpretOutput(clock_t tElapse);

// 输入数据
BOOL InputChar(char *p);
BOOL InputInt(int *p);
BOOL InputDouble(double *p);
BOOL InputString(char **p);

// 输出数据
void OutputChar(char c);
void OutputInt(int i);
void OutputDouble(double d);
void OutputString(char *s);
void OutputCharArray(char *s);

// 排序比较
int DownCompareChar(const void *ch1,const void *ch2);
int DownCompareInt(const void *i1,const void *i2);
int DownCompareDouble(const void *d1,const void *d2);
int UpCompareChar(const void *ch1,const void *ch2);
int UpCompareInt(const void *i1,const void *i2);
int UpCompareDouble(const void *d1,const void *d2);

//
char *TransStringNewline(char *s);

///////////////////////////////////////////////////////////////////////////////
// 对话框主函数
///////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK FcrDialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	g_hWndFcr = hWnd;
	switch(Msg)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case ID_EXECUTE:
			OnFcrExecute();
			return TRUE;
		case ID_SAVE_OUTPUT:
			OnFcrSaveOutput();
			return TRUE;
		case ID_PRECISION_0:
			OnFcrPrecision0();
			return TRUE;
		case ID_PRECISION_6:
			OnFcrPrecision6();
			return TRUE;
		case ID_PRECISION_15:
			OnFcrPrecision15();
			return TRUE;
		case ID_ABOUT:
			OnFcrAbout();
			return TRUE;
		case ID_EXIT:
			OnFcrExit();
			return TRUE;
		}
		break;
	case WM_INITDIALOG:
		InitFcrDialog();
		return FALSE;
	case WM_CLOSE:
		OnFcrExit();
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 消息处理函数
///////////////////////////////////////////////////////////////////////////////

void OnFcrExecute()
{
	// (BUG)多次解释执行时出错 -- 已经修正，系输出部分错误
	Interpret();
}

void OnFcrSaveOutput()
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWndFcr;
	ofn.lpstrDefExt = "txt";
	ofn.lpstrFilter = "文本文件 (*.txt)\0*.txt\0所有文件 (*.*)\0*.*\0\0";
	ofn.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
	ofn.lpstrFile = (char *)malloc(1024);
	strcpy(ofn.lpstrFile, "运行结果.txt");
	ofn.nMaxFile = 1024;

	if(!GetSaveFileName(&ofn))
		return;

	{	// 保存到文件
		FILE *fout = fopen(ofn.lpstrFile, "wt");
		fwrite(g_lpszOutput, 1, g_nOutputLength, fout);
		fclose(fout);
	}
}

void OnFcrPrecision0()
{
	if(g_nPrecision!=0)
	{
		g_nPrecision = 0;
		if(IDYES == MessageBox(g_hWndFcr, "是否要重新执行程序？", "FCR", MB_YESNO|MB_ICONQUESTION))
			PostMessage(g_hWndFcr, WM_COMMAND, ID_EXECUTE, 0);
	}
}

void OnFcrPrecision6()
{
	if(g_nPrecision!=6)
	{
		g_nPrecision = 6;
		if(IDYES == MessageBox(g_hWndFcr, "是否要重新执行程序？", "FCR", MB_YESNO|MB_ICONQUESTION))
			PostMessage(g_hWndFcr, WM_COMMAND, ID_EXECUTE, 0);
	}
}

void OnFcrPrecision15()
{
	if(g_nPrecision!=15)
	{
		g_nPrecision = 15;
		if(IDYES == MessageBox(g_hWndFcr, "是否要重新执行程序？", "FCR", MB_YESNO|MB_ICONQUESTION))
			PostMessage(g_hWndFcr, WM_COMMAND, ID_EXECUTE, 0);
	}
}

void OnFcrAbout()
{
	DialogBoxParam(g_hInstance, (LPCTSTR)IDD_ABOUTBOX, g_hWndFcr, AboutDialogProc, 0);
}

void OnFcrExit()
{
	free(g_lpszTitle);
	PostQuitMessage(0);
}

void InitFcrDialog()
{
	// 设置图标
	HICON hIcon = LoadIcon(g_hInstance, (LPCTSTR)IDR_MAINFRAME);
	SendMessage(g_hWndFcr, WM_SETICON, TRUE, (LPARAM)hIcon);
	SendMessage(g_hWndFcr, WM_SETICON, FALSE, (LPARAM)hIcon);

	g_pCode = NULL;
	g_nCode = 0;

	g_lpszTitle = (char *)malloc(32);
	strcpy(g_lpszTitle, "FCR");

	g_nPrecision = 6;

	if(!ReadCodesFromRes())
	{
		MessageBox(g_hWndFcr, "代码段错误！程序即将被关闭！", "FCR", MB_OK|MB_ICONERROR);
		PostQuitMessage(0);
	}
	else
	{
		SetWindowText(g_hWndFcr, g_lpszTitle);

		{	// 窗口居中
			RECT rcFcr, rcDesktop;
			HWND hWndDesktop = GetDesktopWindow();
			GetWindowRect(hWndDesktop, &rcDesktop);
			GetWindowRect(g_hWndFcr, &rcFcr);

			MoveWindow(
				g_hWndFcr,
				(rcDesktop.right-(rcFcr.right-rcFcr.left))/2,
				(rcDesktop.bottom-(rcFcr.bottom-rcFcr.top))/2,
				rcFcr.right-rcFcr.left,
				rcFcr.bottom-rcFcr.top,
				FALSE);
		}

		ShowWindow(g_hWndFcr, SW_SHOW);
		PostMessage(g_hWndFcr, WM_COMMAND, ID_EXECUTE, 0);
	}
}

///////////////////////////////////////////////////////////////////////////////
// 其他函数
///////////////////////////////////////////////////////////////////////////////

BOOL ReadCodesFromRes()
{
	HRSRC hResourceInfo = FindResource(g_hInstance, (LPCSTR)17333, "FCR");
	BOOL bSuccess = FALSE;

	if(hResourceInfo)
	{
		int size = SizeofResource(g_hInstance, hResourceInfo);
		HGLOBAL hResource = LoadResource(g_hInstance, hResourceInfo);
		if(hResource)
		{
			LPSTR lpRes = (LPSTR)LockResource(hResource);
			if(lpRes)
			{
				bSuccess = ReadCodes(lpRes);

				UnlockResource(hResource);
			}
		}
	}
	return bSuccess;
}

BOOL ReadCodes(LPCTSTR lpCodes)
{
	int i,j,t;
	char header[4];

	// 检验文件头
	for(i=0; i<3; i++,lpCodes++)
		header[i] = *lpCodes;
	header[3] = '\0';
	if(strcmp(header, "FCR") != 0)
		return FALSE;
	if(*lpCodes != (char)23)
		return FALSE;
	lpCodes++;

	// 读入伪代码数量
	g_nCode = *((int *)lpCodes);
	g_pCode = (ZCode *)malloc(sizeof(ZCode)*g_nCode);

	lpCodes += 4;

	// 读入伪代码
	for(i=0; i<g_nCode; i++)
	{
		g_pCode[i].op = (ZOperator)*lpCodes;
		lpCodes++;

		switch(g_pCode[i].op)
		{
		case OP_OUTPUTS:
		case OP_STRLENS://操作数为不定长字符串
			t = *((int *)lpCodes);
			lpCodes += 4;
			g_pCode[i].sd.string = (char *)malloc(t+1);
			for(j=0; j<t; j++,lpCodes++)
				g_pCode[i].sd.string[j] = *lpCodes;
			g_pCode[i].sd.string[t] = '\0';
			break;
		case OP_STRCPYS://从字符串复制到字符数组
			g_pCode[i].sd.iNumber = *((int *)lpCodes);
			lpCodes += 4;
			t = *((int *)lpCodes);
			lpCodes += 4;
			g_pCode[i].sd.string2 = (char *)malloc(t+1);
			for(j=0; j<t; j++,lpCodes++)
				g_pCode[i].sd.string2[j] = *lpCodes;
			g_pCode[i].sd.string2[t] = '\0';
			break;
		case OP_PARAMTRANSA:case OP_PARAMTRANSC:case OP_PARAMTRANSD:case OP_PARAMTRANSI:
		case OP_NEWARRAYC:case OP_NEWARRAYD:case OP_NEWARRAYI:
		case OP_STRCMP:case OP_STRCPYCA://双整数操作数指令
			g_pCode[i].sd.iNumber = *((int *)lpCodes);
			lpCodes += 4;
			g_pCode[i].sd.iNumber2 = *((int *)lpCodes);
			lpCodes += 4;
			break;
		case OP_LOADCONSTD://单实数操作数指令
			g_pCode[i].sd.dNumber = *((double *)lpCodes);
			lpCodes += 8;
			break;
		case OP_LOADCONSTC://单字符操作数指令
			g_pCode[i].sd.cNumber = *lpCodes;
			lpCodes++;
			break;
		case OP_C2D:case OP_C2I:
		case OP_CALL:
		case OP_D2C:case OP_D2I:
		case OP_DELARRAYC:case OP_DELARRAYD:case OP_DELARRAYI:
		case OP_EQUALC:case OP_EQUALD:case OP_EQUALI:
		case OP_FUNCTIONSTART:
		case OP_GREATC:case OP_GREATD:case OP_GREATI:
		case OP_GREATEQUALC:case OP_GREATEQUALD:case OP_GREATEQUALI:
		case OP_I2C:case OP_I2D:
		case OP_INPUTARRAYC:case OP_INPUTARRAYD:case OP_INPUTARRAYI:
		case OP_INPUTCA:
		case OP_INPUTC:case OP_INPUTD:case OP_INPUTI:
		case OP_JUMP:
		case OP_LESSC:case OP_LESSD:case OP_LESSI:
		case OP_LESSEQUALC:case OP_LESSEQUALD:case OP_LESSEQUALI:
		case OP_LOADARRAY:
		case OP_LOADARRAYC:case OP_LOADARRAYD:case OP_LOADARRAYI:
		case OP_LOADCONSTI:
		case OP_LOADVARC:case OP_LOADVARD:case OP_LOADVARI:
		case OP_NOTEQUALC:case OP_NOTEQUALD:case OP_NOTEQUALI:
		case OP_OUTPUTCA:
		case OP_RETURN:case OP_RETURNC:case OP_RETURND:case OP_RETURNI:
		case OP_SAVEC:case OP_SAVED:case OP_SAVEI:
		case OP_SAVEARRAYC:case OP_SAVEARRAYD:case OP_SAVEARRAYI:
		case OP_STRLENCA:case OP_STRLWR:case OP_STRUPR:
		case OP_AVERAGEC:case OP_AVERAGED:case OP_AVERAGEI:
		case OP_DOWNSORTC:case OP_DOWNSORTD:case OP_DOWNSORTI:
		case OP_MAXC:case OP_MAXD:case OP_MAXI:
		case OP_MINC:case OP_MIND:case OP_MINI:
		case OP_SUMC:case OP_SUMD:case OP_SUMI:
		case OP_UPSORTC:case OP_UPSORTD:case OP_UPSORTI://单整数操作数指令
			g_pCode[i].sd.iNumber = *((int *)lpCodes);
			lpCodes += 4;
			break;
		default://其它零操作数指令
			break;
		}
	}

	return TRUE;
}

BOOL TestAddress(int iAddress)
{
	if(iAddress>=MEMORY_SIZE*256)
	{
		MessageBox(g_hWndFcr, "内存溢出，中止解释执行！","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL SlashModZeroChar(char ch)
{
	if(ch=='\0')
	{
		MessageBox(g_hWndFcr, "“/”或“%”的右操作数为0，中止解释执行！",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL SlashModZeroInt(int i)
{
	if(i==0)
	{
		MessageBox(g_hWndFcr, "“/”或“%”的右操作数为0，中止解释执行！", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL SlashModZeroDouble(double d)
{
	if(d==0.0)
	{
		MessageBox(g_hWndFcr, "“/”或“%”的右操作数为0，中止解释执行！", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL ArraySubscript(int index,int length)
{
	if(index<0 || index>=length)
	{
		MessageBox(g_hWndFcr, "数组下标溢出，中止解释执行！", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL ValidArray(void *pArray)
{
	if(pArray==NULL)
	{
		MessageBox(g_hWndFcr, "无法分配数组空间，中止解释执行！", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL ExamCircumscriptive(ZOP op,double value)
{
	char msg[128];

	msg[0] = '\0';
	switch(op)
	{
	case OP_ARCCOS:
		if(value<-1 || value>1)
			strcpy(msg, "arccos");
		break;
	case OP_ARCSIN:
		if(value<-1 || value>1)
			strcpy(msg, "arcsin");
		break;
	case OP_CTH:
		if(value==0)
			strcpy(msg, "cth");
		break;
	case OP_LG:
		if(value<=0)
			strcpy(msg, "lg");
		break;
	case OP_LN:
		if(value<=0)
			strcpy(msg, "ln");
		break;
	case OP_POW:
		if(value<0)
			strcpy(msg, "pow");
		break;
	case OP_SQRT:
		if(value<0)
			strcpy(msg, "sqrt");
		break;
	default:
		return TRUE;
	}

	if(msg[0] != '\0')
	{
		strcat(msg, " 函数调用时变量超出定义域，中止解释执行！");
		MessageBox(g_hWndFcr, msg, "FC", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL Interpret()
{
	char cTemp;
	int iTemp,iTemp2;
	double dTemp;
	char *lpszTemp;

	int i;

	register int iBase = -1;	//函数基地址，指向函数第一个参数地址
	register int iAddress = 0;	//地址指针，指向第一个未用的地址单元
	register int iCode = 0;		//代码指针，指向当前代码
	int memory[MEMORY_SIZE*256];	//虚内存
	const ZCode *pCode = g_pCode;

	register int iOp;			//指令条数计数
	BOOL bWarning = TRUE;		//是否对可能的死循环进行警告

	clock_t tStart,tElapse=0;
	tStart=clock();

	InitInterpretOutput();

	for(iOp=0;;iOp++)
	{
		if(iOp==173330 && bWarning)//17333只是表示173班33号，无其他意义
		{
			UpdateOutput();
			tElapse += clock()-tStart;
			switch(MessageBox(
				g_hWndFcr,
				"程序可能进入死循环，要中止解释执行吗？\n"
				"按“是”中止解释执行，按“否”继续解释执行\n"
				"按“取消”继续解释执行并不再显示此消息框",
				"FCR",
				MB_YESNOCANCEL|MB_ICONQUESTION))
			{
			case IDYES:
				tStart = clock();
				goto EndInterpret;
				//此处无需break语句
			case IDCANCEL:
				bWarning = FALSE;
				//此处不填入break语句
			case IDNO:
				tStart = clock();
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
				*((double*) &memory[iAddress-1])=*((char*) &memory[iAddress-1]);
			else//sd.iNumber==-1
			{
				*((double*) &memory[iAddress-1])=*((double*) &memory[iAddress-2]);
				*((double*) &memory[iAddress-3])=*((char*) &memory[iAddress-3]);
			}
			iAddress++;
			break;
		case OP_C2I:				//类型转换 char => int		（sd.iNumber为地址偏移量）
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-1]=*((char*) &memory[iAddress-1]);
			else
				memory[iAddress-2]=*((char*) &memory[iAddress-1]);
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
			*((char*) (&memory[iAddress-1]))= - *((char*) (&memory[iAddress-1]));
			iCode++;
			break;
		case OP_CHANGESIGNALD:		//改变实数符号
			*((double*) (&memory[iAddress-2]))= - *((double*) (&memory[iAddress-2]));
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
				*((char*)&memory[iAddress-2]) = (char) *((double*)&memory[iAddress-2]);
			else
			{
				*((char*) &memory[iAddress-3]) = (char) *((double*)&memory[iAddress-3]);
				*((char*) &memory[iAddress-2]) = *((char*) &memory[iAddress-1]);
			}
			iAddress--;
			break;
		case OP_D2I:				//类型转换 double => int	（sd.iNumber为地址偏移量）
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-2] = (int) *((double*)&memory[iAddress-2]);
			else
			{
				memory[iAddress-3] = (int) *((double*)&memory[iAddress-3]);
				memory[iAddress-2] = memory[iAddress-1];
			}
			iAddress--;
			break;
		case OP_DELARRAYC:			//删除一个字符数组（sd.iNumber为数组地址）
			free( (char*)(memory[iBase+pCode[iCode++].sd.iNumber]) );
			break;
		case OP_DELARRAYD:			//删除一个实数数组（sd.iNumber为数组地址）
			free( (double*)(memory[iBase+pCode[iCode++].sd.iNumber]) );
			break;
		case OP_DELARRAYI:			//删除一个整数数组（sd.iNumber为数组地址）
			free( (int*)(memory[iBase+pCode[iCode++].sd.iNumber]) );
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
				memory[iAddress-1] = (int) *((char*)&memory[iAddress-1]);
			else
				memory[iAddress-2] = (int) *((char*)&memory[iAddress-2]);
			break;
		case OP_I2D:				//类型转换 int => double（sd.iNumber为地址偏移量）
			if(!TestAddress(iAddress))
				return FALSE;
			if(pCode[iCode++].sd.iNumber==0)
				*((double*) &memory[iAddress-1]) = (double) memory[iAddress-1];
			else
			{
				*((double*) &memory[iAddress-1]) = *((double*) &memory[iAddress-2]);
				*((double*) &memory[iAddress-3]) = (double) memory[iAddress-3];
			}
			iAddress++;
			break;
		case OP_INPUTARRAYC:		//输入字符数组值（sd.iNumber为数组地址，下标在栈顶）
			iTemp =memory[--iAddress];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			tElapse+=clock()-tStart;
			if(!InputChar(&((char*) memory[iTemp2])[iTemp]))
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
			if(!InputDouble(&((double*)memory[iTemp2])[iTemp]))
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
			if(!InputInt(&((int*)memory[iTemp2])[iTemp]))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTC:				//输入字符（sd.iNumber为地址）
			tElapse+=clock()-tStart;
			if(!InputChar((char*) &memory[iBase+pCode[iCode++].sd.iNumber]))
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
			if(!InputString(&lpszTemp))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			if(strlen(lpszTemp) > (UINT)iTemp2)
				lpszTemp[iTemp2] = '\0';
			strcpy( (char*)memory[iTemp], lpszTemp);
			free(lpszTemp); // MUST FREE
			break;
		case OP_INPUTD:				//输入实数（sd.iNumber为地址）
			tElapse+=clock()-tStart;
			if(!InputDouble( (double*) &memory[iBase+pCode[iCode++].sd.iNumber] ))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTI:				//输入整数（sd.iNumber为地址）
			tElapse+=clock()-tStart;
			if(!InputInt( &memory[iBase+pCode[iCode++].sd.iNumber] ))
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
			*((char*)&memory[iAddress-1]) = ((char*)memory[iTemp2])[iTemp];
			break;
		case OP_LOADARRAYD:			//导入实数数组值（sd.iNumber为数组头地址，下标在栈顶）
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =memory[iAddress-1];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			*((double*)&memory[iAddress-1]) = ((double*)memory[iTemp2])[iTemp];
			iAddress++;
			break;
		case OP_LOADARRAYI:			//导入整数数组值（sd.iNumber为数组头地址，下标在栈顶）
			iTemp =memory[iAddress-1];//数组下标
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//数组地址
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			memory[iAddress-1] = ((int*)memory[iTemp2])[iTemp];
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
			if(!SlashModZeroChar(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1]) %= cTemp;
			iCode++;
			break;
		case OP_MODI:				//余（整数）
			iTemp=memory[--iAddress];
			if(!SlashModZeroInt(iTemp))
				return FALSE;
			memory[iAddress-1]%=iTemp;
			iCode++;
			break;
		case OP_NEWARRAYC:			//申请一个字符数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
			iTemp =iBase+pCode[iCode].sd.iNumber;//数组地址
			iTemp2=pCode[iCode++].sd.iNumber2;//数组长度
			memory[iTemp] = (int)malloc(iTemp2+1);//字符数组长度须加一
			if(!ValidArray((char*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<=iTemp2;i++)
				((char*)memory[iTemp])[i]='\0';
			break;
		case OP_NEWARRAYD:			//申请一个实数数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
			iTemp =iBase+pCode[iCode].sd.iNumber;//数组地址
			iTemp2=pCode[iCode++].sd.iNumber2;//数组长度
			memory[iTemp] = (int)malloc(sizeof(double)*iTemp2);
			if(!ValidArray((double*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<iTemp2;i++)
				((double*)memory[iTemp])[i]=0.0;
			break;
		case OP_NEWARRAYI:			//申请一个整数数组（sd.iNumber为数组头地址，sd.iNumber2为长度）
			iTemp =iBase+pCode[iCode].sd.iNumber;//数组地址
			iTemp2=pCode[iCode++].sd.iNumber2;//数组长度
			memory[iTemp] = (int)malloc(sizeof(int)*iTemp2);
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
			OutputChar( *((char*)&memory[--iAddress]) );
			iCode++;
			break;
		case OP_OUTPUTCA:			//输出字符数组（sd为字符数组地址）
			OutputCharArray( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		case OP_OUTPUTD:			//输出实数
			iAddress-=2;
			OutputDouble( *((double*)&memory[iAddress]) );
			iCode++;
			break;
		case OP_OUTPUTI:			//输出整数
			OutputInt(memory[--iAddress]);
			iCode++;
			break;
		case OP_OUTPUTS:			//输出字符串（sd为字符串实地址）
			OutputString(pCode[iCode].sd.string);
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
			if(!SlashModZeroChar(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1])/=cTemp;
			iCode++;
			break;
		case OP_SLASHD:				//除（实数）
			iAddress-=2;
			dTemp=*((double*)&memory[iAddress]);
			if(!SlashModZeroDouble(dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) /= dTemp;
			iCode++;
			break;
		case OP_SLASHI:				//除（整数）
			iTemp=memory[--iAddress];
			if(!SlashModZeroInt(iTemp))
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
			lpszTemp = (char *)malloc(1+strlen((char*)memory[iBase+pCode[iCode].sd.iNumber2]));
			strcpy(lpszTemp, (char*)memory[iBase+pCode[iCode].sd.iNumber2]);//原字符数组
			iTemp =iBase+pCode[iCode++].sd.iNumber;//目标字符数组地址
			iTemp2=memory[iTemp+1];//目标字符数组长
			if(strlen(lpszTemp) > (UINT)iTemp2)//原字符数组长度大于目标字符数组
				lpszTemp[iTemp2] = '\0';
			strcpy( ((char*)memory[iTemp]), lpszTemp );
			free(lpszTemp);
			break;
		case OP_STRCPYS:			//字符数组初始化（sd.iNumber为数组地址，sd.string2为字符串实地址）
			if(pCode[iCode].sd.string2==NULL)
				strcpy((char*)(memory[iBase+pCode[iCode].sd.iNumber]),"");
			else
			{
				lpszTemp = (char *)malloc(1+strlen(pCode[iCode].sd.string2));
				strcpy(lpszTemp, pCode[iCode].sd.string2);
				iTemp =iBase+pCode[iCode].sd.iNumber;//目标字符数组地址
				iTemp2=memory[iTemp+1];//目标字符数组长
				if( strlen(lpszTemp) > (UINT)iTemp2 )
					lpszTemp[iTemp2] = '\0';
				strcpy((char*)(memory[iTemp]), lpszTemp);
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
				dTemp+= (double) ((char*)memory[iTemp])[i];
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
				dTemp+= (double) ((int*)memory[iTemp])[i];
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
				iTemp2+= (int) ((char*)memory[iTemp])[i];
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
		//candy
		case OP_FC23:
			OutputString("\nFC 2.32 - FC编程语言编译器\n版权所有 朱凯峰 2004.02\n");
			iCode++;
			break;
		case OP_ZKF:
			OutputString("\n朱凯峰\nFC 2.32 版权所有者\ne-mail: cafeeee@sina.com\n");
			iCode++;
			break;
		default:
			MessageBox(g_hWndFcr, "解释执行时碰到了非法的操作符，无法继续解释执行，请与程序提供者联系！",
				"FC",MB_OK|MB_ICONINFORMATION);
			return FALSE;
		}
	}
EndInterpret:
	EndInterpretOutput(tElapse+clock()-tStart);
	MessageBeep(0);
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// 输出控制
///////////////////////////////////////////////////////////////////////////////

void InitInterpretOutput()
{
	if(g_lpszOutput != NULL)
		free(g_lpszOutput);

	// 创建内存输出缓存
	g_lpszOutput = (char *)malloc(INIT_OUTPUT_BUFFER_SIZE);
	g_lpszOutput[0] = '\0';
	g_nOutputCapacity = INIT_OUTPUT_BUFFER_SIZE;
	g_nOutputLength = 0;

	UpdateOutput();
}

void UpdateOutput()
{
	SetDlgItemText(
		g_hWndFcr,
		IDC_OUTPUT,
		g_lpszOutput);
	SendDlgItemMessage(	// 把光标放到输出的最后边
		g_hWndFcr,
		IDC_OUTPUT,
		EM_SETSEL,
		g_nOutputLength,
		g_nOutputLength);
	SendDlgItemMessage( // 滚动滚动条到最后
		g_hWndFcr,
		IDC_OUTPUT,
		EM_SCROLLCARET,
		0,
		0);
}

void EndInterpretOutput(clock_t tElapse)
{
	char buffer[128];

	sprintf(buffer, "\n程序执行用时 %.3f 秒\n", tElapse/1000.0);

	OutputString(buffer);
	UpdateOutput();

	// 不能释放内存输出缓存，还须用以保存结果
}

void IncreaseOutputCapacity(int increment)
{
}

///////////////////////////////////////////////////////////////////////////////
// 输入数据
///////////////////////////////////////////////////////////////////////////////

BOOL InputChar(char *p)
{
	BOOL b;

	UpdateOutput();
	b = CallInputDialog(g_hWndFcr, IM_CHAR, p);
	OutputChar(*p);
	OutputChar('\n');
	return b;
}

BOOL InputInt(int *p)
{
	BOOL b;

	UpdateOutput();
	b = CallInputDialog(g_hWndFcr, IM_INT, p);
	OutputInt(*p);
	OutputChar('\n');
	return b;
}

BOOL InputDouble(double *p)
{
	BOOL b;

	UpdateOutput();
	b = CallInputDialog(g_hWndFcr, IM_DOUBLE, p);
	OutputDouble(*p);
	OutputChar('\n');
	return b;
}

BOOL InputString(char **p)
{
	BOOL b;

	UpdateOutput();
	b = CallInputDialog(g_hWndFcr, IM_STRING, p);
	OutputString(*p);
	OutputChar('\n');
	return b;
}

///////////////////////////////////////////////////////////////////////////////
// 输出数据
///////////////////////////////////////////////////////////////////////////////

void OutputChar(char c)
{
	char buffer[2] = {c, '\0'};

	OutputString(buffer);
}

void OutputInt(int i)
{
	char buffer[32];

	wsprintf(buffer, "%d", i);

	OutputString(buffer);
}

void OutputDouble(double d)
{
	char buffer1[32],buffer2[32];

	sprintf(buffer1, "%%.%df", g_nPrecision);
	sprintf(buffer2, buffer1, d);

	OutputString(buffer2);
}

void OutputString(char *s)
{
	LPSTR lpString = TransStringNewline(s);
	int length = strlen(lpString);

	if(g_nOutputLength+length >= g_nOutputCapacity) // increase capacity
	{
		g_nOutputCapacity = max(4*g_nOutputCapacity, g_nOutputLength+length);
		g_nOutputCapacity = max(g_nOutputCapacity, INIT_OUTPUT_BUFFER_SIZE);

		g_lpszOutput = (char *)realloc((void *)g_lpszOutput, g_nOutputCapacity);

		UpdateOutput();
	}

	strcpy(g_lpszOutput+g_nOutputLength, lpString);

	g_nOutputLength += length;
	free(lpString);
}

void OutputCharArray(char *s)
{
	OutputString(s);
}

///////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////

char *TransStringNewline(char *s)
{
	int length = strlen(s);
	char *lpString = (char *)malloc(length*2+1);
	int i=0,j=0;

	while(i<length)
	{
		if(s[i] == '\n')
		{
			lpString[j++] = '\r';
		}
		lpString[j++] = s[i++];
	}
	lpString[j] = '\0';

	return lpString;
}
