#pragma once

#include "Extent.h"
#include "PointD.h"
#include "IJPoint.h"
#include <vector>
using namespace std;
class  BorderLine
{
public:
	/// <summary>
	/// Area
	/// </summary>
	double area;
	/// <summary>
	/// Extent
	/// </summary>
	Extent extent;
	/// <summary>
	/// Is outline
	/// </summary>
	bool isOutLine;
	/// <summary>
	/// Is clockwise
	/// </summary>
	bool isClockwise;
	/// <summary>
	/// Point list
	/// </summary>
	vector<PointD> pointList;
	/// <summary>
	/// IJPoint list
	/// </summary>
	vector<IJPoint> ijPointList;

public:
	BorderLine();
};

