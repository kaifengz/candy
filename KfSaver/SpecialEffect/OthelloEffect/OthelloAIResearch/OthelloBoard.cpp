// OthelloBoard.cpp

#include "StdAfx.h"
#include "OthelloBoard.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

COthelloBoard::COthelloBoard()
{
	memset(m_cells, 0, sizeof(m_cells));
	m_next = BLACK;
}

COthelloBoard::COthelloBoard(const COthelloBoard &board)
{
	memcpy(m_cells, board.m_cells, sizeof(m_cells));
	m_next = board.m_next;
}

COthelloBoard::~COthelloBoard()
{
}

void COthelloBoard::SetInitState()
{
	memset(m_cells, 0, sizeof(m_cells));
	m_cells[3][3] = BLACK;
	m_cells[4][4] = BLACK;
	m_cells[3][4] = WHITE;
	m_cells[4][3] = WHITE;
	m_next = BLACK;
}

void COthelloBoard::SkipOne()
{
	m_next = (m_next == BLACK ? WHITE : BLACK);
}

COthelloBoard& COthelloBoard::operator= (const COthelloBoard &board)
{
	memcpy(m_cells, board.m_cells, sizeof(m_cells));
	m_next = board.m_next;
	return *this;
}

bool COthelloBoard::operator<  (const COthelloBoard &board) const
{
	return memcmp(m_cells, board.m_cells, sizeof(m_cells)) < 0;
}

bool COthelloBoard::operator== (const COthelloBoard &board) const
{
	return memcmp(m_cells, board.m_cells, sizeof(m_cells)) == 0;
}

bool COthelloBoard::Set(int x, int y, Cell cell)
{
	return TestSet(x, y, cell, true);
}

bool COthelloBoard::IsSettable(int x, int y, Cell cell)
{
	return TestSet(x, y, cell, false);
}

bool COthelloBoard::TestSet(int x, int y, Cell cell, bool really_set)
{
	if (m_cells[y][x] != NONE)
		return false;
	if (cell == NONE)
		return false;
	const Cell rcell = (cell == BLACK ? WHITE : BLACK);

	static const struct
	{
		int dx, dy;
	} directions [] =
	{
		{-1, -1}, { 0, -1}, {+1, -1},
		{-1,  0},           {+1,  0},
		{-1, +1}, { 0, +1}, {+1, +1},
	};

	bool placeable = false;
	for (int d=0; d<countof(directions); ++d)
	{
		const int dx = directions[d].dx;
		const int dy = directions[d].dy;
		int rcount = 0;
		int xx = x+dx, yy = y+dy;
		while (xx >= 0 && xx < SIZE && yy >= 0 && yy < SIZE &&
			m_cells[yy][xx] == rcell)
		{
			++rcount;
			xx += dx;
			yy += dy;
		}
		if (rcount > 0 &&
			xx >= 0 && xx < SIZE && yy >= 0 && yy < SIZE &&
			m_cells[yy][xx] == cell)
		{
			placeable = true;
			if (really_set)
			{
				for (int i=1; i<=rcount; ++i)
					m_cells[y + dy*i][x + dx*i] = cell;
			}
		}
	}
	if (placeable && really_set)
	{
		m_cells[y][x] = cell;
		m_next = (m_next == BLACK ? WHITE : BLACK);
	}
	return placeable;
}
