
#ifndef GRAPHICS_H__INCLUDED__2007_06_06_
#define GRAPHICS_H__INCLUDED__2007_06_06_

#include <vector>

//////////////////////////////////////////////////////////////////////////
//  classes
//////////////////////////////////////////////////////////////////////////

class ZPos;
class ZDir;
	class ZOffset;
	class ZVelocity;
	class ZForce;
class ZGraphicObject;
	class ZCircle;
		class ZMovableCircle;
	class ZBar;
	class ZBoundary;

//////////////////////////////////////////////////////////////////////////
//  macros
//////////////////////////////////////////////////////////////////////////

typedef double MEASURE_D;	// the measurement of distance, may be "float" or "double"
typedef double MEASURE_T;	// the measurement of time, may be "float" or "double"
typedef double MEASURE_M;	// the measurement of mass, may be "float" or "double"

#define PI		  3.141592653589793

#define X_SPACE_TO_SCREEN(x, w) (x)
#define Y_SPACE_TO_SCREEN(y, h) ((h)-(y))

//////////////////////////////////////////////////////////////////////////
//  utilities
//////////////////////////////////////////////////////////////////////////

template <typename T>
inline T sq(T v) { return v*v; }

template <typename T>
int RoundToInt(T v)
{
	return (int)round(v);
}

template <typename T>
inline bool DecimalEqual(T v1, T v2, T err = 1e-5)
{
	return abs(v1 - v2) < err;
}

int GetPositiveMinIndex(const std::vector<float> &);
int GetPositiveMinIndex(const std::vector<double> &);

inline double sind(double d) { return sin(d); }
inline double cosd(double d) { return cos(d); }

double coord2radian(double x, double y);
inline double radian_normalize(double radian)
{
	return radian >= 0.0 ? fmod(radian, PI) : (PI - fmod(-radian, PI));
}
inline double radian_distance(double from, double to)
{
	from = radian_normalize(from);
	to   = radian_normalize(to);
	return radian_normalize(to+PI - from);
}

//////////////////////////////////////////////////////////////////////////
//  classes
//////////////////////////////////////////////////////////////////////////

class ZPos
{
public:
	MEASURE_D x, y;

public:
	ZPos(MEASURE_D x=0, MEASURE_D y=0);
	ZPos(const ZPos&);

public:
	// operator
	void Move(const ZOffset&);
	void Move(MEASURE_D x, MEASURE_D y);
	void MoveByVelocity(MEASURE_T time, const ZVelocity &velocity);

	ZPos operator+ (const ZOffset&) const;
	ZOffset operator- (const ZPos&) const;

	// attribute
	MEASURE_D GetDistance(const ZPos&) const;
};

class ZDir
{
public:
	MEASURE_D dx, dy;

public:
	ZDir(MEASURE_D dx=0, MEASURE_D dy=0);
	ZDir(const ZDir&);

	ZDir& FromRadian(MEASURE_D radian);
	MEASURE_D ToRadian() const;

protected:
	void Normalize();
};

class ZOffset : public ZDir
{
public:
	ZOffset(MEASURE_D cx=0, MEASURE_D cy=0);
	ZOffset(const ZOffset&);
};

class ZVelocity : public ZDir
{
public:
	ZVelocity(MEASURE_D vx=0, MEASURE_D vy=0);
	ZVelocity(const ZVelocity&);

	ZVelocity operator+ () const;
	ZVelocity operator- () const;
	ZVelocity operator+ (const ZVelocity&) const;
	ZVelocity operator- (const ZVelocity&) const;
	ZVelocity& operator+= (const ZVelocity&);
	ZVelocity& operator-= (const ZVelocity&);
	ZVelocity operator* (double) const;
	ZVelocity operator/ (double) const;
	ZVelocity& operator*= (double);
	ZVelocity& operator/= (double);

	ZOffset ToOffset(MEASURE_T time) const;
	MEASURE_D ToScalar() const;
	ZVelocity& SetScalar(MEASURE_D scalar_velocity);
};

class ZForce : public ZDir
{
public:
	ZForce(MEASURE_D fx=0, MEASURE_D fy=0);
	ZForce(const ZForce&);

