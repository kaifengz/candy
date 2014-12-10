
#include "stdafx.h"

#include "EffectFactory.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#define IDT_TIMER	17333
#define TIMESLICE	1


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void InitSaver(HWND);
void OnTimer(HWND, UINT nIDEvent);
HBITMAP SaveBkground();
BOOL IsMagicKey(char cKey);


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		InitSaver(hWnd);
		break;

	case WM_TIMER:
		OnTimer(hWnd, (UINT)wParam);
		break;

	case WM_DESTROY:
		KillTimer(hWnd, IDT_TIMER);
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:
#ifdef DEBUG_MODE // in debug mode, user can exit program by press "ESC"
		if(VK_ESCAPE == wParam)
			PostQuitMessage(0);
#endif
		if(IsMagicKey((char)wParam))
			return 0;
		break;
	}

#ifndef DEBUG_MODE
	return DefScreenSaverProc(hWnd, message, wParam, lParam);
#else
	return DefWindowProc(hWnd, message, wParam, lParam);
#endif
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hWnd,UINT message,WPARAM wParam, LPARAM lParam)
{
	// 暂时不需要配置对话框，仅返回FALSE
	return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInstance)
{
	// 一般不需要，仅返回TRUE 
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static HBITMAP g_hBmpBkgnd = NULL;
static int g_nEnableMagic = 0;

void InitSaver(HWND hWnd)
{
	g_hBmpBkgnd = SaveBkground();
	SetTimer(hWnd, IDT_TIMER, TIMESLICE, NULL);
}

void OnTimer(HWND hWnd, UINT nIDEvent)
{
	static CSpecialEffect *pEffect = NULL;
	static int nEnableMagic = 0;

	if(nIDEvent == IDT_TIMER)
	{
		// Magic被激活或被关闭，重新创建特效
		if(g_nEnableMagic != nEnableMagic)
		{
			nEnableMagic = g_nEnableMagic;
			if(pEffect != NULL)
			{
				CEffectFactory::DeleteEffect(pEffect);
				pEffect = NULL;
			}
		}

		if(pEffect == NULL)
		{
			pEffect = CEffectFactory::CreateEffect(hWnd, g_hBmpBkgnd);
			if(pEffect == NULL)
			{
				ASSERT(FALSE);
				return;
			}
		}

		if(!pEffect->DrawFrame(nEnableMagic != 0))
		{
			CEffectFactory::DeleteEffect(pEffect);
			pEffect = NULL;
		}
	}
}

HBITMAP SaveBkground()
{
	HWND hWndDesktop = GetDesktopWindow();
	HDC hDcDesktop = GetDC(hWndDesktop);

    RECT rcSaver;
#ifndef DEBUG_MODE
    GetWindowRect(hWndDesktop, &rcSaver);
#else
    void GetSaverRect(LPRECT);
    GetSaverRect(&rcSaver);
#endif
	const int nWidthSaver = rcSaver.right - rcSaver.left;
	const int nHeightSaver = rcSaver.bottom - rcSaver.top;

	HBITMAP hBmp = CreateCompatibleBitmap(hDcDesktop, nWidthSaver, nHeightSaver);

	HDC hDCMem = CreateCompatibleDC(hDcDesktop);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, hBmp);

	BitBlt(hDCMem, 0, 0, nWidthSaver, nHeightSaver,
        hDcDesktop, rcSaver.left, rcSaver.top, SRCCOPY);

	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);
	ReleaseDC(hWndDesktop, hDcDesktop);

	return hBmp;
}

BOOL IsMagicKey(char cKey)
{
	static const char * const pMagicString = "hong";
	static const char *pMagicKey = pMagicString;

	if(cKey>='A' && cKey<='Z')
		cKey += 'a'-'A';

	if(*pMagicKey == '\0')
		return FALSE;
	if(cKey == *pMagicKey)
	{
		++pMagicKey;
		if(*pMagicKey == '\0')
		{
			pMagicKey = pMagicString;
			++g_nEnableMagic;

			LOG("MagicHong enabled");
		}
		return TRUE;
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//	Functions and Variables for Debug mode
///////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_MODE

#define WND_CLASS_NAME	"MySaver [Debug Mode]"
#define WND_TITLE		"MySaver [Debug Mode]"
#define WND_WIDTH		960
#define WND_HEIGHT		720

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
void GetSaverRect(LPRECT);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;

	// Initialize global strings
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ScreenSaverProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WND_CLASS_NAME;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd = CreateWindow(
		WND_CLASS_NAME, WND_TITLE,
		WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	{	// center window
        RECT rcSaver;
        GetSaverRect(&rcSaver);
		MoveWindow(hWnd,
			rcSaver.left,
            rcSaver.top,
			rcSaver.right - rcSaver.left,
			rcSaver.bottom - rcSaver.top,
            FALSE);
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void GetSaverRect(LPRECT lpRect)
{
    if(lpRect == NULL)
        return;

	HWND hDesktop = GetDesktopWindow();
	RECT rcDesktop;
	GetWindowRect(hDesktop, &rcDesktop);

    lpRect->left = (rcDesktop.right-rcDesktop.left - WND_WIDTH) / 2;
    lpRect->top = (rcDesktop.bottom-rcDesktop.top - WND_HEIGHT) / 2;
    lpRect->right = lpRect->left + WND_WIDTH;
    lpRect->bottom = lpRect->top + WND_HEIGHT;
}

#endif // DEBUG_MODE
