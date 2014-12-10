
#ifndef _OTHELLO_BOARD_H__INCLUDED__2011_09_18_
#define _OTHELLO_BOARD_H__INCLUDED__2011_09_18_

///////////////////////////////////////////////////////////////////////////////
//	COthelloBoard
///////////////////////////////////////////////////////////////////////////////

class COthelloBoard
{
public:
	enum Cell
	{
		NONE = 0,
		BLACK,
		WHITE,
	};

	enum {
		SIZE = 8,
	};

public:
	COthelloBoard();
	COthelloBoard(const COthelloBoard &);
	~COthelloBoard();

	void SetInitState();
	void SkipOne();

public:
	COthelloBoard& operator= (const COthelloBoard &board);

	bool operator<  (const COthelloBoard &board) const;
	bool operator== (const COthelloBoard &board) const;

public:
	Cell Get(int x, int y) const
	{
		return m_cells[y][x];
	}

	bool Set(int x, int y, Cell cell);
	bool IsSettable(int x, int y, Cell cell);

	Cell GetNext() const
	{
		return m_next;
	}

protected:
	bool TestSet(int x, int y, Cell cell, bool really_set);

private:
	Cell m_cells[SIZE][SIZE];
	Cell m_next;
};

#endif // _OTHELLO_BOARD_H__INCLUDED__2011_09_18_
