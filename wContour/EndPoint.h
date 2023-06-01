#pragma once
#include "defines.h"
#include "PointD.h"
class WCONTOUR_API EndPoint
{
public:
	PointD sPoint;
	PointD Point;
	int Index;
	int BorderIdx;

public:
	EndPoint();
};

