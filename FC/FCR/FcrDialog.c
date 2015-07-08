
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
// ȫ�ֱ���
///////////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;

HWND g_hWndFcr;

// α����
ZCode *g_pCode;
int g_nCode;

// �������
int g_nPrecision;

// �������
LPSTR g_lpszOutput;
int g_nOutputCapacity;
int g_nOutputLength;

// �������
LPSTR g_lpszTitle;

///////////////////////////////////////////////////////////////////////////////
// ����Ԥ����
///////////////////////////////////////////////////////////////////////////////

// �ⲿ��������
extern BOOL CALLBACK AboutDialogProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CallInputDialog(HWND, ZIM, void *);

// ��Ϣ����
void OnFcrExecute();
void OnFcrSaveOutput();
void OnFcrPrecision0();
void OnFcrPrecision6();
void OnFcrPrecision15();
void OnFcrAbout();
void OnFcrExit();
void InitFcrDialog();

// ��ȡα����
BOOL ReadCodesFromRes();
BOOL ReadCodes(LPCTSTR lpCodes);

// ����ִ��ʱ��������Ч�Լ��
BOOL TestAddress(int iAddress);
BOOL SlashModZeroChar(char ch);
BOOL SlashModZeroInt(int i);
BOOL SlashModZeroDouble(double d);
BOOL ArraySubscript(int index,int length);
BOOL ValidArray(void *pArray);
BOOL ExamCircumscriptive(ZOP op,double value);

// ����ִ��
BOOL Interpret();

// �������
void InitInterpretOutput();
void UpdateOutput();
void EndInterpretOutput(clock_t tElapse);

// ��������
BOOL InputChar(char *p);
BOOL InputInt(int *p);
BOOL InputDouble(double *p);
BOOL InputString(char **p);

// �������
void OutputChar(char c);
void OutputInt(int i);
void OutputDouble(double d);
void OutputString(char *s);
void OutputCharArray(char *s);

// ����Ƚ�
int DownCompareChar(const void *ch1,const void *ch2);
int DownCompareInt(const void *i1,const void *i2);
int DownCompareDouble(const void *d1,const void *d2);
int UpCompareChar(const void *ch1,const void *ch2);
int UpCompareInt(const void *i1,const void *i2);
int UpCompareDouble(const void *d1,const void *d2);

//
char *TransStringNewline(char *s);

///////////////////////////////////////////////////////////////////////////////
// �Ի���������
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
// ��Ϣ������
///////////////////////////////////////////////////////////////////////////////

void OnFcrExecute()
{
	// (BUG)��ν���ִ��ʱ���� -- �Ѿ�������ϵ������ִ���
	Interpret();
}

void OnFcrSaveOutput()
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = g_hWndFcr;
	ofn.lpstrDefExt = "txt";
	ofn.lpstrFilter = "�ı��ļ� (*.txt)\0*.txt\0�����ļ� (*.*)\0*.*\0\0";
	ofn.Flags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT;
	ofn.lpstrFile = (char *)malloc(1024);
	strcpy(ofn.lpstrFile, "���н��.txt");
	ofn.nMaxFile = 1024;

	if(!GetSaveFileName(&ofn))
		return;

	{	// ���浽�ļ�
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
		if(IDYES == MessageBox(g_hWndFcr, "�Ƿ�Ҫ����ִ�г���", "FCR", MB_YESNO|MB_ICONQUESTION))
			PostMessage(g_hWndFcr, WM_COMMAND, ID_EXECUTE, 0);
	}
}

void OnFcrPrecision6()
{
	if(g_nPrecision!=6)
	{
		g_nPrecision = 6;
		if(IDYES == MessageBox(g_hWndFcr, "�Ƿ�Ҫ����ִ�г���", "FCR", MB_YESNO|MB_ICONQUESTION))
			PostMessage(g_hWndFcr, WM_COMMAND, ID_EXECUTE, 0);
	}
}

