#include "pch.h"
#include "BorderPoint.h"

BorderPoint::BorderPoint()
	:Id(0),BorderIdx(0),BInnerIdx(0),Value(0.0)
{

}

BorderPoint BorderPoint::Clone()
{
	BorderPoint aBP;
	aBP.Id = Id;
	aBP.BorderIdx = BorderIdx;
	aBP.BInnerIdx = BInnerIdx;
	aBP.Point = Point;
	aBP.Value = Value;

	return aBP;
}