
#ifndef _BLOCK_AI_H__INCLUDED__2007_06_27_
#define _BLOCK_AI_H__INCLUDED__2007_06_27_

#include "BlockBase.h"

namespace NBlock {


class CBlockAI
{
public:
	CBlockAI();
	virtual ~CBlockAI();

public:
	ZBA RunAI(	const CBlockBoard& board,
				const CBlockSquare& square,
				const CBlockSquare& square2,
				const int x,
				const int y);

private:
	enum ZAISearchDepth
	{
		SD_USE_SQUARE	= 1,
		SD_USE_SQUARE2	= 2,
	};

private:
	HANDLE m_hSemaDataAccess;

	CBlockBoard m_board;
	CBlockSquare m_square;
	CBlockSquare m_square2;
	int m_x;
	int m_y;

	CBlockBoard m_oldBoard;
	CBlockSquare m_oldSquare;
	CBlockSquare m_oldSquare2;

	CBlockSquare m_squareDest;
	int m_xDest;
	int m_yDest;

private: // the first level control functions
	BOOL IsAIThreadCreated();
	BOOL IsSituationChanged();
	ZBA ExecuteStrategy();

private: // AI thread context
	HANDLE m_hAIThread;
	static DWORD APIENTRY AIThread(LPVOID lpParam);

	BOOL CreateAIThread();

private: // search functions
	static int CreateNewStrategy(const CBlockBoard& board,
								 const CBlockSquare& square,
								 const CBlockSquare& square2,
								 int xCur,
								 int yCur,
								 ZAISearchDepth sd,
								 int &xDest,
								 int &yDest,
								 int &nTurn);

	static int GetMaxYCanMove(const CBlockBoard& board,
							  const CBlockSquare& square,
							  int xSrc,
							  int xDest,
							  int yCur);

	static int GetMaxYCanTurn(const CBlockBoard& board,
							  const CBlockSquare& square,
							  int xDest,
							  int yCur,
							  int nTurn);

	static int GetBoardDanger(const CBlockBoard& board);
};

} // namespace NBlock

#endif // _BLOCK_AI_H__INCLUDED__2007_06_27_
