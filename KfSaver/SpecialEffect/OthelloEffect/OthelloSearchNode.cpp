// OthelloSearchNode.cpp

#include "StdAfx.h"
#include "OthelloSearchNode.h"

namespace NOthello {

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

COthelloSearchNode::COthelloSearchNode(const COthelloBoard &board) :
	m_board(board)
{
}

COthelloSearchNode::~COthelloSearchNode()
{
}

bool COthelloSearchNode::GetBestMovement(Movement &movement, int depth)
{
	int goodness;
	return GetBestMovement(movement, goodness, depth, 0);
}

bool COthelloSearchNode::GetBestMovement(
		Movement &movement, int &goodness, int depth, const int *cut_hint)
{
	const COthelloBoard::Cell next = m_board.GetNext();
	const bool want_max = (next == COthelloBoard::BLACK ? true : false);

	int order[COthelloBoard::SIZE * COthelloBoard::SIZE];
	for (int i=0; i<countof(order); ++i)
		order[i] = i;
	for (int i=0; i<countof(order); ++i)
		swap(order[i], order[rand()%countof(order)]);

	bool first = true;
	for (int i=0; i<countof(order); ++i)
	{
		const int x = order[i] / COthelloBoard::SIZE;
		const int y = order[i] % COthelloBoard::SIZE;

		Movement m(x, y, next);
		COthelloSearchNode d(*this);
		if (!d.m_board.Set(m.x, m.y, m.cell))
			continue;
		int g;
		if (depth == 1)
			g = d.EstimateGoodness();
		else
		{
			Movement m2;
			int g2;
			if (d.GetBestMovement(m2, g2, depth-1, first ? NULL : &goodness))
				g = g2;
			else
				g = d.EstimateGoodness();
		}
		if (first)
		{
			movement = m;
			goodness = g;
			first = false;
		}
		else
		{
			if (want_max && g > goodness)
			{
				movement = m;
				goodness = g;
			}
			else if (!want_max && g < goodness)
			{
				movement = m;
				goodness = g;
			}
		}
		if (cut_hint != 0)
		{
			if (want_max && goodness >= *cut_hint)
				return true;
			else if (!want_max && goodness <= *cut_hint)
				return true;
		}
	}

	if (first)
		return false;
	return true;
}

int COthelloSearchNode::EstimateGoodness()
{
	int diff = 0;
	int total = 0;
	for (int x=0; x<COthelloBoard::SIZE; ++x)
	{
		for (int y=0; y<COthelloBoard::SIZE; ++y)
		{
			switch(m_board.Get(x, y))
			{
			case COthelloBoard::BLACK:
				++diff;
				++total;
				break;
			case COthelloBoard::WHITE:
				--diff;
				++total;
				break;
			}
		}
	}

	static const struct corner_t {
		int x, y, ox, oy;
	} corners[] = {
		{0,                     0,                     +1, +1},
		{0,                     COthelloBoard::SIZE-1, +1, -1},
		{COthelloBoard::SIZE-1, 0,                     -1, +1},
		{COthelloBoard::SIZE-1, COthelloBoard::SIZE-1, -1, -1},
	};
	int corner_score = 0;
	for (int c=0; c<countof(corners); ++c)
	{
#define CELL_TO_SCORE(cell, black, white, otherwise) \
	((cell) == COthelloBoard::BLACK ? (black) : ((cell) == COthelloBoard::WHITE ? (white) : (otherwise)))

		const corner_t &corner = corners[c];
		int score = 0;
		if (m_board.Get(corner.x, corner.y) == COthelloBoard::NONE)
		{
			score += CELL_TO_SCORE(m_board.Get(corner.x, corner.y + corner.oy),
							-1, +1, 0);
			score += CELL_TO_SCORE(m_board.Get(corner.x + corner.ox, corner.y),
							-1, +1, 0);
			score += CELL_TO_SCORE(m_board.Get(corner.x + corner.ox, corner.y + corner.oy),
							-5, +5, 0);
		}
		else
			score += CELL_TO_SCORE(m_board.Get(corner.x, corner.y), +10, -10, 0);
		corner_score += score;
	}
	corner_score = (corner_score >= 0 ? 1 : -1) * min(
		abs(corner_score), (COthelloBoard::SIZE * COthelloBoard::SIZE - total) / 2);

	return diff + corner_score;
}

}; // namespace NOthello
