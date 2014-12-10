
#include "stdafx.h"
#include "OthelloSearchNode.h"
#include "EstimatorRegistry.h"

template <int (*function)(const COthelloBoard &)>
class SimpleEstimator : public IEstimator
{
public:
	virtual int operator() (const COthelloBoard &board)
	{
		return function(board);
	}
	static IEstimator* Creator()
	{
		return new SimpleEstimator();
	}
};

#define DEFINE_ESTIMATOR(name) \
	int name(const COthelloBoard &board); \
	static EstimatorRegister R_ ## name(#name, SimpleEstimator<name>::Creator); \
	int name(const COthelloBoard &board)

DEFINE_ESTIMATOR(E0)
{
	int diff = 0;
	int total = 0;
	for (int x=0; x<COthelloBoard::SIZE; ++x)
	{
		for (int y=0; y<COthelloBoard::SIZE; ++y)
		{
			switch(board.Get(x, y))
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

	static const struct {
		int x, y;
	} corners[] = {
		{0, 0},
		{0, COthelloBoard::SIZE-1},
		{COthelloBoard::SIZE-1, 0},
		{COthelloBoard::SIZE-1, COthelloBoard::SIZE-1},
	};
	int corner_diff = 0;
	for (int c=0; c<countof(corners); ++c)
	{
		switch (board.Get(corners[c].x, corners[c].y))
		{
		case COthelloBoard::BLACK:
			++corner_diff;
			break;
		case COthelloBoard::WHITE:
			--corner_diff;
			break;
		}
	}
	int corner_goodness = (corner_diff >= 0 ? 1 : -1) * min(
		abs(10 * corner_diff), (COthelloBoard::SIZE * COthelloBoard::SIZE - total) / 2);

	return diff + corner_goodness;
}

DEFINE_ESTIMATOR(E1)
{
	int diff = 0;
	int total = 0;
	for (int x=0; x<COthelloBoard::SIZE; ++x)
	{
		for (int y=0; y<COthelloBoard::SIZE; ++y)
		{
			switch(board.Get(x, y))
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
		if (board.Get(corner.x, corner.y) == COthelloBoard::NONE)
		{
			score += CELL_TO_SCORE(board.Get(corner.x, corner.y + corner.oy),
							-1, +1, 0);
			score += CELL_TO_SCORE(board.Get(corner.x + corner.ox, corner.y),
							-1, +1, 0);
			score += CELL_TO_SCORE(board.Get(corner.x + corner.ox, corner.y + corner.oy),
							-5, +5, 0);
		}
		else
			score += CELL_TO_SCORE(board.Get(corner.x, corner.y), +10, -10, 0);
		corner_score += score;
	}
	corner_score = (corner_score >= 0 ? 1 : -1) * min(
		abs(corner_score), (COthelloBoard::SIZE * COthelloBoard::SIZE - total) / 2);

	return diff + corner_score;
}

static int safe_count(const COthelloBoard &board, COthelloBoard::Cell cell)
{
	int safe_dir[8][8];
	memset(safe_dir, 0, sizeof(safe_dir));

	static const struct
	{
		int ox, oy;
	} directions[] =
	{
		{0, 1},
		{1, 0},
		{1, 1},
		{1, -1},
	};

	for (int d=0; d<countof(directions); ++d)
	{
		const int ox = directions[d].ox;
		const int oy = directions[d].oy;

		for (int x=0; x<8; ++x)
		{
			for (int y=0; y<8; ++y)
			{
				if (board.Get(x, y) != cell)
					continue;

				int x1=x+ox, y1=y+oy;
				while (x1>=0 && x1<8 && y1>=0 && y1<8 && board.Get(x1, y1) == cell)
				{
					x1 += ox;
					y1 += oy;
				}
				if (!(x1>=0 && x1<8 && y1>=0 && y1<8))
					++safe_dir[x][y];
				else
				{
					int x2=x-ox, y2=y-oy;
					while (x2>=0 && x2<8 && y2>=0 && y2<8 && board.Get(x2, y2) == cell)
					{
						x2 -= ox;
						y2 -= oy;
					}
					if (!(x1>=0 && x1<8 && y1>=0 && y1<8))
						++safe_dir[x][y];
				}
			}
		}
	}

	int score = 0;
	for (int x=0; x<8; ++x)
	{
		for (int y=0; y<8; ++y)
		{
			if (safe_dir[x][y] >= 4)
				score += 800;
			else if (safe_dir[x][y] >= 3)
				score += 40;
			else if (board.Get(x, y) == cell)
				score += 2;
		}
	}

	return score;
}

DEFINE_ESTIMATOR(E2)
{
	int black = safe_count(board, COthelloBoard::BLACK);
	int white = safe_count(board, COthelloBoard::WHITE);
	return black - white;
}
