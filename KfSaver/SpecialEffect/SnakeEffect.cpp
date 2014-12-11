// CSnakeEffect
//

//	History:
//	智能实在白痴，有望改进										(2005-01-10)
//	智能稍有改进，但仍会误进死胡同，尚需改进					(2005-02-04)
//	智能速度提高，在苹果数目增加的情况下智能计算所需时间不增加	(2005-02-24)

#include "stdafx.h"
#include "SnakeEffect.h"
#include "EffectRegister.h"

REGISTER_EFFECT(CSnakeEffect);

#define TIMESLICE_STEP				100
#define SNAKE_INIT_LENGTH			10
#define MAX_NEW_APPLE_INTERVAL		14
#define MAX_STEP_COUNT				MAX_FRAME_NUM
#define TRY_NUM						256

#define MAX_WIDTH					48
#define MAX_HEIGHT					48
#define MIN_WIDTH					36
#define MIN_HEIGHT					20

#define SNAKE_IMAGE_SIZE			24

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSnakeEffect::CSnakeEffect(HWND hWnd, HBITMAP hBmpBkgnd) :
	CSpecialEffect(hWnd, hBmpBkgnd, ES_FILL_BACKGROUND)
{
	m_snake.clear();
	m_apples.clear();
}

CSnakeEffect::~CSnakeEffect()
{
	FreeImages();
}

