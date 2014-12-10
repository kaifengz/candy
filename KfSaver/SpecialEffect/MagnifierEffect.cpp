// CMagnifierEffect
//

#include "stdafx.h"
#include "MagnifierEffect.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CMagnifierEffect);

#define MAX_VELOCITY			7 // 9 // 
#define FRAME_COUNT				MAX_FRAME_NUM // 100 // 

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
	m_nRadius = random(GetWndHeight()/5, GetWndHeight()/10); // �Ŵ󾵰뾶����Ļ�߶ȵ�1/10��1/5��

	m_x = random(GetWndWidth()-m_nRadius, m_nRadius);
	m_y = random(GetWndHeight()-m_nRadius, m_nRadius);
	m_vx = random(MAX_VELOCITY+1, -MAX_VELOCITY);
	m_vy = random(MAX_VELOCITY+1, -MAX_VELOCITY);

    return TRUE;
}

BOOL CMagnifierEffect::OnTimer(HDC hDC)
{
	if(GetFrameCount() != 0)
	{
		// ���㵱ǰλ��
		m_vx += random(2, -1);
		m_vy += random(2, -1);

		if(m_vx < -MAX_VELOCITY)
			m_vx = -MAX_VELOCITY;
		else if(m_vx > MAX_VELOCITY)
			m_vx = MAX_VELOCITY;

		if(m_vy < -MAX_VELOCITY)
			m_vy = -MAX_VELOCITY;
		else if(m_vy > MAX_VELOCITY)
			m_vy = MAX_VELOCITY;

		m_x += m_vx;
		m_y += m_vy;

		if(m_x < m_nRadius)
		{
			m_x = m_nRadius;
			m_vx = 0;
		}
		else if(m_x > GetWndWidth() - m_nRadius)
		{
			m_x = GetWndWidth() - m_nRadius;
			m_vx = 0;
		}

		if(m_y < m_nRadius)
		{
			m_y = m_nRadius;
			m_vy = 0;
		}
		else if(m_y > GetWndHeight() - m_nRadius)
		{
			m_y = GetWndHeight() - m_nRadius;
			m_vy = 0;
		}
	}

	// ��������ͼ��
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, GetBkgndBmp());

	BitBlt(
		hDC,
		m_x - m_nRadius,
		m_y - m_nRadius,
		2 * m_nRadius,
		2 * m_nRadius,
		hDCMem,
		m_x - m_nRadius,
		m_y - m_nRadius,
		SRCCOPY);

	// ���Ŵ��ͼ���޼���Բ��
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

	// �ͷŶ���
	SelectObject(hDCMem, hOldPen);
	SelectObject(hDCMem, hOldBrush);
	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);
	DeleteObject(hBitmap);

	return GetFrameCount() < FRAME_COUNT;
}

BOOL CMagnifierEffect::MagicHong(HDC hDC)
{
	if(GetFrameCount() != 0)
    {
        // �ǵ�һ�Σ����㵱ǰλ��
		m_vx += random(2, -1);
		m_vy += random(2, -1);

		if(m_vx < -MAX_VELOCITY)
			m_vx = -MAX_VELOCITY;
		else if(m_vx > MAX_VELOCITY)
			m_vx = MAX_VELOCITY;

		if(m_vy < -MAX_VELOCITY)
			m_vy = -MAX_VELOCITY;
		else if(m_vy > MAX_VELOCITY)
			m_vy = MAX_VELOCITY;

		m_x += m_vx;
		m_y += m_vy;

		if(m_x < m_nRadius)
		{
			m_x = m_nRadius;
			m_vx = 0;
		}
		else if(m_x > GetWndWidth() - m_nRadius)
		{
			m_x = GetWndWidth() - m_nRadius;
			m_vx = 0;
		}

		if(m_y < m_nRadius)
		{
			m_y = m_nRadius;
			m_vy = 0;
		}
		else if(m_y > GetWndHeight() - m_nRadius)
		{
			m_y = GetWndHeight() - m_nRadius;
			m_vy = 0;
		}
	}

	// ��������ͼ��
	HDC hDCMem = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCMem, GetBkgndBmp());

	BitBlt(
		hDC,
		m_x - m_nRadius,
		m_y - m_nRadius,
		2 * m_nRadius,
		3 * m_nRadius,
		hDCMem,
		m_x - m_nRadius,
		m_y - m_nRadius,
		SRCCOPY);

	// ���Ŵ��ͼ���޼�������
	HBITMAP hBitmap = CreateCompatibleBitmap(hDCMem,
		2 * m_nRadius, 3 * m_nRadius);
	SelectObject(hDCMem, hBitmap);

	HPEN hOldPen = (HPEN)SelectObject(hDCMem, GetStockObject(WHITE_PEN));
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDCMem, GetStockObject(WHITE_BRUSH));

	{	// ������
		int r = m_nRadius/2;
#define HEIGHT	(r*4/3)
		POINT pt[] = 
		{
			{0, HEIGHT},
			{0,	0},		{r*3/2, 0},	{r*2, HEIGHT},	// ���ε�����ϲ���
			{r*5/2, 0},	{r*4, 0},	{r*4, HEIGHT},	// ���ε��ұ��ϲ���
			{r*4, r*2},	{r*3, r*3},	{r*2, r*4},		// ���ε��ұ��²���
			{r, r*3},	{0, r*2},	{0, HEIGHT},	// ���ε�����²���
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

	// �ͷŶ���
	SelectObject(hDCMem, hOldPen);
	SelectObject(hDCMem, hOldBrush);
	SelectObject(hDCMem, hOldBmp);
	DeleteDC(hDCMem);
	DeleteObject(hBitmap);

	return GetFrameCount() < FRAME_COUNT;
}