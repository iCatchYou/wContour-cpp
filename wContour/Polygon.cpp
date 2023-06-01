#include "pch.h"
#include "Polygon.h"
#include "Contour.h"
WPolygon::WPolygon()
	:IsBorder(false),
	IsInnerBorder(false),
	LowValue(0.0),
	HighValue(0.0),
	IsClockWise(false),
	StartPointIdx(0),
	IsHighCenter(0),
	Area(0.0),
	HoleIndex(0)
{
	
}

WPolygon WPolygon::Clone()
{
	WPolygon aPolygon;
	aPolygon.IsBorder = IsBorder;
	aPolygon.LowValue = LowValue;
	aPolygon.HighValue = HighValue;
	aPolygon.IsClockWise = IsClockWise;
	aPolygon.StartPointIdx = StartPointIdx;
	aPolygon.IsHighCenter = IsHighCenter;
	aPolygon.Extent = Extent;
	aPolygon.Area = Area;
	aPolygon.OutLine = OutLine;
	aPolygon.HoleLines = HoleLines;
	aPolygon.HoleIndex = HoleIndex;

	return aPolygon;
}


bool WPolygon::HasHoles()
{
	return (HoleLines.size() > 0);
}

/// <summary>
/// Add a hole by a polygon
/// </summary>
/// <param name="aPolygon">polygon</param>
void WPolygon::AddHole(WPolygon aPolygon)
{
	HoleLines.push_back(aPolygon.OutLine);
}

/// <summary>
/// Add a hole by point list
/// </summary>
/// <param name="pList">point list</param>
void WPolygon::AddHole(vector<PointD> pList)
{
	if (Contour::IsClockwise(pList))
		//pList.Reverse();
		std::reverse(pList.begin(), pList.end());

	PolyLine aLine;
	aLine.PointList = pList;
	HoleLines.push_back(aLine);
}