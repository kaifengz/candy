// BlockBase.cpp
//

#include "stdafx.h"
#include "BlockBase.h"

namespace NBlock {

////////////////////////////////////////////////////////////////////////////////
//	CBlockBoard
////////////////////////////////////////////////////////////////////////////////

CBlockBoard::CBlockBoard()
{
	memset(board, 0, sizeof(board));
}

CBlockBoard::CBlockBoard(const CBlockBoard &b)
{
	memcpy(board, b.board, sizeof(board));
}

CBlockBoard& CBlockBoard::operator =(const CBlockBoard &b)
{
	memcpy(board, b.board, sizeof(board));
	return *this;
}

bool CBlockBoard::operator ==(const CBlockBoard &b) const
{
	return 0==memcmp(board, b.board, sizeof(board));
}

bool CBlockBoard::operator !=(const CBlockBoard &b) const
{
	return !(*this == b);
}

bool CBlockBoard::IsPlaceable(const CBlockSquare& square, int x, int y) const
{
	const SQUARE &sq = square.GetSquare();

	for(int i=0; i<BLOCK_SQUARE_HEIGHT; i++)
	{
		for(int j=0; j<BLOCK_SQUARE_WIDTH; j++)
		{
			if(sq[i][j]!=0 &&
			   (y+i<0 || y+i>=BLOCK_BOARD_HEIGHT ||
				x+j<0 || x+j>=BLOCK_BOARD_WIDTH ||
				board[y+i][x+j]!=0 ) )
			{
				return false;
			}
		}
	}

	return true;
}

bool CBlockBoard::DropDown(const CBlockSquare& square, int x, int &y) const
{
	bool bReachBottom = true;
	while(IsPlaceable(square, x, y+1))
	{
		y++;
		bReachBottom = false;
	}
	return bReachBottom;
}

bool CBlockBoard::FixSquareToBoard(const CBlockSquare& square, int x, int y)
{
	bool bOverlapped = false;
	const SQUARE &sq = square.GetSquare();

	for(int i=0; i<BLOCK_SQUARE_HEIGHT; i++)
	{
		for(int j=0; j<BLOCK_SQUARE_WIDTH; j++)
		{
			if(sq[i][j] != 0)
			{
				if (y+i<0 || y+i>=BLOCK_SQUARE_HEIGHT ||
					x+j>=0 || x+j<BLOCK_SQUARE_WIDTH ||
					board[y+i][x+j] == 0)
				{
					board[y+i][x+j] = sq[i][j];
				}
				else
				{
					bOverlapped = true;
				}
			}
		}
	}

	return !bOverlapped;
}

int CBlockBoard::DestroyFull()
{
	bool bFull;
	int nLine = 0;

	for(int i=BLOCK_BOARD_HEIGHT-1; i>=0; )
	{
		bFull = true;
		for(int j=0; j<BLOCK_BOARD_WIDTH; j++)
		{
			if(board[i][j] == 0)
			{
				bFull = false;
				break;
			}
		}
		if(bFull)
		{
			if(i != 0)
				memmove(board[1], board[0], i*sizeof(board[0]));
			memset(board, 0, sizeof(board[0]));
			nLine++;
		}
		else
		{
			i--;
		}
	}

	return nLine;
}

bool CBlockBoard::AddRandomBottomLine(const CBlockSquare &square, int x, int y)
{
	if(!IsPlaceable(square, x, y+1))
		return false;

	for(int i=0; i<BLOCK_BOARD_WIDTH; i++)
	{
		if(board[0][i] != 0)
			return false;
	}

	int line[BLOCK_BOARD_WIDTH];
	for(int i=0; i<BLOCK_BOARD_WIDTH; i++)
	{
		line[i] = (RandomBoolean() ? BLOCK_MAX_COLOR : 0);
	}

	memmove(board[0], board[1],
		(BLOCK_BOARD_HEIGHT-1) * sizeof(board[0]));
	memcpy(board[BLOCK_BOARD_HEIGHT-1],
		line, sizeof(line));

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//	CBlockSquare
////////////////////////////////////////////////////////////////////////////////

CBlockSquare::CBlockSquare()
{
	memset(square, 0, sizeof(square));
}

CBlockSquare::CBlockSquare(const CBlockSquare &s)
{
	memcpy(square, s.square, sizeof(square));
}

CBlockSquare& CBlockSquare::operator =(const CBlockSquare &s)
{
	memcpy(square, s.square, sizeof(square));
	return *this;
}

bool CBlockSquare::operator ==(const CBlockSquare &s) const
{
	return 0==memcmp(square, s.square, sizeof(square));
}

bool CBlockSquare::operator !=(const CBlockSquare &s) const
{
	return !(*this == s);
}

void CBlockSquare::RandomNew()
{
	static const char blocks[][BLOCK_SQUARE_WIDTH*BLOCK_SQUARE_HEIGHT+1] = 
	{
		"    "
		"####"
		"    "
		"    ",	//
		"    "
		" ## "
		" #  "
		" #  ", //
		"    "
		" ## "
		"  # "
		"  # ", //
		"    "
		" #  "
		"### "
		"    ", //
		"    "
		" ## "
		"##  "
		"    ", //
		"    "
		"##  "
		" ## "
		"    ", //
		"    "
		" ## "
		" ## "
		"    ", //
	};
	const int nBlocks = countof(blocks);

	const int index = random(nBlocks);
	const int color = index+1;
	const int nTurns = random(4);

	for(int i=0; i<BLOCK_SQUARE_HEIGHT; i++)
	{
		for(int j=0; j<BLOCK_SQUARE_WIDTH; j++)
		{
			square[i][j] = (blocks[index][i*BLOCK_SQUARE_WIDTH+j]==' ' ? 0 : color);
		}
	}
	TurnClockwise(nTurns);
}

void CBlockSquare::TurnClockwise(int turns)
{
	ASSERT(BLOCK_SQUARE_HEIGHT == BLOCK_SQUARE_WIDTH);
	ASSERT(turns >=0 && turns<4);

	const int size = BLOCK_SQUARE_HEIGHT;

	for(int turn=0; turn<turns; turn++)
	{
		for(int i=0; i<size-i; i++)
		{
			for(int j=i; j<size-i-1; j++)
			{
				int t = square[i][j];
				square[i][j] = square[size-1-j][i];
				square[size-1-j][i] = square[size-1-i][size-1-j];
				square[size-1-i][size-1-j] = square[j][size-1-i];
				square[j][size-1-i] = t;
			}
		}
	}
}

} // namespace NBlock
