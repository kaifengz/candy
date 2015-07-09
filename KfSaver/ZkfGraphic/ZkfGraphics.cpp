// ZkfGraphics.cpp
//

#include "StdAfx.h"
#include "ZkfGraphics.h"
#include "util.h"

#include <math.h>

//////////////////////////////////////////////////////////////////////////
//  ZPos
//////////////////////////////////////////////////////////////////////////

ZPos::ZPos(MEASURE_D _x/* =0 */, MEASURE_D _y/* =0 */) :
	x(_x), y(_y)
{
}

ZPos::ZPos(const ZPos& pos) :
	x(pos.x), y(pos.y)
{
}

void ZPos::Move(const ZOffset& offset)
{
	this->x += offset.dx;
	this->y += offset.dy;
}

void ZPos::Move(MEASURE_D x, MEASURE_D y)
{
	this->x += x;
	this->y += y;
}

void ZPos::MoveByVelocity(MEASURE_T time, const ZVelocity& velocity)
{
	Move(velocity.ToOffset(time));
}

ZPos ZPos::operator +(const ZOffset& offset) const
{
	ZPos pos(*this);
	pos.Move(offset);
	return pos;
}

MEASURE_D ZPos::GetDistance(const ZPos& pos) const
{
	const MEASURE_D xDiff = x - pos.x;
	const MEASURE_D yDiff = y - pos.y;
	return sqrt( xDiff * xDiff + yDiff * yDiff );
}

//////////////////////////////////////////////////////////////////////////
//  ZDir
//////////////////////////////////////////////////////////////////////////

ZDir::ZDir(MEASURE_D _dx/* =1 */, MEASURE_D _dy/* =0 */) :
	dx(_dx),
	dy(_dy)
{
}

ZDir::ZDir(const ZDir& dir) :
	dx(dir.dx),
	dy(dir.dy)
{
}

//////////////////////////////////////////////////////////////////////////
//  ZOffset
//////////////////////////////////////////////////////////////////////////

ZOffset::ZOffset(MEASURE_D cx/* =0 */, MEASURE_D cy/* =0 */) :
	ZDir(cx, cy)
{
}

ZOffset::ZOffset(const ZOffset& offset) :
	ZDir(offset)
{
}

////////////////////////////////////////////////////////////////////////////////
//	ZVelocity
////////////////////////////////////////////////////////////////////////////////

ZVelocity::ZVelocity(MEASURE_D vx/* =0 */, MEASURE_D vy/* =0 */) :
	ZDir(vx, vy)
{
}

ZVelocity::ZVelocity(const ZVelocity& velocity) :
	ZDir(velocity)
{
}

ZVelocity ZVelocity::operator+ () const
{
	return ZVelocity(*this);
}

ZVelocity ZVelocity::operator- () const
{
	return ZVelocity(-(this->dx), -(this->dy));
}

ZVelocity ZVelocity::operator+ (const ZVelocity& another) const
{
	return ZVelocity(this->dx + another.dx, this->dy + another.dy);
}

ZVelocity ZVelocity::operator- (const ZVelocity& another) const
{
	return ZVelocity(this->dx - another.dx, this->dy - another.dy);
}

ZVelocity& ZVelocity::operator+= (const ZVelocity& another)
{
	this->dx += another.dx;
	this->dy += another.dy;
	return *this;
}

ZVelocity& ZVelocity::operator-= (const ZVelocity& another)
{
	this->dx -= another.dx;
	this->dy -= another.dy;
	return *this;
}

ZVelocity ZVelocity::operator* (double coeff) const
{
	return ZVelocity((MEASURE_D)(this->dx * coeff), (MEASURE_D)(this->dy * coeff));
}

ZVelocity ZVelocity::operator/ (double coeff) const
{
	return ZVelocity((MEASURE_D)(this->dx / coeff), (MEASURE_D)(this->dy / coeff));
}

ZVelocity& ZVelocity::operator*= (double coeff)
{
	this->dx = (MEASURE_D)(this->dx * coeff);
	this->dy = (MEASURE_D)(this->dy * coeff);
	return *this;
}

ZVelocity& ZVelocity::operator/= (double coeff)
{
	this->dx = (MEASURE_D)(this->dx / coeff);
	this->dy = (MEASURE_D)(this->dy / coeff);
	return *this;
}

ZOffset ZVelocity::ToOffset(MEASURE_T time) const
{
	return ZOffset((MEASURE_D)(this->dx * time), (MEASURE_D)(this->dy * time));
}

MEASURE_D ZVelocity::ToScalar() const
{
	return (MEASURE_D)(sqrt(sq(this->dx) + sq(this->dy)));
}

