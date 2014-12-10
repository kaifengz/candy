// ZkfGraphicsAux.cpp
//

#include "StdAfx.h"
#include "ZkfGraphicsAux.h"
#include "util.h"

void CollideAndMove(
		vector<ZMovableCircle*> objects,
		const ZBoundary &boundary,
		MEASURE_T time,
		float object_rebound_coeff,
		float boundary_rebound_coeff)
{
	ASSERT(objects.size() > 0);
	ASSERT(time > 0);

	const MEASURE_T INVALID_TIME = (MEASURE_T) (-1);
	const MEASURE_T total_time = time;

	while (time > total_time * 0.000001)
	{
		// 物体相互之间的碰撞
		vector<MEASURE_T> collision_time_i;
		for (unsigned int i=0; i<objects.size(); i++)
		{
			for (unsigned int j=0; j<=i; j++)
				collision_time_i.push_back( INVALID_TIME );
			for (unsigned int j=i+1; j<objects.size(); j++)
			{
				collision_time_i.push_back(
					objects[i]->GetCollisionTime( *objects[j] ) );
			}
		}
		const int min_i = GetPositiveMinIndex(collision_time_i);
		const MEASURE_T t_min_i =
			(min_i>=0 ? collision_time_i[min_i] : INVALID_TIME);

		// 与区域边界的碰撞时间
		vector<MEASURE_T> collision_time_b;
		for (unsigned int i=0; i<objects.size(); i++)
		{
			collision_time_b.push_back(
				objects[i]->GetCollisionTime( boundary ) );
		}
		const int min_r = GetPositiveMinIndex(collision_time_b);
		const MEASURE_T t_min_b =
			(min_r>=0 ? collision_time_b[min_r] : INVALID_TIME);

		if ((t_min_i < 0 || t_min_i >= time) &&
			(t_min_b < 0 || t_min_b >= time) )
		{
			// 在当前时间片内没有碰撞
			for (unsigned int i=0; i<objects.size(); i++)
				objects[i]->ActMoving(time);
			break;
		}

		// 发生碰撞的时间
		const MEASURE_T t_min =
			(t_min_i>=0 && t_min_b>=0 ? min(t_min_i, t_min_b) : (t_min_i>=0 ? t_min_i : t_min_b));
		ASSERT(t_min >= 0); // may not ASSERT(t_min <= total_time), notice the float operation presision issue

		// 向前移动
		if (t_min > 0)
		{
			for (unsigned int i=0; i<objects.size(); i++)
				objects[i]->ActMoving(t_min);
		}

		// 执行碰撞
		for (unsigned int i=0; i<objects.size(); i++)
		{
			for (unsigned int j=i+1; j<objects.size(); j++)
			{
				if (collision_time_i[ i*objects.size() + j ] == t_min)
					objects[i]->Collide( *objects[j], object_rebound_coeff);
			}
		}
		for (unsigned int i=0; i<objects.size(); i++)
		{
			if (collision_time_b[i] == t_min)
				objects[i]->Collide( boundary, boundary_rebound_coeff );
		}

		time -= t_min;
	}
}
