// FCR.cpp : Defines the entry point for the application.
//

#include <windows.h>
#include "resource.h"

HINSTANCE g_hInstance;

// �ⲿ��������
BOOL CALLBACK FcrDialogProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	g_hInstance = hInstance;

	DialogBoxParam(hInstance, (LPCTSTR)IDD_FCR_DIALOG, NULL, FcrDialogProc, 0);

	return 0;
}
