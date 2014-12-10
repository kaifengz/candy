// HypocycloidEffect.cpp
//

#include "stdafx.h"
#include "HypocycloridWidget.h"
#include "ZkfGraphics.h"


#define THETA_STEP_MIN                 0.1
#define THETA_STEP_MAX                 0.2


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHypocycloridWidget::CHypocycloridWidget()
{
}

CHypocycloridWidget::~CHypocycloridWidget()
{
}

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

void CHypocycloridWidget::Initialize(unsigned int left, unsigned int top, unsigned int size)
{
	m_left_top.x = left;
	m_left_top.y = top;
	m_outer_radius = size/2;

	m_points.clear();
	m_theta = 0;

	m_theta_step = random(THETA_STEP_MAX, THETA_STEP_MIN);
	while (true)
	{
		m_inner_radius = random(m_outer_radius*2/3, m_outer_radius/3);
		if (gcd(m_outer_radius, m_inner_radius) == 1)
			break;
	}
}

void CHypocycloridWidget::Step()
{
	// 圆内螺线（内摆线）曲线参数方程：
	// http://en.wikipedia.org/wiki/Hypocycloid
	const double R = m_outer_radius;
	const double r = m_inner_radius;
	const double T = m_theta;

	ZPoint point;
	point.x = RoundToInt(m_left_top.x + R + (R - r) * cos(T) + r * cos((R-r)/r * T));
	point.y = RoundToInt(m_left_top.y + R + (R - r) * sin(T) - r * sin((R-r)/r * T));
	m_points.push_back(point);
	m_theta += m_theta_step;
}

void CHypocycloridWidget::Draw(HDC hDC)
{
	if (m_points.size() <= 1)
		return;

	MoveToEx(hDC, m_points[0].x, m_points[0].y, 0);
	for (unsigned int i=1; i<m_points.size(); ++i)
		LineTo(hDC, m_points[i].x, m_points[i].y);
}
