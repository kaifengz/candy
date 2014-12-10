
#ifndef _SNAKE_EFFECT_H__INCLUDED__2005_01_03_
#define _SNAKE_EFFECT_H__INCLUDED__2005_01_03_

#include "SpecialEffect.h"


class CSnakeEffect : public CSpecialEffect  
{
public:
	CSnakeEffect(HWND, HBITMAP);
	virtual ~CSnakeEffect();

protected:
	virtual const char* GetEffectName() const;
    virtual BOOL Initialize(HDC hDC, BOOL bMagicHong);
	virtual BOOL OnTimer(HDC);
	virtual DWORD GetTimeslice() const;

private:
	enum ZDir
	{
		DIR_EAST	= 0,
		DIR_SOUTH	= 1,
		DIR_WEST	= 2,
		DIR_NORTH	= 3,

		DIR_MAX		= 4,
	};

	struct ZPoint
	{
		int x, y;

		ZPoint & operator =(const ZPoint &p)
		{
			x = p.x;
			y = p.y;
			return *this;
		}
		int operator ==(const ZPoint &p) const
		{
			return (x==p.x && y==p.y);
		}
		int operator !=(const ZPoint &p) const
		{
			return (x!=p.x || y!=p.y);
		}
	};

	struct ZSnakeBody : ZPoint
	{
		ZDir dir;
	};

	struct ZApple : ZPoint
	{
	};

	enum ZBitmapIndex
	{
		BI_NULL				= -1,

		BI_BODY_VERTICAL	= 0,
		BI_BODY_HORIZONTAL	= 1,
		BI_BODY_SOUTH_EAST	= 2,
		BI_BODY_NORTH_EAST	= 3,
		BI_BODY_SOUTH_WEST	= 4,
		BI_BODY_NORTH_WEST	= 5,
		BI_HEAD_EAST		= 6,
		BI_HEAD_SOUTH		= 7,
		BI_HEAD_WEST		= 8,
		BI_HEAD_NORTH		= 9,
		BI_TAIL_EAST		= 10,
		BI_TAIL_SOUTH		= 11,
		BI_TAIL_WEST		= 12,
		BI_TAIL_NORTH		= 13,
		BI_APPLE			= 14,

		BI_MAX				= 15,
	};

	typedef vector<ZSnakeBody>	ZSnake;
	typedef vector<ZApple>		ZAppleArray;

private:
	ZSnake			m_snake;
	ZAppleArray		m_apples;

	ZDir			m_dir;

	int				m_nWidth;
	int				m_nHeight;
	int				m_xMargin;
	int				m_yMargin;
	int				m_nSquareSize;

	int				m_iBirthLength;
	int				m_iLastAppleStep;

	HBITMAP			m_hBmp[BI_MAX];
	ZBitmapIndex	m_iSnakeHeadMap[DIR_MAX];
	ZBitmapIndex	m_iSnakeBodyMap[DIR_MAX*DIR_MAX];
	ZBitmapIndex	m_iSnakeTailMap[DIR_MAX];

private:
	void Init(HDC);
	void RunAI();
	BOOL Step();
	void Draw(HDC);
	void NewRandomApple();

	void DrawSnake(HDC);
	void DrawApples(HDC);

	int GetNearestAppleDistance(const ZPoint &);
	int CalcSafeQuotiety(const ZPoint &);
	BOOL CanGo(const ZPoint &);
	static ZDir GetOppositeDir(ZDir);
	static BOOL NextPoint(ZPoint &, ZDir);
	ZAppleArray::iterator IsAppleHere(const ZPoint &);
	ZSnake::iterator IsSnakeHere(const ZPoint &);
	BOOL IsOutOfRange(const ZPoint &);
	static int GetDistance(ZPoint &, ZPoint &);
	void LoadImages(HDC);
	void FreeImages();
};

#endif // _SNAKE_EFFECT_H__INCLUDED__2005_01_03_