const char* CSnakeEffect::GetEffectName() const
{
	return "SnakeEffect";
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

BOOL CSnakeEffect::Initialize(HDC hDC, BOOL)
{
	{	// map size
		for (int i=0; ; ++i)
		{
			m_nWidth = random(MAX_WIDTH, MIN_WIDTH);
			m_nSquareSize = GetWndWidth() / m_nWidth;
			m_nHeight = GetWndHeight() / m_nSquareSize;
			if (m_nHeight >= MIN_HEIGHT && m_nHeight < MAX_HEIGHT)
				break;

			LOG("Width %d, height %d; choosing another ...", m_nWidth, m_nHeight);
			if (i >= 100)
			{
				LOG("Give up...");
				return FALSE;
			}
		}
		LOG("Width %d, height %d; Okay!", m_nWidth, m_nHeight);

		m_xMargin = (GetWndWidth() - m_nWidth*m_nSquareSize) / 2;
		m_yMargin = (GetWndHeight() - m_nHeight*m_nSquareSize) / 2;
	}

	{	// start position
		ZSnakeBody egg; // snake egg, not the MagicHong

		egg.x = random(m_nWidth);
		egg.y = random(m_nHeight);
		egg.dir = (ZDir)random(DIR_MAX);

		m_snake.clear();
		m_snake.push_back(egg);
	}

	m_iBirthLength = 1;
	m_dir = m_snake.front().dir;

	LoadImages(hDC);

	{	// image mapping
		m_iSnakeBodyMap[DIR_EAST * DIR_MAX + DIR_EAST ] = BI_BODY_HORIZONTAL;
		m_iSnakeBodyMap[DIR_EAST * DIR_MAX + DIR_SOUTH] = BI_BODY_SOUTH_WEST;
		m_iSnakeBodyMap[DIR_EAST * DIR_MAX + DIR_WEST ] = BI_NULL;
		m_iSnakeBodyMap[DIR_EAST * DIR_MAX + DIR_NORTH] = BI_BODY_NORTH_WEST;

		m_iSnakeBodyMap[DIR_SOUTH * DIR_MAX + DIR_EAST ] = BI_BODY_NORTH_EAST;
		m_iSnakeBodyMap[DIR_SOUTH * DIR_MAX + DIR_SOUTH] = BI_BODY_VERTICAL;
		m_iSnakeBodyMap[DIR_SOUTH * DIR_MAX + DIR_WEST ] = BI_BODY_NORTH_WEST;
		m_iSnakeBodyMap[DIR_SOUTH * DIR_MAX + DIR_NORTH] = BI_NULL;

		m_iSnakeBodyMap[DIR_WEST * DIR_MAX + DIR_EAST ] = BI_NULL;
		m_iSnakeBodyMap[DIR_WEST * DIR_MAX + DIR_SOUTH] = BI_BODY_SOUTH_EAST;
		m_iSnakeBodyMap[DIR_WEST * DIR_MAX + DIR_WEST ] = BI_BODY_HORIZONTAL;
		m_iSnakeBodyMap[DIR_WEST * DIR_MAX + DIR_NORTH] = BI_BODY_NORTH_EAST;

		m_iSnakeBodyMap[DIR_NORTH * DIR_MAX + DIR_EAST ] = BI_BODY_SOUTH_EAST;
		m_iSnakeBodyMap[DIR_NORTH * DIR_MAX + DIR_SOUTH] = BI_NULL;
		m_iSnakeBodyMap[DIR_NORTH * DIR_MAX + DIR_WEST ] = BI_BODY_SOUTH_WEST;
		m_iSnakeBodyMap[DIR_NORTH * DIR_MAX + DIR_NORTH] = BI_BODY_VERTICAL;

		m_iSnakeHeadMap[DIR_EAST ] = BI_HEAD_EAST ;
		m_iSnakeHeadMap[DIR_SOUTH] = BI_HEAD_SOUTH;
		m_iSnakeHeadMap[DIR_WEST ] = BI_HEAD_WEST ;
		m_iSnakeHeadMap[DIR_NORTH] = BI_HEAD_NORTH;

		m_iSnakeTailMap[DIR_EAST ] = BI_TAIL_EAST ;
		m_iSnakeTailMap[DIR_SOUTH] = BI_TAIL_SOUTH;
		m_iSnakeTailMap[DIR_WEST ] = BI_TAIL_WEST ;
		m_iSnakeTailMap[DIR_NORTH] = BI_TAIL_NORTH;
	}

    return TRUE;
}

BOOL CSnakeEffect::OnTimer(HDC hDC)
{
	if(	m_apples.size() == 0 ||
		GetFrameCount() - m_iLastAppleStep > MAX_NEW_APPLE_INTERVAL)
	{
		NewRandomApple();
		m_iLastAppleStep = GetFrameCount();
	}

	RunAI();
	if(Step())
	{
		Draw(hDC);

		return GetFrameCount() < MAX_STEP_COUNT;
	}
	else
	{
		return FALSE;
	}
}

DWORD CSnakeEffect::GetTimeslice() const
{
	return TIMESLICE_STEP;
}

//////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////

void CSnakeEffect::Draw(HDC hDC)
{
	// snake
	DrawSnake(hDC);

	// apples
	DrawApples(hDC);
}

void CSnakeEffect::DrawSnake(HDC hDC)
{
	HDC hDCTmp = CreateCompatibleDC(hDC);
	ZSnake::iterator iter;

	// While snake is birthing, hide its tail.
	const BOOL bHideTail = (m_iBirthLength < SNAKE_INIT_LENGTH);

	// just save the old bitmap handle, nothing with select
	HBITMAP hOldBmpTmp = (HBITMAP)SelectObject(hDCTmp, m_hBmp[BI_APPLE]);

	{	// head
		iter = (m_snake.end()-1);
		SelectObject(hDCTmp, m_hBmp[m_iSnakeHeadMap[iter->dir]]);
		BitBlt(hDC,
			m_xMargin + iter->x * m_nSquareSize,
			m_yMargin + iter->y * m_nSquareSize,
			m_nSquareSize, m_nSquareSize,
			hDCTmp, 0, 0, SRCCOPY);
	}

	// body
	if( (bHideTail && m_snake.size()>1) ||
		(!bHideTail && m_snake.size()>2) )
	{
		iter = (bHideTail ? m_snake.begin() : m_snake.begin()+1);

		for(; iter+1 != m_snake.end(); ++iter)
		{
			SelectObject(hDCTmp,
				m_hBmp[m_iSnakeBodyMap[iter->dir*DIR_MAX+(iter+1)->dir]]);
			BitBlt(hDC,
				m_xMargin + iter->x * m_nSquareSize,
				m_yMargin + iter->y * m_nSquareSize,
				m_nSquareSize, m_nSquareSize,
				hDCTmp, 0, 0, SRCCOPY);
		}
	}

	// tail
	if(!bHideTail && m_snake.size()>=2)
	{
		iter = m_snake.begin();
		SelectObject(hDCTmp, m_hBmp[m_iSnakeTailMap[(iter+1)->dir]]);
		BitBlt(hDC,
			m_xMargin + iter->x * m_nSquareSize,
			m_yMargin + iter->y * m_nSquareSize,
			m_nSquareSize, m_nSquareSize,
			hDCTmp, 0, 0, SRCCOPY);
	}

	// free memory
	SelectObject(hDCTmp, hOldBmpTmp);
	DeleteDC(hDCTmp);
}

void CSnakeEffect::DrawApples(HDC hDC)
{
	HDC hDCTmp = CreateCompatibleDC(hDC);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDCTmp, m_hBmp[BI_APPLE]);

	for(ZAppleArray::iterator iter=m_apples.begin(); iter!=m_apples.end(); ++iter)
	{
		BitBlt(hDC,
			m_xMargin + iter->x * m_nSquareSize,
			m_yMargin + iter->y * m_nSquareSize,
			m_nSquareSize, m_nSquareSize,
			hDCTmp, 0, 0, SRCCOPY);
	}

	// free memory
	SelectObject(hDCTmp, hOldBmp);
	DeleteDC(hDCTmp);
}

