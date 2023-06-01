#pragma once
class  Extent
{
public:
	double xMin;
	double yMin;
	double xMax;
	double yMax;

public:
	Extent();
	Extent(double minX, double maxX, double minY, double maxY);
	bool Include(Extent bExtent);
};

