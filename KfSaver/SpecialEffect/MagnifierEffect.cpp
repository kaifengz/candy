// CMagnifierEffect
//

#include "stdafx.h"
#include "ZkfGraphics.h"
#include "MagnifierEffect.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CMagnifierEffect);

#define MAX_VELOCITY			7 // 9 // 
#define FRAME_COUNT				MAX_FRAME_NUM // 100 // 
#define STRETCH_COEFF			1.3

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMagnifierEffect::CMagnifierEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
}

CMagnifierEffect::~CMagnifierEffect()
{
}

const char* CMagnifierEffect::GetEffectName() const
{
	return "MagnifierEffect";
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BOOL CMagnifierEffect::Initialize(HDC hDC, BOOL)
{
	m_nRadius = random(GetWndHeight()/5, GetWndHeight()/10); // 放大镜半径在屏幕高度的1/10到1/5间

	m_x = random(GetWndWidth()-m_nRadius, m_nRadius);
	m_y = random(GetWndHeight()-m_nRadius, m_nRadius);
	m_vx = random(MAX_VELOCITY+1, -MAX_VELOCITY);
	m_vy = random(MAX_VELOCITY+1, -MAX_VELOCITY);

    return TRUE;
}

BOOL CMagnifierEffect::OnTimer(HDC hDC)
{
	if (GetFrameCount() != 0)
		RandomMove();

	// 复制桌面图像
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, GetBkgndBmp());

	CopyAndStretch(hDC, hDCMem, m_x - m_nRadius, m_y - m_nRadius, 2 * m_nRadius, 2 * m_nRadius);

	// 将放大的图样修剪成圆形
	HBITMAP hBitmap = CreateCompatibleBitmap(hDCMem,
		2 * m_nRadius, 2 * m_nRadius);
	SelectObject(hDCMem, hBitmap);

	HPEN hOldPen = (HPEN)SelectObject(hDCMem, GetStockObject(WHITE_PEN));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDCMem, GetStockObject(WHITE_BRUSH));

	Ellipse(hDCMem, 0, 0, 2 * m_nRadius, 2 * m_nRadius);
	BitBlt(hDC,
		m_x - m_nRadius, m_y - m_nRadius,
		2 * m_nRadius, 2 * m_nRadius,
		hDCMem,
		0, 0,
		SRCAND);

	// 释放对象
	SelectObject(hDCMem, hOldPen);
	SelectObject(hDCMem, hOldBrush);
	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);
	DeleteObject(hBitmap);

	return GetFrameCount() < FRAME_COUNT;
}

BOOL CMagnifierEffect::MagicHong(HDC hDC)
{
	if (GetFrameCount() != 0)
		RandomMove();

	// 复制桌面图像
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, GetBkgndBmp());

	CopyAndStretch(hDC, hDCMem, m_x - m_nRadius, m_y - m_nRadius, 2 * m_nRadius, 3 * m_nRadius);

	// 将放大的图样修剪成心形
	HBITMAP hBitmap = CreateCompatibleBitmap(hDCMem,
		2 * m_nRadius, 3 * m_nRadius);
	SelectObject(hDCMem, hBitmap);

	HPEN hOldPen = (HPEN)SelectObject(hDCMem, GetStockObject(WHITE_PEN));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDCMem, GetStockObject(WHITE_BRUSH));

	{	// 画心形
		int r = m_nRadius/2;
#define HEIGHT	(r*4/3)
		POINT pt[] = 
		{
			{0, HEIGHT},
			{0,	0},		{r*3/2, 0},	{r*2, HEIGHT},	// 心形的左边上部分
			{r*5/2, 0},	{r*4, 0},	{r*4, HEIGHT},	// 心形的右边上部分
			{r*4, r*2},	{r*3, r*3},	{r*2, r*4},		// 心形的右边下部分
			{r, r*3},	{0, r*2},	{0, HEIGHT},	// 心形的左边下部分
		};

		BeginPath(hDCMem);
		PolyBezier(hDCMem, pt, countof(pt));
		EndPath(hDCMem);
		FillPath(hDCMem);
	}

	BitBlt(hDC,
		m_x - m_nRadius, m_y - m_nRadius,
		2 * m_nRadius, 3 * m_nRadius,
		hDCMem, 0, 0, SRCAND);

	// 释放对象
	SelectObject(hDCMem, hOldPen);
	SelectObject(hDCMem, hOldBrush);
	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);
	DeleteObject(hBitmap);

	return GetFrameCount() < FRAME_COUNT;
}

void CMagnifierEffect::RandomMove()
{
	m_vx += random(2, -1);
	m_vy += random(2, -1);

	if (m_vx < -MAX_VELOCITY)
		m_vx = -MAX_VELOCITY;
	else if (m_vx > MAX_VELOCITY)
		m_vx = MAX_VELOCITY;

	if (m_vy < -MAX_VELOCITY)
		m_vy = -MAX_VELOCITY;
	else if (m_vy > MAX_VELOCITY)
		m_vy = MAX_VELOCITY;

	m_x += m_vx;
	m_y += m_vy;

	if (m_x < m_nRadius)
	{
		m_x = m_nRadius;
		m_vx = 0;
	}
	else if (m_x > GetWndWidth() - m_nRadius)
	{
		m_x = GetWndWidth() - m_nRadius;
		m_vx = 0;
	}

	if (m_y < m_nRadius)
	{
		m_y = m_nRadius;
		m_vy = 0;
	}
	else if (m_y > GetWndHeight() - m_nRadius)
	{
		m_y = GetWndHeight() - m_nRadius;
		m_vy = 0;
	}
}

void CMagnifierEffect::CopyAndStretch(HDC hDCDest, HDC hDCSrc, int x, int y, int w, int h)
{
	int wf = RoundToInt(w / STRETCH_COEFF);
	int hf = RoundToInt(h / STRETCH_COEFF);
	int xf = x + w / 2 - wf / 2;
	int yf = y + h / 2 - hf / 2;
	StretchBlt(hDCDest, x, y, w, h, hDCSrc, xf, yf, wf, hf, SRCCOPY);
}
