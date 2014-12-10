// Othello.cpp

#include "StdAfx.h"
#include "Othello.h"

#define SEARCH_DEPTH         6

namespace NOthello {

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

COthello::COthello()
{
}

BOOL COthello::Initialize()
{
	m_board.SetInitState();
	return TRUE;
}

BOOL COthello::Next()
{
	Movement movement;

	COthelloSearchNode node(m_board);
	if (!node.GetBestMovement(movement, SEARCH_DEPTH))
	{
		m_board.SkipOne();
		COthelloSearchNode node(m_board);
		if (!node.GetBestMovement(movement, SEARCH_DEPTH))
			return FALSE;
	}

	m_board.Set(movement.x, movement.y, movement.cell);
	return TRUE;
}

const COthelloBoard& COthello::GetBoard() const
{
	return m_board;
}

}; // namespace NOthello
