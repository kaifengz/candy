// PlanetaryScene.cpp

#include "StdAfx.h"
#include "3BodyScene.h"

namespace N3Body {

#define STAR_COLOR		RGB(192,192,0)
#define DEFAULT_COLOR	STAR_COLOR

#define COEFF_GRAVITATION	2

#define NEW_STAR_SIZE		20
#define NEW_STAR_SIZE_ERR	3
#define MASS_SIZE_COEFF		0.1

#define INIT_MIN_DISTANCE	200

#define STAR_NUMBER			3
#define NEW_STAR_TRY_NUMBER	128

#define ACT_TIME_INTERVAL		1.0
#define TIMESLICE_PLANETARY		30
#define MAX_TIME_PLANETARY		120*1000

////////////////////////////////////////////////////////////////////////////////
//	C3BodyStar
////////////////////////////////////////////////////////////////////////////////

C3BodyStar::C3BodyStar() :
	ZMovableCircle(),
	color(DEFAULT_COLOR)
{
}

C3BodyStar::C3BodyStar(const C3BodyStar& star) :
	ZMovableCircle(star),
	color(star.color)
{
}

bool C3BodyStar::Initialize(double x, double y, double r, double m, COLORREF color)
{
	this->barycenter = ZPos((MEASURE_D)x, (MEASURE_D)y);
	this->mass = (MEASURE_M)m;
	this->radius = (MEASURE_D)r;
	this->color = color;
	this->velocity = ZVelocity(0, 0);
	return true;
}

ZForce C3BodyStar::GetGravitation(const C3BodyStar& star, double dCoeffGravitation)
{
	return ZMovableCircle::GetGravitation(star, dCoeffGravitation);
}

bool C3BodyStar::Accelerate(const ZForce& force, double duration_time)
{
	ZMovableCircle::Accelerate(force, duration_time);
	return true;
}

bool C3BodyStar::ActMoving(double duration_time)
{
	ZMovableCircle::ActMoving((MEASURE_T)duration_time);
	return true;
}

bool C3BodyStar::Draw(HDC hDC) const
{
	HPEN hPen = CreatePen(PS_SOLID, 1, color);
	HBRUSH hBrush = CreateSolidBrush(color);

	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

	BOOL bRet = Ellipse(
		hDC,
		RoundToInt(barycenter.x - radius),
		RoundToInt(barycenter.y - radius),
		RoundToInt(barycenter.x + radius),
		RoundToInt(barycenter.y + radius) );

	SelectObject(hDC, hOldPen);
	SelectObject(hDC, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	return (bRet ? true : false);
}

bool C3BodyStar::OutOfRange(const RECT &rect, bool bAllOut) const
{
	const double left = (bAllOut ? (rect.left - radius) : (rect.left + radius));
	const double right = (bAllOut ? (rect.right + radius) : (rect.right - radius));
	const double top = (bAllOut ? (rect.top - radius) : (rect.top + radius));
	const double bottom = (bAllOut ? (rect.bottom + radius) : (rect.bottom - radius));

	return (barycenter.x<left || barycenter.x>=right ||
		barycenter.y<top || barycenter.y>=bottom);
}

bool C3BodyStar::IsTooNear(const C3BodyStar& another, double distance) const
{
	if(barycenter.GetDistance(another.barycenter) < radius+another.radius+distance)
		return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
//	C3BodyScene
////////////////////////////////////////////////////////////////////////////////

C3BodyScene::C3BodyScene()
{
}

bool C3BodyScene::Initialize(const RECT rect)
{
	m_rect = rect;

	for(int i=0; i<STAR_NUMBER; i++)
	{
		if(!AddRandomStar())
			return false;
	}

	return true;
}

bool C3BodyScene::AddRandomStar()
{
	for(int i=0; i<NEW_STAR_TRY_NUMBER; i++)
	{
		const double x = random(m_rect.left, m_rect.right);
		const double y = random(m_rect.top, m_rect.bottom);
		const double r = random(NEW_STAR_SIZE-NEW_STAR_SIZE_ERR, 
								NEW_STAR_SIZE+NEW_STAR_SIZE_ERR+1);
		const double m = r*r*r*MASS_SIZE_COEFF;

		C3BodyStar star;
		star.Initialize(x, y, r, m, STAR_COLOR);

		bool bAppropriate = true;
		for(vector<C3BodyStar>::const_iterator iter=m_stars.begin();
			iter!=m_stars.end();
			++iter)
		{
			if( star.IsTooNear(*iter, INIT_MIN_DISTANCE) ||
				star.OutOfRange(m_rect, false) )
			{
				bAppropriate = true;
				break;
			}
		}
		if(!bAppropriate)
			continue;

		m_stars.push_back(star);
		return true;
	}

	return false;
}

bool C3BodyScene::Act()
{
	if(!ActMoving())
		return false;
	if(!ActAffinity())
		return false;
	return true;
}

bool C3BodyScene::ActAffinity()
{
	for(vector<C3BodyStar>::iterator iter_d=m_stars.begin();
		iter_d!=m_stars.end();
		++iter_d)
	{
		C3BodyStar &star_d = (*iter_d);

		ZForce force;
		for(vector<C3BodyStar>::iterator iter_s=m_stars.begin();
			iter_s!=m_stars.end();
			++iter_s)
		{
			if(iter_d == iter_s)
				continue;

			const C3BodyStar &star_s = (*iter_s);
			if(star_d.IsTooNear(star_s, 0))
				continue;

			force += star_d.GetGravitation(star_s, COEFF_GRAVITATION);
		}

		star_d.Accelerate(force, ACT_TIME_INTERVAL);
	}

	return true;
}

bool C3BodyScene::ActMoving()
{
	for(vector<C3BodyStar>::iterator iter=m_stars.begin();
		iter!=m_stars.end();
		++iter)
	{
		C3BodyStar &star = (*iter);
		star.ActMoving(ACT_TIME_INTERVAL);
	}

	return true;
}

bool C3BodyScene::Draw(HDC hDC) const
{
	for(vector<C3BodyStar>::const_iterator iter=m_stars.begin();
		iter!=m_stars.end();
		++iter)
	{
		const C3BodyStar &star = (*iter);
		star.Draw(hDC);
	}

	return true;
}

bool C3BodyScene::SceneEnd() const
{
	int nOutCount = 0;

	for(vector<C3BodyStar>::const_iterator iter=m_stars.begin();
		iter!=m_stars.end();
		++iter)
	{
		const C3BodyStar &star = (*iter);
		if(star.OutOfRange(m_rect, true))
			nOutCount++;
	}

	if(nOutCount+1 >= STAR_NUMBER)
		return true;
	return false;
}

unsigned int C3BodyScene::GetTimeslice() const
{
	return TIMESLICE_PLANETARY;
}

unsigned int C3BodyScene::GetMaxTime() const
{
	return MAX_TIME_PLANETARY;
}

} // namespace N3Body
