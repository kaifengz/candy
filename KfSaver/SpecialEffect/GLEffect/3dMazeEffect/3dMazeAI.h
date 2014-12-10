
#ifndef _3D_MAZE_AI_H__INCLUDED__2005_05_10_
#define _3D_MAZE_AI_H__INCLUDED__2005_05_10_

#include "3dMaze.h"

namespace N3dMaze {

class CMazeAI
{
public:
	CMazeAI();
	virtual ~CMazeAI();
	BOOL	InitAI(const CMaze*);

public:
	int		GetJourney();
	void	FindFarestPositions(int &xOrg, int &yOrg,
								int &xTgt, int &yTgt);

	BOOL	WalkAhead();
	BOOL	GetCurrentPos(double &x, double &y, int &dir);

public:
	struct ZRoutePoint
	{
		int x, y;
	};
	typedef ZRoutePoint				ZRP;
	typedef vector<ZRP>				vector_rp;
	typedef vector_rp::iterator		rp_iter;

private:
	// functions for find route
	BOOL	IsWalkable(int x1, int y1, int x2, int y2);
	BOOL	IsWalkable(const ZRoutePoint&, const ZRoutePoint&);
	BOOL	FindRoute();
	void	FindFarestPoint(int x, int y, int &xTgt, int &yTgt);

	// functions for walk from original to target
	void	InitCurrentPos();
	int		GetDirTo(const ZRP &rpFrom, const ZRP &rpTo);
	int		GetDir2NextGrid();
	void	StepAhead(double &x, double &y, const ZRP& rpCur, const ZRP &rpNext);

private:
	const CMaze			*m_pMaze;
	BOOL				m_bInitSuccess;

	vector_rp			m_route;

	double				m_xGrid, m_yGrid;	// the coord in the current grid
	int					m_dir;				// [0, 360), 0 for positive x, 90 for positive y, etc
};

}; // namespace N3dMaze

#endif // _3D_MAZE_AI_H__INCLUDED__2005_05_10_
