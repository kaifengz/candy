
#ifndef _CHEVY_SCENE_H__INCLUDED__2006_08_16_
#define _CHEVY_SCENE_H__INCLUDED__2006_08_16_

#include "ZkfGraphics.h"

namespace NChevy {

class CChevyScene
{
public:
	CChevyScene() {}

public:
	BOOL Initialize(const RECT rect);
	BOOL Act();
	BOOL Draw(HDC hDC);

	DWORD GetTimeslice() const;
	DWORD GetMaxTime() const;

private:
	typedef ZMovableCircle CChevyObject;
	typedef CChevyObject CChevyBall;
	typedef CChevyObject CChevyRobot;

	BOOL AddNewRandomRobot();
	void DrawObject(HDC, const CChevyObject &, COLORREF);

private:
	CChevyBall m_ball;

	typedef vector<CChevyRobot> RobotList;
	RobotList m_robots;

	typedef vector<CChevyObject*> ChevyObjectList;
	ChevyObjectList m_objects;

	ZBoundary m_boundary;
};

} // namespace NChevy

#endif // _CHEVY_SCENE_H__INCLUDED__2006_08_16_
