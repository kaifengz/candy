
#ifndef _3_BODY_SCENE_H__INCLUDED__2007_06_19_
#define _3_BODY_SCENE_H__INCLUDED__2007_06_19_

#include "ZkfGraphics.h"

namespace N3Body {

class C3BodyStar : protected ZMovableCircle
{
public:
	C3BodyStar();
	C3BodyStar(const C3BodyStar&);

	bool Initialize(double x, double y, double r, double m, COLORREF color);

	ZForce GetGravitation(const C3BodyStar&, double dCoeffGravitation);
	bool Accelerate(const ZForce& force, double duration_time);
	bool ActMoving(double duration_time);

	bool Draw(HDC) const;

	bool OutOfRange(const RECT &rect, bool bAllOut) const;
	bool IsTooNear(const C3BodyStar& another, double distance) const;

protected:
	COLORREF color;
};

class C3BodyScene
{
public:
	C3BodyScene();

	bool Initialize(const RECT rect);
	bool Act();
	bool Draw(HDC) const;

	bool SceneEnd() const;

	unsigned int GetTimeslice() const;
	unsigned int GetMaxTime() const;

protected:
	bool AddRandomStar();

	bool ActAffinity();
	bool ActMoving();

protected:
	typedef vector<C3BodyStar> StarList;
	StarList m_stars;

	RECT m_rect;
};

} // namespace N3Body

#endif // _PLANETARY_SCENE_H__INCLUDED__2007_06_19_
