// PurlerScene.cpp
//

#include "StdAfx.h"
#include "PurlerScene.h"

namespace NPurler {

#define TIMESLICE_PURLER	 30

#define PURLER_BOUNDARY_WIDTH_RATE		0.5
#define PURLER_BOUNDARY_HEIGHT_RATE		20.0
#define PURLER_BAR_LENGTH_RATE			0.3	// times with BOUNDARY WITH
#define PURLER_BAR_THICKNESS			2 // 2 pixels

#define PURLER_BAR_CENTER_Y				0.618

#define PURLER_BAR_MASS					1
#define PURLER_NAIL_MASS				10000	// almost infinite compare with the mass of the bar

#define PURLER_NAILS_Y_OFFSET_MIN		10
#define PURLER_NAILS_Y_OFFSET_MAX		100

#define PURLER_NAIL_RADIUS				2

#define PURLER_BOUNDARY_COLOR			RGB(64, 64, 64)
#define PURLER_BAR_COLOR				RGB(255, 255, 128)

static const COLORREF g_nail_colors[] = {
	RGB(255, 255, 255),
	RGB(128, 128, 128),
	RGB(255, 255,   0),
	RGB(255,   0, 255),
	RGB(  0, 255, 255),
	RGB(255,   0,   0),
	RGB(  0, 255,   0),
	RGB(  0,   0, 255),
};

#define PURLER_ACT_TIME_INTERVAL		((MEASURE_T)1)
#define PURLER_REBOUND_COEFF			(float(0.8))
#define PURLER_GRAVITATION				0.6

//////////////////////////////////////////////////////////////////////////
//  
//////////////////////////////////////////////////////////////////////////

BOOL CPurlerScene::Initialize(const RECT rect)
{
	m_rect = rect;

	const int width = rect.right - rect.left;
	const int height = rect.bottom - rect.top;
	const int nBoundaryWidth = RoundToInt(width * PURLER_BOUNDARY_WIDTH_RATE);
	const int nBoundaryHeight = RoundToInt(height * PURLER_BOUNDARY_HEIGHT_RATE);

	m_boundary.Initialize(nBoundaryWidth, nBoundaryHeight, PURLER_BOUNDARY_COLOR);
	m_bar.Initialize(nBoundaryWidth / 2.0, nBoundaryHeight - height*(1 - PURLER_BAR_CENTER_Y),
		nBoundaryWidth*PURLER_BAR_LENGTH_RATE, PURLER_BAR_THICKNESS,
		PURLER_BAR_MASS, PURLER_BAR_COLOR);
	// TODO: remove these lines of test code
	m_bar.av = 0;
	m_bar.ao = 3.0;
	m_bar.velocity = ZVelocity(7, -1);

	if (!InitNails(nBoundaryWidth, nBoundaryHeight))
		return FALSE;

	return TRUE;
}

BOOL CPurlerScene::Act()
{
	const MEASURE_T total_time = PURLER_ACT_TIME_INTERVAL;

	// act collision and moving
	MEASURE_T elapsed_time = 0;
	while(total_time - elapsed_time > 0)
	{
		const MEASURE_T elapsed_this_cycle =
			ActMoving(total_time - elapsed_time);
		if(elapsed_this_cycle < 0)
			return FALSE;
		elapsed_time += elapsed_this_cycle;
	}

	// TODO: gravation is disabled when debuging
//	// act gravation
//	ZForce force(0, m_bar.mass * PURLER_GRAVITATION);
//	m_bar.Accelerate(force, total_time);

	return TRUE;
}

MEASURE_T CPurlerScene::ActMoving(MEASURE_T total)
{
	const MEASURE_T INVALID_TIME = (MEASURE_T) (-1);

	// The collision between the bar and the nails
	vector<MEASURE_T> collision_time_i;
	for (unsigned int i=0; i<m_nails.size(); i++)
	{
		collision_time_i.push_back(m_bar.GetCollisionTime(m_nails[i]));
	}
	const int min_i = GetPositiveMinIndex(collision_time_i);
	const MEASURE_T t_min_i =
		(min_i>=0 ? collision_time_i[min_i] : INVALID_TIME);

	// The collision between the bar and the boundary
	const MEASURE_T collision_time_b =
		m_bar.GetCollisionTime(m_boundary);
	const MEASURE_T t_min_b = collision_time_b;

	if( (t_min_i < 0 || t_min_i >= total) &&
		(t_min_b < 0 || t_min_b >= total) )
	{
		// No collision in current timeslice
		m_bar.ActMoving(total);
		return total;
	}

	// time to collide
	const MEASURE_T t_min =
		(t_min_i>=0 && t_min_b>=0 ? min(t_min_i, t_min_b) : (t_min_i>=0 ? t_min_i : t_min_b));
	ASSERT(t_min >= 0); // may not ASSERT(t_min <= total_time), notice the float operation presision issue

	// move forward
	if (t_min > 0)
	{
		m_bar.ActMoving(t_min);
	}

	// act collision
	for (unsigned int i=0; i<m_nails.size(); i++)
	{
		if (t_min == collision_time_i[i])
		{
			m_bar.Collide(m_nails[i], PURLER_REBOUND_COEFF);
			m_nails[i].Stop(); // makes the nail to stop; in PurlerEffect, all nails always are still
		}
	}
	if (t_min == t_min_b)
	{
		m_bar.Collide(m_boundary, PURLER_REBOUND_COEFF);
	}

	return t_min;
}

BOOL CPurlerScene::Draw(HDC hDC) const
{
	const int width = m_rect.right - m_rect.left;
	const int height = m_rect.bottom - m_rect.top;

	const int xDrawOffset =
		RoundToInt((width - m_boundary.Width()) / 2);
	const int yDrawOffset =
		RoundToInt(height*PURLER_BAR_CENTER_Y - m_bar.barycenter.y);

	for(NailList::const_iterator iter = m_nails.begin();
		iter != m_nails.end();
		++iter)
	{
		const CPurlerNail &nail = *iter;
		if (!nail.Draw(hDC, xDrawOffset, yDrawOffset, m_rect))
			return FALSE;
	}

	if (!m_boundary.Draw(hDC, xDrawOffset, yDrawOffset, m_rect))
		return FALSE;

	if (!m_bar.Draw(hDC, xDrawOffset, yDrawOffset, m_rect))
		return FALSE;

	return TRUE;
}

BOOL CPurlerScene::IsEnd() const
{
	const int height = m_rect.bottom - m_rect.top;

	const int yDrawOffset =
		RoundToInt(height*PURLER_BAR_CENTER_Y - m_bar.barycenter.y);

	if (RoundToInt(yDrawOffset + m_boundary.Height()) < height)
		return TRUE;
	return FALSE;
}

DWORD CPurlerScene::GetTimeslice() const
{
	return TIMESLICE_PURLER;
}

////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////

BOOL CPurlerScene::InitNails(int nBoundaryWidth, int nBoundaryHeight)
{
	int y_last = -1 * PURLER_NAILS_Y_OFFSET_MIN;

	while(true)
	{
		const int y = y_last + random(PURLER_NAILS_Y_OFFSET_MIN,
			PURLER_NAILS_Y_OFFSET_MAX+1);
		if (y > nBoundaryHeight)
			break;

		const int radius = PURLER_NAIL_RADIUS;
		const int x = random(0+radius, nBoundaryWidth-radius);
		const double mass = PURLER_NAIL_MASS;
		const COLORREF color =
			g_nail_colors[random(countof(g_nail_colors))];

		m_nails.push_back(CPurlerNail(x, y, radius, mass, color));
		y_last = y;
	}

	return TRUE;
}

} // namespace NPurler
