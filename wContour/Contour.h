#pragma once
/************************************************************************************
*  __________  _________.____														*
*  \______   \/   _____/|    |				  wCountour  v1.0.0.0					*
*   |     ___/\_____  \ |    |			Designed for generate contour lines			*
*   |    |    /        \|    |___				on Windows (c++)					*
*   |____|   /_______  /|_______ \													*
*                    \/         \/		(C) 2020-2023. All rights reserved.			*
*																					*
*  @file        Contour.h															*
*  @author      YangZe(709102202@qq.com)											*
*  @date        2023/06/01 09:14													*
*																					*
*  @brief       Generate contour lines												*
*  This file is part of wContour library.											*
*																					*
*  This is a dynamic library for generating contour lines, which has been rewritten *
*  from the C # version of the wContour library.The usage methods are consistent,	*
*  but there are slight differences.												*
*  Github address for C# : https://github.com/meteoinfo/wContour_CSharp				*
*  Github address for C# Demo: https://github.com/abelli85/wContourDemo_2015-1-29	*
*																					*
*  @version																			*
*    - 1.0.0.0    2023/06/01 09:14    YangZe    Create file							*
*************************************************************************************/
#include "defines.h"
#include <vector >
#include "EndPoint.h"
#include "PolyLine.h"
#include "Border.h"
#include "Extent.h"
#include "ArryUtils.h"
#include "PointF.h"
#include "Line.h"
#include "Polygon.h"
#include "BorderPoint.h"
#include <algorithm>

#include <tuple>

using namespace std;
class WCONTOUR_API Contour
{
private:
	 static vector<EndPoint> _endPointList;
	 static int rows;
	 static int cols;

public:
	static void setRowCol(int r,int c);
	/// <summary>
	/// Tracing contour lines from the grid data with undefine data
	/// </summary>
	/// <param name="S0">input grid data</param>
	/// <param name="X">X coordinate array</param>
	/// <param name="Y">Y coordinate array</param>
	/// <param name="nc">number of contour values</param>
	/// <param name="contour">contour value array</param>
	/// <param name="undefData">Undefine data</param>
	/// <param name="borders">borders</param>
	/// <param name="S1">data flag array</param>
	/// <returns>Contour line list</returns>
	static vector<PolyLine> TracingContourLines(double** S0, double* X, double* Y,
		int nc, double* contour, double undefData, vector<Border> borders, int** S1);
	
	/// <summary>
	/// Tracing contour borders of the grid data with undefined data.
	/// Grid data are from left to right and from bottom to top.
	/// Grid data array: first dimention is Y, second dimention is X.
	/// </summary>
	/// <param name="S0">grid data</param>
	/// <param name="X">x coordinate</param>
	/// <param name="Y">y coordinate</param>
	/// <param name="S1"></param>
	/// <param name="undefData">undefine data</param>
	/// <returns>borderline list</returns>
	static vector<Border> TracingBorders(double** S0, double* X, double* Y, int** &S1, double undefData);

	/// <summary>
	/// Determin if the point list is clockwise
	/// </summary>
	/// <param name="pointList">point list</param>
	/// <returns>clockwise or not</returns>
	static bool IsClockwise(vector<PointD> pointList);

	/// <summary>
	/// Get cross point of two line segments
	/// </summary>
	/// <param name="aP1">point 1 of line a</param>
	/// <param name="aP2">point 2 of line a</param>
	/// <param name="bP1">point 1 of line b</param>
	/// <param name="bP2">point 2 of line b</param>
	/// <returns>cross point</returns>
	static PointF GetCrossPoint(PointF aP1, PointF aP2, PointF bP1, PointF bP2);

	
	/// <summary>
	/// Smooth points
	/// </summary>
	/// <param name="pointList">point list</param>
	/// <returns>new points</returns>
	static vector<PointD> SmoothPoints(vector<PointD> pointList);	
	
	/// <summary>
	/// Smooth Polylines
	/// </summary>
	/// <param name="aLineList">Polyline list</param>
	/// <returns>Polyline list after smoothing</returns>
	static vector<PolyLine> SmoothLines(vector<PolyLine> aLineList);


	/// <summary>
	/// Tracing polygons from contour lines and borders
	/// </summary>
	/// <param name="S0">input grid data</param>
	/// <param name="cLineList">contour lines</param>
	/// <param name="borderList">borders</param>
	/// <param name="contour">contour values</param>
	/// <returns>traced contour polygons</returns>
	static vector<WPolygon> TracingPolygons(double** S0, vector<PolyLine> cLineList, vector<Border> borderList, double* contour, int contourNum);

	/// <summary>
	/// Judge if a point is in a polygon
	/// </summary>
	/// <param name="poly">polygon border</param>
	/// <param name="aPoint">point</param>
	/// <returns>If the point is in the polygon</returns>
	static bool PointInPolygon(vector<PointD> poly, PointD aPoint);

