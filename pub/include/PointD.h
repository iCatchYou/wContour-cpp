#pragma once
class  PointD
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

