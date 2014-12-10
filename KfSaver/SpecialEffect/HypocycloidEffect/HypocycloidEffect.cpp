// HypocycloidEffect.cpp
//


#include "stdafx.h"
#include "HypocycloidEffect.h"
#include "EffectRegister.h"
#include "ZkfGraphics.h"

REGISTER_EFFECT(CHypocycloidEffect);

#define HYPOCYCLOID_TIMESLICE               30
#define HYPOCYCLOID_MAX_FRAME_NUM           3000

#define HYPOCYCLOID_NEW_THRESHOLD           4
#define HYPOCYCLOID_HIGH_NEW_WIDGET_PROB    0.01
#define HYPOCYCLOID_LOW_NEW_WIDGET_PROB     0.003

#define HYPOCYCLOID_NEW_WIDGET_MAX_TRY      128
#define HYPOCYCLOID_RANDOM_COLOR()          RGB(random(64,256), random(64,256), random(64,256))
#define HYPOCYCLOID_MAX_AGE                 1500
#define HYPOCYCLOID_MIN_AGE                 600
#define HYPOCYCLOID_DYING_LASTED            100
#define HYPOCYCLOID_MAX_SIZE                0.5
#define HYPOCYCLOID_MIN_SIZE                0.2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHypocycloidEffect::CHypocycloidEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
    CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
}

CHypocycloidEffect::~CHypocycloidEffect()
{
}

const char* CHypocycloidEffect::GetEffectName() const
{
    return "HypocycloidEffect";
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CHypocycloidEffect::Initialize(HDC hDC, BOOL)
{
	return TRUE;
}

BOOL CHypocycloidEffect::OnTimer(HDC hDC)
{
	unsigned int live_count = 0;
	for (unsigned int i=0; i<m_widgets.size(); ++i)
		if (m_widgets[i].alive)
			++live_count;
	const double new_widget_probability = (live_count==0 ? 1.0 :
		(live_count < HYPOCYCLOID_NEW_THRESHOLD ? HYPOCYCLOID_HIGH_NEW_WIDGET_PROB : HYPOCYCLOID_LOW_NEW_WIDGET_PROB));
	if (random(1.0) < new_widget_probability)
		NewWidget();

	for (unsigned int i=0; i<m_widgets.size(); ++i)
		DrawWidget(hDC, m_widgets[i]);

	return (GetFrameCount() < HYPOCYCLOID_MAX_FRAME_NUM);
}

DWORD CHypocycloidEffect::GetTimeslice() const
{
	return HYPOCYCLOID_TIMESLICE;
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

void CHypocycloidEffect::NewWidget()
{
	ZWidget widget;
	widget.alive = true;
	widget.birth = GetFrameCount();
	widget.dead = widget.birth + random(HYPOCYCLOID_MAX_AGE, HYPOCYCLOID_MIN_AGE);
	widget.dying = widget.dead - HYPOCYCLOID_DYING_LASTED;
	widget.color = HYPOCYCLOID_RANDOM_COLOR();

	const unsigned int width = GetWndWidth();
	const unsigned int height = GetWndHeight();
	const unsigned int max_size = min(width, height);
	for (int i=0; i<HYPOCYCLOID_NEW_WIDGET_MAX_TRY; ++i)
	{
		const unsigned int size = RoundToInt(random(max_size*HYPOCYCLOID_MAX_SIZE, max_size*HYPOCYCLOID_MIN_SIZE));
		const unsigned int left = random(width - size);
		const unsigned int top  = random(height - size);
		bool overlap = false;
		for (unsigned int j = 0; j<m_widgets.size(); ++j)
		{
			if (m_widgets[j].alive &&
				left <= m_widgets[j].left + m_widgets[j].size &&
				left + size >= m_widgets[j].left &&
				top <= m_widgets[j].top + m_widgets[j].size &&
				top + size >= m_widgets[j].top)
			{
				overlap = true;
				break;
			}
		}
		if (!overlap)
		{
			widget.left = left;
			widget.top  = top;
			widget.size = size;
			widget.widget.Initialize(widget.left, widget.top, widget.size);
			m_widgets.push_back(widget);
			break;
		}
	}
}

void CHypocycloidEffect::DrawWidget(HDC hDC, ZWidget &widget)
{
	COLORREF color;

	const unsigned int frame_count = GetFrameCount();
	if (frame_count >= widget.dead)
	{
		widget.alive = false;
		return;
	}

	if (frame_count >= widget.dying)
	{
		double rate = 1.0 * (widget.dead - frame_count) / (widget.dead - widget.dying);
#define FADE(x)   RoundToInt(rate*(x))
		color = RGB(FADE(GetRValue(widget.color)), FADE(GetGValue(widget.color)), FADE(GetBValue(widget.color)));
	}
	else
		color = widget.color;


	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	widget.widget.Step();
	widget.widget.Draw(hDC);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}
