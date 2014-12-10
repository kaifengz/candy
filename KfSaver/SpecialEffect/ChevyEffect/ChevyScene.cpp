// ChevyScene.cpp
//

#include "StdAfx.h"
#include "ChevyScene.h"
#include "ZkfGraphicsAux.h"

namespace NChevy {

#define ROBOT_RADIUS(width)     ((width) * 0.02)
#define BALL_RADIUS(width)      ((width) * 0.012)

#define OBJECT_REBOUND_COEFF	(float(1.25))	// 此值大于一，意味着两个圆相互碰撞后会“获得
												// 额外的能量”从而快速离开，避免他们粘在一起
#define BOUNDARY_REBOUND_COEFF	(float(0.8))	// 此值亦可以大于一，不过为了避免“足球”经过
												// 连续几次的碰撞边界而越飞越快，让机器人无法
												// 捕捉（因为机器人有限速），故设置略小于一

#define ACT_TIME_INTERVAL	((MEASURE_T)1)

#define ADD_ROBOT_TRY_NUM	128

#define TIMESLICE_CHEVY	    30
#define MAX_TIME_CHEVY	    (120*1000)

#define COLOR_BALL		    RGB(255, 255, 0)	// yellow
#define COLOR_ROBOT		    RGB(0, 0, 255)	    // blue

#define ROBOT_MAX_SPEED(width)      ((width) * 0.018)
#define ROBOT_CHEVYING_FORCE        3
#define ROBOT_DEACCELERATE_FORCE    0.2
#define ROBOT_CHEVYING_COUNT        5
#define ROBOT_DEACCELERATE_COUNT    3
#define ROBOT_NUM                   (ROBOT_CHEVYING_COUNT + ROBOT_DEACCELERATE_COUNT)

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CChevyScene::Initialize(int nWidth, int nHeight)
{
	m_boundary.Initialize(0.0, 0.0,
		(MEASURE_D)nWidth, (MEASURE_D)nHeight);

	// place BALL in the center of the region
	m_ball.barycenter = m_boundary.GetCenter();
	m_ball.radius = BALL_RADIUS(nWidth);

	// place un-overlapped robots
	m_robots.clear();
	for(unsigned int i=0; i<ROBOT_NUM; i++)
	{
		 if(!AddNewRandomRobot())
			 return FALSE;
	}

	m_objects.clear();
	m_objects.push_back(&m_ball);
	for(unsigned int i=0; i<m_robots.size(); i++)
		m_objects.push_back(&m_robots[i]);

	return TRUE;
}

BOOL CChevyScene::Act()
{
	CollideAndMove(m_objects, m_boundary, ACT_TIME_INTERVAL,
		OBJECT_REBOUND_COEFF, BOUNDARY_REBOUND_COEFF);

	// 距离球较近的几个机器人将追逐球，而其他的机器人在周围等待机会（即慢慢减速）
	vector<double> distances;

	for (unsigned int i=0; i<m_robots.size(); i++)
	{
		CChevyRobot &robot = m_robots[i];
		distances.push_back(robot.barycenter.GetDistance(m_ball.barycenter));
	}

	std::sort(distances.begin(), distances.end());
	const double distance_threshold = distances[ROBOT_CHEVYING_COUNT];

	const MEASURE_D robot_max_speed = ROBOT_MAX_SPEED(m_boundary.Width());
	for (unsigned int i=0; i<m_robots.size(); i++)
	{
		CChevyRobot &robot = m_robots[i];
		if (robot.barycenter.GetDistance(m_ball.barycenter) < distance_threshold)
		{
			ZForce force = robot.GetAffinity(m_ball, ROBOT_CHEVYING_FORCE);
			robot.Accelerate(force, ACT_TIME_INTERVAL, robot_max_speed);
		}
		else
			robot.Decelerate(ROBOT_DEACCELERATE_FORCE, ACT_TIME_INTERVAL);
	}

	return TRUE;
}

BOOL CChevyScene::Draw(HDC hDC)
{
	DrawObject(hDC, m_ball, COLOR_BALL);

	for(unsigned int i=0; i<m_robots.size(); i++)
		DrawObject(hDC, m_robots[i], COLOR_ROBOT);

	return TRUE;
}

DWORD CChevyScene::GetTimeslice() const
{
	return TIMESLICE_CHEVY;
}

DWORD CChevyScene::GetMaxTime() const
{
	return MAX_TIME_CHEVY;
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CChevyScene::AddNewRandomRobot()
{
	CChevyRobot robot;

	const MEASURE_D robot_radius = ROBOT_RADIUS(m_boundary.Width());
	for(int i=0; i<ADD_ROBOT_TRY_NUM; i++)
	{
		const ZPos center(
			(MEASURE_D)random(int(m_boundary.left+robot_radius), int(m_boundary.right-robot_radius)),
			(MEASURE_D)random(int(m_boundary.top+robot_radius), int(m_boundary.bottom-robot_radius)) );

		robot.barycenter = center;
		robot.radius = robot_radius;

		if(robot.OverlappedWith(m_ball))
			continue;

		BOOL bOverlapped = FALSE;
		for(unsigned int i=0; i<m_robots.size(); i++)
		{
			if(robot.OverlappedWith(m_robots[i]))
			{
				bOverlapped = TRUE;
				break;
			}
		}
		if(bOverlapped)
			continue;

		m_robots.push_back(robot);
		return TRUE;
	}

	return FALSE;
}

void CChevyScene::DrawObject(HDC hDC, const CChevyObject &object, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);

	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	Ellipse(hDC,
		RoundToInt(object.barycenter.x - object.radius),
		RoundToInt(object.barycenter.y - object.radius),
		RoundToInt(object.barycenter.x + object.radius),
		RoundToInt(object.barycenter.y + object.radius) );

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);
}

} // namespace NChevy