//////////////////////////////////
//////////////////////////////////


void CSnakeEffect::RunAI()
{
	const ZSnakeBody &head = m_snake.back();

	BOOL bPossible[4];
	ZPoint ptNext[4];
	int i;

	// is it possible to go? (out of range? self crash?)
	for(i=0; i<4; i++)
	{
		ptNext[i] = head;
		NextPoint(ptNext[i], (ZDir)i);

		bPossible[i] = CanGo(ptNext[i]);
	}
	bPossible[GetOppositeDir(head.dir)] = FALSE;

	int distance[4];
	int minDist = -1;
	int nWay = 0;

	for(i=0; i<4; i++)
	{
		distance[i] = -1;

		if(!bPossible[i])
			continue;
		nWay++;

		distance[i] = GetNearestAppleDistance(ptNext[i]);
		if(distance[i]>=0 && (minDist<0 || distance[i]<minDist))
			minDist = distance[i];
	}

	// no way to go!
	if(nWay == 0)
		return;

	if(minDist >= 0)	// there are some way to eat apple
	{
		// if the original dir is acceptable, then keep the original dir
		if(m_dir<4 && distance[m_dir]==minDist)
			return;
		else
		{
			// otherwise, random direction
			for(i=0; i<TRY_NUM; i++)
			{
				ZDir dir = (ZDir)random(4);
				if(distance[dir] == minDist)
				{
					m_dir = dir;
					return;
				}
			}
		}
	}
	else // no way to eat apple
	{
		// go some way to safest
		int nSafe[4];
		int nMaxSafe = 0;

		for(i=0; i<4; i++)// safe quotiety
		{
			nSafe[i] = 0;
			if(bPossible[i])
			{
				nSafe[i] = CalcSafeQuotiety(ptNext[i]);
				if(nSafe[i] > nMaxSafe)
					nMaxSafe = nSafe[i];
			}
		}

		// if the original dir is acceptable, then keep the original dir
		if(m_dir<4 && nSafe[m_dir]==nMaxSafe)
			return;
		else
		{	// otherwise, random direction
			for(i=0; i<TRY_NUM; i++)
			{
				ZDir dir = (ZDir)random(4);
				if(nSafe[dir] == nMaxSafe)
				{
					m_dir = dir;
					return;
				}
			}
		}
	}
}

BOOL CSnakeEffect::Step()
{
	ZSnakeBody head = m_snake.back();
	if(!NextPoint(head, m_dir))
		return FALSE;

	if(IsOutOfRange(head))
		return FALSE;

	BOOL bAppleEat = FALSE;
	{	// eat apple?
		ZAppleArray::iterator iter = IsAppleHere(head);
		if(iter != m_apples.end())
		{
			bAppleEat = TRUE;
			m_apples.erase(iter);
		}
	}

	if(!bAppleEat)
	{	// self crash?
		if(!CanGo(head))
			return FALSE;
	}

	if(!bAppleEat && m_iBirthLength>=SNAKE_INIT_LENGTH)
	{	// if no apple eat, then snake tail step in
		m_snake.erase(m_snake.begin());
	}
	else
	{
		if(!bAppleEat)
			m_iBirthLength++;
	}

	{	// snake head step in
		head.dir = m_dir;

		m_snake.push_back(head);
	}

	return TRUE;
}

