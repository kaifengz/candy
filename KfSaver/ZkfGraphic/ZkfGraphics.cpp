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

	// ���ڣ�
	// x1(t) = x1 + vx1 * t		// x1(t) Ϊ this Բ�ľ���ʱ��t���x���꣬��ͬ
	// y1(t) = y1 + vy1 * t		// y1(t) Ϊ this Բ�ľ���ʱ��t���x���꣬��ͬ
	// x2(t) = x2 + vx2 * t		// x1(t) Ϊ another Բ�ľ���ʱ��t���x���꣬��ͬ
	// y2(t) = y2 + vy2 * t		// y1(t) Ϊ another Բ�ľ���ʱ��t���x���꣬��ͬ
	// ��Ҫ�ⷽ�̣���������ʱ��t������Բ��Բ�ľ�����ڸ���Բ�İ뾶�ͣ�
	// SQUARE(x1(t)-x2(t)) + SQUARE(y1(t)-t2(t)) = SQUARE(r1+r2)
	const double xd		= x1 - x2;
	const double vxd	= vx1 - vx2;
	const double yd		= y1 - y2;
	const double vyd	= vy1 - vy2;
	const double R		= sq(r1 + r2);

	// ��Ԫ�󣬷���Ϊ��
	// SQUARE(xd + vxd*t) + SQUARE(yd + vyd*t) = R
	const double a = sq(vxd) + sq(vyd); // aΪ�Ǹ���
	const double b = 2 * (xd * vxd + yd * vyd);
	const double c = sq(xd) + sq(yd) - R;

	// �ٴλ�Ԫ�󣬷���Ϊ��
	// a * SQUARE(t) + b * t + c = 0;

	// 1. ����÷����޽⣬˵����������ײ
	// 2. ��������������⣨��a,b,cȫΪ�㣩������Բ�����ٶ�ȫ��ͬ������ʼ�մ������е�λ��
	// 3. ����������������⣬˵����������ײ
	// 4. ������������⡢�����һ����˵����Բ�Ѿ������ཻλ�ã��������ڼ���������£�����
	//    4.1. ����������ֵ�Ƚϴ��򲻷�����ײ
	//    4.2. ����������ֵ��ϴ������̷�����ײ
	//    4.3. ���������ֵ��ȵ���������⴦��������������ײ����
	// 5. ����������������⣬��˵������ĳʱ����ײ

	if(a == 0 && b == 0) // �����ϻ�Ԫ��֪����aΪ�㣬��b��Ϊ�㣡
	{
		// �����ܷ�����ײ���������ϵ�2��������Լ����ֵ�1�����
		return WILL_NOT_COLLIDE;
	}
	ASSERT(a != 0);

	const double delta = b*b - 4*a*c;
	if(delta < 0) // ��1�����
		return WILL_NOT_COLLIDE;

	const double t1 = (-b-sqrt(delta)) / (2*a); // t1�ǽ�С�Ľ�
	const double t2 = (-b+sqrt(delta)) / (2*a); // t2�ǽϴ�Ľ�
	ASSERT(t1 <= t2);

	if(t1 <= 0 && t2 <= 0) // ��3�����
		return WILL_NOT_COLLIDE;

	if(t1 <= 0 && t2 >= 0) // ��4�����
	{
		if(abs(t1) > abs(t2)) // ��4.1��4.3�����
			return WILL_NOT_COLLIDE;
		else // ��4.2�����
			return 0;
	}

	return (MEASURE_T)t1; // ��5���������Ϊt1��С��t2
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

	// ��Բ����ʱӦ����������λ�ã���������Ǽ������Ļ�������ô���ݼ򻯵���ײ
	// ģ�ͣ���Բ�����߷�����ٶȷ������ֲ��䣬�ڷ��߷�����ٶȷ����໥��������
	// ˥��������ֵ����fReboundCoeff��С��������������Բ�໥��λ�ù�ϵ�����߷���
	// ����Ϊ���Խ��߷���򸱶Խ��߷���

	const double dx = abs(x1 - x2);
	const double dy = abs(y1 - y2);
	const double d_2 = sq(dx) + sq(dy);
	ASSERT(d_2 > 0);
	const double d = sqrt(d_2);

	const int x_sign = (x1 >= x2 ? 1 : -1);
	const int y_sign = (y1 >= y2 ? 1 : -1);
	const bool same_sign = (x_sign == y_sign);

	// pΪ���߷�����ٶȷ�����qΪ���߷�����ٶȷ���
	const double p1 = (same_sign ? ((vx1 * (dy/d)) + (-vy1 * (dx/d))) : (( vx1 * (dy/d)) + (vy1 * (dx/d))));
	const double q1 = (same_sign ? ((vx1 * (dx/d)) + ( vy1 * (dy/d))) : ((-vx1 * (dx/d)) + (vy1 * (dy/d))));
	const double p2 = (same_sign ? ((vx2 * (dy/d)) + (-vy2 * (dx/d))) : (( vx2 * (dy/d)) + (vy2 * (dx/d))));
	const double q2 = (same_sign ? ((vx2 * (dx/d)) + ( vy2 * (dy/d))) : ((-vx2 * (dx/d)) + (vy2 * (dy/d))));

	// pΪ���߷�����ٶȷ��������ֲ���
	const double pa1 = p1;
	const double pa2 = p2;

	// qΪ���߷�����ٶȷ�����ִ������ײ
	double qa1=0, qa2=0;
	RightCollide(q1, q2, m1, m2, fReboundCoeff, qa1, qa2);

	// ��ײ���������·���ʽ
	// pa1 = (same_sign ? ((vxa1 * (dy/d)) + (-vya1 * (dx/d))) : (( vxa1 * (dy/d)) + (vya1 * (dx/d))));
	// qa1 = (same_sign ? ((vxa1 * (dx/d)) + ( vya1 * (dy/d))) : ((-vxa1 * (dx/d)) + (vya1 * (dy/d))));
	// pa2 = (same_sign ? ((vxa2 * (dy/d)) + (-vya2 * (dx/d))) : (( vxa2 * (dy/d)) + (vya2 * (dx/d))));
	// qa2 = (same_sign ? ((vxa2 * (dx/d)) + ( vya2 * (dy/d))) : ((-vxa2 * (dx/d)) + (vya2 * (dy/d))));
	// ��˷��̵ã�
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

	// ����ײǰ���������·����飺
	// m1*v1 + m2*v2 = m1*va1 + m2 * va2
	// f*(m1*v1*v1 + m2*v2*v2) = m1*va1*va1 + m2*va2*va2
	// 
	// ��v1k=v1-vk��v2k=v2-vk��va1k=va1-vk��va2k=va2-vk
	// ����vk���㣺
	// m1*(v1-vk) + m2*(v2-vk) = 0  ===>  m1*v1 + m2*v2 = (m1+m2)*vk
	// 
	// �����������Ͻ���vk������Բ����ײǰ�����ĵ��˶��ٶȣ�����Բ��Ϊһ�����壩
	// �ɶ����غ㶨���֪����Բ���ĵ��˶��ٶ���ײǰ�󲻱�
	const double vk = (m1*v1 + m2*v2) / (m1+m2);
	const double v1k = v1 - vk;
	const double v2k = v2 - vk;

	// ��Ԫ���У�
	// m1*va1k + m2*va2k = 0
	// m1*va1k*va2k + m2*va2k*va2k = f*(m1*v1k*v1k + m2*v2k*v2k)
	// 
	// ��EE = c*(m1*v1k*v1k + m2*v2k*v2k)
	const double EE = fReboundCoeff * (m1*v1k*v1k + m2*v2k*v2k);

	// �ⷽ����ã�
	const double va1k_sq = m2*EE / ((m1+m2)*m1);
	const double va2k_sq = m1*EE / ((m1+m2)*m2);

	// �������Բ�����ĵ��˶��ٶȶ��ԣ�ĳԲ����ײǰ����˶������Ȼ�෴
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
	// ���������غ㶨�ɺͶ����غ㶨�ɣ���ײ�Ժ�
	// ��ײ������ٶȷ�������ת������ֵ��˥��

	MEASURE_T dummy;
	CollisionSide side = CS_NONE;

	VERIFY(GetCollisionParam(boundary, dummy, side));

	double vx = this->velocity.dx;
	double vy = this->velocity.dy;
	const double coeff = sqrt(fReboundCoeff);

	ASSERT(coeff > 0);
