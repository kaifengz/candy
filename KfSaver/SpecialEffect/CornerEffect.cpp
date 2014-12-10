// CCornerView
//

#include "stdafx.h"
#include "CornerEffect.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CCornerEffect);

#define MAX_CORNER_FRAME	MAX_FRAME_NUM
#define MIN_VELOCITY		3
#define MAX_VELOCITY		11
#define MAX_COLOR_CHANGE	16

/////////////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////////////

CCornerEffect::CCornerEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd)
{
}

CCornerEffect::~CCornerEffect()
{
}

const char* CCornerEffect::GetEffectName() const
{
	return "CornerEffect";
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

BOOL CCornerEffect::Initialize(HDC hDC, BOOL)
{
	{	// copy background bitmap
		HDC hDCTmp = CreateCompatibleDC(hDC);
		HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCTmp, GetBkgndBmp());

		BitBlt(hDC, 0, 0, GetWndWidth(), GetWndHeight(),
			hDCTmp, 0, 0, SRCCOPY);

		SelectObject(hDCTmp, hOldBmp);
		DeleteDC(hDCTmp);
	}

	{	// init position
		for(int i=0; i<MAX_VERTEX_NUM; i++)
		{
			m_quad[0].pt[i].x = random(GetWndWidth());
			m_quad[0].pt[i].y = random(GetWndHeight());
		}
	}

	{	// init velocity
		for(int i=0; i<MAX_VERTEX_NUM; i++)
		{
			m_vx[i] = random(MAX_VELOCITY, MIN_VELOCITY);
			m_vy[i] = random(MAX_VELOCITY, MIN_VELOCITY);

			if(RandomBoolean())
			{
				m_vx[i] = -m_vx[i];
			}
			if(RandomBoolean())
			{
				m_vy[i] = -m_vy[i];
			}
		}
	}

	{	// init color
		m_color = RGB(random(256), random(256), random(256));
	}

    return TRUE;
}

BOOL CCornerEffect::OnTimer(HDC hDC)
{
	if(GetFrameCount() != 0)
	{
		// clear old quads
		DrawQuads(hDC, RGB(0,0,0));

		// quads step in
		QuadStep();

		// color step in
		ColorStep();
	}

	// draw new quads
	DrawQuads(hDC, m_color);

	return GetFrameCount() < MAX_CORNER_FRAME;
}

/////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////

void CCornerEffect::DrawQuads(HDC hDC, COLORREF color)
{
	const int nQuad =
		(GetFrameCount()+1 >= MAX_QUAD_NUM ? MAX_QUAD_NUM : GetFrameCount()+1);

	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	int i, j;

	for(i=0; i<nQuad; i++)
	{
		MoveToEx(hDC,
			m_quad[i].pt[MAX_VERTEX_NUM - 1].x,
			m_quad[i].pt[MAX_VERTEX_NUM - 1].y,
			NULL);

		for(j=0; j<MAX_VERTEX_NUM; j++)
		{
			LineTo(hDC, m_quad[i].pt[j].x, m_quad[i].pt[j].y);
		}
	}

	// free object
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

void CCornerEffect::QuadStep()
{
	int i, j;

	for(i=MAX_QUAD_NUM-1; i>0; i--)
	{
		m_quad[i] = m_quad[i-1];
	}

	for(j=0; j<MAX_VERTEX_NUM; j++)
	{
		m_quad[0].pt[j].x += m_vx[j];
		m_quad[0].pt[j].y += m_vy[j];

		if(m_quad[0].pt[j].x < 0)
		{
			m_quad[0].pt[j].x = 0;

			m_vx[j] = random(MAX_VELOCITY, MIN_VELOCITY);
			m_vy[j] = random(MAX_VELOCITY, MIN_VELOCITY);
			if(RandomBoolean())
				m_vy[j] = -m_vy[j];
		}

		if(m_quad[0].pt[j].x >= GetWndWidth())
		{
			m_quad[0].pt[j].x = GetWndWidth() - 1;

			m_vx[j] = - random(MAX_VELOCITY, MIN_VELOCITY);
			m_vy[j] = random(MAX_VELOCITY, MIN_VELOCITY);
			if(RandomBoolean())
				m_vy[j] = -m_vy[j];
		}

		if(m_quad[0].pt[j].y < 0)
		{
			m_quad[0].pt[j].y = 0;

			m_vx[j] = random(MAX_VELOCITY, MIN_VELOCITY);
			if(RandomBoolean())
				m_vx[j] = -m_vx[j];
			m_vy[j] = random(MAX_VELOCITY, MIN_VELOCITY);
		}

		if(m_quad[0].pt[j].y >= GetWndHeight())
		{
			m_quad[0].pt[j].y = GetWndHeight() - 1;

			m_vx[j] = random(MAX_VELOCITY, MIN_VELOCITY);
			if(RandomBoolean())
				m_vx[j] = -m_vx[j];
			m_vy[j] = - random(MAX_VELOCITY, MIN_VELOCITY);
		}
	}
}

void CCornerEffect::ColorStep()
{
	int r = (m_color & 0xFF);
	int g = ((m_color >> 8) & 0xFF);
	int b = ((m_color >> 16) & 0xFF);

	r += random(MAX_COLOR_CHANGE+1, -MAX_COLOR_CHANGE);
	g += random(MAX_COLOR_CHANGE+1, -MAX_COLOR_CHANGE);
	b += random(MAX_COLOR_CHANGE+1, -MAX_COLOR_CHANGE);

	if(r < 0) r = 0;
	if(r >= 256) r = 255;
	if(g < 0) g = 0;
	if(g >= 256) g = 255;
	if(b < 0) b = 0;
	if(b >= 256) b = 255;

	m_color = RGB(r, g, b);
}
