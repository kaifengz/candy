
#ifndef _OTHELLO_H__INCLUDED__2007_05_04_
#define _OTHELLO_H__INCLUDED__2007_05_04_

#include "OthelloBoard.h"
#include "OthelloSearchNode.h"

namespace NOthello {

///////////////////////////////////////////////////////////////////////////////
//	COthello
///////////////////////////////////////////////////////////////////////////////

class COthello
{
public:
	COthello();

	BOOL Initialize();
	BOOL Next();

	const COthelloBoard& GetBoard() const;

private:
	COthelloBoard m_board;
};

}; // namespace NOthello

#endif // _OTHELLO_H__INCLUDED__2007_05_04_
