#include "pch.h"
#include "Extent.h"

Extent::Extent()
	:xMin(0),yMin(0),xMax(0),yMax(0)
{

}

Extent::Extent(double minX, double maxX, double minY, double maxY)
{
	xMin = minX;
	xMax = maxX;
	yMin = minY;
	yMax = maxY;
}

bool Extent::Include(Extent bExtent)
{
	return xMin <= bExtent.xMin && xMax >= bExtent.xMax && yMin <= bExtent.yMin && yMax >= bExtent.yMax;
}