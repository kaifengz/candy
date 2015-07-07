
#include <windows.h>
#include "resource.h"

#include "FC-define.h"

///////////////////////////////////////////////////////////////////////////////
// 全局变量
///////////////////////////////////////////////////////////////////////////////

extern HINSTANCE g_hInstance;
extern HWND g_hWndFcr;

HWND g_hWndInput;

// 输入结果
char g_cResult;
int g_iResult;
double g_dResult;
char *g_lpszResult;
BOOL g_bSuccess;

// 
ZIM g_im;

///////////////////////////////////////////////////////////////////////////////
// 函数预定义
///////////////////////////////////////////////////////////////////////////////

// 消息处理
void OnInputOK();
void OnInputExit();
void InitInputDialog();

BOOL CallInputDialog(HWND hWndParent, ZIM im, void *p);

///////////////////////////////////////////////////////////////////////////////
// 对话框主函数
///////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK InputDialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	g_hWndInput = hWnd;
	switch(Msg)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
			OnInputOK();
			return TRUE;
		case IDCANCEL:
			OnInputExit();
			return TRUE;
		}
		break;
	case WM_INITDIALOG:
		InitInputDialog();
		return FALSE;
	case WM_CLOSE:
		OnInputExit();
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
// 消息处理函数
///////////////////////////////////////////////////////////////////////////////

void OnInputOK()
{
	char buffer[1028];
	int i,length;
	int sign = 1;

	GetDlgItemText(g_hWndInput, IDC_INPUT, buffer, 1028);
	length = strlen(buffer);

	switch(g_im)
	{
	case IM_CHAR:
		for(i=0;i<length && (buffer[i]==' ' || buffer[i]=='\t');i++)
			;
		if(i<length)
		{
			g_cResult=buffer[i];
			for(i++;i<length && (buffer[i]==' ' || buffer[i]=='\t');i++)
				;
			if(i==length)
				break;
		}
		MessageBox(g_hWndInput, "请输入一个字符！", "FCR", MB_OK|MB_ICONINFORMATION);
		SendDlgItemMessage(g_hWndInput, IDC_INPUT, WM_SETFOCUS, 0, 0);
		SendDlgItemMessage(g_hWndInput, IDC_INPUT, EM_SETSEL, MAKEWPARAM(0,-1), 0);
		return;
	case IM_INT:
		for(i=0;i<length && (buffer[i]==' ' || buffer[i]=='\t');i++)
			;
		if(i<length)
		{
			if(buffer[i]=='+')
				i++;
			else if(buffer[i]=='-')
			{
				i++;
				sign=-1;
			}
		}
		if(i<length)
		{
			g_iResult = 0;
			for(;i<length && buffer[i]>='0' && buffer[i]<='9';i++)
			{
				g_iResult *= 10;
				g_iResult += buffer[i]-'0';
			}
			for(;i<length && (buffer[i]==' ' || buffer[i]=='\t');i++)
				;
			if(i==length)
			{
				g_iResult *= sign;
				break;
			}
		}
		MessageBox(g_hWndInput, "请输入一个整数！", "FCR", MB_OK|MB_ICONINFORMATION);
		SendDlgItemMessage(g_hWndInput, IDC_INPUT, WM_SETFOCUS, 0, 0);
		SendDlgItemMessage(g_hWndInput, IDC_INPUT, EM_SETSEL, MAKEWPARAM(0,-1), 0);
		return;
	case IM_DOUBLE:
		for(i=0;i<length && (buffer[i]==' ' || buffer[i]=='\t');i++)
			;
		if(i<length)
		{
			if(buffer[i]=='+')
				i++;
			else if(buffer[i]=='-')
			{
				i++;
				sign=-1;
			}
		}
		if(i<length)
		{
			g_iResult = 0;
			for(;i<length && buffer[i]>='0' && buffer[i]<='9';i++)
			{
				g_iResult *= 10;
				g_iResult += buffer[i]-'0';
			}
			if(buffer[i]=='.')
			{
				double l = 1;
				g_dResult = 0;
				for(i++;i<length && buffer[i]>='0' && buffer[i]<='9';i++)
				{
					g_dResult *= 10;
					g_dResult += buffer[i]-'0';
					l *= 10;
				}
				g_dResult /= l;
				g_dResult += g_iResult;
			}
			else
				g_dResult = g_iResult;
			for(;i<length && (buffer[i]==' ' || buffer[i]=='\t');i++)
				;
			if(i==length)
			{
				g_dResult *= sign;
				break;
			}
		}
		MessageBox(g_hWndInput, "请输入一个实数！", "FCR", MB_OK|MB_ICONINFORMATION);
		SendDlgItemMessage(g_hWndInput, IDC_INPUT, WM_SETFOCUS, 0, 0);
		SendDlgItemMessage(g_hWndInput, IDC_INPUT, EM_SETSEL, MAKEWPARAM(0,-1), 0);
		return;
	case IM_STRING:
		g_lpszResult = (char *)malloc(1028);
		strcpy(g_lpszResult, buffer);
		break;
	}
	g_bSuccess = TRUE;
	EndDialog(g_hWndInput, 0);
}

void OnInputExit()
{
	g_bSuccess = FALSE;
	EndDialog(g_hWndInput, 0);
}

void InitInputDialog()
{
	{	// 窗口居中
		RECT rcFcr, rcInput;

		GetWindowRect(g_hWndFcr, &rcFcr);
		GetWindowRect(g_hWndInput, &rcInput);

		MoveWindow(
			g_hWndInput,
			rcFcr.left+(rcFcr.right-rcFcr.left-(rcInput.right-rcInput.left))/2,
			rcFcr.top+(rcFcr.bottom-rcFcr.top-(rcInput.bottom-rcInput.top))/2,
			rcInput.right-rcInput.left,
			rcInput.bottom-rcInput.top,
			FALSE);
	}

	switch(g_im)
	{
	case IM_CHAR:
		SetDlgItemText(g_hWndInput, IDC_INFORMATION, "请输入一个字符：");
		break;
	case IM_INT:
		SetDlgItemText(g_hWndInput, IDC_INFORMATION, "请输入一个整数：");
		break;
	case IM_DOUBLE:
		SetDlgItemText(g_hWndInput, IDC_INFORMATION, "请输入一个实数：");
		break;
	case IM_STRING:
		SetDlgItemText(g_hWndInput, IDC_INFORMATION, "请输入一行字符串：");
		break;
	}

	// 限定输入框至多能输入1024个字符
	SendDlgItemMessage(g_hWndInput, IDC_INPUT, EM_LIMITTEXT, 1024, 0);
}

BOOL CallInputDialog(HWND hWndParent, ZIM im, void *p)
{
	g_im = im;

	DialogBoxParam(g_hInstance, (LPCTSTR)IDD_INPUT_DLG, hWndParent, InputDialogProc, 0);

	if(!g_bSuccess)
		return FALSE;

	switch(im)
	{
	case IM_CHAR:
		*(char*)p = g_cResult;
		break;
	case IM_INT:
		*(int *)p = g_iResult;
		break;
	case IM_DOUBLE:
		*(double *)p = g_dResult;
		break;
	case IM_STRING:
		*(char **)p = g_lpszResult;
		break;
	}

	return TRUE;
}