	/// <summary>
	/// Judge if a point is in a polygon
	/// </summary>
	/// <param name="aPolygon">polygon</param>
	/// <param name="aPoint">point</param>
	/// <returns>is in</returns>
	static bool PointInPolygon(WPolygon aPolygon, PointD aPoint);

	/// <summary>
	/// Clip polylines with a border polygon
	/// </summary>
	/// <param name="polylines">polyline list</param>
	/// <param name="clipPList">cutting border point list</param>
	/// <returns>cutted polylines</returns>
	static vector<PolyLine> ClipPolylines(vector<PolyLine> polylines, vector<PointD> clipPList);

	/// <summary>
	/// Clip polygons with a border polygon
	/// </summary>
	/// <param name="polygons">polygon list</param>
	/// <param name="clipPList">cutting border point list</param>
	/// <returns>cutted polygons</returns>
	static vector<WPolygon> ClipPolygons(vector<WPolygon> polygons, vector<PointD> clipPList);

	/// <summary>
	/// Tracing stream lines
	/// </summary>
	/// <param name="U">U component array</param>
	/// <param name="V">V component array</param>
	/// <param name="X">X coordinate array</param>
	/// <param name="Y">Y coordinate array</param>
	/// <param name="UNDEF">undefine data</param>
	/// <param name="density">stream line density</param>
	/// <returns>streamlines</returns>
	static vector<PolyLine> TracingStreamline(double** U, double** V, double* X, double* Y, double UNDEF, int density);

private:	
	static double GetExtentAndArea(vector<PointD> pList, Extent& aExtent);
	static bool DoubleEquals(double a, double b);
		
	/// <summary>
	/// Create contour lines from the grid data with undefine data
	/// </summary>
	/// <param name="S0">input grid data</param>
	/// <param name="X">X coordinate array</param>
	/// <param name="Y">Y coordinate array</param>
	/// <param name="nc">number of contour values</param>
	/// <param name="contour">contour value array</param>
	/// <param name="nx">interval of X coordinate</param>
	/// <param name="ny">interval of Y coordinate</param>
	/// <param name="S1"></param>
	/// <param name="undefData">Undefine data</param>
	/// <param name="borders">Border line list</param>
	/// <returns>Contour line list</returns>
	static vector<PolyLine> CreateContourLines_UndefData(double** S0, double* X, double* Y,
		int nc, double* contour, double nx, double ny, int** S1, double undefData, vector<Border> borders);

	/// <summary>
	/// Create contour lines
	/// </summary>
	/// <param name="S0">input grid data array</param>
	/// <param name="X">X coordinate array</param>
	/// <param name="Y">Y coordinate array</param>
	/// <param name="nc">number of contour values</param>
	/// <param name="contour">contour value array</param>
	/// <param name="nx">Interval of X coordinate</param>
	/// <param name="ny">Interval of Y coordinate</param>
	/// <returns></returns>
	static vector<PolyLine> CreateContourLines(double** S0, double* X, double* Y, int nc, double* contour, double nx, double ny);

	/// <summary>
	/// Cut contour lines with a polygon. Return the polylines inside of the polygon
	/// </summary>
	/// <param name="alinelist">polyline list</param>
	/// <param name="polyList">border points of the cut polygon</param>
	/// <returns>Inside Polylines after cut</returns>
	static vector<PolyLine> CutContourWithPolygon(vector<PolyLine> alinelist, vector<PointD> polyList);

	/// <summary>
	/// Cut contour lines with a polygon. Return the polylines inside of the polygon
	/// </summary>
	/// <param name="alinelist">polyline list</param>
	/// <param name="aBorder">border for cutting</param>
	/// <returns>Inside Polylines after cut</returns>
	static vector<PolyLine> CutContourLines(vector<PolyLine> alinelist, Border aBorder);
		
		
	/// <summary>
	/// Create contour polygons
	/// </summary>
	/// <param name="LineList">contour lines</param>
	/// <param name="aBound">gid data extent</param>
	/// <param name="contour">contour values</param>
	/// <returns>contour polygons</returns>
	static vector<WPolygon> CreateContourPolygons(vector<PolyLine> LineList, Extent aBound, double* contour, int contourNum);

	/// <summary>
	/// Create polygons from cutted contour lines
	/// </summary>
	/// <param name="LineList">polylines</param>
	/// <param name="polyList">Border point list</param>
	/// <param name="aBound">extent</param>
	/// <param name="contour">contour values</param>
	/// <returns>contour polygons</returns>
	static vector<WPolygon> CreateCutContourPolygons(vector<PolyLine> LineList, vector<PointD> polyList, Extent aBound, double* contour, int contourNum);

