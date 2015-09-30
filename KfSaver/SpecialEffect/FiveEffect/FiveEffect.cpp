// FiveEffect.cpp
//


#include "stdafx.h"
#include "FiveEffect.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CFiveEffect);



#define FIVE_DRAW_NEXT_TIP		TRUE
#define FIVE_NEXT_TIP_STYLE		PS_SOLID
#define FIVE_NEXT_TIP_WEIGHT	1
#define FIVE_NEXT_TIP_COLOR		RGB(96, 64, 128)
#define FIVE_NEXT_TIP_RSIZE		0.7		// relative size about the grid size

#define FIVE_BLACK_COLOR		RGB( 32,  32,  32)
#define FIVE_WHITE_COLOR		RGB(160, 160, 160)
#define FIVE_BOUNDARY_COLOR		RGB( 96,  96,  96)
#define FIVE_GRID_COLOR			RGB( 64,  64,  64)

#define FIVE_TIMESLICE			500		// 0.5 seconds
#define FIVE_WAIT_TIP_TIME		5		// wait 2.0 seconds ( = FIVE_WAIT_TIP_TIME * FIVE_TIMESLICE) before the tip
#define FIVE_WAIT_NEXT_TIME		1		// wait 1.0 seconds ( = FIVE_WAIT_NEXT_TIME * FIVE_TIMESLICE) before the next move
#define FIVE_WAIT_END_TIME		20		// wait 10.0 seconds ( = FIVE_WAIT_END_TIME * FIVE_TIMESLICE) before end the game

#define FIVE_MIN_TOP_MARGIN		20		// 20 pixel
#define FIVE_MIN_LEFT_MARGIN	20		// 20 pixel
#define FIVE_GRID_COUNT			15		// 五子棋有十五个格子


// #define TEST_GAME_INDEX			5


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFiveEffect::CFiveEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
    CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_hBmpBoard = NULL;
	m_hBmpBlack = NULL;
	m_hBmpWhite = NULL;
}

CFiveEffect::~CFiveEffect()
{
	ReleaseImages();
	ReleaseGame();
}

const char* CFiveEffect::GetEffectName() const
{
    return "FiveEffect";
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CFiveEffect::Initialize(HDC hDC, BOOL)
{
	if( !LoadImages(hDC) ||
		!LoadGame() )
	{
		return FALSE;
	}

	{	// calc window grid parameters
		const RECT &rc = GetClientArea();

		const unsigned int cx = rc.right - rc.left;
		const unsigned int cy = rc.bottom - rc.top;

		m_nGridSize = (min(cx, cy) - 2 * FIVE_MIN_LEFT_MARGIN) / FIVE_GRID_COUNT;
		m_xMargin = rc.left + (cx - m_nGridSize * FIVE_GRID_COUNT) / 2;
		m_yMargin = rc.top + (cy - m_nGridSize * FIVE_GRID_COUNT) / 2;
	}

	return TRUE;
}

BOOL CFiveEffect::OnTimer(HDC hDC)
{
	DrawAll(hDC);
	return NextStep();
}

DWORD CFiveEffect::GetTimeslice() const
{
	return FIVE_TIMESLICE;
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

void CFiveEffect::DrawAll(HDC hDC)
{
	ASSERT(m_game.state >= FGS_WAIT_TIP && m_game.state < FGS_MAX);
	ASSERT(m_game.state != FGS_ENDED);

	DrawBoard(hDC);

	ASSERT(m_game.nCurrentStep < m_game.steps.size());
	const unsigned int nChessmenToDraw = (unsigned int)(m_game.state == FGS_WAIT_END ?
		m_game.steps.size() : m_game.nCurrentStep+1);
	for(unsigned int i=0; i<nChessmenToDraw; i++)
	{
		DrawChessman(hDC, m_game.steps[i], i%2==0);
	}

#ifdef FIVE_DRAW_NEXT_TIP
	if(	m_game.state != FGS_WAIT_TIP &&
		m_game.state != FGS_WAIT_END &&
		m_game.nCurrentStep+1 < m_game.steps.size())
	{
		DrawNextTip(hDC, m_game.steps[m_game.nCurrentStep+1]);
	}
#endif
}

void CFiveEffect::DrawBoard(HDC hDC)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, FIVE_GRID_COLOR);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	for(unsigned int i=0; i<FIVE_GRID_COUNT; i++)
	{
		MoveToEx(hDC, m_xMargin + m_nGridSize/2,
			m_yMargin + i * m_nGridSize + m_nGridSize/2, NULL);
		LineTo(hDC, m_xMargin + (FIVE_GRID_COUNT-1) * m_nGridSize + m_nGridSize/2,
			m_yMargin + i * m_nGridSize + m_nGridSize/2);
	}

	for(unsigned int i=0; i<FIVE_GRID_COUNT; i++)
	{
		MoveToEx(hDC, m_xMargin + i * m_nGridSize + m_nGridSize/2,
			m_yMargin + m_nGridSize/2, NULL);
		LineTo(hDC, m_xMargin + i * m_nGridSize + m_nGridSize/2,
			m_yMargin + (FIVE_GRID_COUNT-1) * m_nGridSize + m_nGridSize/2);
	}

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
}

