
#ifndef _3D_MAZE_H__INCLUDED__2005_05_10_
#define _3D_MAZE_H__INCLUDED__2005_05_10_

namespace N3dMaze {

class CMaze
{
public:
	CMaze();
	virtual ~CMaze();
	BOOL InitRandomMaze();

public:
	/*
	 *	x of [0, MAZE_MAX_SIZE) for Horizontal, [0, MAZE_MAX_SIZE] for vertical
	 *	y of [0, MAZE_MAX_SIZE] for Horizontal, [0, MAZE_MAX_SIZE) for vertical
	 *	bHorizontal of TRUE for horizontal, FALSE for vertical
	 */
	BOOL IsGap(int x, int y, BOOL bHorizontal) const
	{
		return bHorizontal ? gap_h[y][x] : gap_v[y][x];
	}
	int GetMazeSize() const
	{
		return nSize;
	}
	void GetOriginalPos(int &x, int &y) const
	{
		x = xOrg;
		y = yOrg;
	}
	void GetTargetPos(int &x, int &y) const
	{
		x = xTgt;
		y = yTgt;
	}

private:
	BOOL CreateRandomGapMatrix();

public:
	enum
	{
		MAZE_MIN_SIZE = 12,
		MAZE_MAX_SIZE = 16,
		MAZE_TRY_NUM_GAP_MATRIX = 8,
	};

private:
	BOOL gap_h[MAZE_MAX_SIZE+1][MAZE_MAX_SIZE];
	BOOL gap_v[MAZE_MAX_SIZE][MAZE_MAX_SIZE+1];
	/*
	maze(size=3):      gap_h:        gap_v:
	+--+--+--+         1 1 1                
	|  |     |                       1 1 0 1
	+  +--+  +         0 1 0                
	|     |  |                       1 0 1 1
	+  +--+  +         0 1 0                
	|        |                       1 0 0 1
	+--+--+--+         1 1 1                
	*/

	int nSize;
	int xOrg, yOrg;
	int xTgt, yTgt;
};

}; // namespace N3dMaze

#endif // _3D_MAZE_H__INCLUDED__2005_05_10_
