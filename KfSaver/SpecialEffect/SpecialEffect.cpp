// SpecialEffect.cpp: implementation of the CSpecialEffect class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SpecialEffect.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSpecialEffect::CSpecialEffect(HWND hWnd, HBITMAP hBmpBkgnd, DWORD dwEffectStyle) :
	m_hWnd(hWnd),
	m_hBmpBkgnd(hBmpBkgnd),
	m_dwEffectStyle(dwEffectStyle)
{
	GetClientRect(m_hWnd, &m_rcWnd);
	m_nWndWidth	= m_rcWnd.right - m_rcWnd.left;
	m_nWndHeight	= m_rcWnd.bottom - m_rcWnd.top;

	m_hBmpScreen = NULL;
	m_uFrameCount = 0;
}

CSpecialEffect::~CSpecialEffect()
{
	if(m_hBmpScreen)
		DeleteObject(m_hBmpScreen);
}

BOOL CSpecialEffect::DrawFrame(BOOL bMagic)
{
	if(m_uFrameCount == 0) // is the first frame
	{
        m_tFirstFrame = GetTickCount();
		m_tLastFrame = m_tFirstFrame;
// 		LOG("DrawNewScreen Start");
		const BOOL bRet = DrawNewScreen(bMagic);
// 		LOG("DrawNewScreen Done");
        ++m_uFrameCount;
        return bRet;
	}
	else // not the first frame
	{
		if(GetTickCount() - m_tLastFrame >= GetTimeslice())
		{
		    m_tLastFrame = GetTickCount();
// 			LOG("DrawNewScreen Start");
		    const BOOL bRet = DrawNewScreen(bMagic);
// 			LOG("DrawNewScreen Done");
            ++m_uFrameCount;
            return bRet;
		}
		else
		{
// 			LOG("CopyOldScreen Start");
			CopyOldScreen();
// 			LOG("CopyOldScreen Done");
			return TRUE;
		}
	}
}

void CSpecialEffect::CopyOldScreen()
{
	HDC hDC = GetDC(m_hWnd);
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, m_hBmpScreen);

	BitBlt(hDC,
		m_rcWnd.left, m_rcWnd.top, m_nWndWidth, m_nWndHeight,
		hDCMem, 0, 0, SRCCOPY);

	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);
	ReleaseDC(m_hWnd, hDC);
}

BOOL CSpecialEffect::DrawNewScreen(BOOL bMagic)
{
	HDC hDC = GetDC(m_hWnd);

	BOOL bRet = FALSE;
	BOOL bFillBackgnd = FALSE;

	if(m_hBmpScreen == NULL)
	{
		m_hBmpScreen = CreateCompatibleBitmap(hDC, m_nWndWidth, m_nWndHeight);
		bFillBackgnd = TRUE;
	}

	// double screen buffer
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, m_hBmpScreen);

	// make background black
	HPEN hOldPen = (HPEN)SelectObject(hDCMem, GetStockObject(BLACK_PEN));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDCMem, GetStockObject(BLACK_BRUSH));

	if(bFillBackgnd || ((m_dwEffectStyle & ES_FILL_BACKGROUND) != 0) )
		Rectangle(hDCMem, m_rcWnd.left, m_rcWnd.top, m_nWndWidth, m_nWndHeight);

    BOOL bInitializeFail = FALSE;
    if(m_uFrameCount == 0)
    {
        if(!Initialize(hDCMem, bMagic))
            bInitializeFail = TRUE;
    }

    if(bInitializeFail)
        bRet = FALSE;
    else
    	bRet = (bMagic ? MagicHong(hDCMem) : OnTimer(hDCMem));

    if(bRet)
    {
		IncreaseFrameCount();
		DrawFPS(hDCMem);

		// TODO: something strange here: without BitBlt, 3dMazeEffect and
		// SnakeEffect still can show their drawing. I don't know why.
		BitBlt(hDC,
			m_rcWnd.left, m_rcWnd.top, m_nWndWidth, m_nWndHeight,
			hDCMem, 0, 0, SRCCOPY);
    }

	// free objects
	SelectObject(hDCMem, hOldPen);
	SelectObject(hDCMem, hOldBrush);
	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);

	ReleaseDC(m_hWnd, hDC);
	return bRet;
}

UINT CSpecialEffect::GetFrameCount() const
{
    return m_uFrameCount;
}

HWND CSpecialEffect::GetWndHandle() const
{
	return m_hWnd;
}

HBITMAP CSpecialEffect::GetBkgndBmp() const
{
    return m_hBmpBkgnd;
}

int CSpecialEffect::GetWndWidth() const
{
    return m_nWndWidth;
}

int CSpecialEffect::GetWndHeight() const
{
    return m_nWndHeight;
}

DWORD CSpecialEffect::GetElapsedTime() const
{
    return GetTickCount() - m_tFirstFrame;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

void CSpecialEffect::IncreaseFrameCount()
{
	m_fps.IncreaseCount();
}

unsigned int CSpecialEffect::GetFPS()
{
	return m_fps.GetFPS();
}

BOOL CSpecialEffect::ShouldDrawFPS() const
{
#ifdef DEBUG_MODE
	return TRUE;
#else
	return (m_dwEffectStyle & ES_SHOW_FPS) == ES_SHOW_FPS;
#endif
}

void CSpecialEffect::DrawFPS(HDC hDC)
{
	ASSERT(hDC != NULL);

	if(!ShouldDrawFPS())
		return;

	char buff[16];

#pragma warning(disable:4996)
	sprintf(buff, "%d", GetFPS());
#pragma warning(default:4996)
	const int len = (int)strlen(buff);

	SIZE size;
	GetTextExtentPoint32(hDC, buff, len, &size);
	TextOut(hDC, GetWndWidth()-size.cx, 0, buff, (int)strlen(buff));
}
