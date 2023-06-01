#include "pch.h"
#include "PointD.h"


PointD::PointD()
	:X(0.0), Y(0.0)
{

}

PointD::PointD(double x, double y)
{
	X = x;
	Y = y;
}

PointD& PointD::operator=(const PointD& other) {
	X = other.X;
	Y = other.Y;
	return *this;
}

PointD PointD::Clone()
{
	PointD pt;
	pt.X = X;
	pt.Y = Y;

	return pt;
}