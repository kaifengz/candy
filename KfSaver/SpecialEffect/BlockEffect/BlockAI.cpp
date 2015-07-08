// BlockAI.cpp
//

#include "stdafx.h"
#include "BlockAI.h"

namespace NBlock {

#define SPACE_DANGER				200


//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

CBlockAI::CBlockAI()
{
	m_x = 0;
	m_y = 0;
	m_xDest = 0;
	m_yDest = 0;

	m_hSemaDataAccess = CreateSemaphore(NULL, 1, 1, NULL);
	m_hAIThread = NULL;
}

CBlockAI::~CBlockAI()
{
	while(IsAIThreadCreated())
	{
		Sleep(1);
	}

	WaitForSingleObject(m_hSemaDataAccess, INFINITE);
	CloseHandle(m_hSemaDataAccess);
}

//////////////////////////////////////////////////////////////////////
/********************************************************************\

	AI���ԣ�

	��Ե�ǰ���µķ��飨square���ͼ������ֵķ��飨square2������������ʹ
	���������������º���������������ܣ�ÿ��С����ĸ߶ȵĺͣ�����ճ�
	�ȣ�С�����հ��������Լ����壨�˴����嶨��Ϊÿ�������Ҿ�����߶�
	���ϣ���ĳ��Ȩƽ��������С��

	ÿ��ִ��AI������һ�������һ�����������ɵ�ǰ������AI���ԣ���
	�⿼�ǵ�AI�ļ��������Ҫһ��ʱ�䣬��������̼߳��㣬�����Ӱ�쵽
	����Ч����CPUƵ�ʲ���200Mʱͣ�����󽫱Ƚ����أ���������AI�������£�
	1.	���жϳ�������Ƿ�ı䣬��δ�ı���ִ��ԭ����
	2.	����������Ѿ������仯������AI�����߳��Ƿ��Ѿ����У����Ѿ���
		�У��򷵻ؿգ���ִ���κζ����������߳�û�����У��򴴽�AI������
		�̣�ͬʱ���ؿգ���ִ���κζ��������˴��������ؿյľ�����־���
		��ִ���κζ����������Ϸ��齫��Ȼ���䡣

\********************************************************************/
//////////////////////////////////////////////////////////////////////

ZBA CBlockAI::RunAI(
	const CBlockBoard& board,
	const CBlockSquare& square,
	const CBlockSquare& square2,
	const int x,
	const int y)
{
	m_board = board;
	m_square = square;
	m_square2 = square2;
	m_x = x;
	m_y = y;

	if(IsAIThreadCreated())
	{
		return BA_NOP;
	}
	else if(IsSituationChanged())
	{
		// board situation changed, create new AI thread to search the
		// AI strategy.
		CreateAIThread();

		return BA_NOP;
	}
	else
	{
		// situation not changed, execute old strategy.
		return ExecuteStrategy();
	}
}

BOOL CBlockAI::IsAIThreadCreated()
{
	if(m_hAIThread == NULL)
	{
		return FALSE;
	}
	else if(WAIT_OBJECT_0 == WaitForSingleObject(m_hAIThread, 0))
	{
		CloseHandle(m_hAIThread);
		m_hAIThread = NULL;
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL CBlockAI::IsSituationChanged()
{
	// �����ж��߼���˳��������������ж�������ʱ�������ģ������ǰ����
	// ���ں�

	BOOL bChanged;

	// AI thread need AI member data too, so lock it before use.
	WaitForSingleObject(m_hSemaDataAccess, INFINITE);

	// �����Ѿ�������ԭ���Կ��Եõ���ȷִ�еĸ߶����ޣ�ֻ�ò������ϣ���������
	if(m_y > m_yDest)
	{
		bChanged = TRUE;
		goto l_return;
	}

	// �·�����ˣ�ԭ��������
	if(m_square2 != m_oldSquare2)
	{
		bChanged = TRUE;
		goto l_return;
	}

	// ������ˣ�ԭ��������
	if(m_board != m_oldBoard)
	{
		bChanged = TRUE;
		goto l_return;
	}

	// ������û�䣬�������½��ķ���Ҳ��ԭ����һ������ʲô���ɲ�����ԭ���Ĳ����أ�
	if(m_square == m_oldSquare)
	{
		bChanged = FALSE;
		goto l_return;
	}

	{	// �����½��ķ���ֻ������ת�˼��£���Ȼ��AIҪ������ת�ģ�����Ȼ����ԭ������
		CBlockSquare square = m_oldSquare;

		for(int i=0; i<3; i++)
		{
			square.TurnClockwise(1);
			if(square == m_square)
			{
				bChanged = FALSE;
				goto l_return;
			}
		}
	}

	// ����������û�䣬���������½��ķ�����ˣ���������Ӧ��û�еģ�������Ȼ������
	// ��˳����Ȼ��
//	ASSERT(FALSE);
	bChanged = TRUE;

l_return:
	ReleaseSemaphore(m_hSemaDataAccess, 1, NULL);
	return bChanged;
}

ZBA CBlockAI::ExecuteStrategy()
{
	ZBA ba = BA_NOP;

	// lock data access
	WaitForSingleObject(m_hSemaDataAccess, INFINITE);

	if(m_square != m_squareDest)
		ba = BA_TURN_CLOCKWISE;
	else if(m_x > m_xDest)
		ba = BA_MOVE_LEFT;
	else if(m_x < m_xDest)
		ba = BA_MOVE_RIGHT;
	else
		ba = BA_DROP_DOWN;

	ReleaseSemaphore(m_hSemaDataAccess, 1, NULL);
	return ba;
}

BOOL CBlockAI::CreateAIThread()
{
	if(!IsAIThreadCreated()) // ��û�д���AI�̣߳��򴴽�֮
	{
		m_hAIThread = CreateThread(0, 0, AIThread, this, 0, NULL);
	}

	return (m_hAIThread != NULL);
}

DWORD APIENTRY CBlockAI::AIThread(LPVOID lpParam)
{
	// AI�߳�

	CBlockAI * const pAI = (CBlockAI *)lpParam;

	CBlockBoard boardCur;
	CBlockSquare squareCur;
	CBlockSquare square2;
	CBlockSquare squareDest;
	int xCur, yCur;

	int xDest, yDest, nTurn;
	int danger;

	{	// get necessary data for AI search
		WaitForSingleObject(pAI->m_hSemaDataAccess, INFINITE);

		boardCur = pAI->m_board;
		squareCur = pAI->m_square;
		square2 = pAI->m_square2;
		xCur = pAI->m_x;
		yCur = pAI->m_y;

		ReleaseSemaphore(pAI->m_hSemaDataAccess, 1, NULL);
	}

	{	// AI search
		danger = CreateNewStrategy(
			boardCur, squareCur, square2, xCur, yCur, SD_USE_SQUARE,
			xDest, yDest, nTurn);

		squareDest = squareCur;
		squareDest.TurnClockwise(nTurn);
	}

	{	// Save the AI search result
		WaitForSingleObject(pAI->m_hSemaDataAccess, INFINITE);

		pAI->m_xDest = xDest;
		pAI->m_yDest = yDest;

		pAI->m_squareDest = squareDest;

		// Copy from boardCur (and squareCur, etc.) but not pAI->m_board since 
		// pAI->m_board may be changed during AI searching. And, if pAI->m_board 
		// is changed during AI searching, then AI searching result if useless. 
		// So, copy from boardCur makes situation changed again and makes AI 
		// searching thread recreated.
		pAI->m_oldBoard = boardCur;
		pAI->m_oldSquare = squareCur;
		pAI->m_oldSquare2 = square2;

		ReleaseSemaphore(pAI->m_hSemaDataAccess, 1, NULL);
	}

	return 0;
}

int CBlockAI::CreateNewStrategy(const CBlockBoard& boardCur,
								const CBlockSquare& squareCur,
								const CBlockSquare& blockCur2,
								int xCur,
								int yCur,
								ZAISearchDepth sd,
								int &xDestRet,
								int &yDestRet,
								int &nTurnRet)
{
	ASSERT(sd == SD_USE_SQUARE || sd == SD_USE_SQUARE2);

	const CBlockSquare &blockCurrent = (sd==SD_USE_SQUARE ? squareCur : blockCur2);

	int xDest, yDest, nTurn;
	int yDestMove, yDestTurn;

	int MinDanger1=-1, MinDanger2=-1;
	int danger1, danger2;

	for(nTurn=0; nTurn<4; nTurn++)
	{
		CBlockSquare square = blockCurrent;

		// ����ת
		yDestTurn = GetMaxYCanTurn(boardCur, square, xCur, yCur, nTurn);
		if(yDestTurn < yCur)
			continue;

		if(nTurn != 0)
			square.TurnClockwise(nTurn);

		for(xDest=-3; xDest<BLOCK_BOARD_WIDTH; xDest++)
		{
			// ���ƶ�
			yDestMove = GetMaxYCanMove(boardCur, square, xCur, xDest, yCur);
			if(yDestMove < yCur)
				continue;

			yDest = min(yDestTurn, yDestMove);

			CBlockBoard board = boardCur;

			int x=xDest, y=yDest;
			board.DropDown(square, x, y);
			board.FixSquareToBoard(square, x, y);
			board.DestroyFull();

			if(sd == SD_USE_SQUARE)
			{
				// the action of square2, since they will not be useful immediately, 
				// it is not necessary to store them.
				int xDest2, yDest2, nTurn2;

				danger1 = GetBoardDanger(board);
				danger2 = CreateNewStrategy(
					board, squareCur, blockCur2, 3, 0, SD_USE_SQUARE2,
					xDest2, yDest2, nTurn2);
			}
			else
			{
				danger1 = -1;
				danger2 = GetBoardDanger(board);
			}

			if (MinDanger2 < 0 || 
				danger2 < MinDanger2 || 
				(danger2 == MinDanger2 &&
				 ((MinDanger1 < 0 && danger1 >=0 ) ||
				  danger1 < MinDanger1 ) ) ||
				(danger2 == MinDanger2 &&
				 danger1 == MinDanger1 &&
				 nTurn < nTurnRet ) )
			{
				xDestRet = xDest;
				yDestRet = yDest;
				nTurnRet = nTurn;
				MinDanger2 = danger2;
				MinDanger1 = danger1;
			}
		}
	}

	return MinDanger2;
}

//////////////////////////////////////////////////////////////////////
// 
//////////////////////////////////////////////////////////////////////

int CBlockAI::GetMaxYCanMove(const CBlockBoard& board,
							 const CBlockSquare& square,
							 int xSrc,
							 int xDest,
							 int yCur)
{
	int wide_block[BLOCK_SQUARE_WIDTH][BLOCK_BOARD_WIDTH];
	memset(wide_block, 0, sizeof(wide_block));

	int x, y;
	int i, j;

	const BOARD &boa = board.GetBoard();
	const SQUARE &sq = square.GetSquare();

	for(x=min(xSrc, xDest); x<=max(xSrc, xDest); x++)
	{
		for(i=0; i<BLOCK_SQUARE_WIDTH; i++)
		{
			for(j=0; j<BLOCK_SQUARE_WIDTH; j++)
			{
				if(sq[i][j] == 0)
					continue;
				if(x+j<0 || x+j>=BLOCK_BOARD_WIDTH)
					return -1;
				wide_block[i][x+j] = 1;
			}
		}
	}

	for(y=yCur; ; y++)
	{
		for(i=0; i<BLOCK_SQUARE_WIDTH; i++)
		{
			for(j=0; j<BLOCK_BOARD_WIDTH; j++)
			{
				if(wide_block[i][j] != 0 &&
				   (y+i<0 || y+i>=BLOCK_BOARD_HEIGHT || boa[y+i][j] != 0))
				{
					return y-1;
				}
			}
		}
	}
}

int CBlockAI::GetMaxYCanTurn(const CBlockBoard& board,
							 const CBlockSquare& square,
							 int xDest,
							 int yCur,
							 int nTurn)
{
	for(int y=yCur-1; ; y++)
	{
		CBlockSquare tempblock = square;

		for(int i=0; i<=nTurn; i++)
		{
			if(!board.IsPlaceable(tempblock, xDest, y+1))
				return y;
			tempblock.TurnClockwise(1);
		}
	}
}

int CBlockAI::GetBoardDanger(const CBlockBoard& board)
{
	const BOARD &boa = board.GetBoard();

	int dCells = 0; // Danger of cells' height
	int dSpace = 0; // Danger of space
	int dHeightDiff = 0;

	int x, y;

	// ���� ����Σ��ֵ �Լ� �ն�Σ��ֵ
	for(x=0; x<BLOCK_BOARD_WIDTH; x++)
	{
		for(y=0; y<BLOCK_BOARD_HEIGHT && boa[y][x]==0; y++)
			;

		for(; y<BLOCK_BOARD_HEIGHT; y++)
		{
			if(boa[y][x] == 0)
				// �ն�Σ��ֵ��ֹ��������Ŀն�
				dSpace += SPACE_DANGER;
			else
				// ����Σ��ֵ��ֹ����ѵ�̫��
				dCells += (BLOCK_BOARD_HEIGHT-y) * (BLOCK_BOARD_HEIGHT-y);
		}
	}

	// ��������Σ��ֵ
	int height[BLOCK_BOARD_WIDTH];
	for(x=0; x<BLOCK_BOARD_WIDTH; x++)
	{
		for(y=0; y<BLOCK_BOARD_HEIGHT && boa[y][x]==0; y++)
			;
		height[x] = BLOCK_BOARD_HEIGHT-y;
	}
	for(x=0; x<BLOCK_BOARD_WIDTH; x++)
	{
		int nLeft, nRight;

		if(x == 0)
			nLeft = 4;
		else if(height[x-1] >= height[x]+2)
			nLeft = height[x-1]-height[x];
		else
			nLeft = 0;

		if(x == BLOCK_BOARD_WIDTH-1)
			nRight = 4;
		else if(height[x+1] >= height[x]+2)
			nRight = height[x+1]-height[x];
		else
			nRight = 0;

		if(nLeft >= 2 && nRight >= 2)
		{
			// ��n������ ��Σ��ֵ ��ͬ�� n-1.5���ն��������ȿ��Է�ֹ�����ܸߵ�
			// ���壬�ֿ��Է�ֹ������������
			dSpace += SPACE_DANGER * (2*min(nLeft, nRight)-3) / 2;
		}
	}

	return dSpace + dCells + dHeightDiff;
}

} // namespace NBlock