void CSnakeEffect::NewRandomApple()
{
	ZApple apple;

	for(int i=0; i<TRY_NUM; i++)
	{
		apple.x = random(m_nWidth);
		apple.y = random(m_nHeight);
		if(m_apples.end() == IsAppleHere(apple) && m_snake.end() == IsSnakeHere(apple))
			break;
	}

	m_apples.push_back(apple);
}

BOOL CSnakeEffect::NextPoint(CSnakeEffect::ZPoint &point, CSnakeEffect::ZDir dir)
{
	switch(dir)
	{
	case DIR_EAST:	++point.x; break;
	case DIR_SOUTH:	++point.y; break;
	case DIR_WEST:	--point.x; break;
	case DIR_NORTH:	--point.y; break;
	default: return FALSE;
	}

	return TRUE;
}

CSnakeEffect::ZAppleArray::iterator CSnakeEffect::IsAppleHere(const CSnakeEffect::ZPoint &point)
{
	ZAppleArray::iterator iter;
	for(iter=m_apples.begin(); iter!=m_apples.end(); ++iter)
	{
		if(*iter == point)
			return iter;
	}
	return m_apples.end();
}

CSnakeEffect::ZSnake::iterator CSnakeEffect::IsSnakeHere(const CSnakeEffect::ZPoint &point)
{
	ZSnake::iterator iter;
	for(iter=m_snake.begin(); iter!=m_snake.end(); ++iter)
	{
		if(*iter == point)
			return iter;
	}
	return m_snake.end();
}

BOOL CSnakeEffect::IsOutOfRange(const CSnakeEffect::ZPoint & point)
{
	return (point.x < 0 || point.x >= m_nWidth ||
			point.y < 0 || point.y >= m_nHeight);
}

int CSnakeEffect::GetDistance(CSnakeEffect::ZPoint &p1, CSnakeEffect::ZPoint &p2)
{
	return abs(p1.x-p2.x) + abs(p1.y-p2.y);
}

void CSnakeEffect::LoadImages(HDC hDC)
{
	const int nGap = SNAKE_IMAGE_SIZE;
	const int nSrcSize = SNAKE_IMAGE_SIZE;
	const int nDstSize = m_nSquareSize;

	const ZBitmapIndex mapping[BI_MAX] = 
	{
		BI_APPLE,
		BI_HEAD_EAST,
		BI_HEAD_SOUTH,
		BI_HEAD_WEST,
		BI_HEAD_NORTH,
		BI_BODY_SOUTH_EAST,
		BI_BODY_NORTH_EAST,
		BI_BODY_SOUTH_WEST,
		BI_BODY_NORTH_WEST,
		BI_BODY_HORIZONTAL,
		BI_BODY_VERTICAL,
		BI_TAIL_EAST,
		BI_TAIL_SOUTH,
		BI_TAIL_WEST,
		BI_TAIL_NORTH,
	};

	for(int i=0; i<BI_MAX; i++)
	{
		m_hBmp[mapping[i]] = LoadBmpFromSrc(hDC, 
			IDBMP_SNAKE, i*nGap, 0, nSrcSize,
			nSrcSize, nDstSize, nDstSize);
	}
}

void CSnakeEffect::FreeImages()
{
	for(int i=0; i<BI_MAX; i++)
	{
		if(m_hBmp[i] != 0)
			DeleteObject(m_hBmp[i]);
	}
}

CSnakeEffect::ZDir CSnakeEffect::GetOppositeDir(CSnakeEffect::ZDir dir)
{
	switch(dir)
	{
	case DIR_EAST:	return DIR_WEST;
	case DIR_SOUTH:	return DIR_NORTH;
	case DIR_WEST:	return DIR_EAST;
	case DIR_NORTH:	return DIR_SOUTH;
	default: ASSERT(FALSE); return (ZDir)(-1);
	}
}

