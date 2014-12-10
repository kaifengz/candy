// OthelloSearchNode.cpp

#include "StdAfx.h"
#include "OthelloSearchNode.h"

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

COthelloSearchNode::COthelloSearchNode(const COthelloBoard &board, IEstimator &estimator) :
	m_board(board), m_estimator(estimator)
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
	return m_estimator(m_board);
}
