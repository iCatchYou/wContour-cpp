#pragma once
#include "defines.h"
#include "Extent.h"
#include "PolyLine.h"
#include <vector>
using namespace std;

class WCONTOUR_API WPolygon
{
public:
	/// <summary>
	/// If is border contour polygon
	/// </summary>
	bool IsBorder;
	/// <summary>
	/// If there is only inner border
	/// </summary>
	bool IsInnerBorder;
	/// <summary>
	/// Start value
	/// </summary>
	double LowValue;
	/// <summary>
	/// End value
	/// </summary>
	double HighValue;
	/// <summary>
	/// If clockwise
	/// </summary>
	bool IsClockWise;
	/// <summary>
	/// Start point index
	/// </summary>
	int StartPointIdx;
	/// <summary>
	/// If high center
	/// </summary>
	bool IsHighCenter;
	/// <summary>
	/// Extent - bordering rectangle
	/// </summary>
	Extent Extent;
	/// <summary>
	/// Area
	/// </summary>
	double Area;
	/// <summary>
	/// Outline
	/// </summary>
	PolyLine OutLine;
	/// <summary>
	/// Hole lines
	/// </summary>
	vector<PolyLine> HoleLines;
	/// <summary>
	/// Hole index
	/// </summary>
	int HoleIndex;

public:
	WPolygon();
	
	WPolygon Clone();

	/// <summary>
	/// Get if has holes
	/// </summary>
	bool HasHoles();

	/// <summary>
	/// Add a hole by a polygon
	/// </summary>
	/// <param name="aPolygon">polygon</param>
	void AddHole(WPolygon aPolygon);

	/// <summary>
	/// Add a hole by point list
	/// </summary>
	/// <param name="pList">point list</param>
	void AddHole(vector<PointD> pList);

	
};

