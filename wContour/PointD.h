#pragma once
#include "defines.h"
class WCONTOUR_API PointD
{
public:
	double X;
	double Y;
public:
	PointD();
	PointD(double x, double y);	
	PointD& operator=(const PointD& other);


	PointD Clone();
};