//	ASSERT(coeff <= 1.0); // ��������Դ���һ

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
	// XXX �˴�����mcircle�Ĵ�С��������Ϊһ���ʵ�

	// ƽ������ϵ��ʹ��������ϵ�԰�����ΪԲ�㣬����mcircle�µ�������ٶȣ�
	const double x0 = x2 - x1;
	const double y0 = y2 - y1;
	const double vx0 = vx2 - vx1;
	const double vy0 = vy2 - vy1;

	if (vx0 == 0.0 && vy0 == 0.0)
	{
		// ��dvx��dvy��Ϊ�㣬��mcircle����ڰ���ֹ
		// TODO:
	}
	else
	{
		// ����mcircle���ƶ��켣��Ϊһֱ�ߣ�����Ϊ��
		// vy0 * x - vx0 * y - x0 * vy0 + y0 * vx0 = 0
		// ԭ�㵽��ֱ�ߵľ���Ϊ��
		// abs(x0 * vy0 - y0 * vx0) / sqrt(vx0^2 + vy0^2)
		// ���������length/2������ײ�����ܷ���
		// ���������length/2���˴�Ҳ��Ϊ��������ײ
		const double dd = sq(x0 * vy0 - y0 * vx0) / (sq(vx0) + sq(vy0));
		if (dd >= sq(length/2))
			return WILL_NOT_COLLIDE;

		// ��ֱ������ԭ��Ϊ���ġ�����Ϊֱ����Բ����ΪԲL���Ľ������㷽���飺
		// x = vx0 * t + x0
		// y = vy0 * t + y0
		// x^2 + y^2 = r^2      ����r = length/2
		// ����ɵö��η��̣�
		// (vx0^2+vy0^2)*t^2 + 2*(vx0*x0+vy0*y0)*t + (x0^2+y0^2-r^2) = 0
		const double r = length / 2;
		const double a = sq(vx0) + sq(vy0);
		const double b = 2 * (vx0 * x0 + vy0 * y0);
		const double c = sq(x0) + sq(y0) - sq(r);
		// �ɽ��t��������Ϊ��
		double t1 = (-b + sqrt(sq(b) - 4*a*c)) / (2*a);
		double t2 = (-b - sqrt(sq(b) - 4*a*c)) / (2*a);
		ASSERT (t1 != t2);
		if (t1 > t2)
			std::swap(t1, t2);

		// ��t1��t2��С���㣬��˵��mcircle����ڹ�ȥ��ĳ��ʱ�䣨���ܣ�������ײ
		ASSERT (t1 < t2);
		if (t2 <= 0)
			return WILL_NOT_COLLIDE;

		// ��ֱ����ԲL���������㣨��ΪC1��C2�����꣺
		const double cx1 = vx0 * t1 + x0;
		const double cy1 = vy0 * t1 + y0;
		const double cx2 = vx0 * t2 + x0;
		const double cy2 = vy0 * t2 + y0;

		// ��mcircle�˶���C1��C2ʱ�����ĽǶ�Ϊ��
		const double cao1 = ao1 + av * t1;

		// ����cao1�ٴ�ת����C1��C2��ʱ��Ϊ��
		const double bt1 = radian_distance(cao1, coord2radian(cx1, cy1)) / av1 + t1;
		const double bt2 = radian_distance(cao1, coord2radian(cx2, cy2)) / av1 + t1;

		// ����cao1�ٴ�ת����C1��C2ʱ��mcircle��λ�ã���ΪF1��F2��Ϊ��
		const double fx1 = vx0 * bt1 + x0;
		const double fy1 = vy0 * bt1 + y0;
		const double fx2 = vx0 * bt2 + x0;
		const double fy2 = vy0 * bt2 + y0;

		// �鿴F1��F2�Ƿ���ԲL���ⲿ��
		const bool f1_out = (sq(fx1) + sq(fy1) >= sq(r));
		const bool f2_out = (sq(fx2) + sq(fy2) >= sq(r));

		// �鿴C1��C2�������������ԭ�����ת�����������ת�����Ƿ���ͬ
		// ��ֵ��Ϊ������C1��C2�������������ԭ�����ת����Ϊ��ʱ�룻
		// ��Ϊ����Ϊ˳ʱ�룬����������ζ���������ߣ���������ߣ�����ԭ��
		const double cclockwise = cx1 * cy2 - cy1 * cx2;
		if (cclockwise == 0.0)
		{
			// C1��C2�������ߣ���������ߣ�����ԭ�㣬��龿�������߱�����������߾���ԭ��
			if (t1 < 0)
			{
				// ���ߺ����߾���ԭ�㣬����mcircle������ԲL�İ뾶�����ƶ�
				if (f2_out)
					return WILL_NOT_COLLIDE;
				return bt2;
			}
			else
			{
				// ���߱�����ԭ�㣬��ômcircle�������������ײ
				const double tt = r/sqrt(sq(vx0)+sq(vy0));
				ASSERT (tt > 0.0);
				return (MEASURE_T)(tt + t1);
			}
		}
		else if ((cclockwise > 0.0 && av1 > 0.0) || (cclockwise < 0.0 && av1 < 0.0))
		{
			// C1��C2�������������ԭ�����ת�����������ת������ͬ
		}
		else // if (cclockwise < 0.0)
		{
			// C1��C2�������������ԭ�����ת�����������ת�����෴
		}
	}

	//// ����Բ����Ͼ����ľ���Ϊl��ĳ����ĳʱ��t��ײ�����У�
	//// x1 + vx1 * t + l * cos(ao + av * t) == x2 + vx2 * t
	//// y1 + vy1 * t + l * sin(ao + av * t) == y2 + vy2 * t
	////
	//// �Ա��κ��У�
	//// (x1-x2 + (vx1-vx2) * t)^2 + (y1-y2 + (vy1-vy2) * t)^2 == l^2
	////
	//// ��dx=x1-x2, dy=y1-y2, dvx=vx1-vx2, dvy=vy1-vy2
	//// ����a=dvx^2+dvy^2, b=2*(dx*dvx+dy*dvy), c=dx^2+dy^2
	//// ���ʽ��߿ɻ�Ϊ��
	//// a * t^2 + b * t + c
	////
	//// ��a����0��������СֵΪ��(4*a*c-b^2)/(4*a)
	//// ��a����0����b�����0����������Сֵ����c
	//// �����ֵ��(length/2)^2������ײ�����ܷ���
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
	// ��ײ�����˶�ѧ�����غ㶨�ɣ������غ㶨�ɡ������غ㶨�ɡ��Ƕ����غ㶨��

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

