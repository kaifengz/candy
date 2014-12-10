// 3dMazeAI.cpp: AI for 3dMazeEffect

#include "stdafx.h"
#include "3dMazeAI.h"


#define WALK_SPEED			0.01		// walk through # grid per frame
#define TURN_SPEED			1			// turn left/right # degree per frame, 90.0/# must be an integer.

#define WALK_ARC			TRUE

// macros
#define PI 3.14159265358979323846
#define ANGLE_TO_ARC(angle)		((angle)*PI/180.0)

namespace N3dMaze {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMazeAI::CMazeAI()
{
	m_bInitSuccess = FALSE;
}

CMazeAI::~CMazeAI()
{
}

BOOL CMazeAI::InitAI(const CMaze* pMaze)
{
	m_pMaze = pMaze;

	if(!FindRoute())
		return FALSE;

	InitCurrentPos();

	m_bInitSuccess = TRUE;

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	private functions for find route
///////////////////////////////////////////////////////////////////////////////

BOOL CMazeAI::IsWalkable(int x1, int y1, int x2, int y2)
{
	ASSERT( (x1==x2 && (y1==y2+1 || y1+1==y2)) ||
			(y1==y2 && (x1==x2+1 || x1+1==x2)));

	if(x1 == x2)
	{
		return (! m_pMaze->IsGap(x1, max(y1, y2), TRUE));
	}
	else // if(y1 == y2)
	{
		return (! m_pMaze->IsGap(max(x1, x2), y1, FALSE));
	}
}

BOOL CMazeAI::IsWalkable(const ZRoutePoint& rp1, const ZRoutePoint& rp2)
{
	return IsWalkable(rp1.x, rp1.y, rp2.x, rp2.y);
}

struct ZWalkablePoint : CMazeAI::ZRoutePoint
{
	int distance;
};
typedef ZWalkablePoint		ZWP;
typedef queue<ZWP>			queue_wp;

BOOL CMazeAI::FindRoute()
{
	queue_wp wps;

	// add original point
	ZWP wpOriginal;
	m_pMaze->GetOriginalPos(wpOriginal.x, wpOriginal.y);
	wpOriginal.distance = 0;
	wps.push(wpOriginal);

	// get target point
	ZWP wpTarget;
	m_pMaze->GetTargetPos(wpTarget.x, wpTarget.y);

	// build distance table
	int i, j;
	int distance[CMaze::MAZE_MAX_SIZE][CMaze::MAZE_MAX_SIZE];
	for(i=0; i<m_pMaze->GetMazeSize(); i++)
	{
		for(j=0; j<m_pMaze->GetMazeSize(); j++)
			distance[i][j] = -1;
	}
	distance[wpOriginal.y][wpOriginal.x] = 0;

	// offset
	const static int xOffset[4] = {0, -1, 1, 0};
	const static int yOffset[4] = {-1, 0, 0, 1};

	// find route
	BOOL bTargetFound = FALSE;
	while(!bTargetFound && !wps.empty())
	{
		ZWP wp = wps.front();
		wps.pop();

		ZWP wpNearby;
		for(int i=0; i<4; i++)
		{
			wpNearby.x = wp.x + xOffset[i];
			wpNearby.y = wp.y + yOffset[i];
			wpNearby.distance = wp.distance + 1;

			if( wpNearby.x >=0 && wpNearby.x <= m_pMaze->GetMazeSize() &&
				wpNearby.y >=0 && wpNearby.y <= m_pMaze->GetMazeSize() &&
				distance[wpNearby.y][wpNearby.x] < 0 &&
				IsWalkable(wp, wpNearby))
			{
				wps.push(wpNearby);
				distance[wpNearby.y][wpNearby.x] = wpNearby.distance;
				if( wpNearby.x == wpTarget.x &&
					wpNearby.y == wpTarget.y)
				{
					bTargetFound = TRUE;
					wpTarget.distance = wpNearby.distance;
					break;
				}
			}
		}
	}

	if(!bTargetFound)
	{
		return FALSE;
	}

	// construct route
	m_route.clear();
	m_route.push_back(wpTarget);
	while(TRUE)
	{
		const ZRP &rp = m_route.back();
		ZRP rpNearby;

		BOOL bNearbyFound = FALSE;

		for(int i=0; i<4; i++)
		{
			rpNearby.x = rp.x + xOffset[i];
			rpNearby.y = rp.y + yOffset[i];
			if( rpNearby.x >=0 && rpNearby.x <= m_pMaze->GetMazeSize() &&
				rpNearby.y >=0 && rpNearby.y <= m_pMaze->GetMazeSize() &&
				distance[rpNearby.y][rpNearby.x] == distance[rp.y][rp.x]-1 &&
				IsWalkable(rpNearby, rp))
			{
				m_route.push_back(rpNearby);
				bNearbyFound = TRUE;
				break;
			}
		}

		if(!bNearbyFound)
			return FALSE;

		if( m_route.back().x == wpOriginal.x &&
			m_route.back().y == wpOriginal.y)
		{
			break;
		}
	}

	return TRUE;
}

void CMazeAI::FindFarestPoint(int x, int y, int &xTgt, int &yTgt)
{
	queue_wp wps;

	// add original point
	ZWP wpOriginal;
	wpOriginal.x = x;
	wpOriginal.y = y;
	wpOriginal.distance = 0;
	wps.push(wpOriginal);

	// build distance table
	int i, j;
	int distance[CMaze::MAZE_MAX_SIZE][CMaze::MAZE_MAX_SIZE];
	for(i=0; i<m_pMaze->GetMazeSize(); i++)
	{
		for(j=0; j<m_pMaze->GetMazeSize(); j++)
			distance[i][j] = -1;
	}
	distance[wpOriginal.y][wpOriginal.x] = 0;

	// offset
	const static int xOffset[4] = {0, -1, 1, 0};
	const static int yOffset[4] = {-1, 0, 0, 1};

	ZWP wpFarest;
	// find route
	while(!wps.empty())
	{
		wpFarest = wps.front();
		wps.pop();

		ZWP wpNearby;
		for(int i=0; i<4; i++)
		{
			wpNearby.x = wpFarest.x + xOffset[i];
			wpNearby.y = wpFarest.y + yOffset[i];
			wpNearby.distance = wpFarest.distance + 1;

			if( wpNearby.x >=0 && wpNearby.x <= m_pMaze->GetMazeSize() &&
				wpNearby.y >=0 && wpNearby.y <= m_pMaze->GetMazeSize() &&
				distance[wpNearby.y][wpNearby.x] < 0 &&
				IsWalkable(wpFarest, wpNearby))
			{
				wps.push(wpNearby);
				distance[wpNearby.y][wpNearby.x] = wpNearby.distance;
			}
		}
	}

	xTgt = wpFarest.x;
	yTgt = wpFarest.y;
}

void CMazeAI::FindFarestPositions(int &xOrg, int &yOrg,
								  int &xTgt, int &yTgt)
{
	xOrg = random(m_pMaze->GetMazeSize());
	yOrg = random(m_pMaze->GetMazeSize());

	FindFarestPoint(xOrg, yOrg, xTgt, yTgt);
	FindFarestPoint(xTgt, yTgt, xOrg, yOrg);
}

int CMazeAI::GetJourney()
{
	if(m_bInitSuccess)
		return (int)(m_route.size() - 1);
	else
		return -1;
}

///////////////////////////////////////////////////////////////////////////////
//	private functions for walk from original to target
///////////////////////////////////////////////////////////////////////////////

BOOL CMazeAI::WalkAhead()
{
	if(!m_bInitSuccess)
		return FALSE;

	if(!WALK_ARC) // walk horizontal and vertical
	{
		ASSERT(m_route.size() >= 2);

		if(m_dir == GetDir2NextGrid())	// walk to next grid
		{
			const ZRP &rpCur = m_route.back();
			const ZRP &rpNext = *(&m_route.back() - 1);

			StepAhead(m_xGrid, m_yGrid, rpCur, rpNext);

			if( m_xGrid <= -0.5 ||
				m_xGrid >= 1.5 ||
				m_yGrid <= -0.5 ||
				m_yGrid >= 1.5)	// has reached next grid center
			{
				m_xGrid = 0.5;
				m_yGrid = 0.5;
				m_route.resize( m_route.size()-1 ); // remove the last grid that has been walk through
			}
		}
		else // turning clockwise/anticlockwise
		{
			int next_dir = GetDir2NextGrid();
			if((m_dir+90+360-next_dir)%360 < 90) // turn  clockwise
				m_dir = (m_dir+TURN_SPEED) % 360;
			else
				m_dir = (m_dir+360 - TURN_SPEED) % 360;
		}

		return m_route.size() >= 2;
	}
	else // walk horizontal, vertical and arc.
	{
		if(m_route.size() < 2)
			return FALSE;
		else if(m_route.size() == 2)
		{
			StepAhead(m_xGrid, m_yGrid, m_route.back(), m_route.front());
			if( m_xGrid <= -0.5 || m_xGrid >= 1.5 ||
				m_yGrid <= -0.5 || m_yGrid >= 1.5)
			{	// has reached target
				m_xGrid = 0.5;
				m_yGrid = 0.5;
				m_route.resize(1);
				return FALSE;
			}
			return TRUE;
		}
		else // if(m_rount.size() >= 3)
		{
			const ZRP &rpNext = *(&m_route.back() - 2);
			const ZRP &rpCur  = *(&m_route.back() - 1);
			const ZRP &rpLast = m_route.back();

			if( GetDirTo(rpLast, rpCur) == GetDirTo(rpCur, rpNext) ||
				(m_xGrid>=0.0 && m_xGrid<=1.0 &&
				 m_yGrid>=0.0 && m_yGrid<=1.0 ) )
			{
				StepAhead(m_xGrid, m_yGrid, rpLast, rpCur);
				if( m_xGrid <= -0.5 || m_xGrid >= 1.5 ||
					m_yGrid <= -0.5 || m_yGrid >= 1.5)
				{	// has reached next grid center
					m_xGrid = 0.5;
					m_yGrid = 0.5;
					m_route.resize( m_route.size()-1 ); // remove the last grid that has been walk through
				}
			}
			else
			{
				// arc center
				double xCenter = (rpLast.x + rpNext.x) / 2.0 - rpLast.x;
				double yCenter = (rpLast.y + rpNext.y) / 2.0 - rpLast.y;

				int next_dir = GetDirTo(rpCur, rpNext);
				int normal_angle;

				if((m_dir+90+360-next_dir)%360 < 90) // turn  clockwise
				{
					m_dir = (m_dir+TURN_SPEED) % 360;
					normal_angle = (m_dir - 90 + 360) % 360;
				}
				else
				{
					m_dir = (m_dir+360 - TURN_SPEED) % 360;
					normal_angle = (m_dir + 90) % 360;
				}

				m_xGrid = 0.5 + xCenter + 0.5 * cos(ANGLE_TO_ARC(normal_angle));
				m_yGrid = 0.5 + yCenter + 0.5 * sin(ANGLE_TO_ARC(normal_angle));

				if(m_dir == GetDirTo(rpCur, rpNext))
				{
					m_route.resize( m_route.size() - 1);

					if(m_xGrid <= -0.5)
						m_xGrid += 1.0;
					else if(m_xGrid >= 1.5)
						m_xGrid -= 1.0;

					if(m_yGrid <= -0.5)
						m_yGrid += 1.0;
					else if(m_yGrid >= 1.5)
						m_yGrid -= 1.0;
				}
			}
			return TRUE;
		}
	}
}

void CMazeAI::InitCurrentPos()
{
	m_xGrid = 0.5;
	m_yGrid = 0.5;
	m_dir = GetDir2NextGrid();
}

int CMazeAI::GetDirTo(const ZRP &rpFrom, const ZRP &rpTo)
{
	if(rpFrom.x == rpTo.x)
	{
		if(rpTo.y > rpFrom.y)
			return 90;
		else
			return 270;
	}
	else if(rpFrom.y == rpTo.y)
	{
		if(rpTo.x > rpFrom.x)
			return 0;
		else
			return 180;
	}
	else
		return -1;
}

int CMazeAI::GetDir2NextGrid()
{
	ASSERT(m_route.size() >= 2);

	return GetDirTo(m_route.back(), *(&m_route.back() - 1));
}

BOOL CMazeAI::GetCurrentPos(double &x, double &y, int &dir)
{
	if(!m_bInitSuccess)
		return FALSE;

	x = m_route.back().x + m_xGrid;
	y = m_route.back().y + m_yGrid;
	dir = m_dir;

	return TRUE;
}

void CMazeAI::StepAhead(double &x, double &y, const ZRP &rpCur, const ZRP &rpNext)
{
	if(rpCur.x == rpNext.x)
	{
		if(rpNext.y > rpCur.y)
			y += WALK_SPEED;
		else
			y -= WALK_SPEED;
	}
	else // if(rpCur.y == rpNext.y)
	{
		if(rpNext.x > rpCur.x)
			x += WALK_SPEED;
		else
			x -= WALK_SPEED;
	}
}

}; // namespace N3dMaze
