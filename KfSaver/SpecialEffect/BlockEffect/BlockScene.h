
#ifndef _BLOCK_SCENE_H__INCLUDED__2007_06_27_
#define _BLOCK_SCENE_H__INCLUDED__2007_06_27_

#include "BlockBase.h"
#include "BlockAI.h"

namespace NBlock {


class CBlockScene
{
public:
	CBlockScene();
	~CBlockScene();

	BOOL Initialize(HDC);
	BOOL OnTimer();
	BOOL Draw(HDC, int width, int height);


private:
	enum ZBlockStatus
	{
		BS_PRESTART		= 0,
		BS_MOVE_DOWN	= 1,
		BS_DESTROY_FULL	= 2,
		BS_NEW_SQUARE	= 3,
	};
	typedef ZBlockStatus ZBS;

private:		// flow control
	int			m_iStepCount;	// 帧数，方块下落一格、销去若干行、产生新的方块，均为一帧
	ZBS			m_bs;

private:			// block board datas
	CBlockBoard		m_board;
	CBlockSquare	m_square;
	CBlockSquare	m_square2;
	int				m_x;
	int				m_y;

private:		// 
	HBITMAP		m_hBmp[1+BLOCK_MAX_COLOR];

private:		// 
	void		DrawBoard(HDC, int width, int height);

	void		LoadImages(HDC);
	void		ReleaseImages();

private:		// Block control function
	BOOL		TurnClockwise();
	BOOL		MoveLeft();
	BOOL		MoveRight();
	BOOL		MoveDown();
	BOOL		DropDown();
	BOOL		CanMoveDown();
	BOOL		FixSquareToBoard();
	BOOL		DestroyFull();
	BOOL		NewSquare();

	BOOL		AddRandomBottomLine();

private:		// AI functions
	CBlockAI	m_ai;
	ZBA			RunAI();
};

} // namespace NBlock

#endif // _BLOCK_SCENE_H__INCLUDED__2007_06_27_