//	LOG("��ײǰ����������%.2f", 12*vx1*vx1+w1*w1*length*length);
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
//	LOG("��ײ����������%.2f", 12*vx2*vx2+w2*w2*length*length);
//	LOG("��ײ��vx2=%.2f, w2=%.2f", vx2, w2);

//	// ������������ƽ�ƶ��ܼ���ת���ܣ��غ㶨�ɵã������ܴ���һ����������ģ�
//	// coeff * (m * v1^2 / 2 + m * w1^2 * l^2 / 24) == M * V^2 / 2 + m * v2^2 / 2 + m * w2^2 * l^2 / 24
//	// ����mΪ���԰���������MΪ�߽������������Ϊ����󣩡�v1Ϊ���԰���ײǰ���ٶȣ�ʸ������w1Ϊ���԰���ײ
//	// ǰ�Ľ��ٶȣ��Ի��ȼƣ���lΪ���԰��ĳ��ȡ�VΪ�߽���ײ����ٶȣ����ڷ�������߽����ײ���ԣ��������󣩡�
//	// v2Ϊ���԰���ײ����ٶȣ�ʸ������w2Ϊ���԰���ײ��Ľ��ٶȣ��Ի��ȼƣ���coeffΪ�������ϵ������ͬ
//	// 
//	// �ɶ����غ㶨�ɵã�
//	// m * vx1 == m * vx2 + M * V       ��Aʽ��
//	// m * vy1 == m * vy2
//	// ����vx1Ϊ���԰���ײǰx������ٶȡ�vx2Ϊ��ײ��x������ٶȡ�vy1��vy2��ͬ
//	// 
//	// �ɽǶ����غ㶨�ɵã�
//	// m * w1 * l^2 / 12 == m * w2 * l^2 / 12 + M * V * l * sin(seta) / 2
//	// ����setaΪ���԰���ײ��һ����ˮƽ����ļнǣ����ڷ�������߽����ײ���ԣ�
//	//
//	// ���ϵ�δ֪���У�
//	// 1.  v2
//	// 2.  w2
//	// 3.  V
//	// 4.  vx2
//	// 5.  vy2
//	//
//	// ���У�
//	// vx2^2 + vy2^2 == v2^2
//	// 
//	// ���� R = M / m, ͬʱ�������»�Ԫ��
//	const double kinetic_energy = coeff * (
//		(sq(vx1) + sq(vy1)) / 2.0 + sq(w1) * sq(length) / 24.0);
//	const double momentum_x = vx1;
//	const double angular_momentum = w1 * sq(length) / 12.0;
//	// �����·����飺
//	// 1.  R * V^2 / 2 + v2^2 / 2 + w2^2 * l^2 / 24 == kinetic_energy
//	// 2.  R * V + vx2 == momentum_x
//	// 3.  vy2 == vy1
//	// 4.  vx2^2 + vy2^2 == v2^2
//	// 5.  R * V * l * sin(seta) / 2 + w2 * l^2 / 12 == angular_momentum
//	//
//	// ��3��4��ʽ����1ʽ�ã�
//	// 1.  R * V^2 / 2 + (vx2^2 + vy1^2) / 2 + w2^2 * l^2 / 24 == kinetic_energy
//	// 2.  R * V + vx2 == momentum_x
//	// 3.  vy2 == vy1
//	// 4.  R * V * l * sin(seta) / 2 + w2 * l^2 / 12 == angular_momentum
//	// 
//	// ���ڼ���߽������Ϊ����󣬹�R = M / m��Ϊ��������Ϸ�����û���Ϊ��
//	// 1.  (vx2^2 + vy1^2) / 2 + w2^2 * l^2 / 24 == kinetic_energy
//	// 2.  V == 0
//	// 3.  vy2 == vy1
//	// 4.  (momentum_x - vx2) * l * sin(seta) / 2 + w2 * l^2 / 12 == angular_momentum
//	// 
//	// �������»�Ԫ��
//	const double a00 = 1 / 2.0;
//	const double a01 = sq(length) / 24.0;
//	const double b0  = kinetic_energy - sq(vy1) / 2.0;
//	const double a10 = -1 * length * sin(seta) / 2.0;
//	const double a11 = sq(length) / 12.0;
//	const double b1  = angular_momentum - momentum_x * length * sin(seta) / 2.0;
//	// �򷽳��黯Ϊ��
//	// 1.  a00 * vx2^2 + a01 * w2^2 == b0
//	// 2.  a10 * vx2   + a11 * w2   == b1
//	// 
////	// ��Ԫw2���У�
//	const double A = a00 * a11 * a11 + a01 * a10 * a10;
//	const double B1 = -2 * a01 * a10 * b1;
//	const double B2 = -2 * a00 * a11 * b1;
//	const double C1 = a01 * b1 * b1 - b0 * a11 * a11;
//	const double C2 = a00 * b1 * b1 - b0 * a10 * a10;
//	// �򷽳̻�Ϊ��
//	// 1.  A * vx2^2 + B1 * vx2 + C1 == 0
//	// 2.  A * w2^2  + B2 * w2  + C2 == 0
//	// 
//	// ����1ʽ�����ڷ��̱�Ȼ�н⣬�ʣ�
//	const double delta_1 = B1*B1 - 4*A*C1;
//	ASSERT(A != 0.0);
//	ASSERT(delta_1 >= 0.0);
//	const double vx2_1 = (-B1 - sqrt(delta_1)) / 2.0 / A;
//	const double vx2_2 = (-B1 + sqrt(delta_1)) / 2.0 / A;
//	const double w2_1 = (b1 - a10 * vx2_1) / a11;
//	const double w2_2 = (b1 - a10 * vx2_2) / a11;
//	// ��Aʽ��
//	// m * vx1 == m * vx2 + M * V       ��Aʽ��
//	// ��֪��vx2 > vx1 �����ڷ�������߽����ײ���ԣ�
//	// �ʣ�
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

	{	// �鿴�������Ƿ��Ѿ��ڱ߽�����
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

	// ����������������귽��Ϊ��
	// x1(t) = x + vx * t + l*cos(w0+w*t)/2
	// y1(t) = y + vy * t + l*sin(w0+w*t)/2
	// x2(t) = x + vx * t - l*cos(w0+w*t)/2
	// y2(t) = y + vy * t - l*sin(w0+w*t)/2
	// 
	// ���ڷ��� x1(t) = left �Ⱦ������ô���������⣬�ʿ�������ֵ�ⷨ
	//
	// �ö��ַ�����Щ���̣���Ҫ�ҵ�����ֵt1��t2��ʹ�õ�t����t1<=t<=t2ʱ��
	// f(t)Ϊ�����������������������f(t1)��f(t2)��ţ�����ʹ�ö��ַ�
	// ���GetCollisionTimeToBoundary

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
	CollisionSide side, // ��Ҫ������ײ����
	double length,      // ����ȫ��
	double z0,          // �����ĵ����꣨x��y���꣬����ײ�����йأ�
	double zz,          // �÷���߽��λ�ã�x��y���꣬����ײ�����йأ�
	double vz,          // ���ڸ÷����ƶ������ٶ�
	double cur_ao,      // ��ǰ������б����
	double av)          // ��ǰ������ת���ٶȣ���λΪ���ȣ���ʱ��Ϊ����
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

	double ao_h; // ����ǰ����ת����ת��ˮƽ���򣨶����ұ߽���ԣ�������±�
	             // �������Ϊ��ֱ������ͬ������Ļ���ƫ����
	if (side == CS_LEFT || side == CS_RIGHT)
		ao_h = radian_distance(cur_ao, 0);
	else
		ao_h = radian_distance(cur_ao, PI/2);
	ASSERT(ao_h >= 0.0 && ao_h < PI);
	const double t_h = fabs(ao_h / av);	// ת��ˮƽ��������ʱ��
	const double z_h = z0 + vz * t_h;	// ת��ˮƽ����ʱ�����ĵ��λ��

	double t1, t2;
	if (side == CS_LEFT || side == CS_TOP)
	{
		if (z_h - length/2.0 <= zz)
		{
			// �ӵ�ǰλ��ת����һ��ˮƽλ�õ�ʱ���Ѿ�����߽�
			t1 = 0.0;
			t2 = t_h;
		}
		else
		{
			// ����ת��Ȧ�����ʱ�䣬�༴������ˮƽ����ʼ��ת���ٴλص�ˮƽ�����ʱ��
			const double t_semicircle = PI / fabs(av);
			t1 = t_h + t_semicircle * floor((z_h - length/2.0 - zz) / (fabs(vz) * t_semicircle));
			t2 = t1 + t_semicircle;
		}
	}
	else // if (side == CS_RIGHT || side == CS_BOTTOM)
	{
		if (z_h + length/2.0 >= zz)
		{
			// �ӵ�ǰλ��ת����һ��ˮƽλ�õ�ʱ���Ѿ�����߽�
			t1 = 0.0;
			t2 = t_h;
		}
		else
		{
			// ����ת��Ȧ�����ʱ�䣬�༴������ˮƽ����ʼ��ת���ٴλص�ˮƽ�����ʱ��
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