void CFiveEffect::DrawChessman(HDC hDC, const ZFivePoint& pos, BOOL bBlack)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, FIVE_BOUNDARY_COLOR);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	HBRUSH hBrush = CreateSolidBrush(bBlack ? FIVE_BLACK_COLOR : FIVE_WHITE_COLOR);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Ellipse(hDC, m_xMargin + pos.x * m_nGridSize + 2,
		m_yMargin + pos.y * m_nGridSize + 2,
		m_xMargin + (pos.x + 1) * m_nGridSize - 2,
		m_yMargin + (pos.y + 1) * m_nGridSize - 2);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	SelectObject(hDC, hOldBrush);
	DeleteObject(hBrush);
}

#ifdef FIVE_DRAW_NEXT_TIP
void CFiveEffect::DrawNextTip(HDC hDC, const ZFivePoint& pos)
{
	HPEN hPen = CreatePen(FIVE_NEXT_TIP_STYLE, FIVE_NEXT_TIP_WEIGHT, FIVE_NEXT_TIP_COLOR);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

	const unsigned int x = m_xMargin + pos.x * m_nGridSize;
	const unsigned int y = m_yMargin + pos.y * m_nGridSize;

	const unsigned int size = (int)(FIVE_NEXT_TIP_RSIZE * m_nGridSize + 0.5);
	const unsigned int margin = (m_nGridSize - size) / 2;

	Rectangle(hDC, x + margin, y + margin,
		x + m_nGridSize - margin, y + m_nGridSize - margin);

	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);
	SelectObject(hDC, hOldBrush);
}
#endif

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CFiveEffect::NextStep()
{
	ASSERT(m_game.state >= FGS_WAIT_TIP && m_game.state < FGS_MAX);

	switch(m_game.state)
	{
	case FGS_WAIT_TIP:
		ASSERT(m_game.nWaitTipCount < FIVE_WAIT_TIP_TIME);
		if(++m_game.nWaitTipCount == FIVE_WAIT_TIP_TIME)
		{
			m_game.state = FGS_WAIT_NEXT;
			m_game.nWaitTipCount = 0;
		}
		break;

	case FGS_WAIT_NEXT:
		ASSERT(m_game.nWaitNextCount < FIVE_WAIT_NEXT_TIME);
		if(++m_game.nWaitNextCount == FIVE_WAIT_NEXT_TIME)
		{
			ASSERT(m_game.nCurrentStep < m_game.steps.size());
			if(++m_game.nCurrentStep == m_game.steps.size())
			{
				m_game.state = FGS_WAIT_END;
				m_game.nCurrentStep = 0;
			}
			else
				m_game.state = FGS_WAIT_TIP;

			m_game.nWaitNextCount = 0;
		}
		break;

	case FGS_WAIT_END:
		ASSERT(m_game.nWaitEndCount < FIVE_WAIT_END_TIME);
		if(++m_game.nWaitEndCount == FIVE_WAIT_END_TIME)
			return FALSE;
		break;

	case FGS_ENDED:
		ASSERT(m_game.state != FGS_ENDED);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CFiveEffect::LoadImages(HDC hDC)
{
	// Current implementation will not use images...
	return TRUE;
}

void CFiveEffect::ReleaseImages()
{
#define SAFE_RELEASE(hBmp)	if(hBmp) { DeleteObject(hBmp); hBmp=NULL; }
	SAFE_RELEASE(m_hBmpBoard);
	SAFE_RELEASE(m_hBmpBlack);
	SAFE_RELEASE(m_hBmpWhite);
}

BOOL CFiveEffect::LoadGame()
{
	static ZFiveGameLib glib;
	static bool lib_initialized = false;
	static unsigned int *weight = NULL;

	if(!lib_initialized)
	{
		HRSRC hResourceInfo = FindResource(NULL,
			MAKEINTRESOURCE(IDFIVE_GAME_LIB), "FIVEGAMELIB");
		BOOL bSuccess = FALSE;

		if(hResourceInfo)
		{
			const DWORD nSize = SizeofResource(NULL, hResourceInfo);
			HGLOBAL hResource = LoadResource(NULL, hResourceInfo);
			if(hResource)
			{
				LPSTR lpRes = (LPSTR)LockResource(hResource);
				if(lpRes)
				{
					bSuccess = LoadGameLib(lpRes, nSize, glib);
					UnlockResource(hResource);
				}
			}
		}

		if(!bSuccess)
		{
			glib.clear();
			return FALSE;
		}

		lib_initialized = true;
	}

	if(glib.size() == 0)
		return FALSE;

	if(weight == NULL)
	{
		weight = new unsigned int[glib.size()];
		if(weight == NULL)
			return FALSE;
		memset(weight, 0, sizeof(int)*glib.size());
	}

#if defined(DEBUG_MODE) && defined(TEST_GAME_INDEX)
	const unsigned int index = TEST_GAME_INDEX;
#else
	const unsigned int index = RandomSelect(weight, (unsigned int)glib.size());
#endif
	m_game.steps = glib[index].steps;
	m_game.state = FGS_WAIT_TIP;
	m_game.nCurrentStep		= 0;
	m_game.nWaitTipCount	= 0;
	m_game.nWaitNextCount	= 0;
	m_game.nWaitEndCount	= 0;
	return TRUE;
}

void CFiveEffect::ReleaseGame()
{
	// nothing to be done since m_game will be automaticlly release when destruct
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

BOOL CFiveEffect::LoadGameLib(LPCSTR lpResource, ZFiveGameLib &glib)
{
	// Format like this:
	// H8I9H9...

	string lines = lpResource;
	string::size_type line_begin=0, line_end=0;

	while(true)
	{
		ZFiveGameStored game_stored;

		line_end = lines.find_first_of("\r\n", line_begin);
		if(line_end == string::npos)
			line_end = lines.length();

		do {	// parse line
			if(lines[line_begin] == '#') // comment line, skip
				break;

			string::size_type i=line_begin;
			while(i < line_end)
			{
				ZFivePoint pos;

#define TO_UPPER(c)		( (c)>='a' && (c)<='z' ? (c)-'a'+'A' : (c) )
				const char col = TO_UPPER(lines[i]);
				if( !(col>='A'+0 && col<'A'+FIVE_GRID_COUNT) )
					break;

				pos.y = (col - 'A');

				const char row = lines[i+1];
				if( !(row>='0' && row<='9') )
					break;

				pos.x = atoi(lines.c_str() + (i+1)) - 1;
				if( !(pos.x >= 0 && pos.x < FIVE_GRID_COUNT) ||
					!(pos.y >= 0 && pos.y < FIVE_GRID_COUNT) )
				{
					break;
				}

				game_stored.steps.push_back(pos);

				if( pos.x + 1 >= 10 )
					i += 3;
				else
					i += 2;
			}
			if(i < line_end)
				break;

			if(game_stored.steps.size() == 0)
				break;

			glib.push_back(game_stored);
		} while(0);

		if(line_end == lines.length())
			break;
		line_begin = lines.find_first_not_of("\r\n", line_end);
		if(line_begin == string::npos)
			break;
	}

	return glib.size() > 0;
}

BOOL CFiveEffect::LoadGameLib(LPCSTR lpResource, DWORD nSize, ZFiveGameLib &glib)
{
	// Format like this (in hex):
	// 0x889989...

	const unsigned char* bytes =
		reinterpret_cast<const unsigned char*>(lpResource);

	while(nSize>0)
	{
		ZFiveGameStored game_stored;

		ZFivePoint pos;
		while(nSize>0 && *bytes!='\0')
		{
			pos.x = ((*bytes) >> 4) - 1;
			pos.y = ((*bytes) & 0xF) - 1;
			game_stored.steps.push_back(pos);

			++bytes;
			--nSize;
		}

		if (game_stored.steps.size() > 0)
			glib.push_back(game_stored);

		if (nSize == 0)
			break;
		++bytes;
		--nSize;
	}

	return glib.size() > 0;
}
