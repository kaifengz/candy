// BumperAI.cpp
// 

#include "StdAfx.h"
#include "BumperAI.h"


namespace NBumper {

CBumperAI::CBumperAI(
		const ZBoundary &boundary,
		CBumperRobots &ourselves,
		const CBumperRobots &rivals,
		const CBumperBall &ball) :
	m_boundary(boundary),
	m_ourselves(ourselves),
	m_rivals(rivals),
	m_ball(ball)
{
}

void CBumperAI::RunAI()
{
	// TODO: implement CBumperAI::RunAI

	CBumperRobotRefs jmembers, fmembers;
	DevideMembers(jmembers, fmembers);

	for (unsigned int i=0; i<jmembers.size(); i++)
	{
		CBumperRobot &robot = *jmembers[i];

		ZForce f_affinity = robot.GetAffinity(m_ball, (MEASURE_D)3);
		robot.Accelerate(f_affinity, 1, random(15,20));
	}

	for (unsigned int i=0; i<fmembers.size(); i++)
	{
		CBumperRobot &robot = *fmembers[i];
		robot.Decelerate(0.2, 1);
	}
}

void CBumperAI::DevideMembers(
		CBumperRobotRefs &jmembers,
		CBumperRobotRefs &fmembers)
{
	jmembers.clear();
	fmembers.clear();

	vector<double> distances;

	for (unsigned int i=0; i<m_ourselves.size(); i++)
	{
		CBumperRobot &robot = m_ourselves[i];
		distances.push_back(robot.barycenter.GetDistance(m_ball.barycenter));
	}

	std::sort(distances.begin(), distances.end());
	const double middle_distance = distances[(distances.size()+1)/2];

	for (unsigned int i=0; i<m_ourselves.size(); i++)
	{
		CBumperRobot &robot = m_ourselves[i];
		if (robot.barycenter.GetDistance(m_ball.barycenter) < middle_distance)
			jmembers.push_back(&robot);
		else
			fmembers.push_back(&robot);
	}
}

} // namespace NBumper
