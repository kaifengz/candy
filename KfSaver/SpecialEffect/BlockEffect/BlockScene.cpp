// BlockScene.cpp
//


// BUG: 方块落定后应该停顿一小时间才产生新的方块(2005-02-27)
// 图形太小，可考虑放大一倍						(2005-02-27)
// Bug fixed.									(2005-03-02)
// 图形已经放大至1.5倍							(2005-03-02)
// 智能有待提高									(2005-03-02)
// 修改势阱危险值的计算方法，智能大幅提高		(2005-03-03)
// 增加feature，底部插入随机行					(2005-03-04)


#include "stdafx.h"
#include "BlockScene.h"

namespace NBlock {

#define BLOCK_TIMESLICE_DOWN		200
#define BLOCK_TIMESLICE_AI			60
#define BLOCK_TIMESLICE_ADD_BOTTOM_MIN		9000
#define BLOCK_TIMESLICE_ADD_BOTTOM_MAX		29000

#define BLOCK_MAX_FRAME_NUM			500

#define BLOCK_SQUARE_BMP_SIZE		16
#define BLOCK_SQUARE_SIZE			24


#define SPACE_DANGER				200


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBlockScene::CBlockScene()
{
	memset(m_hBmp, 0, sizeof(m_hBmp));
}

CBlockScene::~CBlockScene()
{
	ReleaseImages();
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BOOL CBlockScene::Initialize(HDC hDC)
{
	m_iStepCount = 0;
	m_bs = BS_PRESTART;

	LoadImages(hDC);

	NewSquare();
	NewSquare();

	return TRUE;
}

BOOL CBlockScene::OnTimer()
{
	static int tLastFrame, tLastAI;
	static int tLastAddBottom, tAddBottomInterval;
	BOOL bNextFrame=FALSE, bAI=FALSE, bAddBottom=FALSE;

	if(m_iStepCount == 0)
	{
		tLastFrame = clock();
		tLastAI = clock();
		tLastAddBottom = clock();
		tAddBottomInterval = random(BLOCK_TIMESLICE_ADD_BOTTOM_MAX, BLOCK_TIMESLICE_ADD_BOTTOM_MIN);
		bNextFrame = FALSE; // Init() will create the new blocks, so don't move down
		bAI = TRUE;
		bAddBottom = FALSE;

		++m_iStepCount;
		m_bs = BS_MOVE_DOWN;
	}
	else
	{
		if(clock() - tLastFrame >= BLOCK_TIMESLICE_DOWN)
		{
			tLastFrame = clock();
			bNextFrame = TRUE;

			++m_iStepCount;
		}
		if(clock() - tLastAI >= BLOCK_TIMESLICE_AI)
		{
			tLastAI = clock();
			bAI = TRUE;
		}
		if(clock() - tLastAddBottom >= tAddBottomInterval)
		{
			bAddBottom = TRUE;
		}
	}

	if(bAddBottom && AddRandomBottomLine())
	{
		tLastAddBottom = clock();
		tAddBottomInterval = random(BLOCK_TIMESLICE_ADD_BOTTOM_MAX, BLOCK_TIMESLICE_ADD_BOTTOM_MIN);
	}
	else
	{
		bAddBottom = FALSE;
	}

	if(bAI && m_bs==BS_MOVE_DOWN)
	{
		switch(RunAI())
		{
		case BA_TURN_CLOCKWISE:	TurnClockwise();	break;
		case BA_MOVE_LEFT:		MoveLeft();			break;
		case BA_MOVE_RIGHT:		MoveRight();		break;
		case BA_MOVE_DOWN:		MoveDown();			break;
		case BA_DROP_DOWN:		DropDown();			break;
		}
	}

	if(m_bs==BS_MOVE_DOWN && !CanMoveDown())
	{
		if(!bNextFrame)
		{
			tLastFrame = clock();
			bNextFrame = TRUE;

			++m_iStepCount;
		}
	}

	if(bNextFrame)
	{
		// 控制规则：
		// 一旦方块触及底部，则一个时间片后进行销去
		// 若销去成功，则一个时间片后生成新的方块
		// 若销去不成功，则马上生成新的方块
		// 若生成新的方块成功，则一个时间片后开始下落
		// 若生成新的方块不成功，结束

		switch(m_bs)
		{
		case BS_MOVE_DOWN:
			if(MoveDown()) // 下移成功，一个时间片后继续下移
				;
			else // 下移不成功，则进行销去
			{
				FixSquareToBoard();
				m_bs = BS_DESTROY_FULL;
			}
			break;

		case BS_DESTROY_FULL:
			if(DestroyFull())
				// 销去成功，则等待一个时间片
				// 一个时间片后将再次进行销去，那时将会销去不成功
				break;
			else // 销去不成功，则生成新的方块
				m_bs = BS_NEW_SQUARE;

			// no break here
		case BS_NEW_SQUARE:
			if(!NewSquare()) // 无法生成新的方块（因为无法在顶部摆下此方块），游戏结束
				return FALSE;
			// 生成新方块成功，一个时间片后开始下移
			m_bs = BS_MOVE_DOWN;
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}

	return (m_iStepCount <= BLOCK_MAX_FRAME_NUM);
}

BOOL CBlockScene::Draw(HDC hDC, int width, int height)
{
	DrawBoard(hDC, width, height);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void CBlockScene::DrawBoard(HDC hDC, int width, int height)
{
	const int xBoard = (width - BLOCK_BOARD_WIDTH*BLOCK_SQUARE_SIZE)/2;
	const int yBoard = (height - BLOCK_BOARD_HEIGHT*BLOCK_SQUARE_SIZE)/2;
	const int xSquare = xBoard + m_x * BLOCK_SQUARE_SIZE;
	const int ySquare = yBoard + m_y * BLOCK_SQUARE_SIZE;
	const int xSquare2 = xBoard + BLOCK_BOARD_WIDTH*BLOCK_SQUARE_SIZE + BLOCK_SQUARE_SIZE;
	const int ySquare2 = yBoard;

	int i, j;

	HDC hDCTmp = CreateCompatibleDC(hDC);
	HBITMAP hOldBmpTmp = (HBITMAP)SelectObject(hDCTmp, m_hBmp[1]);

	// Draw Board
	const BOARD &board = m_board.GetBoard();
	for(i=0; i<BLOCK_BOARD_HEIGHT; i++)
	{
		for(j=0; j<BLOCK_BOARD_WIDTH; j++)
		{
			if(board[i][j] != 0)
			{
				SelectObject(hDCTmp, m_hBmp[board[i][j]]);
				BitBlt(hDC,
					xBoard + j * BLOCK_SQUARE_SIZE,
					yBoard + i * BLOCK_SQUARE_SIZE,
					BLOCK_SQUARE_SIZE, BLOCK_SQUARE_SIZE,
					hDCTmp, 0, 0, SRCCOPY);
			}
		}
	}

	// Draw Square
	const SQUARE &square = m_square.GetSquare();
	if(m_bs == BS_MOVE_DOWN)
	{
		for(i=0; i<BLOCK_SQUARE_HEIGHT; i++)
		{
			for(j=0; j<BLOCK_SQUARE_WIDTH; j++)
			{
				if(square[i][j] != 0)
				{
					SelectObject(hDCTmp, m_hBmp[square[i][j]]);
					BitBlt(hDC,
						xBoard + (m_x + j) * BLOCK_SQUARE_SIZE,
						yBoard + (m_y + i) * BLOCK_SQUARE_SIZE,
						BLOCK_SQUARE_SIZE, BLOCK_SQUARE_SIZE,
						hDCTmp, 0, 0, SRCCOPY);
				}
			}
		}
	}

	// Draw Square2
	const SQUARE &square2 = m_square2.GetSquare();
	for(i=0; i<BLOCK_SQUARE_HEIGHT; i++)
	{
		for(j=0; j<BLOCK_SQUARE_WIDTH; j++)
		{
			if(square2[i][j] != 0)
			{
				SelectObject(hDCTmp, m_hBmp[square2[i][j]]);
				BitBlt(hDC,
					xSquare2 + j * BLOCK_SQUARE_SIZE,
					ySquare2 + i * BLOCK_SQUARE_SIZE,
					BLOCK_SQUARE_SIZE, BLOCK_SQUARE_SIZE,
					hDCTmp, 0, 0, SRCCOPY);
			}
		}
	}

	{	// contour lines
		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
		HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

		const POINT ptBoard[] =
		{
			{xBoard-1, yBoard-1},
			{xBoard-1, yBoard + BLOCK_BOARD_HEIGHT*BLOCK_SQUARE_SIZE + 1},
			{xBoard + BLOCK_BOARD_WIDTH*BLOCK_SQUARE_SIZE + 1, yBoard + BLOCK_BOARD_HEIGHT*BLOCK_SQUARE_SIZE + 1},
			{xBoard + BLOCK_BOARD_WIDTH*BLOCK_SQUARE_SIZE + 1, yBoard-1},
			{xBoard-1, yBoard-1},
		};
		const POINT ptSquare2[] =
		{
			{xSquare2 - 1, ySquare2 - 1},
			{xSquare2 - 1, ySquare2 + BLOCK_SQUARE_HEIGHT*BLOCK_SQUARE_SIZE + 1},
			{xSquare2 + BLOCK_SQUARE_WIDTH*BLOCK_SQUARE_SIZE + 1, ySquare2 + BLOCK_SQUARE_HEIGHT*BLOCK_SQUARE_SIZE + 1},
			{xSquare2 + BLOCK_SQUARE_WIDTH*BLOCK_SQUARE_SIZE + 1, ySquare2 - 1},
			{xSquare2 - 1, ySquare2 - 1},
		};

		Polyline(hDC, ptBoard, countof(ptBoard));
		Polyline(hDC, ptSquare2, countof(ptSquare2));

		SelectObject(hDC, hOldPen);
		DeleteObject(hPen);
	}

	SelectObject(hDCTmp, hOldBmpTmp);
	DeleteDC(hDCTmp);
}

void CBlockScene::LoadImages(HDC hDC)
{
	ReleaseImages();

	m_hBmp[0] = NULL;

	for(int i=0; i<BLOCK_MAX_COLOR; i++)
	{
		m_hBmp[1+i] = LoadBmpFromSrc(hDC, IDBMP_BLOCK,
			0, i*BLOCK_SQUARE_BMP_SIZE, BLOCK_SQUARE_BMP_SIZE, BLOCK_SQUARE_BMP_SIZE,
			BLOCK_SQUARE_SIZE, BLOCK_SQUARE_SIZE);
	}
}

void CBlockScene::ReleaseImages()
{
	for(int i=0; i<countof(m_hBmp); i++)
	{
		HBITMAP &hBmp = m_hBmp[i];
		if(hBmp)
		{
			DeleteObject(hBmp);
			hBmp = NULL;
		}
	}
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BOOL CBlockScene::TurnClockwise()
{
	CBlockSquare square = m_square;
	square.TurnClockwise(1);

	if(!m_board.IsPlaceable(square, m_x, m_y))
		return FALSE;

	m_square = square;
	return TRUE;
}

BOOL CBlockScene::MoveLeft()
{
	if(m_board.IsPlaceable(m_square, m_x-1, m_y))
	{
		m_x--;
		return TRUE;
	}
	return FALSE;
}

BOOL CBlockScene::MoveRight()
{
	if(m_board.IsPlaceable(m_square, m_x+1, m_y))
	{
		m_x++;
		return TRUE;
	}
	return FALSE;
}

BOOL CBlockScene::MoveDown()
{
	if(m_board.IsPlaceable(m_square, m_x, m_y+1))
	{
		m_y++;
		return TRUE;
	}
	return FALSE;
}

BOOL CBlockScene::DropDown()
{
	return m_board.DropDown(m_square, m_x, m_y);
}

BOOL CBlockScene::CanMoveDown()
{
	return m_board.IsPlaceable(m_square, m_x, m_y+1);
}

BOOL CBlockScene::FixSquareToBoard()
{
	return m_board.FixSquareToBoard(m_square, m_x, m_y);
}

BOOL CBlockScene::DestroyFull()
{
	return (m_board.DestroyFull() > 0);
}

BOOL CBlockScene::NewSquare()
{
	m_square = m_square2;
	m_x = (BLOCK_BOARD_WIDTH - BLOCK_SQUARE_WIDTH)/2;
	m_y = 0;

	m_square2.RandomNew();

	return m_board.IsPlaceable(m_square, m_x, m_y);
}

BOOL CBlockScene::AddRandomBottomLine()
{
	return m_board.AddRandomBottomLine(m_square, m_x, m_y);
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

ZBA CBlockScene::RunAI()
{
	return m_ai.RunAI(m_board, m_square, m_square2, m_x, m_y);
}

} // namespace NBlock
