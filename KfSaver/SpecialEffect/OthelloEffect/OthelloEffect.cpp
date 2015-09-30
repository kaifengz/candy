// OthelloEffect.cpp

#include "StdAfx.h"
#include "OthelloEffect.h"
#include "Othello.h"
#include "EffectRegister.h"
#include "ZkfGraphics.h"

REGISTER_EFFECT(COthelloEffect);

#define OTHELLO_CELL_MARGIN            6
#define OTHELLO_WHITE_COLOR            RGB(160,160,160)
#define OTHELLO_BLACK_COLOR            RGB( 64, 64, 64)
#define OTHELLO_GRID_COLOR             RGB( 96, 96, 96)

#define OTHELLO_MAX_SWITCH             32 // must be an even
#define OTHELLO_MAX_WAIT               96
#define OTHELLO_MAX_TERMINATE          256

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COthelloEffect::COthelloEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_pOthello = new NOthello::COthello;
	m_state = WAITING;
	m_nCount = 0;
	m_pPrevBoard = new NOthello::COthelloBoard;
	m_pCurrBoard = new NOthello::COthelloBoard;
}

COthelloEffect::~COthelloEffect()
{
	if (m_pOthello != NULL)
		delete m_pOthello;
	if (m_pPrevBoard != NULL)
		delete m_pPrevBoard;
	if (m_pCurrBoard != NULL)
		delete m_pCurrBoard;
}

const char* COthelloEffect::GetEffectName() const
{
	return "OthelloEffect";
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

BOOL COthelloEffect::Initialize(HDC, BOOL)
{
	m_pOthello->Initialize();
	m_state = WAITING;
	m_nCount = 0;
	*m_pCurrBoard = m_pOthello->GetBoard();
	return TRUE;
}

BOOL COthelloEffect::OnTimer(HDC hDC)
{
	switch (m_state)
	{
	case WAITING:
		if (++m_nCount <= OTHELLO_MAX_WAIT)
			break;

		m_state = THINKING;
		// no break here

	case THINKING:
		*m_pPrevBoard = *m_pCurrBoard;
		if (GetFrameCount() != 0 && !m_pOthello->Next())
		{
			m_state = TERMINATED;
			m_nCount = 0;
			break;
		}

		*m_pCurrBoard = m_pOthello->GetBoard();
		m_state = SWITCHING;
		m_nCount = 0;
		// no break here

	case SWITCHING:
		if (++m_nCount <= OTHELLO_MAX_SWITCH)
			break;

		m_state = WAITING;
		m_nCount = 0;

	case TERMINATED:
		if (++m_nCount <= OTHELLO_MAX_TERMINATE)
			break;
		return FALSE;
	}

	DrawBoard(hDC);
	return TRUE;
}

void COthelloEffect::DrawBoard(HDC hDC)
{
	const RECT &rect     = GetClientArea();
	const int wnd_width  = rect.right - rect.left;
	const int wnd_height = rect.bottom - rect.top;
	const int cell_count = NOthello::COthelloBoard::SIZE;
	const int cell_size  = min(wnd_width, wnd_height) / (cell_count + 1);
	const int offset_x   = (rect.left + rect.right - cell_size * cell_count) / 2;
	const int offset_y   = (rect.top + rect.bottom - cell_size * cell_count) / 2;

	HPEN hGridPen  = CreatePen(PS_SOLID, 1, OTHELLO_GRID_COLOR);
	HPEN hWhitePen = CreatePen(PS_SOLID, 1, OTHELLO_WHITE_COLOR);
	HPEN hBlackPen = CreatePen(PS_SOLID, 1, OTHELLO_BLACK_COLOR);
	HBRUSH hWhiteBrush = CreateSolidBrush(OTHELLO_WHITE_COLOR);
	HBRUSH hBlackBrush = CreateSolidBrush(OTHELLO_BLACK_COLOR);

	HPEN hOldPen = (HPEN)SelectObject(hDC, hGridPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hWhiteBrush);

	for (int n=0; n<=cell_count; ++n)
	{
		MoveToEx(hDC, offset_x, offset_y + cell_size * n, NULL);
		LineTo(hDC, offset_x + cell_size * cell_count, offset_y + cell_size * n);
		MoveToEx(hDC, offset_x + cell_size * n, offset_y, NULL);
		LineTo(hDC, offset_x + cell_size * n, offset_y + cell_size * cell_count);
	}

	for (int x=0; x<cell_count; ++x)
	{
		for (int y=0; y<cell_count; ++y)
		{
			const NOthello::COthelloBoard::Cell prev = m_pPrevBoard->Get(x, y);
			const NOthello::COthelloBoard::Cell curr = m_pCurrBoard->Get(x, y);

			if (curr == NOthello::COthelloBoard::NONE)
				continue;

			HPEN hPen = (curr == NOthello::COthelloBoard::BLACK ?
					hBlackPen : hWhitePen);
			HBRUSH hBrush = (curr == NOthello::COthelloBoard::BLACK ?
					hBlackBrush : hWhiteBrush);
			int y_margin_rate = 0;
			if (m_state == SWITCHING)
			{
				if (prev == NOthello::COthelloBoard::NONE || prev == curr)
					y_margin_rate = 0;
				else if (m_nCount < OTHELLO_MAX_SWITCH/2)
				{
					y_margin_rate = m_nCount - 1;
					hPen = (curr == NOthello::COthelloBoard::BLACK ?
							hWhitePen : hBlackPen);
					hBrush = (curr == NOthello::COthelloBoard::BLACK ?
							hWhiteBrush : hBlackBrush);
				}
				else
					y_margin_rate = OTHELLO_MAX_SWITCH - m_nCount;
			}
			else
				y_margin_rate = 0;

			const int x_margin = 6;
			const int y_margin = 6 + RoundToInt(1.0 * (cell_size - 12) *
					y_margin_rate / OTHELLO_MAX_SWITCH);

			SelectObject(hDC, hPen);
			SelectObject(hDC, hBrush);
			Ellipse(hDC,
				offset_x + cell_size * x + x_margin,
				offset_y + cell_size * y + y_margin,
				offset_x + cell_size * (x+1) - x_margin,
				offset_y + cell_size * (y+1) - y_margin);
		}
	}

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);

	DeleteObject(hGridPen);
	DeleteObject(hWhitePen);
	DeleteObject(hBlackPen);
	DeleteObject(hWhiteBrush);
	DeleteObject(hBlackBrush);
}