	/// <summary>
	/// Create contour polygons from borders
	/// </summary>
	/// <param name="S0">Input grid data array</param>
	/// <param name="cLineList">Contour lines</param>
	/// <param name="borderList">borders</param>
	/// <param name="aBound">extent</param>
	/// <param name="contour">contour value</param>
	/// <returns>contour polygons</returns>
	static vector<WPolygon> CreateBorderContourPolygons(double** S0, vector<PolyLine> cLineList, vector<Border> borderList, Extent aBound, double* contour, int contourNum);

	static bool TracingStreamlinePoint(PointD& aPoint, double** Dx, double** Dy, double* X, double* Y,
		int& ii, int& jj, bool isForward);

	static bool TraceBorder(int** S1, int i1, int i2, int j1, int j2, int& i3, int& j3);

	static vector<PolyLine> Isoline_UndefData(double** S0, double* X, double* Y,
		double W, double nx, double ny,
		double**& S, double**& H, int*** SB, int*** HB, int lineNum);

	static bool TraceIsoline_UndefData(int i1, int i2, double**& H, double**& S, int j1, int j2, double* X,
		double* Y, double nx, double ny, double a2x, int& i3,
		int& j3, double& a3x, double& a3y, bool& isS);
	
	static std::tuple<int, int, double, double> TraceIsoline(int i1, int i2, double**& H, double**& S, int j1, int j2, double* X,
		double* Y, double nx, double ny, double a2x);

	static vector<PolyLine> Isoline_Bottom(double** S0, double* X, double* Y, double W, double nx, double ny,
		double**& S, double**& H);

	static vector<PolyLine> Isoline_Left(double** S0, double* X, double* Y, double W, double nx, double ny,
		double**& S, double**& H);

	static vector<PolyLine> Isoline_Top(double** S0, double* X, double* Y, double W, double nx, double ny,
		double** &S, double** &H);

	static vector<PolyLine> Isoline_Right(double** S0, double* X, double* Y, double W, double nx, double ny,
		double** &S, double** &H);

	static vector<PolyLine> Isoline_Close(double** S0, double* X, double* Y, double W, double nx, double ny,
		double** &S, double** &H);

	static vector<WPolygon> TracingPolygons(vector<PolyLine> LineList, vector<BorderPoint> borderList, Extent bBound, double* contour, int contoursNum);

	static vector<WPolygon> TracingPolygons(vector<PolyLine> LineList, vector<BorderPoint> borderList);

	static vector<WPolygon> TracingClipPolygons(WPolygon inPolygon, vector<PolyLine> LineList, vector<BorderPoint> borderList);

	static vector<WPolygon> JudgePolygonHighCenter(vector<WPolygon> borderPolygons, vector<WPolygon> closedPolygons,
		vector<PolyLine> aLineList, vector<BorderPoint> borderList);

	static vector<WPolygon> JudgePolygonHighCenter_old(vector<WPolygon> borderPolygons, vector<WPolygon> closedPolygons,
		vector<PolyLine> aLineList, vector<BorderPoint> borderList);
	
	static vector<WPolygon> TracingPolygons_Ring(vector<PolyLine> LineList, vector<BorderPoint> borderList, Border aBorder,
		double* contour, int* pNums);

	static vector<WPolygon> AddPolygonHoles(vector<WPolygon> polygonList);

	static vector<WPolygon> AddPolygonHoles_Ring(vector<WPolygon> polygonList);

	static void AddHoles_Ring(vector<WPolygon>& polygonList, vector<vector<PointD>> holeList);

	static vector<PolyLine> CutPolyline(PolyLine inPolyline, vector<PointD> clipPList);

	static vector<WPolygon> CutPolygon_Hole(WPolygon inPolygon, vector<PointD> clipPList);

	static vector<WPolygon> CutPolygon(WPolygon inPolygon, vector<PointD> clipPList);

	static bool TwoPointsInside(int a1, int a2, int b1, int b2);

	static vector<PointD> BSplineScanning(vector<PointD> pointList, int sum);

	static void BSpline(vector<PointD> pointList, double t, int i, double& X, double& Y);

	static double f0(double t);

	static double f1(double t);

	static double f2(double t);

	static double f3(double t);

	static void fb(double t, double* &fs);

	static Extent GetExtent(vector<PointD> pList);

	static bool IsLineSegmentCross(Line lineA, Line lineB);

	static bool IsExtentCross(Extent aBound, Extent bBound);

	static PointD GetCrossPoint(Line lineA, Line lineB);

	static vector<BorderPoint> InsertPoint2Border(vector<BorderPoint> bPList, vector<BorderPoint> aBorderList);

	static vector<BorderPoint> InsertPoint2RectangleBorder(vector<PolyLine> LineList, Extent aBound);

	static vector<BorderPoint> InsertEndPoint2Border(vector<EndPoint> EPList, vector<BorderPoint> aBorderList);

	static vector<BorderPoint> InsertPoint2Border_Ring(double** S0, vector<BorderPoint> bPList, Border aBorder, int* &pNums);

};

