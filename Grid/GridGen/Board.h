#pragma once

#include <assert.h>
#include <vector>
#include <utility>

typedef unsigned int uint;

class Board
{
public:

#define BOARD_DIMENSION		2
#if BOARD_DIMENSION == 1
	static const uint HEIGHT = 3;
	static const uint WIDTH = 3;
	static const uint MAX_STONES_PER_SIDE = 3;
#elif BOARD_DIMENSION == 2
	static const uint HEIGHT = 4;
	static const uint WIDTH = 3;
	static const uint MAX_STONES_PER_SIDE = 5;
#elif BOARD_DIMENSION == 3
	static const uint HEIGHT = 4;
	static const uint WIDTH = 4;
	static const uint MAX_STONES_PER_SIDE = 6;
#endif

	struct Position
	{
		uint x, y;
	};

	struct Move
	{
		Position start;
		uint direction;

		uint Encode() const;
		bool Decode(uint);
	};

	enum class Cell : unsigned char
	{
		None = 0,
		Black,
		White,
	};

	enum class Situation : unsigned char
	{
		Unclear = 0,
		Win,
		Lose,
	};

public:
	Board();

	~Board() = default;

	Board(const Board&) = default;
	Board& operator= (const Board&) = default;

public:
	void Reset()
	{
		memset(m_cells, 0, sizeof(m_cells));
	}

	Cell NextMove() const
	{
		return m_nextMove;
	}

	void Initialize();

	void Expand(
		std::vector<std::pair<Move, Board>>& expansions) const;

	uint Encode() const;

	bool Decode(uint);

	static Cell Reverse(Cell cell)
	{
		switch (cell)
		{
		case Cell::Black:	return Cell::White;
		case Cell::White:	return Cell::Black;
		case Cell::None:
		default:			return Cell::None;
		}
	}

private:
	Cell m_cells[HEIGHT][WIDTH];
	Cell m_nextMove;

	struct Direction
	{
		int dx, dy;
	};

	static const Direction ms_directions[4];
};
