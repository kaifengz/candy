// BumperScene.cpp
// 

#include "StdAfx.h"
#include "BumperScene.h"
#include "BumperAI.h"
#include "ZkfGraphicsAux.h"

#define BUMPER_GATE_HEIGHT_RATE                0.3
#define BUMPER_ROBOTS_PER_TEAM                 5
#define BUMPER_ROBOT_SIZE                      0.025
#define BUMPER_BALL_SIZE                       0.02
#define BUMPER_LEFT_COLOR                      RGB(192,64,192)
#define BUMPER_RIGHT_COLOR                     RGB(192,192,64)
#define BUMPER_BALL_COLOR                      RGB(128,128,128)

#define BUMPER_FIELD_LINE_WIDTH                1   // pixel
#define BUMPER_FIELD_LINE_COLOR                RGB(255,255,255)
#define BUMPER_CENTER_CIRCLE_RADIUS            0.1

#define BUMPER_TIMESLICE                       30
#define BUMPER_MAX_TIME                        (120 * 1000)

#define ACT_TIME_INTERVAL                      ((MEASURE_T)1)
#define REBOUND_COEFF                          (float(0.8))

namespace NBumper {

CBumperScene::CBumperScene() :
	m_left_ai(0),
	m_right_ai(0)
{
}

CBumperScene::~CBumperScene()
{
	if (m_left_ai)
		delete m_left_ai;
	if (m_right_ai)
		delete m_right_ai;
}

BOOL CBumperScene::Initialize(const RECT rect)
{
	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;

	m_rect = rect;
	m_boundary.Initialize((MEASURE_D)rect.left, (MEASURE_D)rect.top,
		(MEASURE_D)rect.right, (MEASURE_D)rect.bottom);
	m_gate_height = RoundToInt(height * BUMPER_GATE_HEIGHT_RATE);

	const int robot_size = RoundToInt(min(width, height) * BUMPER_ROBOT_SIZE);

	// create robots
	for (int team=0; team<2; ++team)
	{
		const BOOL left = (team == 0);
		CBumperRobots &robots = (left ? m_lefts : m_rights);
		robots.resize(BUMPER_ROBOTS_PER_TEAM);

		const int x = (left ? rect.left + robot_size * 2 : rect.right - robot_size * 2);
		const COLORREF color = (left ? BUMPER_LEFT_COLOR : BUMPER_RIGHT_COLOR);
		for (int i=0; i<BUMPER_ROBOTS_PER_TEAM; ++i)
		{
			const int y = rect.top + RoundToInt(height * (2*i+1) / 2.0 / BUMPER_ROBOTS_PER_TEAM);

			CBumperRobot &robot = robots[i];
			m_objects.push_back(&robot);

			robot.barycenter.x = x;
			robot.barycenter.y = y;
			robot.radius = robot_size;
		}
	}

	// init ball
	m_ball.barycenter = m_boundary.GetCenter();
	m_ball.radius = RoundToInt(min(width, height) * BUMPER_BALL_SIZE);
	m_objects.push_back(&m_ball);

	// init AI's
	m_left_ai  = new CBumperAI(m_boundary, m_lefts, m_rights, m_ball);
	m_right_ai = new CBumperAI(m_boundary, m_rights, m_lefts, m_ball);

	return TRUE;
}

BOOL CBumperScene::Act()
{
	CollideAndMove(m_objects, m_boundary, ACT_TIME_INTERVAL, 2*REBOUND_COEFF, REBOUND_COEFF);

	// TODO: check goal and fire skyrockets

	m_left_ai->RunAI();
	m_right_ai->RunAI();
	return TRUE;
}

BOOL CBumperScene::Draw(HDC hDC)
{
	HPEN hPen = CreatePen(PS_SOLID,
		BUMPER_FIELD_LINE_WIDTH, BUMPER_FIELD_LINE_COLOR);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

	const int center_x = (m_rect.left + m_rect.right) / 2;
	const int center_y = (m_rect.top + m_rect.bottom) / 2;

	// draw center line
	MoveToEx(hDC, center_x, m_rect.top, NULL);
	LineTo(hDC, center_x, m_rect.bottom);

	// draw center circle
	const int center_circle_radius =
		RoundToInt(min(m_rect.right - m_rect.left, m_rect.bottom - m_rect.top) * BUMPER_CENTER_CIRCLE_RADIUS);
	Ellipse(hDC,
		center_x - center_circle_radius,
		center_y - center_circle_radius,
		center_x + center_circle_radius,
		center_y + center_circle_radius);

	SelectObject(hDC, hOldBrush);
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);

	// TODO: draw gate and penalty area

	// draw robots and ball
	DrawObject(hDC, m_ball, BUMPER_BALL_COLOR);
	for (unsigned int i=0; i<m_lefts.size(); ++i)
		DrawObject(hDC, m_lefts[i], BUMPER_LEFT_COLOR);
	for (unsigned int i=0; i<m_rights.size(); ++i)
		DrawObject(hDC, m_rights[i], BUMPER_RIGHT_COLOR);

	return TRUE;
}

DWORD CBumperScene::GetTimeslice() const
{
	return BUMPER_TIMESLICE;
}

DWORD CBumperScene::GetMaxTime() const
{
	return BUMPER_MAX_TIME;
}

BOOL CBumperScene::DrawObject(HDC hDC, const CBumperObject &obj, COLORREF color)
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);

	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	BOOL bRet = Ellipse(
		hDC,
		RoundToInt(obj.barycenter.x - obj.radius),
		RoundToInt(obj.barycenter.y - obj.radius),
		RoundToInt(obj.barycenter.x + obj.radius),
		RoundToInt(obj.barycenter.y + obj.radius) );

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	return bRet;
}

} // namespace NBumper
