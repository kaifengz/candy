
#ifndef _BLOCK_BASE_H__INCLUDED__2007_06_27_
#define _BLOCK_BASE_H__INCLUDED__2007_06_27_

namespace NBlock {

class CBlockBoard;
class CBlockSquare;
enum ZBlockAction;
typedef ZBlockAction ZBA;

#define BLOCK_MAX_COLOR			8
#define BLOCK_BOARD_WIDTH		10
#define BLOCK_BOARD_HEIGHT		20
#define BLOCK_SQUARE_WIDTH		4
#define BLOCK_SQUARE_HEIGHT		4

typedef int BOARD[BLOCK_BOARD_HEIGHT][BLOCK_BOARD_WIDTH];
typedef int SQUARE[BLOCK_SQUARE_HEIGHT][BLOCK_SQUARE_WIDTH];

class CBlockBoard
{
public:
	CBlockBoard();
	CBlockBoard(const CBlockBoard&);

	CBlockBoard& operator =(const CBlockBoard&);
	bool operator ==(const CBlockBoard&) const;
	bool operator !=(const CBlockBoard&) const;

	bool IsPlaceable(const CBlockSquare&, int x, int y) const;
	bool DropDown(const CBlockSquare&, int x, int &y) const;
	bool FixSquareToBoard(const CBlockSquare&, int x, int y);
	int DestroyFull();
	bool AddRandomBottomLine(const CBlockSquare&, int x, int y);

	const BOARD& GetBoard() const { return board; }

protected:
	BOARD board;
};

class CBlockSquare
{
public:
	CBlockSquare();
	CBlockSquare(const CBlockSquare&);

	CBlockSquare& operator =(const CBlockSquare&);
	bool operator ==(const CBlockSquare&) const;
	bool operator !=(const CBlockSquare&) const;

	void RandomNew();
	void TurnClockwise(int turns=1);

	const SQUARE& GetSquare() const { return square; }

protected:
	SQUARE square;
};

enum ZBlockAction
{
	BA_NOP				= 0,
	BA_TURN_CLOCKWISE	= 1,
	BA_TURN				= 1,
	BA_MOVE_LEFT		= 2,
	BA_MOVE_RIGHT		= 3,
	BA_MOVE_DOWN		= 4,
	BA_DROP_DOWN		= 5,

	BA_MAX				= 6,
};

} // namespace NBlock

#endif // _BLOCK_BASE_H__INCLUDED__2007_06_27_