ZVelocity& ZVelocity::SetScalar(MEASURE_D scalar_velocity)
{
	const double org_scalar_v = sqrt(sq(this->dx) + sq(this->dy));
	ASSERT(org_scalar_v != 0);

	this->dx = (MEASURE_D)(this->dx * scalar_velocity / org_scalar_v);
	this->dy = (MEASURE_D)(this->dy * scalar_velocity / org_scalar_v);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
//	ZForce
////////////////////////////////////////////////////////////////////////////////

ZForce::ZForce(MEASURE_D fx/* =0 */, MEASURE_D fy/* =0 */) :
	ZDir(fx, fy)
{
}

ZForce::ZForce(const ZForce& force) :
	ZDir(force)
{
}

ZForce ZForce::operator+ () const
{
	return ZForce(*this);
}

ZForce ZForce::operator- () const
{
	return ZForce(-(this->dx), -(this->dy));
}

ZForce ZForce::operator+ (const ZForce& another) const
{
	return ZForce(this->dx + another.dx, this->dy + another.dy);
}

ZForce ZForce::operator- (const ZForce& another) const
{
	return ZForce(this->dx - another.dx, this->dy - another.dy);
}

ZForce& ZForce::operator+= (const ZForce& another)
{
	this->dx += another.dx;
	this->dy += another.dy;
	return *this;
}

ZForce& ZForce::operator-= (const ZForce& another)
{
	this->dx -= another.dx;
	this->dy -= another.dy;
	return *this;
}

ZForce ZForce::operator* (double coeff) const
{
	return ZForce((MEASURE_D)(this->dx * coeff), (MEASURE_D)(this->dy * coeff));
}

ZForce ZForce::operator/ (double coeff) const
{
	return ZForce((MEASURE_D)(this->dx / coeff), (MEASURE_D)(this->dy / coeff));
}

ZForce& ZForce::operator*= (double coeff)
{
	this->dx = (MEASURE_D)(this->dx * coeff);
	this->dy = (MEASURE_D)(this->dy * coeff);
	return *this;
}

ZForce& ZForce::operator/= (double coeff)
{
	this->dx = (MEASURE_D)(this->dx / coeff);
	this->dy = (MEASURE_D)(this->dy / coeff);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
//	ZGraphicObject
////////////////////////////////////////////////////////////////////////////////

ZGraphicObject::ZGraphicObject() :
	barycenter(0, 0),
	velocity(0, 0),
	mass(1.0)
{
}

ZGraphicObject::ZGraphicObject(const ZPos& _barycenter,
							   const ZVelocity& _velocity,
							   MEASURE_M _mass/*=1.0*/) :
	barycenter(_barycenter),
	velocity(_velocity),
	mass(_mass)
{
}

ZGraphicObject::ZGraphicObject(const ZGraphicObject& gobj) :
	barycenter(gobj.barycenter),
	velocity(gobj.velocity),
	mass(gobj.mass)
{
}

ZGraphicObject::~ZGraphicObject()
{
}

ZForce ZGraphicObject::GetGravitation(const ZGraphicObject& another,
									  double dCoeffGravitation/*=1.0*/) const
{
	const double dx = another.barycenter.x - this->barycenter.x;
	const double dy = another.barycenter.y - this->barycenter.y;
	const double distance_sq = sq(dx) + sq(dy);
	const double distance = sqrt(distance_sq);
	const double force = dCoeffGravitation * this->mass * another.mass / distance_sq;
	return ZForce((MEASURE_D)(force*dx/distance), (MEASURE_D)(force*dy/distance));
}

ZForce ZGraphicObject::GetAffinity(const ZGraphicObject& another,
								   MEASURE_D scalar_affinity) const
{
	const double dx = another.barycenter.x - this->barycenter.x;
	const double dy = another.barycenter.y - this->barycenter.y;
	const double distance_sq = sq(dx) + sq(dy);
	const double distance = sqrt(distance_sq);
	const double force = scalar_affinity;
	return ZForce((MEASURE_D)(force*dx/distance), (MEASURE_D)(force*dy/distance));
}

ZForce ZGraphicObject::GetExclusion(const ZGraphicObject& another,
									MEASURE_D scalar_exclusion) const
{
	const double dx = another.barycenter.x - this->barycenter.x;
	const double dy = another.barycenter.y - this->barycenter.y;
	const double distance_sq = sq(dx) + sq(dy);
	const double distance = sqrt(distance_sq);
	const double force = -scalar_exclusion;
	return ZForce((MEASURE_D)(force*dx/distance), (MEASURE_D)(force*dy/distance));
}

void ZGraphicObject::Accelerate(const ZForce& force, MEASURE_T duration,
	MEASURE_D scalar_velocity_limit /* =0 */)
{
	ASSERT(duration >= 0);

	if(duration == 0 || (force.dx==0 && force.dy==0))
		return;

	velocity.dx = (MEASURE_D)(velocity.dx + force.dx * duration / mass);
	velocity.dy = (MEASURE_D)(velocity.dy + force.dy * duration / mass);

	if(scalar_velocity_limit>0 && velocity.ToScalar()>scalar_velocity_limit)
		velocity.SetScalar(scalar_velocity_limit);
}

void ZGraphicObject::Decelerate(MEASURE_D force, MEASURE_T duration)
{
	ASSERT(force >= 0);
	ASSERT(duration >= 0);

	if (force == 0 || duration == 0)
		return;

	MEASURE_D dd = sqrt(sq(velocity.dx) + sq(velocity.dy));
	if (dd > 0.0)
	{
		const MEASURE_D ddx = velocity.dx/dd * force * duration / mass;
		const MEASURE_D ddy = velocity.dy/dd * force * duration / mass;

		if (abs(velocity.dx) <= abs(ddx))
			velocity.dx = 0;
		else
			velocity.dx -= ddx;

		if (abs(velocity.dy) <= abs(ddy))
			velocity.dy = 0;
		else
			velocity.dy -= ddy;
	}
}

//////////////////////////////////////////////////////////////////////////
//  ZCircle
//////////////////////////////////////////////////////////////////////////

ZCircle::ZCircle() :
	ZGraphicObject(),
	radius(0)
{
}

ZCircle::ZCircle(const ZPos& pos, MEASURE_D r/* =0 */) :
	ZGraphicObject(),
	radius(r)
{
	barycenter = pos;
}

ZCircle::ZCircle(const ZCircle& circle) :
	ZGraphicObject(circle),
	radius(circle.radius)
{
}

ZCircle::~ZCircle()
{
}

bool ZCircle::OverlappedWith(const ZCircle &another) const
{
	return barycenter.GetDistance(another.barycenter) < radius + another.radius;
}

//////////////////////////////////////////////////////////////////////////
//  ZMovableCircle
//////////////////////////////////////////////////////////////////////////

ZMovableCircle::ZMovableCircle()
{
}

ZMovableCircle::ZMovableCircle(const ZMovableCircle& mcircle) :
	ZCircle(mcircle)
{
}

ZMovableCircle::~ZMovableCircle()
{
}

MEASURE_T ZMovableCircle::GetCollisionTime(const ZMovableCircle &another) const
{
	const MEASURE_T WILL_NOT_COLLIDE = -1;

	const double x1 = this->barycenter.x;
	const double y1 = this->barycenter.y;
	const double vx1 = this->velocity.dx;
	const double vy1 = this->velocity.dy;
	const double r1 = this->radius;
	const double x2 = another.barycenter.x;
	const double y2 = another.barycenter.y;
	const double vx2 = another.velocity.dx;
	const double vy2 = another.velocity.dy;
	const double r2 = another.radius;

	// 由于：
	// x1(t) = x1 + vx1 * t		// x1(t) 为 this 圆心经过时间t后的x坐标，下同
	// y1(t) = y1 + vy1 * t		// y1(t) 为 this 圆心经过时间t后的x坐标，下同
	// x2(t) = x2 + vx2 * t		// x1(t) 为 another 圆心经过时间t后的x坐标，下同
	// y2(t) = y2 + vy2 * t		// y1(t) 为 another 圆心经过时间t后的x坐标，下同
	// 需要解方程：（即经过时间t后，两个圆的圆心距离等于该两圆的半径和）
	// SQUARE(x1(t)-x2(t)) + SQUARE(y1(t)-t2(t)) = SQUARE(r1+r2)
	const double xd		= x1 - x2;
	const double vxd	= vx1 - vx2;
	const double yd		= y1 - y2;
	const double vyd	= vy1 - vy2;
	const double R		= sq(r1 + r2);

	// 换元后，方程为：
	// SQUARE(xd + vxd*t) + SQUARE(yd + vyd*t) = R
	const double a = sq(vxd) + sq(vyd); // a为非负数
	const double b = 2 * (xd * vxd + yd * vyd);
	const double c = sq(xd) + sq(yd) - R;

	// 再次换元后，方程为：
	// a * SQUARE(t) + b * t + c = 0;

	// 1. 如果该方程无解，说明不可能碰撞
	// 2. 如果方程有无数解（即a,b,c全为零），则两圆方向速度全相同，并且始终处于相切的位置
	// 3. 如果方程有两个负解，说明不可能碰撞
	// 4. 如果方程有正解、负解各一个，说明两圆已经处于相交位置（可能由于计算误差所致），则：
	//    4.1. 如果负解绝对值比较大，则不发生碰撞
	//    4.2. 如果正解绝对值表较大，则立刻发生碰撞
	//    4.3. 正负解绝对值相等的情况可随意处理，此作不发生碰撞处理
	// 5. 如果方程有两个正解，则说明将于某时刻碰撞

	if(a == 0 && b == 0) // 由以上换元可知：若a为零，则b必为零！
	{
		// 不可能发生碰撞，包含以上第2种情况，以及部分第1种情况
		return WILL_NOT_COLLIDE;
	}
	ASSERT(a != 0);

	const double delta = b*b - 4*a*c;
	if(delta < 0) // 第1种情况
		return WILL_NOT_COLLIDE;

	const double t1 = (-b-sqrt(delta)) / (2*a); // t1是较小的解
	const double t2 = (-b+sqrt(delta)) / (2*a); // t2是较大的解
	ASSERT(t1 <= t2);

	if(t1 <= 0 && t2 <= 0) // 第3种情况
		return WILL_NOT_COLLIDE;

	if(t1 <= 0 && t2 >= 0) // 第4种情况
	{
		if(abs(t1) > abs(t2)) // 第4.1、4.3种情况
			return WILL_NOT_COLLIDE;
		else // 第4.2种情况
			return 0;
	}

	return (MEASURE_T)t1; // 第5种情况，因为t1总小于t2
}

void ZMovableCircle::Collide(ZMovableCircle &another, float fReboundCoeff)
{
	const double x1 = this->barycenter.x;
	const double y1 = this->barycenter.y;
	const double r1 = this->radius;
	const double vx1 = this->velocity.dx;
	const double vy1 = this->velocity.dy;
	const double m1 = this->mass;
	const double x2 = another.barycenter.x;
	const double y2 = another.barycenter.y;
	const double r2 = another.radius;
	const double vx2 = another.velocity.dx;
	const double vy2 = another.velocity.dy;
	const double m2 = another.mass;

	// 两圆相碰时应当处于相切位置（如果不考虑计算误差的话），那么根据简化的碰撞
	// 模型，两圆在切线方向的速度分量保持不变，在法线方向的速度分量相互交换并且
	// 衰减（或增值，视fReboundCoeff大小而定），根据两圆相互的位置关系，切线方向
	// 可能为正对角线方向或副对角线方向

	const double dx = abs(x1 - x2);
	const double dy = abs(y1 - y2);
	const double d_2 = sq(dx) + sq(dy);
	ASSERT(d_2 > 0);
	const double d = sqrt(d_2);

	const int x_sign = (x1 >= x2 ? 1 : -1);
	const int y_sign = (y1 >= y2 ? 1 : -1);
	const bool same_sign = (x_sign == y_sign);

	// p为切线方向的速度分量，q为法线方向的速度分量
	const double p1 = (same_sign ? ((vx1 * (dy/d)) + (-vy1 * (dx/d))) : (( vx1 * (dy/d)) + (vy1 * (dx/d))));
	const double q1 = (same_sign ? ((vx1 * (dx/d)) + ( vy1 * (dy/d))) : ((-vx1 * (dx/d)) + (vy1 * (dy/d))));
	const double p2 = (same_sign ? ((vx2 * (dy/d)) + (-vy2 * (dx/d))) : (( vx2 * (dy/d)) + (vy2 * (dx/d))));
	const double q2 = (same_sign ? ((vx2 * (dx/d)) + ( vy2 * (dy/d))) : ((-vx2 * (dx/d)) + (vy2 * (dy/d))));

	// p为切线方向的速度分量，保持不变
	const double pa1 = p1;
	const double pa2 = p2;

	// q为法线方向的速度分量，执行正碰撞
	double qa1=0, qa2=0;
	RightCollide(q1, q2, m1, m2, fReboundCoeff, qa1, qa2);

	// 碰撞后亦有如下方程式
	// pa1 = (same_sign ? ((vxa1 * (dy/d)) + (-vya1 * (dx/d))) : (( vxa1 * (dy/d)) + (vya1 * (dx/d))));
	// qa1 = (same_sign ? ((vxa1 * (dx/d)) + ( vya1 * (dy/d))) : ((-vxa1 * (dx/d)) + (vya1 * (dy/d))));
	// pa2 = (same_sign ? ((vxa2 * (dy/d)) + (-vya2 * (dx/d))) : (( vxa2 * (dy/d)) + (vya2 * (dx/d))));
	// qa2 = (same_sign ? ((vxa2 * (dx/d)) + ( vya2 * (dy/d))) : ((-vxa2 * (dx/d)) + (vya2 * (dy/d))));
	// 解此方程得：
	const double vxa1 = same_sign ? ((qa1*d*dx + pa1*d*dy) / d_2) : ((pa1*d*dy - qa1*d*dx) / d_2);
	const double vya1 = same_sign ? ((qa1*d*dy - pa1*d*dx) / d_2) : ((pa1*d*dx + qa1*d*dy) / d_2);
	const double vxa2 = same_sign ? ((qa2*d*dx + pa2*d*dy) / d_2) : ((pa2*d*dy - qa2*d*dx) / d_2);
	const double vya2 = same_sign ? ((qa2*d*dy - pa2*d*dx) / d_2) : ((pa2*d*dx + qa2*d*dy) / d_2);

	this->velocity = ZVelocity((MEASURE_D)vxa1, (MEASURE_D)vya1);
	another.velocity = ZVelocity((MEASURE_D)vxa2, (MEASURE_D)vya2);
}

void ZMovableCircle::RightCollide(
	const MEASURE_D v1, const MEASURE_D v2,
	const MEASURE_M m1, const MEASURE_M m2,
	const float fReboundCoeff,
	MEASURE_D &va1, MEASURE_D &va2)
{
	ASSERT(m1 > 0 && m2 > 0);

	// 正碰撞前后满足如下方程组：
	// m1*v1 + m2*v2 = m1*va1 + m2 * va2
	// f*(m1*v1*v1 + m2*v2*v2) = m1*va1*va1 + m2*va2*va2
	// 
	// 设v1k=v1-vk，v2k=v2-vk，va1k=va1-vk，va2k=va2-vk
	// 其中vk满足：
	// m1*(v1-vk) + m2*(v2-vk) = 0  ===>  m1*v1 + m2*v2 = (m1+m2)*vk
	// 
	// 从物理意义上讲，vk即是两圆在碰撞前的重心的运动速度（把两圆视为一个整体）
	// 由动量守恒定理可知，两圆重心的运动速度碰撞前后不变
	const double vk = (m1*v1 + m2*v2) / (m1+m2);
	const double v1k = v1 - vk;
	const double v2k = v2 - vk;

	// 换元后有：
	// m1*va1k + m2*va2k = 0
	// m1*va1k*va2k + m2*va2k*va2k = f*(m1*v1k*v1k + m2*v2k*v2k)
	// 
	// 令EE = c*(m1*v1k*v1k + m2*v2k*v2k)
	const double EE = fReboundCoeff * (m1*v1k*v1k + m2*v2k*v2k);

	// 解方程组得：
	const double va1k_sq = m2*EE / ((m1+m2)*m1);
	const double va2k_sq = m1*EE / ((m1+m2)*m2);

	// 相对于两圆的重心的运动速度而言，某圆在碰撞前后的运动方向必然相反
	const double va1k = (v1k >=0 ? -1 : 1) * sqrt(va1k_sq);
	const double va2k = (v2k >=0 ? -1 : 1) * sqrt(va2k_sq);

	va1 = va1k + vk;
	va2 = va2k + vk;
}

MEASURE_T ZMovableCircle::GetCollisionTime(const ZBoundary& boundary) const
{
	MEASURE_T time = -1;
	CollisionSide side_dummy;

	VERIFY(GetCollisionParam(boundary, time, side_dummy));
	return time;
}

void ZMovableCircle::Collide(const ZBoundary &boundary, float fReboundCoeff)
{
	// 满足能量守恒定律和动量守恒定律，碰撞以后：
	// 碰撞方向的速度分量将折转方向，数值上衰减

	MEASURE_T dummy;
	CollisionSide side = CS_NONE;

	VERIFY(GetCollisionParam(boundary, dummy, side));

	double vx = this->velocity.dx;
	double vy = this->velocity.dy;
	const double coeff = sqrt(fReboundCoeff);

	ASSERT(coeff > 0);
//	ASSERT(coeff <= 1.0); // 假设其可以大于一

	switch(side)
	{
	case CS_LEFT:
	case CS_RIGHT:
		vx *= -coeff;
		break;

	case CS_TOP:
	case CS_BOTTOM:
		vy *= -coeff;
		break;

	case CS_NONE:
	default:
		ASSERT(0);
		break;
	}

	this->velocity = ZVelocity((MEASURE_D)vx, (MEASURE_D)vy);
}

void ZMovableCircle::ActMoving(MEASURE_T time)
{
	barycenter.MoveByVelocity(time, this->velocity);
}

bool ZMovableCircle::GetCollisionParam(const ZBoundary &boundary, MEASURE_T &time, CollisionSide &side) const
{
	const double x = this->barycenter.x;
	const double y = this->barycenter.y;
	const double vx = this->velocity.dx;
	const double vy = this->velocity.dy;
	const double r = this->radius;

	const double left   = boundary.left + r;
	const double top    = boundary.top + r;
	const double right  = boundary.right  - r;
	const double bottom = boundary.bottom - r;

	if(x < left && vx < 0) {
		side = CS_LEFT;
		time = 0;
		return true;
	} else if(y < top && vy < 0) {
		side = CS_TOP;
		time = 0;
		return true;
	} else if(x > right && vx > 0) {
		side = CS_RIGHT;
		time = 0;
		return true;
	} else if(y > bottom && vy > 0) {
		side = CS_BOTTOM;
		time = 0;
		return true;
	}

	const double time_left   = (vx<0 ? (left   - x)/vx : -1);
	const double time_top    = (vy<0 ? (top    - y)/vy : -1);
	const double time_right  = (vx>0 ? (right  - x)/vx : -1);
	const double time_bottom = (vy>0 ? (bottom - y)/vy : -1);

	const double time_x = (time_left<0 ? (time_right<0 ? -1 : time_right) : time_left);
	const double time_y = (time_top<0 ? (time_bottom<0 ? -1 : time_bottom) : time_top);
	const double time__ = (time_x<0 ? (time_y<0 ? -1 : time_y) : time_x);

	time = (MEASURE_T)time__;

	if(time__ >= 0)
	{
		if(time__ == time_left)
			side = CS_LEFT;
		else if(time__ == time_top)
			side = CS_TOP;
		else if(time__ == time_right)
			side = CS_RIGHT;
		else if(time__ == time_bottom)
			side = CS_BOTTOM;
		else
			ASSERT(0);
	}
	else
		side = CS_NONE;

	return true;
}

//////////////////////////////////////////////////////////////////////////
//  ZBar
//////////////////////////////////////////////////////////////////////////

ZBar::ZBar()
{
}

ZBar::ZBar(const ZBar& bar) :
	ZGraphicObject(bar),
	ao(bar.ao),
	av(bar.av),
	length(bar.length)
{
}

ZBar::~ZBar()
{
}

MEASURE_T ZBar::GetCollisionTime(const ZMovableCircle &mcircle) const
{
	const MEASURE_T WILL_NOT_COLLIDE = -1;

	const double x1 = this->barycenter.x;
	const double y1 = this->barycenter.y;
	const double ao1 = radian_normalize(this->ao);
	const double vx1 = this->velocity.dx;
	const double vy1 = this->velocity.dy;
	const double av1 = this->av;
	const double length = this->length;
	const double x2 = mcircle.barycenter.x;
	const double y2 = mcircle.barycenter.y;
	const double vx2 = mcircle.velocity.dx;
	const double vy2 = mcircle.velocity.dy;
	// XXX 此处忽略mcircle的大小，把它视为一个质点

	// 平移坐标系，使得新坐标系以棒中心为圆点，则有mcircle新的坐标和速度：
	const double x0 = x2 - x1;
	const double y0 = y2 - y1;
	const double vx0 = vx2 - vx1;
	const double vy0 = vy2 - vy1;

	if (vx0 == 0.0 && vy0 == 0.0)
	{
		// 若dvx，dvy均为零，则mcircle相对于棒静止
		// TODO:
	}
	else
	{
		// 否则mcircle的移动轨迹（为一直线）方程为：
		// vy0 * x - vx0 * y - x0 * vy0 + y0 * vx0 = 0
		// 原点到此直线的距离为：
		// abs(x0 * vy0 - y0 * vx0) / sqrt(vx0^2 + vy0^2)
		// 若距离大于length/2，则碰撞不可能发生
		// 若距离等于length/2，此处也视为不发生碰撞
		const double dd = sq(x0 * vy0 - y0 * vx0) / (sq(vx0) + sq(vy0));
		if (dd >= sq(length/2))
			return WILL_NOT_COLLIDE;

		// 该直线与以原点为中心、棒长为直径的圆（设为圆L）的交点满足方程组：
		// x = vx0 * t + x0
		// y = vy0 * t + y0
		// x^2 + y^2 = r^2      其中r = length/2
		// 化简可得二次方程：
		// (vx0^2+vy0^2)*t^2 + 2*(vx0*x0+vy0*y0)*t + (x0^2+y0^2-r^2) = 0
		const double r = length / 2;
		const double a = sq(vx0) + sq(vy0);
		const double b = 2 * (vx0 * x0 + vy0 * y0);
		const double c = sq(x0) + sq(y0) - sq(r);
		// 可解得t的两个解为：
		double t1 = (-b + sqrt(sq(b) - 4*a*c)) / (2*a);
		double t2 = (-b - sqrt(sq(b) - 4*a*c)) / (2*a);
		ASSERT (t1 != t2);
		if (t1 > t2)
			std::swap(t1, t2);

		// 若t1，t2均小于零，则说明mcircle与棒在过去的某个时间（可能）发生碰撞
		ASSERT (t1 < t2);
		if (t2 <= 0)
			return WILL_NOT_COLLIDE;

		// 该直线与圆L的两个交点（设为C1，C2）坐标：
		const double cx1 = vx0 * t1 + x0;
		const double cy1 = vy0 * t1 + y0;
		const double cx2 = vx0 * t2 + x0;
		const double cy2 = vy0 * t2 + y0;

		// 当mcircle运动到C1，C2时，棒的角度为：
		const double cao1 = ao1 + av * t1;

		// 棒从cao1再次转动到C1，C2的时刻为：
		const double bt1 = radian_distance(cao1, coord2radian(cx1, cy1)) / av1 + t1;
		const double bt2 = radian_distance(cao1, coord2radian(cx2, cy2)) / av1 + t1;

		// 棒从cao1再次转动到C1，C2时，mcircle的位置（设为F1，F2）为：
		const double fx1 = vx0 * bt1 + x0;
		const double fy1 = vy0 * bt1 + y0;
		const double fx2 = vx0 * bt2 + x0;
		const double fy2 = vy0 * bt2 + y0;

		// 查看F1，F2是否在圆L的外部：
		const bool f1_out = (sq(fx1) + sq(fy1) >= sq(r));
		const bool f2_out = (sq(fx2) + sq(fy2) >= sq(r));

		// 查看C1到C2这条射线相对于原点的旋转方向与棒的旋转方向是否相同
		// 下值若为正，则C1到C2这条射线相对于原点的旋转方向为逆时针；
		// 若为负则为顺时针，等于零则意味着这条射线（或其后延线）经过原点
		const double cclockwise = cx1 * cy2 - cy1 * cx2;
		if (cclockwise == 0.0)
		{
			// C1到C2这条射线（或其后延线）经过原点，检查究竟是射线本身还是其后延线经过原点
			if (t1 < 0)
			{
				// 射线后延线经过原点，即，mcircle正沿着圆L的半径往外移动
				if (f2_out)
					return WILL_NOT_COLLIDE;
				return bt2;
			}
			else
			{
				// 射线本身经过原点，那么mcircle将与棒的中心碰撞
				const double tt = r/sqrt(sq(vx0)+sq(vy0));
				ASSERT (tt > 0.0);
				return (MEASURE_T)(tt + t1);
			}
		}
		else if ((cclockwise > 0.0 && av1 > 0.0) || (cclockwise < 0.0 && av1 < 0.0))
		{
			// C1到C2这条射线相对于原点的旋转方向与棒的旋转方向相同
		}
		else // if (cclockwise < 0.0)
		{
			// C1到C2这条射线相对于原点的旋转方向与棒的旋转方向相反
		}
	}

	//// 假设圆与棒上距中心距离为l的某点于某时刻t相撞，则有：
	//// x1 + vx1 * t + l * cos(ao + av * t) == x2 + vx2 * t
	//// y1 + vy1 * t + l * sin(ao + av * t) == y2 + vy2 * t
	////
	//// 稍变形后有：
	//// (x1-x2 + (vx1-vx2) * t)^2 + (y1-y2 + (vy1-vy2) * t)^2 == l^2
	////
	//// 设dx=x1-x2, dy=y1-y2, dvx=vx1-vx2, dvy=vy1-vy2
	//// 再设a=dvx^2+dvy^2, b=2*(dx*dvx+dy*dvy), c=dx^2+dy^2
	//// 则等式左边可化为：
	//// a * t^2 + b * t + c
	////
	//// 若a大于0，它的最小值为：(4*a*c-b^2)/(4*a)
	//// 若a等于0，则b亦等于0，则它的最小值就是c
	//// 如果此值比(length/2)^2大，则碰撞不可能发生
	//{
	//	const double dx = x1 - x2;
	//	const double dy = y1 - y2;
	//	const double dvx = vx1 - vx2;
	//	const double dvy = vy1 - vy2;
	//	const double a = sq(dvx) + sq(dvy);
	//	const double b = 2 * (dx * dvx + dy + dvy);
	//	const double c = sq(dx) + sq(dy);
	//	const double left_min = (a!=0 ? (4*a*c - sq(b)) / (4*a) : c);
	//	if (left_min >= sq(length)/4)
	//		return WILL_NOT_COLLIDE;
	//}

	// TODO:
	return 10;
}

void ZBar::Collide(ZMovableCircle &, float fReboundCoeff)
{
	// TODO:
}

MEASURE_T ZBar::GetCollisionTime(const ZBoundary &boundary) const
{
	MEASURE_T time;
	CollisionSide side_dummy;

	VERIFY(GetCollisionParam(boundary, time, side_dummy));
	return time;
}

void ZBar::Collide(const ZBoundary &boundary, float fReboundCoeff)
{
	// 碰撞满足运动学三大守恒定律：能量守恒定律、动量守恒定律、角动量守恒定律

	MEASURE_T dummy_time;
	CollisionSide side;

	VERIFY(GetCollisionParam(boundary, dummy_time, side));
	// TODO:

	switch(side)
	{
	case CS_LEFT:
	case CS_RIGHT:
		break;
	case CS_TOP:
	case CS_BOTTOM:
		// TODO: no collision with top/bottom boundary now.
		return;
	default:
		ASSERT(0);
	}

//	const double coeff = fReboundCoeff;
	const double vx1 = this->velocity.dx;
	const double vy1 = this->velocity.dy;
	const double length = this->length;
	const double w1 = this->av;
	const double seta = radian_normalize(this->ao);
	const double sin_seta = sin(seta);
	const double sq_sin_seta = sq(sin_seta);

//	LOG("碰撞前，总能量：%.2f", 12*vx1*vx1+w1*w1*length*length);
	double vx2, w2;
	if (seta == PI/2.0)
	{
		vx2 = - vx1;
		w2 = w1;
	}
	else if ((side == CS_LEFT && seta > PI/2.0) ||
	         (side == CS_RIGHT && seta < PI/2.0))
	{
		vx2 = (w1*length*sin_seta + 3*vx1*sq_sin_seta - vx1) /
			(1 + 3*sq_sin_seta);
		w2 = (w1 - 3*w1*sq_sin_seta + 12*vx1*sin_seta/length) /
			(1 + 3*sq_sin_seta);
	}
	else // if ((side == CS_LEFT && seta < PI/2.0) ||
	     //     (side == CS_RIGHT && seta > PI/2.0))
	{
		vx2 = (-w1*length*sin_seta + 3*vx1*sq_sin_seta - vx1) /
			(1 + 3*sq_sin_seta);
		w2 = (w1 - 3*w1*sq_sin_seta - 12*vx1*sin_seta/length) /
			(1 + 3*sq_sin_seta);
	}
	this->velocity.dx = vx2;
	this->av = w2;
//	LOG("碰撞后，总能量：%.2f", 12*vx2*vx2+w2*w2*length*length);
//	LOG("碰撞后，vx2=%.2f, w2=%.2f", vx2, w2);

//	// 由能量（包括平移动能及旋转动能）守恒定律得：（动能存在一定比例的损耗）
//	// coeff * (m * v1^2 / 2 + m * w1^2 * l^2 / 24) == M * V^2 / 2 + m * v2^2 / 2 + m * w2^2 * l^2 / 24
//	// 其中m为刚性棒的质量、M为边界的质量（假设为无穷大）、v1为刚性棒碰撞前的速度（矢量）、w1为刚性棒碰撞
//	// 前的角速度（以弧度计）、l为刚性棒的长度、V为边界碰撞后的速度（对于发生在左边界的碰撞而言，方向向左）、
//	// v2为刚性棒碰撞后的速度（矢量）、w2为刚性棒碰撞后的角速度（以弧度计），coeff为能量损耗系数；下同
//	// 
//	// 由动量守恒定律得：
//	// m * vx1 == m * vx2 + M * V       （A式）
//	// m * vy1 == m * vy2
//	// 其中vx1为刚性棒碰撞前x方向的速度、vx2为碰撞后x方向的速度、vy1和vy2类同
//	// 
//	// 由角动量守恒定律得：
//	// m * w1 * l^2 / 12 == m * w2 * l^2 / 12 + M * V * l * sin(seta) / 2
//	// 其中seta为刚性棒碰撞那一刻与水平方向的夹角（对于发生在左边界的碰撞而言）
//	//
//	// 以上的未知量有：
//	// 1.  v2
//	// 2.  w2
//	// 3.  V
//	// 4.  vx2
//	// 5.  vy2
//	//
//	// 另有：
//	// vx2^2 + vy2^2 == v2^2
//	// 
//	// 再令 R = M / m, 同时进行以下换元：
//	const double kinetic_energy = coeff * (
//		(sq(vx1) + sq(vy1)) / 2.0 + sq(w1) * sq(length) / 24.0);
//	const double momentum_x = vx1;
//	const double angular_momentum = w1 * sq(length) / 12.0;
//	// 得以下方程组：
//	// 1.  R * V^2 / 2 + v2^2 / 2 + w2^2 * l^2 / 24 == kinetic_energy
//	// 2.  R * V + vx2 == momentum_x
//	// 3.  vy2 == vy1
//	// 4.  vx2^2 + vy2^2 == v2^2
//	// 5.  R * V * l * sin(seta) / 2 + w2 * l^2 / 12 == angular_momentum
//	//
//	// 将3、4两式代入1式得：
//	// 1.  R * V^2 / 2 + (vx2^2 + vy1^2) / 2 + w2^2 * l^2 / 24 == kinetic_energy
//	// 2.  R * V + vx2 == momentum_x
//	// 3.  vy2 == vy1
//	// 4.  R * V * l * sin(seta) / 2 + w2 * l^2 / 12 == angular_momentum
//	// 
//	// 由于假设边界的质量为无穷大，故R = M / m亦为无穷大，以上方程组得化简为：
//	// 1.  (vx2^2 + vy1^2) / 2 + w2^2 * l^2 / 24 == kinetic_energy
//	// 2.  V == 0
//	// 3.  vy2 == vy1
//	// 4.  (momentum_x - vx2) * l * sin(seta) / 2 + w2 * l^2 / 12 == angular_momentum
//	// 
//	// 进行以下换元：
//	const double a00 = 1 / 2.0;
//	const double a01 = sq(length) / 24.0;
//	const double b0  = kinetic_energy - sq(vy1) / 2.0;
//	const double a10 = -1 * length * sin(seta) / 2.0;
//	const double a11 = sq(length) / 12.0;
//	const double b1  = angular_momentum - momentum_x * length * sin(seta) / 2.0;
//	// 则方程组化为：
//	// 1.  a00 * vx2^2 + a01 * w2^2 == b0
//	// 2.  a10 * vx2   + a11 * w2   == b1
//	// 
////	// 消元w2后有：
//	const double A = a00 * a11 * a11 + a01 * a10 * a10;
//	const double B1 = -2 * a01 * a10 * b1;
//	const double B2 = -2 * a00 * a11 * b1;
//	const double C1 = a01 * b1 * b1 - b0 * a11 * a11;
//	const double C2 = a00 * b1 * b1 - b0 * a10 * a10;
//	// 则方程化为：
//	// 1.  A * vx2^2 + B1 * vx2 + C1 == 0
//	// 2.  A * w2^2  + B2 * w2  + C2 == 0
//	// 
//	// 对于1式，由于方程必然有解，故：
//	const double delta_1 = B1*B1 - 4*A*C1;
//	ASSERT(A != 0.0);
//	ASSERT(delta_1 >= 0.0);
//	const double vx2_1 = (-B1 - sqrt(delta_1)) / 2.0 / A;
//	const double vx2_2 = (-B1 + sqrt(delta_1)) / 2.0 / A;
//	const double w2_1 = (b1 - a10 * vx2_1) / a11;
//	const double w2_2 = (b1 - a10 * vx2_2) / a11;
//	// 由A式：
//	// m * vx1 == m * vx2 + M * V       （A式）
//	// 可知，vx2 > vx1 （对于发生在左边界的碰撞而言）
//	// 故：
//	const double vx2 = (vx2_1 > vx1 ? vx2_1 : vx2_2);
//	ASSERT(vx2 > vx1);
}

bool ZBar::GetCollisionParam(const ZBoundary &boundary, MEASURE_T &time, CollisionSide &side) const
{
	const double x = this->barycenter.x;
	const double y = this->barycenter.y;
	const double vx = this->velocity.dx;
	const double vy = this->velocity.dy;
	const double l = this->length;
	const double w0 = this->ao;
	const double w = this->av;

	const double left   = boundary.left;
	const double top    = boundary.top;
	const double right  = boundary.right;
	const double bottom = boundary.bottom;

	{	// 查看棒两端是否已经在边界外了
		const double x_vertex_1 = x + l * cos(w0) / 2.0;
		const double y_vertex_1 = y + l * sin(w0) / 2.0;
		const double x_vertex_2 = x * 2 - x_vertex_1;
		const double y_vertex_2 = y * 2 - y_vertex_1;

		const double vx_vertex_1 = vx - l * w * sin(w0) / 2.0;
		const double vy_vertex_1 = vy + l * w * cos(w0) / 2.0;
		const double vx_vertex_2 = vx + l * w * sin(w0) / 2.0;
		const double vy_vertex_2 = vy - l * w * cos(w0) / 2.0;

		if ((x_vertex_1 < left && vx_vertex_1 < 0) || (x_vertex_2 < left && vx_vertex_2 < 0)) {
			side = CS_LEFT;
			time = 0;
			return true;
		} else if ((y_vertex_1 < top && vy_vertex_1 < 0) || (y_vertex_2 < top && vy_vertex_1 < 0)) {
			side = CS_TOP;
			time = 0;
			return true;
		} else if ((x_vertex_1 > right && vx_vertex_1 > 0) || (x_vertex_2 > right && vx_vertex_2 > 0)) {
			side = CS_RIGHT;
			time = 0;
			return true;
		} else if ((y_vertex_1 > bottom && vy_vertex_1 > 0) || (y_vertex_2 > bottom && vy_vertex_1 > 0)) {
			side = CS_BOTTOM;
			time = 0;
			return true;
		}
	}

	// 棒的两个顶点的坐标方程为：
	// x1(t) = x + vx * t + l*cos(w0+w*t)/2
	// y1(t) = y + vy * t + l*sin(w0+w*t)/2
	// x2(t) = x + vx * t - l*cos(w0+w*t)/2
	// y2(t) = y + vy * t - l*sin(w0+w*t)/2
	// 
	// 由于方程 x1(t) = left 等均不能用代数方法求解，故考虑用数值解法
	//
	// 用二分法解这些方程，需要找到两个值t1和t2，使得当t满足t1<=t<=t2时，
	// f(t)为单调增函数或减函数，并且f(t1)与f(t2)异号，方能使用二分法
	// 详见GetCollisionTimeToBoundary

	double t_left	= 0;
	double t_top	= 0;
	double t_right	= 0;
	double t_bottom	= 0;

	if (x > left+l/2 && vx >= 0)
		t_left = -1;
	else
		t_left = GetCollisionTimeToBoundary(CS_LEFT, l, x, left, vx, w0, w);

	if (y > top+l/2 && vy >= 0)
		t_top = -1;
	else
		t_top = GetCollisionTimeToBoundary(CS_TOP, l, y, top, vy, w0, w);

	if (x < right-l/2 && vx <= 0)
		t_right = -1;
	else
		t_right = GetCollisionTimeToBoundary(CS_RIGHT, l, x, right, vx, w0, w);

	if (x < bottom-l/2 && vy <= 0)
		t_bottom = -1;
	else
		t_bottom = GetCollisionTimeToBoundary(CS_BOTTOM, l, y, bottom, vy, w0, w);

	const double time_x = (t_left<0 ? (t_right<0 ? -1 : t_right) : t_left);
	const double time_y = (t_top<0 ? (t_bottom<0 ? -1 : t_bottom) : t_top);
	const double time__ = (time_x<0 ? (time_y<0 ? -1 : time_y) : time_x);

	time = (MEASURE_T)time__;

	if(time__ >= 0)
	{
		if(time__ == t_left)
			side = CS_LEFT;
		else if(time__ == t_top)
			side = CS_TOP;
		else if(time__ == t_right)
			side = CS_RIGHT;
		else if(time__ == t_bottom)
			side = CS_BOTTOM;
		else
			ASSERT(0);
	}
	else
		side = CS_NONE;

	return true;
}

MEASURE_T ZBar::GetCollisionTimeToBoundary(
	CollisionSide side, // 需要检查的碰撞方向
	double length,      // 棒的全长
	double z0,          // 棒中心的坐标（x或y坐标，与碰撞方向有关）
	double zz,          // 该方向边界的位置（x或y坐标，与碰撞方向有关）
	double vz,          // 棒在该方向移动的线速度
	double cur_ao,      // 当前棒的倾斜弧度
	double av)          // 当前棒的旋转角速度（单位为弧度，逆时针为正）
{
	ASSERT(side == CS_LEFT || side == CS_TOP || side == CS_RIGHT || side == CS_BOTTOM);

	// x1(t) = x + vx * t + l*cos(w0+w*t)/2
	// y1(t) = y + vy * t + l*sin(w0+w*t)/2
	// x2(t) = x + vx * t - l*cos(w0+w*t)/2
	// y2(t) = y + vy * t - l*sin(w0+w*t)/2
	double (* const z_cos)(double) = ((side == CS_LEFT || side == CS_RIGHT) ? cosd : sind);

	if (av == 0.0)
	{
		const double z1 = z0 + length * z_cos(cur_ao)/2.0;
		const double z2 = z0 - length * z_cos(cur_ao)/2.0;

		if (side == CS_LEFT || side == CS_RIGHT)
			return (min(z1, z2) - zz) / vz;
		else
			return (zz - max(z1, z2)) / vz;
	}

	double ao_h; // 按当前的旋转方向转到水平方向（对左右边界而言，如对上下边
	             // 界而言则为竖直方向，下同）所需的弧度偏移量
	if (side == CS_LEFT || side == CS_RIGHT)
		ao_h = radian_distance(cur_ao, 0);
	else
		ao_h = radian_distance(cur_ao, PI/2);
	ASSERT(ao_h >= 0.0 && ao_h < PI);
	const double t_h = fabs(ao_h / av);	// 转到水平方向所需时间
	const double z_h = z0 + vz * t_h;	// 转到水平方向时，中心点的位置

	double t1, t2;
	if (side == CS_LEFT || side == CS_TOP)
	{
		if (z_h - length/2.0 <= zz)
		{
			// 从当前位置转到第一个水平位置的时候已经到达边界
			t1 = 0.0;
			t2 = t_h;
		}
		else
		{
			// 棒旋转半圈所需的时间，亦即，棒由水平方向开始旋转到再次回到水平方向的时间
			const double t_semicircle = PI / fabs(av);
			t1 = t_h + t_semicircle * floor((z_h - length/2.0 - zz) / (fabs(vz) * t_semicircle));
			t2 = t1 + t_semicircle;
		}
	}
	else // if (side == CS_RIGHT || side == CS_BOTTOM)
	{
		if (z_h + length/2.0 >= zz)
		{
			// 从当前位置转到第一个水平位置的时候已经到达边界
			t1 = 0.0;
			t2 = t_h;
		}
		else
		{
			// 棒旋转半圈所需的时间，亦即，棒由水平方向开始旋转到再次回到水平方向的时间
			const double t_semicircle = PI / fabs(av);
			t1 = t_h + t_semicircle * floor((zz - z_h - length/2.0) / (fabs(vz) * t_semicircle));
			t2 = t1 + t_semicircle;
		}
	}

	ASSERT(t1 >= 0.0);
	ASSERT(t2 >= 0.0);

	double t_boundary;

#define PRECISION	1e-6
	while (t2 - t1 > PRECISION)
	{
		t_boundary = (t1 + t2)/2.0;
		const double z1 = z0 + vz * t_boundary + length * z_cos(cur_ao + av*t_boundary)/2.0;
		const double z2 = z0 + vz * t_boundary - length * z_cos(cur_ao + av*t_boundary)/2.0;
		if (z1 == zz || z2 == zz)
			break;

		if (side == CS_LEFT || side == CS_TOP)
		{
			if (z1 < zz || z2 < zz)
				t2 = t_boundary;
			else
				t1 = t_boundary;
		}
		else // if (side == CS_RIGHT || side == CS_BOTTOM)
		{
			if (z1 > zz || z2 > zz)
				t2 = t_boundary;
			else
				t1 = t_boundary;
		}
	}

	return (MEASURE_T)t_boundary;
}

void ZBar::ActMoving(MEASURE_T time)
{
	barycenter.MoveByVelocity(time, this->velocity);
	ao += (MEASURE_D)(av * time);
}

//////////////////////////////////////////////////////////////////////////
//  ZBoundary
//////////////////////////////////////////////////////////////////////////

ZBoundary::ZBoundary(MEASURE_D l, MEASURE_D t, MEASURE_D r, MEASURE_D b) :
	left(l), top(t), right(r), bottom(b)
{
}

ZBoundary::ZBoundary(const ZBoundary& boundary) :
	left(boundary.left), right(boundary.right),
	top(boundary.top), bottom(boundary.bottom)
{
}

ZBoundary::~ZBoundary()
{
}

void ZBoundary::Initialize(MEASURE_D l, MEASURE_D t, MEASURE_D r, MEASURE_D b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

ZPos ZBoundary::GetCenter() const
{
	return ZPos( (left + right)/2,
	             (top + bottom)/2 );
}

//////////////////////////////////////////////////////////////////////////
//  Misc
//////////////////////////////////////////////////////////////////////////

int RoundToInt(double d)
{
	return d > 0 ? (int)(d + 0.5) : (int)(d - 0.5);
}

int RoundToInt(float f)
{
	return f > 0 ? (int)(f + 0.5) : (int)(f - 0.5);
}

int GetPositiveMinIndex(const std::vector<double> &v)
{
	int index = -1;
	for(unsigned int i=0; i<v.size(); i++)
	{
		if(v[i] >= 0 && (index<0 || v[i] < v[index]))
			index = i;
	}

	return index;
}

int GetPositiveMinIndex(const std::vector<float> &v)
{
	int index = -1;
	for(unsigned int i=0; i<v.size(); i++)
	{
		if(v[i] >= 0 && (index<0 || v[i] < v[index]))
			index = i;
	}

	return index;
}

double coord2radian(double x, double y)
{
	if (x == 0.0)
	{
		if (y > 0.0)
			return PI/2;
		else if (y == 0.0)
			return 0.0;
		else
			return PI*3/2;
	}

	const double tan_r = y / x;
	const double r = atan(tan_r);
	if (r > 0.0)
		return (y > 0.0 ? r : PI + r);
	else
		return (y > 0.0 ? PI + r : PI*2 + r);
}