	ZForce operator+ () const;
	ZForce operator- () const;
	ZForce operator+ (const ZForce&) const;
	ZForce operator- (const ZForce&) const;
	ZForce& operator+= (const ZForce&);
	ZForce& operator-= (const ZForce&);
	ZForce operator* (double) const;
	ZForce operator/ (double) const;
	ZForce& operator*= (double);
	ZForce& operator/= (double);
};

class ZGraphicObject
{
public:
	ZPos barycenter;
	ZVelocity velocity;
	MEASURE_M mass;

public:
	ZGraphicObject();
	ZGraphicObject(const ZPos&, const ZVelocity&, MEASURE_M=1.0);
	ZGraphicObject(const ZGraphicObject&);
	virtual ~ZGraphicObject();

	// Get the gravitation force which act on `this' ZGraphicObject
	ZForce GetGravitation(const ZGraphicObject&, double dCoeffGravitation=1.0) const;

	ZForce GetAffinity(const ZGraphicObject&, MEASURE_D scalar_affinity) const;
	ZForce GetExclusion(const ZGraphicObject&, MEASURE_D scalar_exclusion) const;

	void Accelerate(const ZForce& force, MEASURE_T duration,
		MEASURE_D scalar_velocity_limit=0);
	void Decelerate(MEASURE_D force, MEASURE_T duration);
};

class ZCircle : public ZGraphicObject
{
public:
	MEASURE_D radius;

public:
	ZCircle();
	ZCircle(const ZPos&, MEASURE_D r=0);
	ZCircle(const ZCircle&);
	virtual ~ZCircle();

public:
	bool OverlappedWith(const ZCircle &) const;
};

class ZMovableCircle : public ZCircle
{
public:
	ZMovableCircle();
	ZMovableCircle(const ZMovableCircle&);
	virtual ~ZMovableCircle();

public:
	MEASURE_T GetCollisionTime(const ZMovableCircle &) const;
	MEASURE_T GetCollisionTime(const ZBoundary &) const;
	void Collide(ZMovableCircle &, float fReboundCoeff);
	void Collide(const ZBoundary &, float fReboundCoeff);
	void ActMoving(MEASURE_T);

protected:
	static void RightCollide(
		const MEASURE_D v1, const MEASURE_D v2,
		const MEASURE_M m1, const MEASURE_M m2,
		const float fReboundCoeff,
		MEASURE_D &va1, MEASURE_D &va2);

private:
	enum CollisionSide {CS_NONE=0, CS_LEFT, CS_TOP, CS_RIGHT, CS_BOTTOM};
	bool GetCollisionParam(const ZBoundary&, MEASURE_T &time, CollisionSide &side) const;
};

class ZBar : public ZGraphicObject
{
public:
	ZBar();
	ZBar(const ZBar&);
	virtual ~ZBar();

public:
	MEASURE_D ao; // angular offset
	MEASURE_D av; // angular velocity
	MEASURE_D length;

public:
	MEASURE_T GetCollisionTime(const ZMovableCircle &) const;
	MEASURE_T GetCollisionTime(const ZBoundary &) const;
	void Collide(ZMovableCircle &, float fReboundCoeff);
	void Collide(const ZBoundary &, float fReboundCoeff);
	void ActMoving(MEASURE_T);

private:
	enum CollisionSide {CS_NONE=0, CS_LEFT, CS_TOP, CS_RIGHT, CS_BOTTOM};
	bool GetCollisionParam(const ZBoundary&, MEASURE_T &time, CollisionSide &side) const;
	static MEASURE_T GetCollisionTimeToBoundary(CollisionSide side,
		double length, double z0, double zz, double vz, double cur_ao, double av);
};

class ZBoundary : public ZGraphicObject
{
public:
	MEASURE_D left, right;
	MEASURE_D top, bottom;

public:
	ZBoundary(MEASURE_D l=0, MEASURE_D t=0, MEASURE_D r=0, MEASURE_D b=0);
	ZBoundary(const ZBoundary&);
	virtual ~ZBoundary();

	void Initialize(MEASURE_D l=0, MEASURE_D t=0, MEASURE_D r=0, MEASURE_D b=0);

	ZPos GetCenter() const;
	inline MEASURE_D Width() const { return right - left; }
	inline MEASURE_D Height() const { return bottom - top; }
};

#endif // GRAPHICS_H__INCLUDED__2007_06_06_