BOOL CSnakeEffect::CanGo(const CSnakeEffect::ZPoint &point)
{
	if(IsOutOfRange(point))
		return FALSE;

	const ZSnake::iterator last =
		((m_iBirthLength < SNAKE_INIT_LENGTH) ? m_snake.end()-1 : m_snake.end());

	ZSnake::iterator iter;
	for(iter=m_snake.begin(); iter!=last; ++iter)
	{
		if(*iter == point)
			return FALSE;
	}
	return TRUE;
}

int CSnakeEffect::CalcSafeQuotiety(const CSnakeEffect::ZPoint &point)
{
	if(!CanGo(point))
		return 0;

	BOOL bDanger[MAX_HEIGHT][MAX_WIDTH];
	memset(bDanger, 0, MAX_HEIGHT*MAX_WIDTH*sizeof(BOOL));

	ZSnake::iterator iter;
	for(iter=m_snake.begin(); iter!=m_snake.end(); iter++)
	{
		bDanger[iter->y][iter->x] = TRUE;
	}

	typedef stack<ZPoint> point_stack;
	point_stack sWalkable;
	sWalkable.push(point);

	int nWalkable = 1;

	while(!sWalkable.empty())
	{
		const ZPoint ptOriginal = sWalkable.top();
		sWalkable.pop();

		for(int dir=0; dir<DIR_MAX; ++dir)
		{
			ZPoint pt = ptOriginal;
			NextPoint(pt, (ZDir)dir);
			if(!IsOutOfRange(pt) && !bDanger[pt.y][pt.x])
			{
				sWalkable.push(pt);
				bDanger[pt.y][pt.x] = TRUE;

				++nWalkable;
			}
		}
	}

	return nWalkable;
}

int CSnakeEffect::GetNearestAppleDistance(const CSnakeEffect::ZPoint &point)
{
#define UNREACHED		0
#define UNREACHABLE		(-1)
#define APPLE			(-2)

	if(!CanGo(point))
		return -1;

	int nDistance[MAX_HEIGHT][MAX_WIDTH];
	memset(nDistance, 0, MAX_HEIGHT*MAX_WIDTH*sizeof(int));

	for(ZSnake::iterator iter=m_snake.begin(); iter!=m_snake.end(); ++iter)
	{
		nDistance[iter->y][iter->x] = UNREACHABLE;
	}

	for(ZAppleArray::iterator iter=m_apples.begin(); iter!=m_apples.end(); ++iter)
	{
		nDistance[iter->y][iter->x] = APPLE;
	}

	if(nDistance[point.y][point.x] == UNREACHABLE)
		return -1;
	if(nDistance[point.y][point.x] == APPLE)
		return 0;

	queue<ZPoint> qReachable;
	qReachable.push(point);
	nDistance[point.y][point.x] = 1;

	BOOL bAppleFound = FALSE;
	int nNearestAppleDistance = -1;

	while(!qReachable.empty() && !bAppleFound)
	{
		const ZPoint ptOriginal = qReachable.front();
		qReachable.pop();

		const unsigned int nDist = nDistance[ptOriginal.y][ptOriginal.x];
		int dir;

		if(nDist < m_snake.size())
		{
			ZSnake::iterator iter = m_snake.begin()+(nDist-1);
			if(nDistance[iter->y][iter->x] == UNREACHABLE)
				nDistance[iter->y][iter->x] = UNREACHED;
		}

		for(dir=0; !bAppleFound && dir<DIR_MAX; ++dir)
		{
			ZPoint pt = ptOriginal;
			NextPoint(pt, (ZDir)dir);

			if(!IsOutOfRange(pt))
			{
				switch(nDistance[pt.y][pt.x])
				{
				case UNREACHED:
					qReachable.push(pt);
					nDistance[pt.y][pt.x] = nDist+1;
					break;
				case APPLE:
					nNearestAppleDistance = nDist+1;
					bAppleFound = TRUE;
					break;
				case UNREACHABLE:
				default:
					break;
				}
			}
		}
	}

	if(!bAppleFound)
		return -1;
	return nNearestAppleDistance-1;
}
