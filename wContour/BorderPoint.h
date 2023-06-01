#pragma once
#include "defines.h"
#include "PointD.h"
class WCONTOUR_API BorderPoint
{
public:
	/// <summary>
	/// Identifer
	/// </summary>
	int Id;
	/// <summary>
	/// Border index
	/// </summary>
	int BorderIdx;
	/// <summary>
	/// Border inner index
	/// </summary>
	int BInnerIdx;
	/// <summary>
	/// Point
	/// </summary>
	PointD Point;
	/// <summary>
	/// Value
	/// </summary>
	double Value;
	/// <summary>
	/// Clone
	/// </summary>
	/// <returns>cloned borderpoint</returns>
public:
	BorderPoint();
	BorderPoint Clone();
};

