
#ifndef _BUMPER_SCENE_H__INCLUDED__2007_06_28_
#define _BUMPER_SCENE_H__INCLUDED__2007_06_28_

#include "BumperAI.h"
#include "ZkfGraphics.h"

namespace NBumper {

class CBumperScene
{
public:
	CBumperScene();
	~CBumperScene();

	BOOL Initialize(int width, int height);
	BOOL Act();
	BOOL Draw(HDC);
	DWORD GetTimeslice() const;
	DWORD GetMaxTime() const;

public:
	typedef CBumperAI::CBumperObject  CBumperObject;
	typedef CBumperAI::CBumperObjects CBumperObjects;
	typedef CBumperAI::CBumperRobot   CBumperRobot;
	typedef CBumperAI::CBumperBall    CBumperBall;
	typedef CBumperAI::CBumperRobots  CBumperRobots;

protected:
	BOOL DrawObject(HDC, const CBumperObject &, COLORREF);

private:
	int m_width, m_height;
	int m_gate_height;
	ZBoundary m_boundary;

	CBumperRobots m_lefts;
	CBumperRobots m_rights;
	CBumperBall m_ball;
	CBumperObjects m_objects;

	CBumperAI *m_left_ai;
	CBumperAI *m_right_ai;
};

} // namespace NBumper

#endif // _BUMPER_SCENE_H__INCLUDED__2007_06_28_
