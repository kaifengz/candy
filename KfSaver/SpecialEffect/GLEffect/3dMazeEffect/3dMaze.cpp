// 3dMaze.cpp: Maze for 3dMazeEffect

#include "stdafx.h"
#include "3dMaze.h"
#include "3dMazeAI.h"


namespace N3dMaze {

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMaze::CMaze()
{
}

CMaze::~CMaze()
{
}

BOOL CMaze::InitRandomMaze()
{
	for(int i=0; i<MAZE_TRY_NUM_GAP_MATRIX; i++)
	{
		nSize = random(MAZE_MAX_SIZE, MAZE_MIN_SIZE);
		if(!CreateRandomGapMatrix())
			continue;

		xOrg = 0;
		yOrg = 0;
		xTgt = 0;
		yTgt = 1;

		CMazeAI ai;
		if(ai.InitAI(this))
		{
			ai.FindFarestPositions(xOrg, yOrg, xTgt, yTgt);
			return TRUE;
		}
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//	
///////////////////////////////////////////////////////////////////////////////

struct ZCrossPoint
{
	int x, y;
	ZCrossPoint(int x=0, int y=0)
	{
		this->x = x;
		this->y = y;
	}
};

BOOL CMaze::CreateRandomGapMatrix()
{
	int i;

	// clear gaps
	memset(gap_h, 0, sizeof(gap_h));
	memset(gap_v, 0, sizeof(gap_v));

	typedef ZCrossPoint ZCP;
	typedef vector<ZCP> vector_cp;
	typedef vector_cp::iterator cp_iter;

	vector_cp cpsInSet; // the cross points that have select in set
	cp_iter iter;

	// select the out boundary point in set
	for(i=0; i<nSize+1; i++)
	{
		cpsInSet.push_back(ZCP(0, i));
		cpsInSet.push_back(ZCP(i, 0));
		cpsInSet.push_back(ZCP(nSize, i));
		cpsInSet.push_back(ZCP(i, nSize));
	}

	// init cross set
	BOOL bInSet[MAZE_MAX_SIZE+1][MAZE_MAX_SIZE+1];
	memset(bInSet, 0, sizeof(bInSet));
	for(iter=cpsInSet.begin(); iter!=cpsInSet.end(); ++iter)
	{
		bInSet[iter->y][iter->x] = TRUE;
	}

	// select other points in set until all points have been selected in.
	const static int xOffset[4] = {0, -1, 1, 0};
	const static int yOffset[4] = {-1, 0, 0, 1};
	while(cpsInSet.size() > 0)
	{
		const int index = random(cpsInSet.size());
		const int dir = random(4);

		const ZCP cp = cpsInSet[index];
		ZCP cpNearby;
		bool bNearbyValid = FALSE;

		for(i=0; i<4; i++)
		{
			cpNearby = ZCP(cp.x+xOffset[(dir+i)%4],
						   cp.y+yOffset[(dir+i)%4]);
			if (cpNearby.x >=0 && cpNearby.x <= nSize &&
				cpNearby.y >=0 && cpNearby.y <= nSize &&
				!bInSet[cpNearby.y][cpNearby.x])
			{
				bNearbyValid = TRUE;
				break;
			}
		}
		if(!bNearbyValid)
		{
			// there is no valid point near the point
			cpsInSet.erase( cpsInSet.begin()+index );
			continue;
		}

		// select the nearby point in set and make the gap
		cpsInSet.push_back(cpNearby);
		bInSet[cpNearby.y][cpNearby.x] = TRUE;
		if(cpNearby.x == cp.x)
		{
			gap_v[ min(cpNearby.y, cp.y) ][cp.x] = TRUE;
		}
		else // if(cpNearby.y == cp.y)
		{
			gap_h[cp.y][ min(cpNearby.x, cp.x) ] = TRUE;
		}
	}

	// fill the out boundary gaps
	for(i=0; i<nSize; i++)
	{
		gap_h[0][i] = TRUE;
		gap_h[nSize][i] = TRUE;
		gap_v[i][0] = TRUE;
		gap_v[i][nSize] = TRUE;
	}

	return TRUE;
}

}; // namespace N3dMaze
