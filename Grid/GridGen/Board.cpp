#include "Board.h"

const Board::Direction Board::ms_directions[] =
{
	{+1,  0},
	{ 0, +1},
	{-1,  0},
	{ 0, -1},
};

Board::Board()
{
	Reset();
}

void Board::Initialize()
{
	Reset();

#if BOARD_DIMENSION == 1
	m_cells[0][0] = m_cells[0][1] = m_cells[0][2] = Cell::White;
	m_cells[2][0] = m_cells[2][1] = m_cells[2][2] = Cell::Black;

#elif BOARD_DIMENSION == 2
	m_cells[0][0] = m_cells[0][1] = m_cells[0][2] = Cell::White;
	m_cells[1][0] = m_cells[1][2] = Cell::White;
	m_cells[2][0] = m_cells[2][2] = Cell::Black;
	m_cells[3][0] = m_cells[3][1] = m_cells[3][2] = Cell::Black;

#elif BOARD_DIMENSION == 3
	m_cells[0][0] = m_cells[0][1] = m_cells[0][2] = m_cells[0][3] = Cell::White;
	m_cells[1][0] = m_cells[1][3] = Cell::White;
	m_cells[2][0] = m_cells[2][3] = Cell::Black;
	m_cells[3][0] = m_cells[3][1] = m_cells[3][2] = m_cells[3][3] = Cell::Black;

#endif

	m_nextMove = Cell::Black;
}

void Board::Expand(
	std::vector<std::pair<Move, Board>>& expansions) const
{
	expansions.clear();
	expansions.reserve(MAX_STONES_PER_SIDE * 4);

	const Cell childNextMove = Reverse(m_nextMove);

	for (uint y = 0; y < HEIGHT; ++y)
	{
		for (uint x = 0; x < WIDTH; ++x)
		{
			if (m_cells[y][x] != m_nextMove)
				continue;

			for (uint dir = 0; dir < 4; ++dir)
			{
				const int x2 = x + ms_directions[dir].dx;
				const int y2 = y + ms_directions[dir].dy;
				if (x2 >= 0 && x2 < WIDTH && y2 >= 0 && y2 < HEIGHT && m_cells[y2][x2] == Cell::None)
				{
					expansions.resize(expansions.size() + 1);
					auto& exp = expansions.back();

					auto& move = exp.first;
					move.start = { x, y };
					move.direction = dir;

					auto& child = exp.second;
					child = *this;
					child.m_cells[y][x] = Cell::None;
					child.m_cells[y2][x2] = m_nextMove;
					child.m_nextMove = childNextMove;
					child.PerformAttacking(x2, y2);
				}
			}
		}
	}
}

uint Board::PerformAttacking(uint x, uint y)
{
	Cell self = Reverse(m_nextMove);
	verify(m_cells[y][x] == self);

	uint killed = 0;

	if (PerformAttacking(0, y, 1, 0, x, WIDTH))
	{
		++killed;
	}

	if (PerformAttacking(x, 0, 0, 1, y, HEIGHT))
	{
		++killed;
	}

	return killed;
}

bool PerformAttacking(uint x0, uint y0, uint dx, uint dy, uint self, uint max)
{
	verify(self >= 0 && self < max);

	// TODO
	return false;
}

uint Board::Encode() const
{
	uint n = 0;
	for (uint h = 0; h < HEIGHT; ++h)
	{
		for (uint w = 0; w < WIDTH; ++w)
		{
			n *= 3;
			n += static_cast<uint>(m_cells[h][w]);
		}
	}

	n |= (m_nextMove == Cell::White) ? 0x8000'0000 : 0x0;
	return n;
}

bool Board::Decode(uint n)
{
	m_nextMove = ((n & 0x8000'0000) != 0) ? Cell::White : Cell::Black;
	n &= 0x7fff'ffff;

	for (int h = HEIGHT - 1; h >= 0; --h)
	{
		for (int w = WIDTH - 1; w >= 0; --w)
		{
			m_cells[h][w] = static_cast<Cell>(n % 3);
			n /= 3;
		}
	}

	return (n == 0);
}