void OnFcrPrecision15()
{
	if(g_nPrecision!=15)
	{
		g_nPrecision = 15;
		if(IDYES == MessageBox(g_hWndFcr, "�Ƿ�Ҫ����ִ�г���", "FCR", MB_YESNO|MB_ICONQUESTION))
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
	// ����ͼ��
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
		MessageBox(g_hWndFcr, "����δ��󣡳��򼴽����رգ�", "FCR", MB_OK|MB_ICONERROR);
		PostQuitMessage(0);
	}
	else
	{
		SetWindowText(g_hWndFcr, g_lpszTitle);

		{	// ���ھ���
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
// ��������
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

	// �����ļ�ͷ
	for(i=0; i<3; i++,lpCodes++)
		header[i] = *lpCodes;
	header[3] = '\0';
	if(strcmp(header, "FCR") != 0)
		return FALSE;
	if(*lpCodes != (char)23)
		return FALSE;
	lpCodes++;

	// ����α��������
	g_nCode = *((int *)lpCodes);
	g_pCode = (ZCode *)malloc(sizeof(ZCode)*g_nCode);

	lpCodes += 4;

	// ����α����
	for(i=0; i<g_nCode; i++)
	{
		g_pCode[i].op = (ZOperator)*lpCodes;
		lpCodes++;

		switch(g_pCode[i].op)
		{
		case OP_OUTPUTS:
		case OP_STRLENS://������Ϊ�������ַ���
			t = *((int *)lpCodes);
			lpCodes += 4;
			g_pCode[i].sd.string = (char *)malloc(t+1);
			for(j=0; j<t; j++,lpCodes++)
				g_pCode[i].sd.string[j] = *lpCodes;
			g_pCode[i].sd.string[t] = '\0';
			break;
		case OP_STRCPYS://���ַ������Ƶ��ַ�����
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
		case OP_STRCMP:case OP_STRCPYCA://˫����������ָ��
			g_pCode[i].sd.iNumber = *((int *)lpCodes);
			lpCodes += 4;
			g_pCode[i].sd.iNumber2 = *((int *)lpCodes);
			lpCodes += 4;
			break;
		case OP_LOADCONSTD://��ʵ��������ָ��
			g_pCode[i].sd.dNumber = *((double *)lpCodes);
			lpCodes += 8;
			break;
		case OP_LOADCONSTC://���ַ�������ָ��
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
		case OP_UPSORTC:case OP_UPSORTD:case OP_UPSORTI://������������ָ��
			g_pCode[i].sd.iNumber = *((int *)lpCodes);
			lpCodes += 4;
			break;
		default://�����������ָ��
			break;
		}
	}

	return TRUE;
}

BOOL TestAddress(int iAddress)
{
	if(iAddress>=MEMORY_SIZE*256)
	{
		MessageBox(g_hWndFcr, "�ڴ��������ֹ����ִ�У�","FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL SlashModZeroChar(char ch)
{
	if(ch=='\0')
	{
		MessageBox(g_hWndFcr, "��/����%�����Ҳ�����Ϊ0����ֹ����ִ�У�",
			"FC",MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL SlashModZeroInt(int i)
{
	if(i==0)
	{
		MessageBox(g_hWndFcr, "��/����%�����Ҳ�����Ϊ0����ֹ����ִ�У�", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL SlashModZeroDouble(double d)
{
	if(d==0.0)
	{
		MessageBox(g_hWndFcr, "��/����%�����Ҳ�����Ϊ0����ֹ����ִ�У�", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL ArraySubscript(int index,int length)
{
	if(index<0 || index>=length)
	{
		MessageBox(g_hWndFcr, "�����±��������ֹ����ִ�У�", "FCR", MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	return TRUE;
}

BOOL ValidArray(void *pArray)
{
	if(pArray==NULL)
	{
		MessageBox(g_hWndFcr, "�޷���������ռ䣬��ֹ����ִ�У�", "FCR", MB_OK|MB_ICONINFORMATION);
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
		strcat(msg, " ��������ʱ����������������ֹ����ִ�У�");
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

	register int iBase = -1;	//��������ַ��ָ������һ��������ַ
	register int iAddress = 0;	//��ַָ�룬ָ���һ��δ�õĵ�ַ��Ԫ
	register int iCode = 0;		//����ָ�룬ָ��ǰ����
	int memory[MEMORY_SIZE*256];	//���ڴ�
	const ZCode *pCode = g_pCode;

	register int iOp;			//ָ����������
	BOOL bWarning = TRUE;		//�Ƿ�Կ��ܵ���ѭ�����о���

	clock_t tStart,tElapse=0;
	tStart=clock();

	InitInterpretOutput();

	for(iOp=0;;iOp++)
	{
		if(iOp==173330 && bWarning)//17333ֻ�Ǳ�ʾ173��33�ţ�����������
		{
			UpdateOutput();
			tElapse += clock()-tStart;
			switch(MessageBox(
				g_hWndFcr,
				"������ܽ�����ѭ����Ҫ��ֹ����ִ����\n"
				"�����ǡ���ֹ����ִ�У������񡱼�������ִ��\n"
				"����ȡ������������ִ�в�������ʾ����Ϣ��",
				"FCR",
				MB_YESNOCANCEL|MB_ICONQUESTION))
			{
			case IDYES:
				tStart = clock();
				goto EndInterpret;
				//�˴�����break���
			case IDCANCEL:
				bWarning = FALSE;
				//�˴�������break���
			case IDNO:
				tStart = clock();
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
				*((double*) &memory[iAddress-1])=*((char*) &memory[iAddress-1]);
			else//sd.iNumber==-1
			{
				*((double*) &memory[iAddress-1])=*((double*) &memory[iAddress-2]);
				*((double*) &memory[iAddress-3])=*((char*) &memory[iAddress-3]);
			}
			iAddress++;
			break;
		case OP_C2I:				//����ת�� char => int		��sd.iNumberΪ��ַƫ������
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-1]=*((char*) &memory[iAddress-1]);
			else
				memory[iAddress-2]=*((char*) &memory[iAddress-1]);
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
			*((char*) (&memory[iAddress-1]))= - *((char*) (&memory[iAddress-1]));
			iCode++;
			break;
		case OP_CHANGESIGNALD:		//�ı�ʵ������
			*((double*) (&memory[iAddress-2]))= - *((double*) (&memory[iAddress-2]));
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
				*((char*)&memory[iAddress-2]) = (char) *((double*)&memory[iAddress-2]);
			else
			{
				*((char*) &memory[iAddress-3]) = (char) *((double*)&memory[iAddress-3]);
				*((char*) &memory[iAddress-2]) = *((char*) &memory[iAddress-1]);
			}
			iAddress--;
			break;
		case OP_D2I:				//����ת�� double => int	��sd.iNumberΪ��ַƫ������
			if(pCode[iCode++].sd.iNumber==0)
				memory[iAddress-2] = (int) *((double*)&memory[iAddress-2]);
			else
			{
				memory[iAddress-3] = (int) *((double*)&memory[iAddress-3]);
				memory[iAddress-2] = memory[iAddress-1];
			}
			iAddress--;
			break;
		case OP_DELARRAYC:			//ɾ��һ���ַ����飨sd.iNumberΪ�����ַ��
			free( (char*)(memory[iBase+pCode[iCode++].sd.iNumber]) );
			break;
		case OP_DELARRAYD:			//ɾ��һ��ʵ�����飨sd.iNumberΪ�����ַ��
			free( (double*)(memory[iBase+pCode[iCode++].sd.iNumber]) );
			break;
		case OP_DELARRAYI:			//ɾ��һ���������飨sd.iNumberΪ�����ַ��
			free( (int*)(memory[iBase+pCode[iCode++].sd.iNumber]) );
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
				memory[iAddress-1] = (int) *((char*)&memory[iAddress-1]);
			else
				memory[iAddress-2] = (int) *((char*)&memory[iAddress-2]);
			break;
		case OP_I2D:				//����ת�� int => double��sd.iNumberΪ��ַƫ������
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
		case OP_INPUTARRAYC:		//�����ַ�����ֵ��sd.iNumberΪ�����ַ���±���ջ����
			iTemp =memory[--iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
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
		case OP_INPUTARRAYD:		//����ʵ������ֵ��sd.iNumberΪ�����ַ���±���ջ����
			iTemp =memory[--iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
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
		case OP_INPUTARRAYI:		//������������ֵ��sd.iNumberΪ�����ַ���±���ջ����
			iTemp =memory[--iAddress];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
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
		case OP_INPUTC:				//�����ַ���sd.iNumberΪ��ַ��
			tElapse+=clock()-tStart;
			if(!InputChar((char*) &memory[iBase+pCode[iCode++].sd.iNumber]))
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
		case OP_INPUTD:				//����ʵ����sd.iNumberΪ��ַ��
			tElapse+=clock()-tStart;
			if(!InputDouble( (double*) &memory[iBase+pCode[iCode++].sd.iNumber] ))
			{
				tStart=clock();
				goto EndInterpret;
			}
			tStart=clock();
			break;
		case OP_INPUTI:				//����������sd.iNumberΪ��ַ��
			tElapse+=clock()-tStart;
			if(!InputInt( &memory[iBase+pCode[iCode++].sd.iNumber] ))
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
			*((char*)&memory[iAddress-1]) = ((char*)memory[iTemp2])[iTemp];
			break;
		case OP_LOADARRAYD:			//����ʵ������ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
			if(!TestAddress(iAddress))
				return FALSE;
			iTemp =memory[iAddress-1];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			*((double*)&memory[iAddress-1]) = ((double*)memory[iTemp2])[iTemp];
			iAddress++;
			break;
		case OP_LOADARRAYI:			//������������ֵ��sd.iNumberΪ����ͷ��ַ���±���ջ����
			iTemp =memory[iAddress-1];//�����±�
			iTemp2=iBase+pCode[iCode++].sd.iNumber;//�����ַ
			if(!ArraySubscript(iTemp,memory[iTemp2+1]))
				return FALSE;
			memory[iAddress-1] = ((int*)memory[iTemp2])[iTemp];
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
			if(!SlashModZeroChar(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1]) %= cTemp;
			iCode++;
			break;
		case OP_MODI:				//�ࣨ������
			iTemp=memory[--iAddress];
			if(!SlashModZeroInt(iTemp))
				return FALSE;
			memory[iAddress-1]%=iTemp;
			iCode++;
			break;
		case OP_NEWARRAYC:			//����һ���ַ����飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
			iTemp =iBase+pCode[iCode].sd.iNumber;//�����ַ
			iTemp2=pCode[iCode++].sd.iNumber2;//���鳤��
			memory[iTemp] = (int)malloc(iTemp2+1);//�ַ����鳤�����һ
			if(!ValidArray((char*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<=iTemp2;i++)
				((char*)memory[iTemp])[i]='\0';
			break;
		case OP_NEWARRAYD:			//����һ��ʵ�����飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
			iTemp =iBase+pCode[iCode].sd.iNumber;//�����ַ
			iTemp2=pCode[iCode++].sd.iNumber2;//���鳤��
			memory[iTemp] = (int)malloc(sizeof(double)*iTemp2);
			if(!ValidArray((double*)memory[iTemp]))
				return FALSE;
			memory[iTemp+1]=iTemp2;
			for(i=0;i<iTemp2;i++)
				((double*)memory[iTemp])[i]=0.0;
			break;
		case OP_NEWARRAYI:			//����һ���������飨sd.iNumberΪ����ͷ��ַ��sd.iNumber2Ϊ���ȣ�
			iTemp =iBase+pCode[iCode].sd.iNumber;//�����ַ
			iTemp2=pCode[iCode++].sd.iNumber2;//���鳤��
			memory[iTemp] = (int)malloc(sizeof(int)*iTemp2);
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
			OutputChar( *((char*)&memory[--iAddress]) );
			iCode++;
			break;
		case OP_OUTPUTCA:			//����ַ����飨sdΪ�ַ������ַ��
			OutputCharArray( (char*)memory[iBase+pCode[iCode++].sd.iNumber] );
			break;
		case OP_OUTPUTD:			//���ʵ��
			iAddress-=2;
			OutputDouble( *((double*)&memory[iAddress]) );
			iCode++;
			break;
		case OP_OUTPUTI:			//�������
			OutputInt(memory[--iAddress]);
			iCode++;
			break;
		case OP_OUTPUTS:			//����ַ�����sdΪ�ַ���ʵ��ַ��
			OutputString(pCode[iCode].sd.string);
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
			if(!SlashModZeroChar(cTemp))
				return FALSE;
			*((char*)&memory[iAddress-1])/=cTemp;
			iCode++;
			break;
		case OP_SLASHD:				//����ʵ����
			iAddress-=2;
			dTemp=*((double*)&memory[iAddress]);
			if(!SlashModZeroDouble(dTemp))
				return FALSE;
			*((double*)&memory[iAddress-2]) /= dTemp;
			iCode++;
			break;
		case OP_SLASHI:				//����������
			iTemp=memory[--iAddress];
			if(!SlashModZeroInt(iTemp))
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
			lpszTemp = (char *)malloc(1+strlen((char*)memory[iBase+pCode[iCode].sd.iNumber2]));
			strcpy(lpszTemp, (char*)memory[iBase+pCode[iCode].sd.iNumber2]);//ԭ�ַ�����
			iTemp =iBase+pCode[iCode++].sd.iNumber;//Ŀ���ַ������ַ
			iTemp2=memory[iTemp+1];//Ŀ���ַ����鳤
			if(strlen(lpszTemp) > (UINT)iTemp2)//ԭ�ַ����鳤�ȴ���Ŀ���ַ�����
				lpszTemp[iTemp2] = '\0';
			strcpy( ((char*)memory[iTemp]), lpszTemp );
			free(lpszTemp);
			break;
		case OP_STRCPYS:			//�ַ������ʼ����sd.iNumberΪ�����ַ��sd.string2Ϊ�ַ���ʵ��ַ��
			if(pCode[iCode].sd.string2==NULL)
				strcpy((char*)(memory[iBase+pCode[iCode].sd.iNumber]),"");
			else
			{
				lpszTemp = (char *)malloc(1+strlen(pCode[iCode].sd.string2));
				strcpy(lpszTemp, pCode[iCode].sd.string2);
				iTemp =iBase+pCode[iCode].sd.iNumber;//Ŀ���ַ������ַ
				iTemp2=memory[iTemp+1];//Ŀ���ַ����鳤
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
			iTemp=iBase+pCode[iCode++].sd.iNumber;//�����ַ
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
				dTemp+= (double) ((int*)memory[iTemp])[i];
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
				iTemp2+= (int) ((char*)memory[iTemp])[i];
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
		//candy
		case OP_FC23:
			OutputString("\nFC 2.32 - FC������Ա�����\n��Ȩ���� �쿭�� 2004.02\n");
			iCode++;
			break;
		case OP_ZKF:
			OutputString("\n�쿭��\nFC 2.32 ��Ȩ������\ne-mail: cafeeee@sina.com\n");
			iCode++;
			break;
		default:
			MessageBox(g_hWndFcr, "����ִ��ʱ�����˷Ƿ��Ĳ��������޷���������ִ�У���������ṩ����ϵ��",
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
// �������
///////////////////////////////////////////////////////////////////////////////

void InitInterpretOutput()
{
	if(g_lpszOutput != NULL)
		free(g_lpszOutput);

	// �����ڴ��������
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
	SendDlgItemMessage(	// �ѹ��ŵ����������
		g_hWndFcr,
		IDC_OUTPUT,
		EM_SETSEL,
		g_nOutputLength,
		g_nOutputLength);
	SendDlgItemMessage( // ���������������
		g_hWndFcr,
		IDC_OUTPUT,
		EM_SCROLLCARET,
		0,
		0);
}

void EndInterpretOutput(clock_t tElapse)
{
	char buffer[128];

	sprintf(buffer, "\n����ִ����ʱ %.3f ��\n", tElapse/1000.0);

	OutputString(buffer);
	UpdateOutput();

	// �����ͷ��ڴ�������棬�������Ա�����
}

void IncreaseOutputCapacity(int increment)
{
}

///////////////////////////////////////////////////////////////////////////////
// ��������
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
// �������
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
