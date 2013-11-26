#pragma once

#include <math.h>

typedef struct stPolarVertex     //極座標
{
    int		r;			// 原点からの距離 
    double	p;			// Z軸回りの角度 
	double	q;			// X-Y平面からの角度 
} Polar ;

#include "Vector3D.h"
#include "Matrix4D.h"

