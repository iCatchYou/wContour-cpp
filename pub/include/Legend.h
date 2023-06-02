#pragma once
#include "PointD.h"
#include <vector>

using namespace std;

/// <summary>
		/// Legend parameter
		/// </summary>
struct legendPara
{
	/// <summary>
	/// If is vertical
	/// </summary>
	bool isVertical;
	/// <summary>
	/// Start point
	/// </summary>
	PointD startPoint;
	/// <summary>
	/// Length
	/// </summary>
	double length;
	/// <summary>
	/// Width
	/// </summary>
	double width;
	/// <summary>
	/// Contour values
	/// </summary>
	std::vector<double> contourValues;
	/// <summary>
	/// If is triangle
	/// </summary>
	bool isTriangle;
};

/// <summary>
/// Legend polygon
/// </summary>
struct lPolygon
{
	/// <summary>
	/// Value
	/// </summary>
	double value;
	/// <summary>
	/// If is first
	/// </summary>
	bool isFirst;
	/// <summary>
	/// Point list
	/// </summary>
	vector<PointD> pointList;
};


class Legend
{
public:

	Legend();
	
	/// <summary>
	/// Create legend polygons
	/// </summary>
	/// <param name="aLegendPara"> legend parameters</param>
	/// <returns>legend polygons</returns>
	static vector<lPolygon> CreateLegend(legendPara aLegendPara);

};

