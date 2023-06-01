#pragma once
#include "defines.h"
#include "ArryUtils.h"
#include <algorithm>
using namespace std;
class WCONTOUR_API Interpolate
{
private:
	const double pi = 14159265358;
	const double twopi = 2.0 * 3.14159265358;

	static int rows;
	static int cols;

public:
	static void setRowCol(int r, int c);
	/// <summary>
	/// Create grid x/y coordinate arrays with x/y delt
	/// </summary>
	/// <param name="Xlb">X of left-bottom</param>
	/// <param name="Ylb">Y of left-bottom</param>
	/// <param name="Xrt">X of right-top</param>
	/// <param name="Yrt">Y of right-top</param>
	/// <param name="XDelt">X delt</param>
	/// <param name="YDelt">Y delt</param>
	/// <param name="X">Output X array</param>
	/// <param name="Y">Output Y array</param>
	static void CreateGridXY_Delt(double Xlb, double Ylb, double Xrt, double Yrt,
		double XDelt, double YDelt, double* &X, double* &Y);

	/// <summary>
	/// Create grid x/y coordinate arrays with x/y number
	/// </summary>
	/// <param name="Xlb">X of left-bottom</param>
	/// <param name="Ylb">Y of left-bottom</param>
	/// <param name="Xrt">X of right-top</param>
	/// <param name="Yrt">Y of right-top</param>
	/// <param name="Xnum">X number</param>
	/// <param name="Ynum">Y number</param>
	/// <param name="X">Output X array</param>
	/// <param name="Y">Output Y array</param>
	static void CreateGridXY_Num(double Xlb, double Ylb, double Xrt, double Yrt,
		int Xnum, int Ynum, double* &X, double* &Y);

	/// <summary>
	/// Interpolation with IDW neighbor method
	/// </summary>
	/// <param name="SCoords">Discrete data array</param>
	/// <param name="X">Grid X array</param>
	/// <param name="Y">Grid Y array</param>
	/// <param name="NumberOfNearestNeighbors">Number of nearest neighbors</param>
	/// <returns>Interpolated grid Data</returns>
	static double** Interpolation_IDW_Neighbor(double** SCoords, double* X, double* Y,
		int NumberOfNearestNeighbors);

	/// <summary>
	/// Interpolation with IDW neighbor method
	/// </summary>
	/// <param name="SCoords">Discrete data array</param>
	/// <param name="X">Grid X array</param>
	/// <param name="Y">Grid Y array</param>
	/// <param name="NumberOfNearestNeighbors">Number of nearest neighbors</param>
	/// <param name="unDefData">Undefine data value</param>
	/// <returns>Interpolated grid data</returns>
	static double** Interpolation_IDW_Neighbor(double** SCoords, double* X, double* Y,
		int NumberOfNearestNeighbors, double unDefData);

	/// <summary>
	/// Interpolation with IDW neighbor method
	/// </summary>
	/// <param name="SCoords">Discrete data array</param>
	/// <param name="X">Grid X array</param>
	/// <param name="Y">Grid Y array</param>
	/// <param name="NeededPointNum">Needed at leat point number</param>
	/// <param name="radius">Search radius</param>
	/// <param name="unDefData">Undefine data</param>
	/// <returns>Interpolated grid data</returns>
	static double** Interpolation_IDW_Radius(double** SCoords, double* X, double* Y,
		int NeededPointNum, double radius, double unDefData);

	/// <summary>
	/// Interpolate from grid data using bi-linear method
	/// </summary>
	/// <param name="GridData">input grid data</param>
	/// <param name="X">input x coordinates</param>
	/// <param name="Y">input y coordinates</param>
	/// <param name="unDefData">Undefine data</param>
	/// <param name="nX">output x coordinates</param>
	/// <param name="nY">output y coordinates</param>
	/// <returns>Output grid data</returns>
	static double** Interpolation_Grid(double** GridData, double* X, double* Y, double unDefData,
		double* &nX, double* &nY);

	/// <summary>
	/// Cressman analysis with default radius of 10, 7, 4, 2, 1
	/// </summary>
	/// <param name="stationData">station data array - x,y,value</param>
	/// <param name="X">X array</param>
	/// <param name="Y">Y array</param>
	/// <param name="unDefData">undefine data</param>        
	/// <returns>result grid data</returns>
	static double** Cressman(double** stationData, double* X, double* Y, double unDefData);

	/// <summary>
	/// Cressman analysis
	/// </summary>
	/// <param name="stationData">station data array - x,y,value</param>
	/// <param name="X">X array</param>
	/// <param name="Y">Y array</param>
	/// <param name="unDefData">undefine data</param>
	/// <param name="radList">radius list</param>
	/// <returns>result grid data</returns>
	static double** Cressman(double** stationData, double* X, double* Y, double unDefData, vector<double> radList);


	/// <summary>
	/// Assign point value to grid value
	/// </summary>
	/// <param name="SCoords">point value array</param>
	/// <param name="X">X coordinate</param>
	/// <param name="Y">Y coordinate</param>
	/// <param name="unDefData">undefined value</param>
	/// <returns>grid data</returns>
	static double** AssignPointToGrid(double** SCoords, double* X, double* Y,
		double unDefData);

private:
	static bool DoubleEquals(double a, double b);
		
};

