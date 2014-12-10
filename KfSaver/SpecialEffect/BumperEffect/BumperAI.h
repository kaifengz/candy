
#ifndef _BUMPER_AI_H__INCLUDED__2011_01_29_
#define _BUMPER_AI_H__INCLUDED__2011_01_29_

#include "ZkfGraphics.h"

namespace NBumper {

class CBumperAI
{
public:
	typedef ZMovableCircle CBumperObject;
	typedef CBumperObject CBumperRobot;
	typedef CBumperObject CBumperBall;
	typedef vector<CBumperRobot> CBumperRobots;
	typedef vector<CBumperObject*> CBumperObjects;

	CBumperAI(
		const ZBoundary &boundary,
		CBumperRobots &ourselves,
		const CBumperRobots &rivals,
		const CBumperBall &ball);

	void RunAI();

protected:
	typedef vector<CBumperRobot*> CBumperRobotRefs;
	void DevideMembers(CBumperRobotRefs &jmembers, CBumperRobotRefs &fmembers);

protected:
	const ZBoundary &m_boundary;
	CBumperRobots &m_ourselves;
	const CBumperRobots &m_rivals;
	const CBumperBall &m_ball;
};

} // namespace NBumper

#endif // _BUMPER_AI_H__INCLUDED__2011_01_29_
