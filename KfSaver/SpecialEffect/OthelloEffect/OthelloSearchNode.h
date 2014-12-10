
#ifndef _OTHELLO_SEARCH_NODE_H__INCLUDED__2011_09_18_
#define _OTHELLO_SEARCH_NODE_H__INCLUDED__2011_09_18_

#include "OthelloBoard.h"

namespace NOthello {

///////////////////////////////////////////////////////////////////////////////
//	Movement
///////////////////////////////////////////////////////////////////////////////

class Movement
{
public:
	int x, y;
	COthelloBoard::Cell cell;

	Movement() : x(0), y(0), cell(COthelloBoard::NONE) {}
	Movement(int x_, int y_, COthelloBoard::Cell cell_) : x(x_), y(y_), cell(cell_) {}
	Movement(const Movement &m) : x(m.x), y(m.y), cell(m.cell) {}

	bool operator< (const Movement &m) const
	{
		return (x != m.x ? x < m.x :
			(y != m.y ? y < m.y : cell < m.cell));
	}
};

class COthelloSearchNode
{
public:
	COthelloSearchNode(const COthelloBoard &board);
	~COthelloSearchNode();

public:
	bool operator< (const COthelloSearchNode &node) const
	{
		return m_board < node.m_board;
	}

public:
	bool GetBestMovement(Movement &movement, int depth);

protected:
	bool GetBestMovement(Movement &movement, int &goodness, int depth, const int *cut_hint);
	int EstimateGoodness();

private:
	COthelloBoard m_board;
};

}; // namespace NOthello

#endif // _OTHELLO_SEARCH_NODE_H__INCLUDED__2011_09_18_
