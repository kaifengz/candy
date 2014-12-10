
#ifndef GRAPHICS_AUX_H__INCLUDED__2011_02_01_
#define GRAPHICS_AUX_H__INCLUDED__2011_02_01_

#include "ZkfGraphics.h"

void CollideAndMove(
		vector<ZMovableCircle*> objects,
		const ZBoundary &boundary,
		MEASURE_T time,
		float object_rebound_coeff,
		float boundary_rebound_coeff);

#endif // GRAPHICS_AUX_H__INCLUDED__2011_02_01_
