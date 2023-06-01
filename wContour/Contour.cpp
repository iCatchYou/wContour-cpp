#include "pch.h"
#include "Contour.h"

#include <iostream>

vector<EndPoint> Contour::_endPointList;
int Contour::rows = 0;
int Contour::cols = 0;

void Contour::setRowCol(int r, int c)
{
	Contour::rows = r;
	Contour::cols = c;
}


vector<PolyLine> Contour::TracingContourLines(double** S0, double* X, double* Y,
	int nc, double* contour, double undefData, vector<Border> borders, int** S1)
{
	double dx = X[1] - X[0];
	double dy = Y[1] - Y[0];
	vector<PolyLine> contourLines = CreateContourLines_UndefData(S0, X, Y, nc, contour, dx, dy, S1, undefData, borders);

	return contourLines;
}

vector<Border> Contour::TracingBorders(double** S0, double* X, double* Y, int** &S1, double undefData)
{
	vector<BorderLine> borderLines;

	int m = rows, n = cols, i, j;
	//ArryUtils::Get2DArryRowCol(S0, m, n);
	//m = S0.GetLength(0);    //Y
	//n = S0.GetLength(1);    //X

	S1 = ArryUtils::create2DArray<int>(m, n,0);
	//S1 = new int[m, n];    //---- New array (0 with undefine data, 1 with data)
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (DoubleEquals(S0[i][j], undefData))    //Undefine data
				S1[i][j] = 0;
			else
				S1[i][j] = 1;
		}
	}

	//---- Border points are 1, undefine points are 0, inside data points are 2
	//l - Left; r - Right; b - Bottom; t - Top; lb - LeftBottom; rb - RightBottom; lt - LeftTop; rt - RightTop
	int l, r, b, t, lb, rb, lt, rt;
	for (i = 1; i < m - 1; i++)
	{
		for (j = 1; j < n - 1; j++)
		{
			if (S1[i][j] == 1)    //data point
			{
				l = S1[i][j - 1];
				r = S1[i][j + 1];
				b = S1[i - 1][j];
				t = S1[i + 1][j];
				lb = S1[i - 1][j - 1];
				rb = S1[i - 1][j + 1];
				lt = S1[i + 1][j - 1];
				rt = S1[i + 1][j + 1];

				if (l > 0 && r > 0 && b > 0 && t > 0 && lb > 0 && rb > 0 && lt > 0 && rt > 0)
					S1[i][j] = 2;    //Inside data point

				if (l + r + b + t + lb + rb + lt + rt <= 2)
					S1[i][j] = 0;    //Data point, but not more than 3 continued data points together.
				//So they can't be traced as a border (at least 4 points together).

			}
		}
	}

	//---- Remove isolated data points (up, down, left and right points are all undefine data).
	bool isContinue = false;
	while (true)
	{
		isContinue = false;
		for (i = 1; i < m - 1; i++)
		{
			for (j = 1; j < n - 1; j++)
			{
				if (S1[i][j] == 1)    //data point
				{
					l = S1[i][j - 1];
					r = S1[i][j + 1];
					b = S1[i - 1][j];
					t = S1[i + 1][j];
					lb = S1[i - 1][j - 1];
					rb = S1[i - 1][j + 1];
					lt = S1[i + 1][j - 1];
					rt = S1[i + 1][j + 1];
					if ((l == 0 && r == 0) || (b == 0 && t == 0))    //Up, down, left and right points are all undefine data
					{
						S1[i][j] = 0;
						isContinue = true;
					}
					if ((lt == 0 && r == 0 && b == 0) || (rt == 0 && l == 0 && b == 0) ||
						(lb == 0 && r == 0 && t == 0) || (rb == 0 && l == 0 && t == 0))
					{
						S1[i][ j] = 0;
						isContinue = true;
					}
				}
			}
		}
		if (!isContinue)    //untile no more isolated data point.
			break;
	}
	//Deal with grid data border points
	for (j = 0; j < n; j++)    //Top and bottom border points
	{
		if (S1[0][j] == 1)
		{
			if (S1[1][j] == 0)    //up point is undefine
				S1[0][j] = 0;
			else
			{
				if (j == 0)
				{
					if (S1[0][j + 1] == 0)
						S1[0][j] = 0;
				}
				else if (j == n - 1)
				{
					if (S1[0][n - 2] == 0)
						S1[0][j] = 0;
				}
				else
				{
					if (S1[0][j - 1] == 0 && S1[0][j + 1] == 0)
						S1[0][j] = 0;
				}
			}
		}
		if (S1[m - 1][j] == 1)
		{
			if (S1[m - 2][j] == 0)    //down point is undefine
				S1[m - 1][j] = 0;
			else
			{
				if (j == 0)
				{
					if (S1[m - 1][j + 1] == 0)
						S1[m - 1][j] = 0;
				}
				else if (j == n - 1)
				{
					if (S1[m - 1][n - 2] == 0)
						S1[m - 1][j] = 0;
				}
				else
				{
					if (S1[m - 1][j - 1] == 0 && S1[m - 1][j + 1] == 0)
						S1[m - 1][j] = 0;
				}
			}
		}
	}
	for (i = 0; i < m; i++)    //Left and right border points
	{
		if (S1[i][0] == 1)
		{
			if (S1[i][1] == 0)    //right point is undefine
				S1[i][0] = 0;
			else
			{
				if (i == 0)
				{
					if (S1[i + 1][0] == 0)
						S1[i][0] = 0;
				}
				else if (i == m - 1)
				{
					if (S1[m - 2][0] == 0)
						S1[i][0] = 0;
				}
				else
				{
					if (S1[i - 1][0] == 0 && S1[i + 1][0] == 0)
						S1[i][0] = 0;
				}
			}
		}
		if (S1[i][n - 1] == 1)
		{
			if (S1[i][n - 2] == 0)    //left point is undefine
				S1[i][n - 1] = 0;
			else
			{
				if (i == 0)
				{
					if (S1[i + 1][n - 1] == 0)
						S1[i][n - 1] = 0;
				}
				else if (i == m - 1)
				{
					if (S1[m - 2][n - 1] == 0)
						S1[i][n - 1] = 0;
				}
				else
				{
					if (S1[i - 1][n - 1] == 0 && S1[i + 1][n - 1] == 0)
						S1[i][n - 1] = 0;
				}
			}
		}
	}

	//---- Generate S2 array from S1, add border to S2 with undefine data.
	//int[, ] S2 = new int[m + 2, n + 2];
	int** S2 = ArryUtils::create2DArray<int>(m + 2, n + 2,0);
	for (i = 0; i < m + 2; i++)
	{
		for (j = 0; j < n + 2; j++)
		{
			if (i == 0 || i == m + 1)    //bottom or top border
				S2[i][j] = 0;
			else if (j == 0 || j == n + 1)    //left or right border
				S2[i][j] = 0;
			else
				S2[i][j] = S1[i - 1][j - 1];
		}
	}

	//---- Using times number of each point during chacing process.
	//int[, ] UNum = new int[m + 2, n + 2];
	int** UNum = ArryUtils::create2DArray<int>(m + 2, n + 2,0);
	for (i = 0; i < m + 2; i++)
	{
		for (j = 0; j < n + 2; j++)
		{
			if (S2[i][j] == 1)
			{
				l = S2[i][j - 1];
				r = S2[i][j + 1];
				b = S2[i - 1][j];
				t = S2[i + 1][j];
				lb = S2[i - 1][j - 1];
				rb = S2[i - 1][j + 1];
				lt = S2[i + 1][j - 1];
				rt = S2[i + 1][j + 1];
				//---- Cross point with two boder lines, will be used twice.
				if (l == 1 && r == 1 && b == 1 && t == 1 && ((lb == 0 && rt == 0) || (rb == 0 && lt == 0)))
					UNum[i][j] = 2;
				else
					UNum[i][j] = 1;
			}
			else
				UNum[i][j] = 0;
		}
	}

	//---- Tracing borderlines

	PointD aPoint;
	IJPoint aijPoint;
	BorderLine aBLine;
	vector<PointD> pointList;
	vector<IJPoint> ijPList;

	int sI, sJ, i1, j1, i2, j2, i3 = 0, j3 = 0;
	for (i = 1; i < m + 1; i++)
	{
		for (j = 1; j < n + 1; j++)
		{
			if (S2[i][j] == 1)    //Tracing border from any border point
			{				
				pointList = vector<PointD>();
				ijPList = vector<IJPoint>();
				aPoint = PointD();
				aPoint.X = X[j - 1];
				aPoint.Y = Y[i - 1];
				aijPoint = IJPoint();
				aijPoint.I = i - 1;
				aijPoint.J = j - 1;
				pointList.push_back(aPoint);
				ijPList.push_back(aijPoint);
				sI = i;
				sJ = j;
				i2 = i;
				j2 = j;
				i1 = i2;
				j1 = -1;    //Trace from left firstly                        

				while (true)
				{
					if (TraceBorder(S2, i1, i2, j1, j2, i3, j3))
					{
						i1 = i2;
						j1 = j2;
						i2 = i3;
						j2 = j3;
						UNum[i3][j3] = UNum[i3][j3] - 1;
						if (UNum[i3][j3] == 0)
							S2[i3][j3] = 3;    //Used border point
					}
					else
						break;

					aPoint.X = X[j3 - 1];
					aPoint.Y = Y[i3 - 1];
					aijPoint.I = i3 - 1;
					aijPoint.J = j3 - 1;
					pointList.push_back(aPoint);
					ijPList.push_back(aijPoint);
					if (i3 == sI && j3 == sJ)
						break;
				}
				UNum[i][j] = UNum[i][j] - 1;
				if (UNum[i][j] == 0)
					S2[i][j] = 3;    //Used border point
				//UNum[i, j] = UNum[i, j] - 1;
				if (pointList.size() > 1)
				{
					aBLine =  BorderLine();
					aBLine.area = GetExtentAndArea(pointList, aBLine.extent);
					aBLine.isOutLine = true;
					aBLine.isClockwise = true;
					aBLine.pointList = pointList;
					aBLine.ijPointList = ijPList;
					borderLines.push_back(aBLine);
				}
			}
		}
	}

	//---- Form borders
	vector<Border> borders;
	Border aBorder;
	BorderLine aLine, bLine;
	//---- Sort borderlines with area from small to big.
	//For inside border line analysis
	for (i = 1; i < borderLines.size(); i++)
	{
		aLine = (BorderLine)borderLines[i];
		for (j = 0; j < i; j++)
		{
			bLine = (BorderLine)borderLines[j];
			if (aLine.area > bLine.area)
			{
				//borderLines.RemoveAt(i);
				//borderLines.Insert(j, aLine);
				borderLines.erase(borderLines.begin() + i);
				borderLines.insert(borderLines.begin() + j, aLine);
				break;
			}
		}
	}
	
	vector<BorderLine> lineList;
	if (borderLines.size() == 1)    //Only one boder line
	{
		aLine = (BorderLine)borderLines[0];
		if (!IsClockwise(aLine.pointList))
		{
			//aLine.pointList.Reverse();
			//aLine.ijPointList.Reverse();
			std::reverse(aLine.pointList.begin(), aLine.pointList.end());
			std::reverse(aLine.ijPointList.begin(), aLine.ijPointList.end());
		}
		aLine.isClockwise = true;
		lineList = vector<BorderLine>();
		lineList.push_back(aLine);
		aBorder =  Border();
		aBorder.LineList = lineList;
		borders.push_back(aBorder);
	}
	else    //muti border lines
	{
		for (i = 0; i < borderLines.size(); i++)
		{
			if (i == borderLines.size())
				break;

			aLine = borderLines[i];
			if (!IsClockwise(aLine.pointList))
			{
				//aLine.pointList.Reverse();
				//aLine.ijPointList.Reverse();

				std::reverse(aLine.pointList.begin(), aLine.pointList.end());
				std::reverse(aLine.ijPointList.begin(), aLine.ijPointList.end());
			}
			aLine.isClockwise = true;	

			lineList = vector<BorderLine>();
			lineList.push_back(aLine);
			//Try to find the boder lines are inside of aLine.
			for (j = i + 1; j < borderLines.size(); j++)
			{
				if (j == borderLines.size())
					break;

				bLine = borderLines[j];
				if (bLine.extent.xMin > aLine.extent.xMin && bLine.extent.xMax < aLine.extent.xMax &&
					bLine.extent.yMin > aLine.extent.yMin && bLine.extent.yMax < aLine.extent.yMax)
				{
					aPoint = bLine.pointList[0];
					if (PointInPolygon(aLine.pointList, aPoint))    //bLine is inside of aLine
					{
						bLine.isOutLine = false;
						if (IsClockwise(bLine.pointList))
						{
							//pointList.Reverse();
							//bLine.ijPointList.Reverse();

							std::reverse(pointList.begin(), pointList.end());
							std::reverse(bLine.ijPointList.begin(), bLine.ijPointList.end());


						}
						bLine.isClockwise = false;
						lineList.push_back(bLine);
						//borderLines.RemoveAt(j);
						borderLines.erase(borderLines.begin() + j);
						j = j - 1;
					}
				}
			}
			//aBorder = new Border();
			Border aBorder;
			aBorder.LineList = lineList;
			borders.push_back(aBorder);
		}
	}


	ArryUtils::delete2DArray(UNum, m + 2, n + 2);
	ArryUtils::delete2DArray(S2, m + 2, n + 2);

	return borders;
}

vector<PolyLine>  Contour::CreateContourLines_UndefData(double** S0, double* X, double* Y,
	int nc, double* contour, double nx, double ny, int** S1, double undefData, vector<Border> borders)
{
	vector <PolyLine> contourLineList;
	vector <PolyLine> cLineList;
	int m = rows, n = cols, i, j;
	//ArryUtils::Get2DArryRowCol(S0, m, n);
	//m = S0.GetLength(0);    //---- Y
	//n = S0.GetLength(1);    //---- X

	//---- Add a small value to aviod the contour point as same as data point
	double dShift = contour[0] * 0.00001;
	if (dShift == 0)
		dShift = 0.00001;
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (!(DoubleEquals(S0[i][j], undefData)))
				//S0[i, j] = S0[i, j] + (contour[1] - contour[0]) * 0.0001;
				S0[i][j] = S0[i][j] + dShift;
		}
	}

	//---- Define if H S are border
	int*** SB = ArryUtils::create3DArray<int>(2, m, n - 1,0);
	int*** HB = ArryUtils::create3DArray<int>(2, m - 1, n,0);  //---- Which border and trace direction

	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (j < n - 1)
			{
				SB[0][i][j] = -1;
				SB[1][i][j] = -1;
			}
			if (i < m - 1)
			{
				HB[0][i][j] = -1;
				HB[1][i][j] = -1;
			}
		}
	}
	
	Border aBorder;
	BorderLine aBLine;
	vector<IJPoint> ijPList;
	int k, si, sj;
	IJPoint aijP, bijP;
	for (i = 0; i < borders.size(); i++)
	{
		aBorder = borders[i];
		for (j = 0; j < aBorder.LineNum(); j++)
		{
			aBLine = aBorder.LineList[j];
			ijPList = aBLine.ijPointList;
			for (k = 0; k < ijPList.size() - 1; k++)
			{
				aijP = ijPList[k];
				bijP = ijPList[k + 1];
				if (aijP.I == bijP.I)
				{
					si = aijP.I;
					sj = min(aijP.J, bijP.J);
					SB[0][si][sj] = i;
					if (bijP.J > aijP.J)    //---- Trace from top
						SB[1][si][sj] = 1;
					else
						SB[1][si][sj] = 0;    //----- Trace from bottom
				}
				else
				{
					sj = aijP.J;
					si = min(aijP.I, bijP.I);
					HB[0][si][sj] = i;
					if (bijP.I > aijP.I)    //---- Trace from left
						HB[1][si][sj] = 0;
					else
						HB[1][si][sj] = 1;    //---- Trace from right

				}
			}
		}
	}

	//---- Define horizontal and vertical arrays with the position of the tracing value, -2 means no tracing point. 
	double** S = ArryUtils::create2DArray<double>(m, n - 1,0.0);
	double** H = ArryUtils::create2DArray<double>(m - 1, n,0.0);

	//double[, ] S = new double[m, n - 1], H = new double[m - 1, n];
	double w;    //---- Tracing value
	int c;
	//ArrayList _endPointList = new ArrayList();    //---- Contour line end points for insert to border
	for (c = 0; c < nc; c++)
	{
		w = contour[c];
		for (i = 0; i < m; i++)
		{
			for (j = 0; j < n; j++)
			{
				if (j < n - 1)
				{
					if (S1[i][j] != 0 && S1[i][ j + 1] != 0)
					{
						if ((S0[i][j] - w) * (S0[i][j + 1] - w) < 0)    //---- Has tracing value
							S[i][j] = (w - S0[i][j]) / (S0[i][j + 1] - S0[i][j]);
						else
							S[i][j] = -2;
					}
					else
						S[i][j] = -2;
				}
				if (i < m - 1)
				{
					if (S1[i][ j] != 0 && S1[i + 1][ j] != 0)
					{
						if ((S0[i][j] - w) * (S0[i + 1][j] - w) < 0)    //---- Has tracing value
							H[i][j] = (w - S0[i][j]) / (S0[i + 1][j] - S0[i][j]);
						else
							H[i][j] = -2;
					}
					else
						H[i][j] = -2;
				}
			}
		}


		cLineList = Isoline_UndefData(S0, X, Y, w, nx, ny, S, H, SB, HB, contourLineList.size());
		//contourLineList.AddRange(cLineList);
		contourLineList.insert(contourLineList.end(), cLineList.begin(), cLineList.end());
	}

	//---- Set border index for close contours
	PolyLine aLine;
	//ArrayList pList = new ArrayList();
	PointD aPoint;
	for (i = 0; i < borders.size(); i++)
	{
		aBorder = borders[i];
		aBLine = aBorder.LineList[0];
		for (j = 0; j < contourLineList.size(); j++)
		{
			aLine = contourLineList[j];
			if (aLine.Type == "Close")
			{
				aPoint = aLine.PointList[0];
				if (PointInPolygon(aBLine.pointList, aPoint))
					aLine.BorderIdx = i;
			}
			//contourLineList.RemoveAt(j);
			//contourLineList.Insert(j, aLine);
			//contourLineList[j] = aLine;
			contourLineList.erase(contourLineList.begin() + j);
			contourLineList.insert(contourLineList.begin() + j, aLine);
		}
	}


	ArryUtils::delete3DArray(SB, 2, m, n - 1);
	ArryUtils::delete3DArray(HB, 2, m - 1, n);
	ArryUtils::delete2DArray(S, m, n - 1);
	ArryUtils::delete2DArray(H, m - 1, n);

	return contourLineList;
}

vector<PolyLine> Contour::CreateContourLines(double** S0, double *X, double *Y, int nc, double *contour, double nx, double ny)
{
	vector<PolyLine> contourLineList;
	vector<PolyLine> bLineList;
	vector<PolyLine> lLineList;
	vector<PolyLine> tLineList;
	vector<PolyLine> rLineList;
	vector<PolyLine> cLineList;
	int m=rows, n=cols, i, j;
	//ArryUtils::Get2DArryRowCol(S0, m, n);
	//m = S0.GetLength(0);    //---- Y
	//n = S0.GetLength(1);    //---- X

	//---- Define horizontal and vertical arrays with the position of the tracing value, -2 means no tracing point. 
	//double[, ] S = new double[m, n - 1], H = new double[m - 1, n];
	double** S = ArryUtils::create2DArray<double>(m, n - 1,0.0);
	double** H = ArryUtils::create2DArray<double>(m - 1, n,0.0);

	double dShift;
	dShift = contour[0] * 0.00001;
	if (dShift == 0)
		dShift = 0.00001;
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
			S0[i][j] = S0[i][ j] + dShift;
	}

	double w;    //---- Tracing value
	int c;
	for (c = 0; c < nc; c++)
	{
		w = contour[c];
		for (i = 0; i < m; i++)
		{
			for (j = 0; j < n; j++)
			{
				if (j < n - 1)
				{
					if ((S0[i][ j] - w) * (S0[i][ j + 1] - w) < 0)    //---- Has tracing value
						S[i][ j] = (w - S0[i][ j]) / (S0[i][ j + 1] - S0[i][ j]);
					else
						S[i][ j] = -2;
				}
				if (i < m - 1)
				{
					if ((S0[i][ j] - w) * (S0[i + 1][ j] - w) < 0)    //---- Has tracing value
						H[i][ j] = (w - S0[i][ j]) / (S0[i + 1][ j] - S0[i][ j]);
					else
						H[i][ j] = -2;
				}
			}
		}

		bLineList = Isoline_Bottom(S0, X, Y, w, nx, ny,  S,  H);
		lLineList = Isoline_Left(S0, X, Y, w, nx, ny,  S,  H);
		tLineList = Isoline_Top(S0, X, Y, w, nx, ny,  S,  H);
		rLineList = Isoline_Right(S0, X, Y, w, nx, ny,  S,  H);
		cLineList = Isoline_Close(S0, X, Y, w, nx, ny,  S,  H);
		contourLineList.insert(contourLineList.end(), bLineList.begin(), bLineList.end());
		contourLineList.insert(contourLineList.end(), lLineList.begin(), lLineList.end());
		contourLineList.insert(contourLineList.end(), tLineList.begin(), tLineList.end());
		contourLineList.insert(contourLineList.end(), rLineList.begin(), rLineList.end());
		contourLineList.insert(contourLineList.end(), cLineList.begin(), cLineList.end());
		//contourLineList.AddRange(bLineList);
		//contourLineList.AddRange(lLineList);
		//contourLineList.AddRange(tLineList);
		//contourLineList.AddRange(rLineList);
		//contourLineList.AddRange(cLineList);
	}

	ArryUtils::delete2DArray(S, m, n - 1);
	ArryUtils::delete2DArray(H, m - 1, n);

	return contourLineList;
}

vector<PolyLine> Contour::CutContourWithPolygon(vector<PolyLine> alinelist, vector<PointD> polyList)
{
	vector<PolyLine> newLineList;
	int i, j, k;
	PolyLine aLine, bLine;
	vector<PointD> aPList;
	double aValue;
	string aType;
	bool ifInPolygon;
	PointD q1, q2, p1, p2, IPoint;
	Line lineA, lineB;
	EndPoint aEndPoint;

	//_endPointList = new List<EndPoint>();
	if (!IsClockwise(polyList))    //---- Make cut polygon clockwise
	{
		//polyList.Reverse();
		std::reverse(polyList.begin(), polyList.end());
	}


	for (i = 0; i < alinelist.size(); i++)
	{
		aLine = alinelist[i];
		aValue = aLine.Value;
		aType = aLine.Type;
		aPList = vector<PointD>(aLine.PointList);
		//aPList = new List<PointD>(aLine.PointList);
		ifInPolygon = false;
		vector<PointD> newPlist;
		//---- For "Close" type contour,the start point must be outside of the cut polygon.
		if (aType == "Close" && PointInPolygon(polyList, aPList[0]))
		{
			bool isAllIn = true;
			int notInIdx = 0;
			for (j = 0; j < aPList.size(); j++)
			{
				if (!PointInPolygon(polyList, aPList[j]))
				{
					notInIdx = j;
					isAllIn = false;
					break;
				}
			}
			if (!isAllIn)
			{
				vector<PointD> bPList;
				for (j = notInIdx; j < aPList.size(); j++)
					bPList.push_back(aPList[j]);

				for (j = 1; j < notInIdx; j++)
					bPList.push_back(aPList[j]);

				bPList.push_back(bPList[0]);
				aPList = bPList;
			}
		}
		p1 = PointD();
		for (j = 0; j < aPList.size(); j++)
		{
			p2 = aPList[j];
			if (PointInPolygon(polyList, p2))
			{
				if (!ifInPolygon && j > 0)
				{
					lineA = Line();
					lineA.P1 = p1;
					lineA.P2 = p2;
					q1 = polyList[polyList.size() - 1];
					IPoint = PointD();
					for (k = 0; k < polyList.size(); k++)
					{
						q2 = polyList[k];
						lineB = Line();
						lineB.P1 = q1;
						lineB.P2 = q2;
						if (IsLineSegmentCross(lineA, lineB))
						{
							IPoint = GetCrossPoint(lineA, lineB);
							aEndPoint.sPoint = q1;
							aEndPoint.Point = IPoint;
							aEndPoint.Index = newLineList.size();
							_endPointList.push_back(aEndPoint);    //---- Generate _endPointList for border insert
							break;
						}
						q1 = q2;
					}
					newPlist.push_back(IPoint);
					aType = "Border";
				}
				newPlist.push_back(aPList[j]);
				ifInPolygon = true;
			}
			else
			{
				if (ifInPolygon)
				{
					lineA = Line();
					lineA.P1 = p1;
					lineA.P2 = p2;
					q1 = polyList[polyList.size() - 1];
					IPoint = PointD();
					for (k = 0; k < polyList.size(); k++)
					{
						q2 = polyList[k];
						lineB = Line();
						lineB.P1 = q1;
						lineB.P2 = q2;
						if (IsLineSegmentCross(lineA, lineB))
						{
							IPoint = GetCrossPoint(lineA, lineB);
							aEndPoint.sPoint = q1;
							aEndPoint.Point = IPoint;
							aEndPoint.Index = newLineList.size();
							_endPointList.push_back(aEndPoint);
							break;
						}
						q1 = q2;
					}
					newPlist.push_back(IPoint);

					bLine.Value = aValue;
					bLine.Type = aType;
					bLine.PointList = newPlist;
					newLineList.push_back(bLine);
					ifInPolygon = false;
					newPlist = vector<PointD>();
					aType = "Border";
				}
			}
			p1 = p2;
		}
		if (ifInPolygon && newPlist.size() > 1)
		{
			bLine.Value = aValue;
			bLine.Type = aType;
			bLine.PointList = newPlist;
			newLineList.push_back(bLine);
		}
	}

	return newLineList;
}

vector<PolyLine> Contour::CutContourLines(vector<PolyLine> alinelist, Border aBorder)
{
	vector<PointD> pointList = aBorder.LineList[0].pointList;
	vector<PolyLine> newLineList;
	int i, j, k;
	PolyLine aLine, bLine;
	vector<PointD> aPList;
	double aValue;
	string aType;
	bool ifInPolygon;
	PointD q1, q2, p1, p2, IPoint;
	Line lineA, lineB;
	EndPoint aEndPoint;

	//_endPointList = new List<EndPoint>();
	if (!IsClockwise(pointList))    //---- Make cut polygon clockwise
	{
		//pointList.Reverse();
		std::reverse(pointList.begin(), pointList.end());
	}


	for (i = 0; i < alinelist.size(); i++)
	{
		aLine = alinelist[i];
		aValue = aLine.Value;
		aType = aLine.Type;
		//aPList = new List<PointD>(aLine.PointList);
		aPList = aLine.PointList;
		ifInPolygon = false;
		vector<PointD> newPlist;
		//---- For "Close" type contour,the start point must be outside of the cut polygon.
		if (aType == "Close" && PointInPolygon(pointList, (PointD)aPList[0]))
		{
			bool isAllIn = true;
			int notInIdx = 0;
			for (j = 0; j < aPList.size(); j++)
			{
				if (!PointInPolygon(pointList, (PointD)aPList[j]))
				{
					notInIdx = j;
					isAllIn = false;
					break;
				}
			}
			if (!isAllIn)
			{
				vector<PointD> bPList;
				for (j = notInIdx; j < aPList.size(); j++)
					bPList.push_back(aPList[j]);

				for (j = 1; j < notInIdx; j++)
					bPList.push_back(aPList[j]);

				bPList.push_back(bPList[0]);
				aPList = bPList;
			}
		}

		p1 = PointD();
		for (j = 0; j < aPList.size(); j++)
		{
			p2 = aPList[j];
			if (PointInPolygon(pointList, p2))
			{
				if (!ifInPolygon && j > 0)
				{
					lineA = Line();
					lineA.P1 = p1;
					lineA.P2 = p2;
					q1 = pointList[pointList.size() - 1];
					IPoint = PointD();
					for (k = 0; k < pointList.size(); k++)
					{
						q2 = pointList[k];
						lineB = Line();
						lineB.P1 = q1;
						lineB.P2 = q2;
						if (IsLineSegmentCross(lineA, lineB))
						{
							IPoint = GetCrossPoint(lineA, lineB);
							aEndPoint.sPoint = q1;
							aEndPoint.Point = IPoint;
							aEndPoint.Index = newLineList.size();
							_endPointList.push_back(aEndPoint);    //---- Generate _endPointList for border insert
							break;
						}
						q1 = q2;
					}
					newPlist.push_back(IPoint);
					aType = "Border";
				}
				newPlist.push_back(aPList[j]);
				ifInPolygon = true;
			}
			else
			{
				if (ifInPolygon)
				{
					lineA = Line();
					lineA.P1 = p1;
					lineA.P2 = p2;
					q1 = pointList[pointList.size() - 1];
					IPoint = PointD();
					for (k = 0; k < pointList.size(); k++)
					{
						q2 = pointList[k];
						lineB = Line();
						lineB.P1 = q1;
						lineB.P2 = q2;
						if (IsLineSegmentCross(lineA, lineB))
						{
							IPoint = GetCrossPoint(lineA, lineB);
							aEndPoint.sPoint = q1;
							aEndPoint.Point = IPoint;
							aEndPoint.Index = newLineList.size();
							_endPointList.push_back(aEndPoint);
							break;
						}
						q1 = q2;
					}
					newPlist.push_back(IPoint);

					bLine = PolyLine();
					bLine.Value = aValue;
					bLine.Type = aType;
					bLine.PointList = newPlist;
					newLineList.push_back(bLine);
					ifInPolygon = false;
					newPlist = vector<PointD>();
					aType = "Border";
				}
			}
			p1 = p2;
		}
		if (ifInPolygon && newPlist.size() > 1)
		{
			bLine = PolyLine();
			bLine.Value = aValue;
			bLine.Type = aType;
			bLine.PointList = newPlist;
			newLineList.push_back(bLine);
		}
	}

	return newLineList;
}

vector<PolyLine> Contour::SmoothLines(vector<PolyLine> aLineList)
{
	vector<PolyLine> newLineList;
	int i;
	PolyLine aline;
	vector<PointD> newPList;
	double aValue;
	string aType;

	for (i = 0; i < aLineList.size(); i++)
	{
		aline = aLineList[i];
		aValue = aline.Value;
		aType = aline.Type;
		//newPList = new List<PointD>(aline.PointList);
		newPList = aline.PointList;
		if (newPList.size() <= 1)
			continue;

		if (newPList.size() == 2)
		{
			PointD bP;
			PointD aP = newPList[0];
			PointD cP = newPList[1];
			bP.X = (cP.X - aP.X) / 4 + aP.X;
			bP.Y = (cP.Y - aP.Y) / 4 + aP.Y;
			//newPList.Insert(1, bP);
			newPList.insert(newPList.begin() + 1, bP);
			bP = PointD();
			bP.X = (cP.X - aP.X) / 4 * 3 + aP.X;
			bP.Y = (cP.Y - aP.Y) / 4 * 3 + aP.Y;
			//newPList.Insert(2, bP);
			newPList.insert(newPList.begin() + 2, bP);
		}
		if (newPList.size() == 3)
		{
			PointD bP;
			PointD aP = newPList[0];
			PointD cP = newPList[1];
			bP.X = (cP.X - aP.X) / 2 + aP.X;
			bP.Y = (cP.Y - aP.Y) / 2 + aP.Y;
			//newPList.Insert(1, bP);
			newPList.insert(newPList.begin() + 1, bP);
		}
		newPList = BSplineScanning(newPList, newPList.size());
		aline.PointList = newPList;
		newLineList.push_back(aline);
	}

	return newLineList;
}

vector<PointD> Contour::SmoothPoints(vector<PointD> pointList)
{
	return BSplineScanning(pointList, pointList.size());
}

vector<WPolygon> Contour::TracingPolygons(double** S0, vector<PolyLine> cLineList, vector<Border> borderList, double* contour,int contourNum)
{
	vector<WPolygon> aPolygonList, newPolygonList;
	vector<BorderPoint> newBPList;
	vector<BorderPoint> bPList;
	vector<PointD> PList;
	Border aBorder;
	BorderLine aBLine;
	PointD aPoint;
	BorderPoint aBPoint;
	int i, j;
	vector<PolyLine> lineList;
	vector<BorderPoint> aBorderList;
	PolyLine aLine;
	WPolygon aPolygon;
	IJPoint aijP;
	double aValue = 0;
	int* pNums = NULL;

	//Borders loop
	for (i = 0; i < borderList.size(); i++)
	{
		aBorderList.clear();
		bPList.clear();
		lineList.clear();
		aPolygonList.clear();
		aBorder = borderList[i];

		aBLine = aBorder.LineList[0];
		PList = aBLine.pointList;
		if (!IsClockwise(PList))    //Make sure the point list is clockwise
		{
			//PList.Reverse();
			std::reverse(PList.begin(), PList.end());
		}


		if (aBorder.LineNum() == 1)    //The border has just one line
		{
			//Construct border point list
			for (j = 0; j < PList.size(); j++)
			{
				aPoint = PList[j];
				aBPoint = BorderPoint();
				aBPoint.Id = -1;
				aBPoint.Point = aPoint;
				aBPoint.Value = S0[aBLine.ijPointList[j].I][aBLine.ijPointList[j].J];
				aBorderList.push_back(aBPoint);
			}

			//Find the contour lines of this border
			for (j = 0; j < cLineList.size(); j++)
			{
				aLine = cLineList[j];
				if (aLine.BorderIdx == i)
				{
					lineList.push_back(aLine);    //Construct contour line list
					//Construct border point list of the contour line
					if (aLine.Type == "Border")    //The contour line with the start/end point on the border
					{
						aPoint = aLine.PointList[0];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
						aPoint = aLine.PointList[aLine.PointList.size() - 1];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
					}
				}
			}

			if (lineList.size() == 0)    //No contour lines in this border, the polygon is the border
			{
				//Judge the value of the polygon
				aijP = aBLine.ijPointList[0];
				aPolygon = WPolygon();
				if (S0[aijP.I][aijP.J] < contour[0])
				{
					aValue = contour[0];
					aPolygon.IsHighCenter = false;
				}
				else
				{
					int length = contourNum;
					for (j = length - 1; j >= 0; j--)
					{
						if (S0[aijP.I][aijP.J] > contour[j])
						{
							aValue = contour[j];
							break;
						}
					}
					aPolygon.IsHighCenter = true;
				}
				if (PList.size() > 0)
				{
					aPolygon.IsBorder = true;
					aPolygon.HighValue = aValue;
					aPolygon.LowValue = aValue;
					aPolygon.Extent = Extent();
					aPolygon.Area = GetExtentAndArea(PList, aPolygon.Extent);
					aPolygon.StartPointIdx = 0;
					aPolygon.IsClockWise = true;
					aPolygon.OutLine.Type = "Border";
					aPolygon.OutLine.Value = aValue;
					aPolygon.OutLine.BorderIdx = i;
					aPolygon.OutLine.PointList = PList;
					aPolygon.HoleLines = vector<PolyLine>();
					aPolygonList.push_back(aPolygon);
				}
			}
			else    //Has contour lines in this border
			{
				//Insert the border points of the contour lines to the border point list of the border
				newBPList = InsertPoint2Border(bPList, aBorderList);
				//aPolygonList = TracingPolygons(lineList, newBPList, aBound, contour);
				aPolygonList = TracingPolygons(lineList, newBPList);
			}
			aPolygonList = AddPolygonHoles(aPolygonList);
		}
		else    //---- The border has holes
		{
			aBLine = aBorder.LineList[0];
			//Find the contour lines of this border
			for (j = 0; j < cLineList.size(); j++)
			{
				aLine = cLineList[j];
				if (aLine.BorderIdx == i)
				{
					lineList.push_back(aLine);
					if (aLine.Type == "Border")
					{
						aPoint = aLine.PointList[0];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
						aPoint = aLine.PointList[aLine.PointList.size() - 1];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
					}
				}
			}
			if (lineList.size() == 0)  //No contour lines in this border, the polygon is the border and the holes
			{
				aijP = aBLine.ijPointList[0];
				aPolygon = WPolygon();
				if (S0[aijP.I][aijP.J] < contour[0])
				{
					aValue = contour[0];
					aPolygon.IsHighCenter = false;
				}
				else
				{
					int length = contourNum;
					for (j = length - 1; j >= 0; j--)
					{
						if (S0[aijP.I][aijP.J] > contour[j])
						{
							aValue = contour[j];
							break;
						}
					}
					aPolygon.IsHighCenter = true;
				}
				if (PList.size() > 0)
				{
					aPolygon.IsBorder = true;
					aPolygon.HighValue = aValue;
					aPolygon.LowValue = aValue;
					aPolygon.Area = GetExtentAndArea(PList, aPolygon.Extent);
					aPolygon.StartPointIdx = 0;
					aPolygon.IsClockWise = true;
					aPolygon.OutLine.Type = "Border";
					aPolygon.OutLine.Value = aValue;
					aPolygon.OutLine.BorderIdx = i;
					aPolygon.OutLine.PointList = PList;
					aPolygon.HoleLines = vector<PolyLine>();
					aPolygonList.push_back(aPolygon);
				}
			}
			else
			{
				pNums = new int[aBorder.LineNum()]();
				newBPList = InsertPoint2Border_Ring(S0, bPList, aBorder, pNums);

				aPolygonList = TracingPolygons_Ring(lineList, newBPList, aBorder, contour, pNums);
				//aPolygonList = TracingPolygons(lineList, newBPList, contour);

				//Sort polygons by area
				vector<WPolygon> sortList;
				while (aPolygonList.size() > 0)
				{
					bool isInsert = false;
					for (j = 0; j < sortList.size(); j++)
					{
						if (aPolygonList[0].Area > sortList[j].Area)
						{
							sortList.push_back(aPolygonList[0]);
							isInsert = true;
							break;
						}
					}
					if (!isInsert)
					{
						sortList.push_back(aPolygonList[0]);
					}
					//aPolygonList.RemoveAt(0);
					aPolygonList.erase(aPolygonList.begin());
				}
				aPolygonList = sortList;
			}

			vector<vector<PointD>> holeList;
			for (j = 0; j < aBorder.LineNum(); j++)
			{
				//if (aBorder.LineList[j].pointList.Count == pNums[j])
				//{
				//    holeList.Add(aBorder.LineList[j].pointList);
				//}
				holeList.push_back(aBorder.LineList[j].pointList);
			}

			if (holeList.size() > 0)
			{
				AddHoles_Ring(aPolygonList, holeList);
			}
			aPolygonList = AddPolygonHoles_Ring(aPolygonList);
		}
		//newPolygonList.AddRange(aPolygonList);
		newPolygonList.insert(newPolygonList.end(), aPolygonList.begin(), aPolygonList.end());
	}

	//newPolygonList = AddPolygonHoles(newPolygonList);
	for (WPolygon nPolygon : newPolygonList)
	{
		if (!IsClockwise(nPolygon.OutLine.PointList))
		{
			//nPolygon.OutLine.PointList.Reverse();
			std::reverse(nPolygon.OutLine.PointList.begin(), nPolygon.OutLine.PointList.end());
		}

	}

	if (pNums != NULL)
	{
		delete[] pNums;
		pNums = NULL;
	}

	return newPolygonList;
}

vector<WPolygon> Contour::CreateContourPolygons(vector<PolyLine> LineList, Extent aBound, double* contour,int contourNum)
{
	vector<WPolygon> aPolygonList;
	vector<BorderPoint> newBorderList;

	//---- Insert points to border list
	newBorderList = InsertPoint2RectangleBorder(LineList, aBound);

	//---- Tracing polygons
	aPolygonList = TracingPolygons(LineList, newBorderList, aBound, contour, contourNum);

	return aPolygonList;
}

vector<WPolygon> Contour::CreateCutContourPolygons(vector<PolyLine> LineList, vector<PointD> polyList, Extent aBound, double* contour, int contourNum)
{
	vector<WPolygon> aPolygonList;
	vector<BorderPoint> newBorderList;
	vector<BorderPoint> borderList;
	PointD aPoint;
	BorderPoint aBPoint;
	int i;

	//---- Get border point list
	if (!IsClockwise(polyList))
		std::reverse(polyList.begin(), polyList.end());
	//polyList.Reverse();

	for (i = 0; i < polyList.size(); i++)
	{
		aPoint = polyList[i];
		aBPoint = BorderPoint();
		aBPoint.Id = -1;
		aBPoint.Point = aPoint;
		borderList.push_back(aBPoint);
	}

	//---- Insert points to border list
	newBorderList = InsertEndPoint2Border(_endPointList, borderList);

	//---- Tracing polygons
	aPolygonList = TracingPolygons(LineList, newBorderList, aBound, contour,  contourNum);

	return aPolygonList;
}

vector<WPolygon> Contour::CreateBorderContourPolygons(double** S0, vector<PolyLine> cLineList, vector<Border> borderList, Extent aBound, double* contour,int contourNum)
{
	vector<WPolygon> aPolygonList, newPolygonList;
	vector<BorderPoint> newBPList;
	vector<BorderPoint> bPList;
	vector<PointD> PList;
	Border aBorder;
	BorderLine aBLine;
	PointD aPoint;
	BorderPoint aBPoint;
	int i, j;
	vector<PolyLine> lineList;
	vector<BorderPoint> aBorderList;
	PolyLine aLine;
	WPolygon aPolygon;
	IJPoint aijP;
	double aValue = 0;
	int* pNums = NULL;

	//Borders loop
	for (i = 0; i < borderList.size(); i++)
	{
		aBorderList.clear();
		bPList.clear();
		lineList.clear();
		aPolygonList.clear();
		aBorder = borderList[i];
		if (aBorder.LineNum() == 1)    //The border has just one line
		{
			aBLine = aBorder.LineList[0];
			PList = aBLine.pointList;
			if (!IsClockwise(PList))    //Make sure the point list is clockwise
				std::reverse(PList.begin(), PList.end());
			//PList.Reverse();

		//Construct border point list
			for (j = 0; j < PList.size(); j++)
			{
				aPoint = PList[j];
				aBPoint = BorderPoint();
				aBPoint.Id = -1;
				aBPoint.Point = aPoint;
				aBPoint.Value = S0[aBLine.ijPointList[j].I][aBLine.ijPointList[j].J];
				aBorderList.push_back(aBPoint);
			}

			//Find the contour lines of this border
			for (j = 0; j < cLineList.size(); j++)
			{
				aLine = cLineList[j];
				if (aLine.BorderIdx == i)
				{
					lineList.push_back(aLine);    //Construct contour line list
					//Construct border point list of the contour line
					if (aLine.Type == "Border")    //The contour line with the start/end point on the border
					{
						aPoint = aLine.PointList[0];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
						aPoint = aLine.PointList[aLine.PointList.size() - 1];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
					}
				}
			}

			if (lineList.size() == 0)    //No contour lines in this border, the polygon is the border
			{
				//Judge the value of the polygon
				aijP = aBLine.ijPointList[0];
				aPolygon = WPolygon();
				if (S0[aijP.I][aijP.J] < contour[0])
				{
					aValue = contour[0];
					aPolygon.IsHighCenter = false;
				}
				else
				{
					int length = contourNum;
					for (j = length - 1; j >= 0; j--)
					{
						if (S0[aijP.I][aijP.J] > contour[j])
						{
							aValue = contour[j];
							break;
						}
					}
					aPolygon.IsHighCenter = true;
				}
				if (PList.size() > 0)
				{
					aPolygon.HighValue = aValue;
					aPolygon.LowValue = aValue;
					aPolygon.Extent = Extent();
					aPolygon.Area = GetExtentAndArea(PList, aPolygon.Extent);
					aPolygon.StartPointIdx = 0;
					aPolygon.IsClockWise = true;
					aPolygon.OutLine.Type = "Border";
					aPolygon.OutLine.Value = aValue;
					aPolygon.OutLine.BorderIdx = i;
					aPolygon.OutLine.PointList = PList;
					aPolygonList.push_back(aPolygon);
				}
			}
			else    //Has contour lines in this border
			{
				//Insert the border points of the contour lines to the border point list of the border
				newBPList = InsertPoint2Border(bPList, aBorderList);
				//aPolygonList = TracingPolygons(lineList, newBPList, aBound, contour);
				aPolygonList = TracingPolygons(lineList, newBPList);
			}
		}
		else    //---- The border has holes
		{
			aBLine = aBorder.LineList[0];
			//Find the contour lines of this border
			for (j = 0; j < cLineList.size(); j++)
			{
				aLine = cLineList[j];
				if (aLine.BorderIdx == i)
				{
					lineList.push_back(aLine);
					if (aLine.Type == "Border")
					{
						aPoint = aLine.PointList[0];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
						aPoint = aLine.PointList[aLine.PointList.size() - 1];
						aBPoint = BorderPoint();
						aBPoint.Id = lineList.size() - 1;
						aBPoint.Point = aPoint;
						aBPoint.Value = aLine.Value;
						bPList.push_back(aBPoint);
					}
				}
			}
			if (lineList.size() == 0)  //No contour lines in this border, the polygon is the border and the holes
			{
				aPolygon = WPolygon();
				aijP = aBLine.ijPointList[0];
				if (S0[aijP.I][aijP.J] < contour[0])
				{
					aValue = contour[0];
					aPolygon.IsHighCenter = false;
				}
				else
				{
					int length = contourNum;
					for (j = length - 1; j >= 0; j--)
					{
						if (S0[aijP.I][ aijP.J] > contour[j])
						{
							aValue = contour[j];
							break;
						}
					}
					aPolygon.IsHighCenter = true;
				}
				if (PList.size() > 0)
				{
					aPolygon.HighValue = aValue;
					aPolygon.LowValue = aValue;
					aPolygon.Area = GetExtentAndArea(PList, aPolygon.Extent);
					aPolygon.StartPointIdx = 0;
					aPolygon.IsClockWise = true;
					aPolygon.OutLine.Type = "Border";
					aPolygon.OutLine.Value = aValue;
					aPolygon.OutLine.BorderIdx = i;
					aPolygon.OutLine.PointList = PList;
					aPolygonList.push_back(aPolygon);
				}
			}
			else
			{
				pNums = new int[aBorder.LineNum()];
				newBPList = InsertPoint2Border_Ring(S0, bPList, aBorder, pNums);
				aPolygonList = TracingPolygons_Ring(lineList, newBPList, aBorder, contour, pNums);
				//aPolygonList = TracingPolygons(lineList, newBPList, contour);
			}
		}
		//newPolygonList.AddRange(aPolygonList);
		newPolygonList.insert(newPolygonList.end(), aPolygonList.begin(), aPolygonList.end());
	}

	if (pNums != NULL)
	{
		delete[] pNums;
		pNums = NULL;
	}
		

	return newPolygonList;
}

bool Contour::PointInPolygon(vector<PointD> poly, PointD aPoint)
{
	double xNew, yNew, xOld, yOld;
	double x1, y1, x2, y2;
	int i;
	bool inside = false;
	int nPoints = poly.size();

	if (nPoints < 3)
		return false;

	xOld = poly[nPoints - 1].X;
	yOld = poly[nPoints - 1].Y;
	for (i = 0; i < nPoints; i++)
	{
		xNew = poly[i].X;
		yNew = poly[i].Y;
		if (xNew > xOld)
		{
			x1 = xOld;
			x2 = xNew;
			y1 = yOld;
			y2 = yNew;
		}
		else
		{
			x1 = xNew;
			x2 = xOld;
			y1 = yNew;
			y2 = yOld;
		}

		//---- edge "open" at left end
		if ((xNew < aPoint.X) == (aPoint.X <= xOld) &&
			(aPoint.Y - y1) * (x2 - x1) < (y2 - y1) * (aPoint.X - x1))
			inside = !inside;

		xOld = xNew;
		yOld = yNew;
	}

	return inside;
}

bool Contour::PointInPolygon(WPolygon aPolygon, PointD aPoint)
{
	if (aPolygon.HasHoles())
	{
		bool isIn = PointInPolygon(aPolygon.OutLine.PointList, aPoint);
		if (isIn)
		{
			for (PolyLine aLine : aPolygon.HoleLines)
			{
				if (PointInPolygon(aLine.PointList, aPoint))
				{
					isIn = false;
					break;
				}
			}
		}

		return isIn;
	}
	else
		return PointInPolygon(aPolygon.OutLine.PointList, aPoint);
}

vector<PolyLine> Contour::ClipPolylines(vector<PolyLine> polylines, vector<PointD> clipPList)
{
	vector<PolyLine> newPolylines;
	for (PolyLine aPolyline : polylines)
	{
		vector<PolyLine> cutPolyLine = CutPolyline(aPolyline, clipPList);
		//newPolylines.AddRange(CutPolyline(aPolyline, clipPList));
		newPolylines.insert(newPolylines.end(), cutPolyLine.begin(), cutPolyLine.end());
	}

	return newPolylines;
}

vector<WPolygon> Contour::ClipPolygons(vector<WPolygon> polygons, vector<PointD> clipPList)
{
	vector<WPolygon> newPolygons;
	for (int i = 0; i < polygons.size(); i++)
	{
		WPolygon aPolygon = polygons[i];
		if (aPolygon.HasHoles())
		{
			vector<WPolygon> cutPolygon_Hole = CutPolygon_Hole(aPolygon, clipPList);
			newPolygons.insert(newPolygons.end(), cutPolygon_Hole.begin(), cutPolygon_Hole.end());
			//newPolygons.AddRange(CutPolygon_Hole(aPolygon, clipPList));
		}
		else
		{
			vector<WPolygon> cutPolygon = CutPolygon(aPolygon, clipPList);
			newPolygons.insert(newPolygons.end(), cutPolygon.begin(), cutPolygon.end());
			//newPolygons.AddRange(CutPolygon(aPolygon, clipPList));
		}
	}

	//Sort polygons with bording rectangle area
	vector<WPolygon> outPolygons;
	bool isInserted = false;
	for (int i = 0; i < newPolygons.size(); i++)
	{
		WPolygon aPolygon = newPolygons[i];
		isInserted = false;
		for (int j = 0; j < outPolygons.size(); j++)
		{
			if (aPolygon.Area > outPolygons[j].Area)
			{
				//outPolygons.Insert(j, aPolygon);
				outPolygons.insert(outPolygons.begin() + j, aPolygon);
				isInserted = true;
				break;
			}
		}

		if (!isInserted)
			outPolygons.push_back(aPolygon);
	}

	return outPolygons;
}

vector<PolyLine> Contour::TracingStreamline(double** U, double** V, double* X, double* Y, double UNDEF, int density)
{
	vector<PolyLine> streamLines;

	int yNum = rows; // 获取数组的行数
	int xNum = cols; // 获取数组的列数

	//ArryUtils::Get2DArryRowCol(U, xNum, yNum);

	//int xNum = U.GetLength(1);
	//int yNum = U.GetLength(0);
	//double[, ] Dx = new double[yNum, xNum];
	//double[, ] Dy = new double[yNum, xNum];
	double** Dx = ArryUtils::create2DArray<double>(yNum, xNum,0.0);
	double** Dy = ArryUtils::create2DArray<double>(yNum, xNum,0.0);
	double deltX = X[1] - X[0];
	double deltY = Y[1] - Y[0];
	if (density == 0)
		density = 1;
	double radius = deltX / (density * density);
	int i, j;

	//Normalize wind components
	for (i = 0; i < yNum; i++)
	{
		for (j = 0; j < xNum; j++)
		{
			if (abs(U[i][j] / UNDEF - 1) < 0.01)
			{
				Dx[i][j] = 0.1;
				Dy[i][j] = 0.1;
			}
			else
			{
				double WS = sqrt(U[i][j] * U[i][j] + V[i][j] * V[i][j]);
				if (WS == 0)
					WS = 1;
				Dx[i][j] = (U[i][j] / WS) * deltX / density;
				Dy[i][j] = (V[i][j] / WS) * deltY / density;
			}
		}
	}

	//Flag the grid boxes
	//vector<PointD>[, ] SPoints = new List<PointD>[yNum - 1, xNum - 1];
	vector<PointD>** SPoints = ArryUtils::create2DArray<vector<PointD>>(yNum - 1, xNum - 1, vector<PointD>());
	//int[, ] flags = new int[yNum - 1, xNum - 1];
	int** flags = ArryUtils::create2DArray<int>(yNum - 1, xNum - 1,0);
	for (i = 0; i < yNum - 1; i++)
	{
		for (j = 0; j < xNum - 1; j++)
		{
			if (i % 2 == 0 && j % 2 == 0)
				flags[i][j] = 0;
			else
				flags[i][j] = 1;

			SPoints[i][j] = vector<PointD>();
		}
	}

	//Tracing streamline            
	for (i = 0; i < yNum - 1; i++)
	{
		for (j = 0; j < xNum - 1; j++)
		{
			if (flags[i][ j] == 0)    //No streamline started form this grid box, a new streamline started
			{
				vector<PointD> pList;
				PointD aPoint;
				int ii, jj;
				int loopN;
				PolyLine aPL;

				//Start point - the center of the grid box
				aPoint.X = X[j] + deltX / 2;
				aPoint.Y = Y[i] + deltY / 2;
				pList.push_back((PointD)aPoint.Clone());
				SPoints[i][j].push_back((PointD)aPoint.Clone());
				flags[i][j] = 1;    //Flag the grid box and no streamline will start from this box again
				ii = i;
				jj = j;
				int loopLimit = 500;

				//Tracing forward
				loopN = 0;
				while (loopN < loopLimit)
				{
					//Trace next streamline point
					bool isInDomain = TracingStreamlinePoint(aPoint, Dx, Dy, X, Y, ii, jj, true);

					//Terminating the streamline
					if (isInDomain)
					{
						if (abs(U[ii][jj] / UNDEF - 1) < 0.01 || abs(U[ii][jj + 1] / UNDEF - 1) < 0.01 ||
							abs(U[ii + 1][jj] / UNDEF - 1) < 0.01 || abs(U[ii + 1][jj + 1] / UNDEF - 1) < 0.01)
							break;
						else
						{
							bool isTerminating = false;
							for (PointD sPoint : SPoints[ii][jj])
							{
								if (sqrt((aPoint.X - sPoint.X) * (aPoint.X - sPoint.X) +
									(aPoint.Y - sPoint.Y) * (aPoint.Y - sPoint.Y)) < radius)
								{
									isTerminating = true;
									break;
								}
							}
							if (!isTerminating)
							{
								pList.push_back((PointD)aPoint.Clone());
								SPoints[ii][jj].push_back((PointD)aPoint.Clone());
								flags[ii][jj] = 1;
							}
							else
								break;
						}
					}
					else
					{
						break;
					}

					loopN += 1;
				}

				//Tracing backword
				aPoint.X = X[j] + deltX / 2;
				aPoint.Y = Y[i] + deltY / 2;
				ii = i;
				jj = j;
				loopN = 0;
				while (loopN < loopLimit)
				{
					//Trace next streamline point
					bool isInDomain = TracingStreamlinePoint(aPoint, Dx, Dy, X, Y, ii, jj, false);

					//Terminating the streamline
					if (isInDomain)
					{
						if (abs(U[ii][jj] / UNDEF - 1) < 0.01 || abs(U[ii][jj + 1] / UNDEF - 1) < 0.01 ||
							abs(U[ii + 1][jj] / UNDEF - 1) < 0.01 || abs(U[ii + 1][jj + 1] / UNDEF - 1) < 0.01)
							break;
						else
						{
							bool isTerminating = false;
							for (PointD sPoint : SPoints[ii][jj])
							{
								if (sqrt((aPoint.X - sPoint.X) * (aPoint.X - sPoint.X) +
									(aPoint.Y - sPoint.Y) * (aPoint.Y - sPoint.Y)) < radius)
								{
									isTerminating = true;
									break;
								}
							}
							if (!isTerminating)
							{
								//pList.Insert(0, (PointD)aPoint.Clone());
								pList.insert(pList.begin(), (PointD)aPoint.Clone());
								SPoints[ii][jj].push_back((PointD)aPoint.Clone());
								flags[ii][jj] = 1;
							}
							else
								break;
						}
					}
					else
					{
						break;
					}

					loopN += 1;
				}
				if (pList.size() > 1)
				{
					aPL.PointList = pList;
					streamLines.push_back(aPL);
				}

			}
		}
	}

	ArryUtils::delete2DArray(Dx, yNum, xNum);
	ArryUtils::delete2DArray(Dy, yNum, xNum);

	ArryUtils::delete2DArray(SPoints, yNum - 1, xNum - 1);
	ArryUtils::delete2DArray(flags, yNum - 1, xNum - 1);
	//Return
	return streamLines;
}

bool Contour::TracingStreamlinePoint(PointD& aPoint, double** Dx, double** Dy, double* X, double* Y,
	int& ii, int& jj, bool isForward)
{
	double a, b, c, d, val1, val2;
	double dx, dy;
	int xNum = cols;
	int yNum = rows;

	//int xNum = X.Length;
	//int yNum = Y.Length;
	double deltX = X[1] - X[0];
	double deltY = Y[1] - Y[0];

	//Interpolation the U/V displacement components to the point
	a = Dx[ii][jj];
	b = Dx[ii][jj + 1];
	c = Dx[ii + 1][jj];
	d = Dx[ii + 1][jj + 1];
	val1 = a + (c - a) * ((aPoint.Y - Y[ii]) / deltY);
	val2 = b + (d - b) * ((aPoint.Y - Y[ii]) / deltY);
	dx = val1 + (val2 - val1) * ((aPoint.X - X[jj]) / deltX);
	a = Dy[ii][jj];
	b = Dy[ii][jj + 1];
	c = Dy[ii + 1][jj];
	d = Dy[ii + 1][jj + 1];
	val1 = a + (c - a) * ((aPoint.Y - Y[ii]) / deltY);
	val2 = b + (d - b) * ((aPoint.Y - Y[ii]) / deltY);
	dy = val1 + (val2 - val1) * ((aPoint.X - X[jj]) / deltX);

	//Tracing forward by U/V displacement components            
	if (isForward)
	{
		aPoint.X += dx;
		aPoint.Y += dy;
	}
	else
	{
		aPoint.X -= dx;
		aPoint.Y -= dy;
	}

	//Find the grid box that the point is located 
	if (!(aPoint.X >= X[jj] && aPoint.X <= X[jj + 1] && aPoint.Y >= Y[ii] && aPoint.Y <= Y[ii + 1]))
	{
		int xLength = cols;
		int yLength = rows;
		if (aPoint.X < X[0] || aPoint.X > X[xLength - 1] || aPoint.Y < Y[0] || aPoint.Y > Y[yLength - 1])
		{
			return false;
		}

		//Get the grid box of the point located
		for (int ti = ii - 2; ti < ii + 3; ti++)
		{
			if (ti >= 0 && ti < yNum)
			{
				if (aPoint.Y >= Y[ti] && aPoint.Y <= Y[ti + 1])
				{
					ii = ti;
					for (int tj = jj - 2; tj < jj + 3; tj++)
					{
						if (tj >= 0 && tj < xNum)
						{
							if (aPoint.X >= X[tj] && aPoint.X <= X[tj + 1])
							{
								jj = tj;
								break;
							}
						}
					}
					break;
				}
			}
		}
	}

	return true;
}

bool Contour::TraceBorder(int** S1, int i1, int i2, int j1, int j2, int& i3, int& j3)
{
	bool canTrace = true;
	int a, b, c, d;
	if (i1 < i2)    //---- Trace from bottom
	{
		if (S1[i2][j2 - 1] == 1 && S1[i2][j2 + 1] == 1)
		{
			a = S1[i2 - 1][j2 - 1];
			b = S1[i2 + 1][j2];
			c = S1[i2 + 1][j2 - 1];
			if ((a != 0 && b == 0) || (a == 0 && b != 0 && c != 0))
			{
				i3 = i2;
				j3 = j2 - 1;
			}
			else
			{
				i3 = i2;
				j3 = j2 + 1;
			}
		}
		else if (S1[i2][j2 - 1] == 1 && S1[i2 + 1][j2] == 1)
		{
			a = S1[i2 + 1][j2 - 1];
			b = S1[i2 + 1][j2 + 1];
			c = S1[i2][j2 - 1];
			d = S1[i2][j2 + 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2;
					j3 = j2 - 1;
				}
				else
				{
					i3 = i2 + 1;
					j3 = j2;
				}
			}
			else
			{
				i3 = i2;
				j3 = j2 - 1;
			}
		}
		else if (S1[i2][j2 + 1] == 1 && S1[i2 + 1][j2] == 1)
		{
			a = S1[i2 + 1][j2 - 1];
			b = S1[i2 + 1][j2 + 1];
			c = S1[i2][j2 - 1];
			d = S1[i2][j2 + 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2;
					j3 = j2 + 1;
				}
				else
				{
					i3 = i2 + 1;
					j3 = j2;
				}
			}
			else
			{
				i3 = i2;
				j3 = j2 + 1;
			}
		}
		else if (S1[i2][j2 - 1] == 1)
		{
			i3 = i2;
			j3 = j2 - 1;
		}
		else if (S1[i2][j2 + 1] == 1)
		{
			i3 = i2;
			j3 = j2 + 1;
		}
		else if (S1[i2 + 1][j2] == 1)
		{
			i3 = i2 + 1;
			j3 = j2;
		}
		else
		{
			canTrace = false;
		}
	}
	else if (j1 < j2)    //---- Trace from left
	{
		if (S1[i2 + 1][j2] == 1 && S1[i2 - 1][j2] == 1)
		{
			a = S1[i2 + 1][j2 - 1];
			b = S1[i2][j2 + 1];
			c = S1[i2 + 1][j2 + 1];
			if ((a != 0 && b == 0) || (a == 0 && b != 0 && c != 0))
			{
				i3 = i2 + 1;
				j3 = j2;
			}
			else
			{
				i3 = i2 - 1;
				j3 = j2;
			}
		}
		else if (S1[i2 + 1][j2] == 1 && S1[i2][j2 + 1] == 1)
		{
			c = S1[i2 - 1][j2];
			d = S1[i2 + 1][j2];
			a = S1[i2 - 1][j2 + 1];
			b = S1[i2 + 1][j2 + 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2 + 1;
					j3 = j2;
				}
				else
				{
					i3 = i2;
					j3 = j2 + 1;
				}
			}
			else
			{
				i3 = i2 + 1;
				j3 = j2;
			}
		}
		else if (S1[i2 - 1][j2] == 1 && S1[i2][j2 + 1] == 1)
		{
			c = S1[i2 - 1][j2];
			d = S1[i2 + 1][j2];
			a = S1[i2 - 1][j2 + 1];
			b = S1[i2 + 1][j2 + 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2 - 1;
					j3 = j2;
				}
				else
				{
					i3 = i2;
					j3 = j2 + 1;
				}
			}
			else
			{
				i3 = i2 - 1;
				j3 = j2;
			}
		}
		else if (S1[i2 + 1][j2] == 1)
		{
			i3 = i2 + 1;
			j3 = j2;
		}
		else if (S1[i2 - 1][j2] == 1)
		{
			i3 = i2 - 1;
			j3 = j2;
		}
		else if (S1[i2][j2 + 1] == 1)
		{
			i3 = i2;
			j3 = j2 + 1;
		}
		else
		{
			canTrace = false;
		}
	}
	else if (i1 > i2)    //---- Trace from top
	{
		if (S1[i2][j2 - 1] == 1 && S1[i2][j2 + 1] == 1)
		{
			a = S1[i2 + 1][j2 - 1];
			b = S1[i2 - 1][j2];
			c = S1[i2 - 1][j2 + 1];
			if ((a != 0 && b == 0) || (a == 0 && b != 0 && c != 0))
			{
				i3 = i2;
				j3 = j2 - 1;
			}
			else
			{
				i3 = i2;
				j3 = j2 + 1;
			}
		}
		else if (S1[i2][j2 - 1] == 1 && S1[i2 - 1][j2] == 1)
		{
			a = S1[i2 - 1][j2 - 1];
			b = S1[i2 - 1][j2 + 1];
			c = S1[i2][j2 - 1];
			d = S1[i2][j2 + 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2;
					j3 = j2 - 1;
				}
				else
				{
					i3 = i2 - 1;
					j3 = j2;
				}
			}
			else
			{
				i3 = i2;
				j3 = j2 - 1;
			}
		}
		else if (S1[i2][j2 + 1] == 1 && S1[i2 - 1][j2] == 1)
		{
			a = S1[i2 - 1][j2 - 1];
			b = S1[i2 - 1][j2 + 1];
			c = S1[i2][j2 - 1];
			d = S1[i2][j2 + 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2;
					j3 = j2 + 1;
				}
				else
				{
					i3 = i2 - 1;
					j3 = j2;
				}
			}
			else
			{
				i3 = i2;
				j3 = j2 + 1;
			}
		}
		else if (S1[i2][j2 - 1] == 1)
		{
			i3 = i2;
			j3 = j2 - 1;
		}
		else if (S1[i2][j2 + 1] == 1)
		{
			i3 = i2;
			j3 = j2 + 1;
		}
		else if (S1[i2 - 1][j2] == 1)
		{
			i3 = i2 - 1;
			j3 = j2;
		}
		else
		{
			canTrace = false;
		}
	}
	else if (j1 > j2)    //---- Trace from right
	{
		if (S1[i2 + 1][j2] == 1 && S1[i2 - 1][j2] == 1)
		{
			a = S1[i2 + 1][j2 + 1];
			b = S1[i2][j2 - 1];
			c = S1[i2 - 1][j2 - 1];
			if ((a != 0 && b == 0) || (a == 0 && b != 0 && c != 0))
			{
				i3 = i2 + 1;
				j3 = j2;
			}
			else
			{
				i3 = i2 - 1;
				j3 = j2;
			}
		}
		else if (S1[i2 + 1][j2] == 1 && S1[i2][j2 - 1] == 1)
		{
			c = S1[i2 - 1][j2];
			d = S1[i2 + 1][j2];
			a = S1[i2 - 1][j2 - 1];
			b = S1[i2 + 1][j2 - 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2 + 1;
					j3 = j2;
				}
				else
				{
					i3 = i2;
					j3 = j2 - 1;
				}
			}
			else
			{
				i3 = i2 + 1;
				j3 = j2;
			}
		}
		else if (S1[i2 - 1][j2] == 1 && S1[i2][j2 - 1] == 1)
		{
			c = S1[i2 - 1][j2];
			d = S1[i2 + 1][j2];
			a = S1[i2 - 1][j2 - 1];
			b = S1[i2 + 1][j2 - 1];
			if (a == 0 || b == 0 || c == 0 || d == 0)
			{
				if ((a == 0 && d == 0) || (b == 0 && c == 0))
				{
					i3 = i2 - 1;
					j3 = j2;
				}
				else
				{
					i3 = i2;
					j3 = j2 - 1;
				}
			}
			else
			{
				i3 = i2 - 1;
				j3 = j2;
			}
		}
		else if (S1[i2 + 1][j2] == 1)
		{
			i3 = i2 + 1;
			j3 = j2;
		}
		else if (S1[i2 - 1][j2] == 1)
		{
			i3 = i2 - 1;
			j3 = j2;
		}
		else if (S1[i2][j2 - 1] == 1)
		{
			i3 = i2;
			j3 = j2 - 1;
		}
		else
		{
			canTrace = false;
		}
	}

	return canTrace;
}

bool Contour::IsClockwise(vector<PointD> pointList)
{
	int i;
	PointD aPoint;
	double yMax = 0;
	int yMaxIdx = 0;
	for (i = 0; i < pointList.size() - 1; i++)
	{
		aPoint = pointList[i];
		if (i == 0)
		{
			yMax = aPoint.Y;
			yMaxIdx = 0;
		}
		else
		{
			if (yMax < aPoint.Y)
			{
				yMax = aPoint.Y;
				yMaxIdx = i;
			}
		}
	}
	PointD p1, p2, p3;
	int p1Idx, p2Idx, p3Idx;
	p1Idx = yMaxIdx - 1;
	p2Idx = yMaxIdx;
	p3Idx = yMaxIdx + 1;
	if (yMaxIdx == 0)
		p1Idx = pointList.size() - 2;

	p1 = pointList[p1Idx];
	p2 = pointList[p2Idx];
	p3 = pointList[p3Idx];
	if ((p3.X - p1.X) * (p2.Y - p1.Y) - (p2.X - p1.X) * (p3.Y - p1.Y) > 0)
		return true;
	else
		return false;

}

PointF Contour::GetCrossPoint(PointF aP1, PointF aP2, PointF bP1, PointF bP2)
{
	PointF IPoint(0, 0);
	PointF p1, p2, q1, q2;
	double tempLeft, tempRight;

	double XP1 = (bP1.X - aP1.X) * (aP2.Y - aP1.Y) -
		(aP2.X - aP1.X) * (bP1.Y - aP1.Y);
	double XP2 = (bP2.X - aP1.X) * (aP2.Y - aP1.Y) -
		(aP2.X - aP1.X) * (bP2.Y - aP1.Y);
	if (XP1 == 0)
		IPoint = bP1;
	else if (XP2 == 0)
		IPoint = bP2;
	else
	{
		p1 = aP1;
		p2 = aP2;
		q1 = bP1;
		q2 = bP2;

		tempLeft = (q2.X - q1.X) * (p1.Y - p2.Y) - (p2.X - p1.X) * (q1.Y - q2.Y);
		tempRight = (p1.Y - q1.Y) * (p2.X - p1.X) * (q2.X - q1.X) + q1.X * (q2.Y - q1.Y) * (p2.X - p1.X) - p1.X * (p2.Y - p1.Y) * (q2.X - q1.X);
		IPoint.X = (float)(tempRight / tempLeft);

		tempLeft = (p1.X - p2.X) * (q2.Y - q1.Y) - (p2.Y - p1.Y) * (q1.X - q2.X);
		tempRight = p2.Y * (p1.X - p2.X) * (q2.Y - q1.Y) + (q2.X - p2.X) * (q2.Y - q1.Y) * (p1.Y - p2.Y) - q2.Y * (q1.X - q2.X) * (p2.Y - p1.Y);
		IPoint.Y = (float)(tempRight / tempLeft);
	}

	return IPoint;
}

bool Contour::DoubleEquals(double a, double b)
{
	double difference = abs(a * 0.00001);
	if (abs(a - b) < difference)
		return true;
	else
		return false;
}

vector<PolyLine> Contour::Isoline_UndefData(double** S0, double* X, double* Y,
	double W, double nx, double ny,
	double** &S,double** &H, int*** SB, int*** HB, int lineNum)
{
	vector<PolyLine> cLineList;
	int m, n, i, j;
	m = rows;
	n = cols;

	int i1, i2, j1, j2, i3 = 0, j3 = 0;
	double a2x, a2y, a3x = 0, a3y = 0, sx, sy;
	PointD aPoint;
	PolyLine aLine;
	vector<PointD> pList;
	bool isS = true;
	EndPoint aEndPoint;
	//---- Tracing from border
	for (i = 0; i < m; i++)
	{
		for (j = 0; j < n; j++)
		{
			if (j < n - 1)
			{
				if (SB[0][ i][j] > -1)    //---- Border
				{
					if (S[i][ j] != -2)
					{
						pList = vector<PointD>();
						i2 = i;
						j2 = j;
						a2x = X[j2] + S[i2][ j2] * nx;    //---- x of first point
						a2y = Y[i2];                   //---- y of first point
						if (SB[1][ i][ j] == 0)    //---- Bottom border
						{
							i1 = -1;
							aEndPoint.sPoint.X = X[j + 1];
							aEndPoint.sPoint.Y = Y[i];
						}
						else
						{
							i1 = i2;
							aEndPoint.sPoint.X = X[j];
							aEndPoint.sPoint.Y = Y[i];
						}
						j1 = j2;
						aPoint =  PointD();
						aPoint.X = a2x;
						aPoint.Y = a2y;
						pList.push_back(aPoint);

						aEndPoint.Index = lineNum + cLineList.size();
						aEndPoint.Point = aPoint;
						aEndPoint.BorderIdx = SB[0][ i][j];
						_endPointList.push_back(aEndPoint);

						aLine =  PolyLine();
						aLine.Type = "Border";
						aLine.BorderIdx = SB[0][ i][ j];
						while (true)
						{
							if (TraceIsoline_UndefData(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x,  i3,  j3,  a3x,  a3y,  isS))
							{
								aPoint =  PointD();
								aPoint.X = a3x;
								aPoint.Y = a3y;
								pList.push_back(aPoint);
								if (isS)
								{
									if (SB[0][ i3][j3] > -1)
									{
										if (SB[1][i3][ j3] == 0)
										{
											aEndPoint.sPoint.X = X[j3 + 1];
											aEndPoint.sPoint.Y = Y[i3];
										}
										else
										{
											aEndPoint.sPoint.X = X[j3];
											aEndPoint.sPoint.Y = Y[i3];
										}
										break;
									}
								}
								else
								{
									if (HB[0][ i3][j3] > -1)
									{
										if (HB[1][ i3][ j3] == 0)
										{
											aEndPoint.sPoint.X = X[j3];
											aEndPoint.sPoint.Y = Y[i3];
										}
										else
										{
											aEndPoint.sPoint.X = X[j3];
											aEndPoint.sPoint.Y = Y[i3 + 1];
										}
										break;
									}
								}
								a2x = a3x;
								a2y = a3y;
								i1 = i2;
								j1 = j2;
								i2 = i3;
								j2 = j3;
							}
							else
							{
								aLine.Type = "Error";
								break;
							}
						}
						S[i][j] = -2;
						if (pList.size() > 1 && aLine.Type != "Error")
						{
							aEndPoint.Point = aPoint;
							_endPointList.push_back(aEndPoint);

							aLine.Value = W;
							aLine.PointList = pList;
							cLineList.push_back(aLine);
						}
						else
							//_endPointList.RemoveAt(_endPointList.Count - 1);
							_endPointList.erase(_endPointList.begin() + (_endPointList.size() - 1));

					}
				}
			}
			if (i < m - 1)
			{
				if (HB[0][ i][j] > -1)    //---- Border
				{
					if (H[i][ j] != -2)
					{
						pList = vector<PointD>();
						i2 = i;
						j2 = j;
						a2x = X[j2];
						a2y = Y[i2] + H[i2][ j2] * ny;
						i1 = i2;
						if (HB[1][i][ j] == 0)
						{
							j1 = -1;
							aEndPoint.sPoint.X = X[j];
							aEndPoint.sPoint.Y = Y[i];
						}
						else
						{
							j1 = j2;
							aEndPoint.sPoint.X = X[j];
							aEndPoint.sPoint.Y = Y[i + 1];
						}
						aPoint =  PointD();
						aPoint.X = a2x;
						aPoint.Y = a2y;
						pList.push_back(aPoint);

						aEndPoint.Index = lineNum + cLineList.size();
						aEndPoint.Point = aPoint;
						aEndPoint.BorderIdx = HB[0][i][ j];
						_endPointList.push_back(aEndPoint);

						aLine =  PolyLine();
						aLine.Type = "Border";
						aLine.BorderIdx = HB[0][ i][ j];
						while (true)
						{
							if (TraceIsoline_UndefData(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x,  i3,  j3,  a3x,  a3y,  isS))
							{
								aPoint =  PointD();
								aPoint.X = a3x;
								aPoint.Y = a3y;
								pList.push_back(aPoint);
								if (isS)
								{
									if (SB[0][ i3][j3] > -1)
									{
										if (SB[1][ i3][ j3] == 0)
										{
											aEndPoint.sPoint.X = X[j3 + 1];
											aEndPoint.sPoint.Y = Y[i3];
										}
										else
										{
											aEndPoint.sPoint.X = X[j3];
											aEndPoint.sPoint.Y = Y[i3];
										}
										break;
									}
								}
								else
								{
									if (HB[0][ i3][ j3] > -1)
									{
										if (HB[1][ i3][j3] == 0)
										{
											aEndPoint.sPoint.X = X[j3];
											aEndPoint.sPoint.Y = Y[i3];
										}
										else
										{
											aEndPoint.sPoint.X = X[j3];
											aEndPoint.sPoint.Y = Y[i3 + 1];
										}
										break;
									}
								}
								a2x = a3x;
								a2y = a3y;
								i1 = i2;
								j1 = j2;
								i2 = i3;
								j2 = j3;
							}
							else
							{
								aLine.Type = "Error";
								break;
							}
						}
						H[i][ j] = -2;
						if (pList.size() > 1 && aLine.Type != "Error")
						{
							aEndPoint.Point = aPoint;
							_endPointList.push_back(aEndPoint);

							aLine.Value = W;
							aLine.PointList = pList;
							cLineList.push_back(aLine);
						}
						else
							_endPointList.erase(_endPointList.begin() + (_endPointList.size() - 1));
							//_endPointList.RemoveAt(_endPointList.Count - 1);

					}
				}
			}
		}
	}

	//---- Clear border points
	for (j = 0; j < n - 1; j++)
	{
		if (S[0][ j] != -2)
			S[0][ j] = -2;
		if (S[m - 1][ j] != -2)
			S[m - 1][ j] = -2;
	}

	for (i = 0; i < m - 1; i++)
	{
		if (H[i][ 0] != -2)
			H[i][ 0] = -2;
		if (H[i][ n - 1] != -2)
			H[i][ n - 1] = -2;
	}

	//---- Tracing close lines
	for (i = 1; i < m - 2; i++)
	{
		for (j = 1; j < n - 1; j++)
		{
			if (H[i][ j] != -2)
			{
				vector<PointD> pointList;
				i2 = i;
				j2 = j;
				a2x = X[j2];
				a2y = Y[i] + H[i][ j2] * ny;
				j1 = -1;
				i1 = i2;
				sx = a2x;
				sy = a2y;
				aPoint =  PointD();
				aPoint.X = a2x;
				aPoint.Y = a2y;
				pointList.push_back(aPoint);
				aLine =  PolyLine();
				aLine.Type = "Close";

				while (true)
				{
					if (TraceIsoline_UndefData(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x,  i3,  j3,  a3x,  a3y,  isS))
					{
						aPoint =  PointD();
						aPoint.X = a3x;
						aPoint.Y = a3y;
						pointList.push_back(aPoint);
						if (abs(a3y - sy) < 0.000001 && abs(a3x - sx) < 0.000001)
							break;

						a2x = a3x;
						a2y = a3y;
						i1 = i2;
						j1 = j2;
						i2 = i3;
						j2 = j3;
						//If X[j2] < a2x && i2 = 0 )
						//    aLine.type = "Error"
						//    Exit Do
						//End If
					}
					else
					{
						aLine.Type = "Error";
						break;
					}
				}
				H[i][ j] = -2;
				if (pointList.size() > 1 && aLine.Type != "Error")
				{
					aLine.Value = W;
					aLine.PointList = pointList;
					cLineList.push_back(aLine);
				}
			}
		}
	}

	for (i = 1; i < m - 1; i++)
	{
		for (j = 1; j < n - 2; j++)
		{
			if (S[i][ j] != -2)
			{
				vector<PointD> pointList;
				i2 = i;
				j2 = j;
				a2x = X[j2] + S[i][ j] * nx;
				a2y = Y[i];
				j1 = j2;
				i1 = -1;
				sx = a2x;
				sy = a2y;
				aPoint =  PointD();
				aPoint.X = a2x;
				aPoint.Y = a2y;
				pointList.push_back(aPoint);
				aLine =  PolyLine();
				aLine.Type = "Close";

				while (true)
				{
					if (TraceIsoline_UndefData(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x,  i3,  j3,  a3x,  a3y,  isS))
					{
						aPoint =  PointD();
						aPoint.X = a3x;
						aPoint.Y = a3y;
						pointList.push_back(aPoint);
						if (abs(a3y - sy) < 0.000001 && abs(a3x - sx) < 0.000001)
							break;

						a2x = a3x;
						a2y = a3y;
						i1 = i2;
						j1 = j2;
						i2 = i3;
						j2 = j3;
					}
					else
					{
						aLine.Type = "Error";
						break;
					}
				}
				S[i][j] = -2;
				if (pointList.size() > 1 && aLine.Type != "Error")
				{
					aLine.Value = W;
					aLine.PointList = pointList;
					cLineList.push_back(aLine);
				}
			}
		}
	}

	return cLineList;
}

bool Contour::TraceIsoline_UndefData(int i1, int i2, double** &H, double** &S, int j1, int j2, double* X,
	double* Y, double nx, double ny, double a2x, int &i3,
	int &j3, double &a3x, double &a3y, bool &isS)
{
	bool canTrace = true;
	if (i1 < i2)    //---- Trace from bottom
	{
		if (H[i2][ j2] != -2 && H[i2][ j2 + 1] != -2)
		{
			if (H[i2][ j2] < H[i2][j2 + 1])
			{
				a3x = X[j2];
				a3y = Y[i2] + H[i2][ j2] * ny;
				i3 = i2;
				j3 = j2;
				H[i3][ j3] = -2;
				isS = false;
			}
			else
			{
				a3x = X[j2 + 1];
				a3y = Y[i2] + H[i2][ j2 + 1] * ny;
				i3 = i2;
				j3 = j2 + 1;
				H[i3][ j3] = -2;
				isS = false;
			}
		}
		else if (H[i2][ j2] != -2 && H[i2][ j2 + 1] == -2)
		{
			a3x = X[j2];
			a3y = Y[i2] + H[i2][ j2] * ny;
			i3 = i2;
			j3 = j2;
			H[i3][ j3] = -2;
			isS = false;
		}
		else if (H[i2][ j2] == -2 && H[i2][ j2 + 1] != -2)
		{
			a3x = X[j2 + 1];
			a3y = Y[i2] + H[i2][ j2 + 1] * ny;
			i3 = i2;
			j3 = j2 + 1;
			H[i3][ j3] = -2;
			isS = false;
		}
		else if (S[i2 + 1][ j2] != -2)
		{
			a3x = X[j2] + S[i2 + 1][ j2] * nx;
			a3y = Y[i2 + 1];
			i3 = i2 + 1;
			j3 = j2;
			S[i3][j3] = -2;
			isS = true;
		}
		else
			canTrace = false;
	}
	else if (j1 < j2)    //---- Trace from left
	{
		if (S[i2][j2] != -2 && S[i2 + 1][ j2] != -2)
		{
			if (S[i2][ j2] < S[i2 + 1][ j2])
			{
				a3x = X[j2] + S[i2][ j2] * nx;
				a3y = Y[i2];
				i3 = i2;
				j3 = j2;
				S[i3][ j3] = -2;
				isS = true;
			}
			else
			{
				a3x = X[j2] + S[i2 + 1][ j2] * nx;
				a3y = Y[i2 + 1];
				i3 = i2 + 1;
				j3 = j2;
				S[i3][ j3] = -2;
				isS = true;
			}
		}
		else if (S[i2][ j2] != -2 && S[i2 + 1][ j2] == -2)
		{
			a3x = X[j2] + S[i2][j2] * nx;
			a3y = Y[i2];
			i3 = i2;
			j3 = j2;
			S[i3][ j3] = -2;
			isS = true;
		}
		else if (S[i2][ j2] == -2 && S[i2 + 1][ j2] != -2)
		{
			a3x = X[j2] + S[i2 + 1][ j2] * nx;
			a3y = Y[i2 + 1];
			i3 = i2 + 1;
			j3 = j2;
			S[i3][ j3] = -2;
			isS = true;
		}
		else if (H[i2][j2 + 1] != -2)
		{
			a3x = X[j2 + 1];
			a3y = Y[i2] + H[i2][ j2 + 1] * ny;
			i3 = i2;
			j3 = j2 + 1;
			H[i3][ j3] = -2;
			isS = false;
		}
		else
			canTrace = false;

	}
	else if (X[j2] < a2x)    //---- Trace from top
	{
		if (H[i2 - 1][ j2] != -2 && H[i2 - 1][ j2 + 1] != -2)
		{
			if (H[i2 - 1][ j2] > H[i2 - 1][ j2 + 1])    //---- < changed to >
			{
				a3x = X[j2];
				a3y = Y[i2 - 1] + H[i2 - 1][ j2] * ny;
				i3 = i2 - 1;
				j3 = j2;
				H[i3][ j3] = -2;
				isS = false;
			}
			else
			{
				a3x = X[j2 + 1];
				a3y = Y[i2 - 1] + H[i2 - 1][ j2 + 1] * ny;
				i3 = i2 - 1;
				j3 = j2 + 1;
				H[i3][ j3] = -2;
				isS = false;
			}
		}
		else if (H[i2 - 1][ j2] != -2 && H[i2 - 1][ j2 + 1] == -2)
		{
			a3x = X[j2];
			a3y = Y[i2 - 1] + H[i2 - 1][ j2] * ny;
			i3 = i2 - 1;
			j3 = j2;
			H[i3][ j3] = -2;
			isS = false;
		}
		else if (H[i2 - 1][ j2] == -2 && H[i2 - 1][ j2 + 1] != -2)
		{
			a3x = X[j2 + 1];
			a3y = Y[i2 - 1] + H[i2 - 1][ j2 + 1] * ny;
			i3 = i2 - 1;
			j3 = j2 + 1;
			H[i3][ j3] = -2;
			isS = false;
		}
		else if (S[i2 - 1][ j2] != -2)
		{
			a3x = X[j2] + S[i2 - 1][ j2] * nx;
			a3y = Y[i2 - 1];
			i3 = i2 - 1;
			j3 = j2;
			S[i3][ j3] = -2;
			isS = true;
		}
		else
			canTrace = false;
	}
	else    //---- Trace from right
	{
		if (S[i2 + 1][ j2 - 1] != -2 && S[i2][ j2 - 1] != -2)
		{
			if (S[i2 + 1][j2 - 1] > S[i2][j2 - 1])    //---- < changed to >
			{
				a3x = X[j2 - 1] + S[i2 + 1][ j2 - 1] * nx;
				a3y = Y[i2 + 1];
				i3 = i2 + 1;
				j3 = j2 - 1;
				S[i3][ j3] = -2;
				isS = true;
			}
			else
			{
				a3x = X[j2 - 1] + S[i2][ j2 - 1] * nx;
				a3y = Y[i2];
				i3 = i2;
				j3 = j2 - 1;
				S[i3][ j3] = -2;
				isS = true;
			}
		}
		else if (S[i2 + 1][ j2 - 1] != -2 && S[i2][ j2 - 1] == -2)
		{
			a3x = X[j2 - 1] + S[i2 + 1][j2 - 1] * nx;
			a3y = Y[i2 + 1];
			i3 = i2 + 1;
			j3 = j2 - 1;
			S[i3][ j3] = -2;
			isS = true;
		}
		else if (S[i2 + 1][ j2 - 1] == -2 && S[i2][ j2 - 1] != -2)
		{
			a3x = X[j2 - 1] + S[i2][ j2 - 1] * nx;
			a3y = Y[i2];
			i3 = i2;
			j3 = j2 - 1;
			S[i3][ j3] = -2;
			isS = true;
		}
		else if (H[i2][ j2 - 1] != -2)
		{
			a3x = X[j2 - 1];
			a3y = Y[i2] + H[i2][ j2 - 1] * ny;
			i3 = i2;
			j3 = j2 - 1;
			H[i3][ j3] = -2;
			isS = false;
		}
		else
			canTrace = false;
	}

	return canTrace;
}


std::tuple<int, int, double, double> Contour::TraceIsoline(int i1, int i2, double** &H, double** &S, int j1, int j2, double* X,
	double* Y, double nx, double ny, double a2x)
{
	int i3, j3;
	double a3x, a3y;
	if (i1 < i2)    //---- Trace from bottom
	{
		if (H[i2][j2] != -2 && H[i2][j2 + 1] != -2)
		{
			if (H[i2][j2] < H[i2][j2 + 1])
			{
				a3x = X[j2];
				a3y = Y[i2] + H[i2][j2] * ny;
				i3 = i2;
				j3 = j2;
				H[i3][j3] = -2;
			}
			else
			{
				a3x = X[j2 + 1];
				a3y = Y[i2] + H[i2][j2 + 1] * ny;
				i3 = i2;
				j3 = j2 + 1;
				H[i3][j3] = -2;
			}
		}
		else if (H[i2][j2] != -2 && H[i2][j2 + 1] == -2)
		{
			a3x = X[j2];
			a3y = Y[i2] + H[i2][j2] * ny;
			i3 = i2;
			j3 = j2;
			H[i3][j3] = -2;
		}
		else if (H[i2][j2] == -2 && H[i2][j2 + 1] != -2)
		{
			a3x = X[j2 + 1];
			a3y = Y[i2] + H[i2][j2 + 1] * ny;
			i3 = i2;
			j3 = j2 + 1;
			H[i3][j3] = -2;
		}
		else
		{
			a3x = X[j2] + S[i2 + 1][j2] * nx;
			a3y = Y[i2 + 1];
			i3 = i2 + 1;
			j3 = j2;
			S[i3][j3] = -2;
		}
	}
	else if (j1 < j2)    //---- Trace from left
	{
		if (S[i2][j2] != -2 && S[i2 + 1][j2] != -2)
		{
			if (S[i2][j2] < S[i2 + 1][j2])
			{
				a3x = X[j2] + S[i2][j2] * nx;
				a3y = Y[i2];
				i3 = i2;
				j3 = j2;
				S[i3][j3] = -2;
			}
			else
			{
				a3x = X[j2] + S[i2 + 1][j2] * nx;
				a3y = Y[i2 + 1];
				i3 = i2 + 1;
				j3 = j2;
				S[i3][j3] = -2;
			}
		}
		else if (S[i2][j2] != -2 && S[i2 + 1][j2] == -2)
		{
			a3x = X[j2] + S[i2][j2] * nx;
			a3y = Y[i2];
			i3 = i2;
			j3 = j2;
			S[i3][j3] = -2;
		}
		else if (S[i2][j2] == -2 && S[i2 + 1][j2] != -2)
		{
			a3x = X[j2] + S[i2 + 1][j2] * nx;
			a3y = Y[i2 + 1];
			i3 = i2 + 1;
			j3 = j2;
			S[i3][j3] = -2;
		}
		else
		{
			a3x = X[j2 + 1];
			a3y = Y[i2] + H[i2][j2 + 1] * ny;
			i3 = i2;
			j3 = j2 + 1;
			H[i3][j3] = -2;
		}
	}
	else if (X[j2] < a2x)    //---- Trace from top
	{
		if (H[i2 - 1][j2] != -2 && H[i2 - 1][j2 + 1] != -2)
		{
			if (H[i2 - 1][j2] > H[i2 - 1][j2 + 1])    //---- < changed to >
			{
				a3x = X[j2];
				a3y = Y[i2 - 1] + H[i2 - 1][j2] * ny;
				i3 = i2 - 1;
				j3 = j2;
				H[i3][j3] = -2;
			}
			else
			{
				a3x = X[j2 + 1];
				a3y = Y[i2 - 1] + H[i2 - 1][j2 + 1] * ny;
				i3 = i2 - 1;
				j3 = j2 + 1;
				H[i3][j3] = -2;
			}
		}
		else if (H[i2 - 1][j2] != -2 && H[i2 - 1][j2 + 1] == -2)
		{
			a3x = X[j2];
			a3y = Y[i2 - 1] + H[i2 - 1][j2] * ny;
			i3 = i2 - 1;
			j3 = j2;
			H[i3][j3] = -2;
		}
		else if (H[i2 - 1][j2] == -2 && H[i2 - 1][j2 + 1] != -2)
		{
			a3x = X[j2 + 1];
			a3y = Y[i2 - 1] + H[i2 - 1][j2 + 1] * ny;
			i3 = i2 - 1;
			j3 = j2 + 1;
			H[i3][j3] = -2;
		}
		else
		{
			a3x = X[j2] + S[i2 - 1][j2] * nx;
			a3y = Y[i2 - 1];
			i3 = i2 - 1;
			j3 = j2;
			S[i3][j3] = -2;
		}
	}
	else    //---- Trace from right
	{
		if (S[i2 + 1][j2 - 1] != -2 && S[i2][j2 - 1] != -2)
		{
			if (S[i2 + 1][j2 - 1] > S[i2][j2 - 1])    //---- < changed to >
			{
				a3x = X[j2 - 1] + S[i2 + 1][j2 - 1] * nx;
				a3y = Y[i2 + 1];
				i3 = i2 + 1;
				j3 = j2 - 1;
				S[i3][j3] = -2;
			}
			else
			{
				a3x = X[j2 - 1] + S[i2][j2 - 1] * nx;
				a3y = Y[i2];
				i3 = i2;
				j3 = j2 - 1;
				S[i3][j3] = -2;
			}
		}
		else if (S[i2 + 1][j2 - 1] != -2 && S[i2][j2 - 1] == -2)
		{
			a3x = X[j2 - 1] + S[i2 + 1][j2 - 1] * nx;
			a3y = Y[i2 + 1];
			i3 = i2 + 1;
			j3 = j2 - 1;
			S[i3][j3] = -2;
		}
		else if (S[i2 + 1][j2 - 1] == -2 && S[i2][j2 - 1] != -2)
		{
			a3x = X[j2 - 1] + S[i2][j2 - 1] * nx;
			a3y = Y[i2];
			i3 = i2;
			j3 = j2 - 1;
			S[i3][j3] = -2;
		}
		else
		{
			a3x = X[j2 - 1];
			a3y = Y[i2] + H[i2][j2 - 1] * ny;
			i3 = i2;
			j3 = j2 - 1;
			H[i3][j3] = -2;
		}
	}

	return std::tuple<int, int, double, double>(i3, j3, a3x, a3y);
	//return new Object[]{ i3, j3, a3x, a3y };
}

vector<PolyLine> Contour::Isoline_Bottom(double** S0, double* X, double* Y, double W, double nx, double ny,
	double** &S, double** &H)
{
	vector<PolyLine> bLineList;
	int  m = rows, n = cols, j;
	//ArryUtils::Get2DArryRowCol(S0, m, n);
	//m = S0.GetLength(0);
	//n = S0.GetLength(1);

	int i1, i2, j1 = 0, j2, i3, j3;
	double a2x, a2y, a3x, a3y;
	std::tuple<int, int, double, double> returnVal;
	PointD aPoint;
	PolyLine aLine;
	for (j = 0; j < n - 1; j++)    //---- Trace isoline from bottom
	{
		if (S[0][ j] != -2)    //---- Has tracing value
		{
			vector<PointD> pointList;
			i2 = 0;
			j2 = j;
			a2x = X[j] + S[0][ j] * nx;    //---- x of first point
			a2y = Y[0];                   //---- y of first point
			i1 = -1;
			aPoint =  PointD();
			aPoint.X = a2x;
			aPoint.Y = a2y;
			pointList.push_back(aPoint);
			while (true)
			{
				returnVal = TraceIsoline(i1, i2, H,  S, j1, j2, X, Y, nx, ny, a2x);
				i3 = std::get<0>(returnVal);// (int)returnVal[0];
				j3 = std::get<1>(returnVal);// (int)returnVal[1];
				a3x = std::get<2>(returnVal);// (double)returnVal[2];
				a3y = std::get<3>(returnVal);// (double)returnVal[3];
				aPoint =  PointD();
				aPoint.X = a3x;
				aPoint.Y = a3y;
				pointList.push_back(aPoint);
				if (i3 == m - 1 || j3 == n - 1 || a3y == Y[0] || a3x == X[0])
					break;

				a2x = a3x;
				a2y = a3y;
				i1 = i2;
				j1 = j2;
				i2 = i3;
				j2 = j3;
			}
			S[0][ j] = -2;
			if (pointList.size() > 4)
			{
				aLine =  PolyLine();
				aLine.Value = W;
				aLine.Type = "Bottom";
				aLine.PointList = pointList;
				//m_LineList.Add(aLine);
				bLineList.push_back(aLine);
			}
		}
	}

	return bLineList;
}

vector<PolyLine> Contour::Isoline_Left(double** S0, double* X, double* Y, double W, double nx, double ny,
	double** &S, double** &H)
{
	vector<PolyLine> lLineList;
	int m = rows, n = cols, i;
	//m = S0.GetLength(0);
	//n = S0.GetLength(1);
	
	//ArryUtils::Get2DArryRowCol(S0, m, n);

	int i1, i2, j1, j2, i3, j3;
	double a2x, a2y, a3x, a3y;
	//object[] returnVal;
	std::tuple<int, int, double, double> returnVal;
	PointD aPoint;
	PolyLine aLine;
	for (i = 0; i < m - 1; i++)    //---- Trace isoline from Left
	{
		if (H[i][ 0] != -2)
		{
			vector<PointD> pointList;
			i2 = i;
			j2 = 0;
			a2x = X[0];
			a2y = Y[i] + H[i][ 0] * ny;
			j1 = -1;
			i1 = i2;
			aPoint =  PointD();
			aPoint.X = a2x;
			aPoint.Y = a2y;
			pointList.push_back(aPoint);
			while (true)
			{
				returnVal = TraceIsoline(i1, i2, H, S, j1, j2, X, Y, nx, ny, a2x);

				i3 = std::get<0>(returnVal);// (int)returnVal[0];
				j3 = std::get<1>(returnVal);// (int)returnVal[1];
				a3x = std::get<2>(returnVal);// (double)returnVal[2];
				a3y = std::get<3>(returnVal);// (double)returnVal[3];

				aPoint =  PointD();
				aPoint.X = a3x;
				aPoint.Y = a3y;
				pointList.push_back(aPoint);
				if (i3 == m - 1 || j3 == n - 1 || a3y == Y[0] || a3x == X[0])
					break;

				a2x = a3x;
				a2y = a3y;
				i1 = i2;
				j1 = j2;
				i2 = i3;
				j2 = j3;
			}
			if (pointList.size() > 4)
			{
				aLine =  PolyLine();
				aLine.Value = W;
				aLine.Type = "Left";
				aLine.PointList = pointList;
				//m_LineList.Add(aLine);
				lLineList.push_back(aLine);
			}
		}
	}

	return lLineList;
}

vector<PolyLine> Contour::Isoline_Top(double** S0, double* X, double* Y, double W, double nx, double ny,
	double** &S, double** &H)
{
	vector<PolyLine> tLineList;
	int  m = rows, n = cols, j;

	//ArryUtils::Get2DArryRowCol(S0, m, n);
	
	//m = S0.GetLength(0);
	//n = S0.GetLength(1);

	int i1, i2, j1, j2, i3, j3;
	double a2x, a2y, a3x, a3y;
	//object[] returnVal;
	std::tuple<int, int, double, double> returnVal;
	PointD aPoint;
	PolyLine aLine;
	for (j = 0; j < n - 1; j++)
	{
		if (S[m - 1][ j] != -2)
		{
			vector<PointD> pointList;
			i2 = m - 1;
			j2 = j;
			a2x = X[j] + S[i2][ j] * nx;
			a2y = Y[i2];
			i1 = i2;
			j1 = j2;
			aPoint =  PointD();
			aPoint.X = a2x;
			aPoint.Y = a2y;
			pointList.push_back(aPoint);
			while (true)
			{
				returnVal = TraceIsoline(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x);			
				i3 = std::get<0>(returnVal);// (int)returnVal[0];
				j3 = std::get<1>(returnVal);// (int)returnVal[1];
				a3x = std::get<2>(returnVal);// (double)returnVal[2];
				a3y = std::get<3>(returnVal);// (double)returnVal[3];
				aPoint = PointD();
				aPoint.X = a3x;
				aPoint.Y = a3y;
				pointList.push_back(aPoint);
				if (i3 == m - 1 || j3 == n - 1 || a3y == Y[0] || a3x == X[0])
					break;

				a2x = a3x;
				a2y = a3y;
				i1 = i2;
				j1 = j2;
				i2 = i3;
				j2 = j3;
			}
			S[m - 1][ j] = -2;
			if (pointList.size() > 4)
			{
				aLine =  PolyLine();
				aLine.Value = W;
				aLine.Type = "Top";
				aLine.PointList = pointList;
				//m_LineList.Add(aLine);
				tLineList.push_back(aLine);
			}
		}
	}

	return tLineList;
}

vector<PolyLine> Contour::Isoline_Right(double** S0, double* X, double* Y, double W, double nx, double ny,
	double** &S, double** &H)
{
	vector<PolyLine> rLineList;
	int  m = rows, n = cols, i;
	
	//ArryUtils::Get2DArryRowCol(S0, m, n);
	//m = S0.GetLength(0);
	//n = S0.GetLength(1);

	int i1, i2, j1, j2, i3, j3;
	double a2x, a2y, a3x, a3y;
	std::tuple<int, int, double, double> returnVal;
	//object[] returnVal;
	PointD aPoint;
	PolyLine aLine;
	for (i = 0; i < m - 1; i++)
	{
		if (H[i][ n - 1] != -2)
		{
			vector<PointD> pointList;
			i2 = i;
			j2 = n - 1;
			a2x = X[j2];
			a2y = Y[i] + H[i][ j2] * ny;
			j1 = j2;
			i1 = i2;
			aPoint =  PointD();
			aPoint.X = a2x;
			aPoint.Y = a2y;
			pointList.push_back(aPoint);
			while (true)
			{
				returnVal = TraceIsoline(i1, i2, H, S, j1, j2, X, Y, nx, ny, a2x);
				i3 = std::get<0>(returnVal);// (int)returnVal[0];
				j3 = std::get<1>(returnVal);// (int)returnVal[1];
				a3x = std::get<2>(returnVal);// (double)returnVal[2];
				a3y = std::get<3>(returnVal);// (double)returnVal[3];
				aPoint =  PointD();
				aPoint.X = a3x;
				aPoint.Y = a3y;
				pointList.push_back(aPoint);
				if (i3 == m - 1 || j3 == n - 1 || a3y == Y[0] || a3x == X[0])
					break;

				a2x = a3x;
				a2y = a3y;
				i1 = i2;
				j1 = j2;
				i2 = i3;
				j2 = j3;
			}
			if (pointList.size() > 4)
			{
				aLine =  PolyLine();
				aLine.Value = W;
				aLine.Type = "Right";
				aLine.PointList = pointList;
				//m_LineList.Add(aLine)
				rLineList.push_back(aLine);
			}
		}
	}

	return rLineList;
}

vector<PolyLine> Contour::Isoline_Close(double** S0, double* X, double* Y, double W, double nx, double ny,
	double** &S, double** &H)
{
	vector<PolyLine> cLineList;
	int  m = rows, n = cols, i, j;

	//ArryUtils::Get2DArryRowCol(S0, m, n);

	//m = S0.GetLength(0);
	//n = S0.GetLength(1);

	int i1, i2, j1, j2, i3, j3;
	double a2x, a2y, a3x, a3y, sx, sy;
	//object[] returnVal;
	std::tuple<int, int, double, double> returnVal;
	PointD aPoint;
	PolyLine aLine;
	for (i = 1; i < m - 2; i++)
	{
		for (j = 1; j < n - 1; j++)
		{
			if (H[i][ j] != -2)
			{
				vector<PointD> pointList;
				i2 = i;
				j2 = j;
				a2x = X[j2];
				a2y = Y[i] + H[i][j2] * ny;
				j1 = 0;
				i1 = i2;
				sx = a2x;
				sy = a2y;
				aPoint =  PointD();
				aPoint.X = a2x;
				aPoint.Y = a2y;
				pointList.push_back(aPoint);
				while (true)
				{
					returnVal = TraceIsoline(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x);
					i3 = std::get<0>(returnVal);// (int)returnVal[0];
					j3 = std::get<1>(returnVal);// (int)returnVal[1];
					a3x = std::get<2>(returnVal);// (double)returnVal[2];
					a3y = std::get<3>(returnVal);// (double)returnVal[3];
					if (i3 == 0 && j3 == 0)
						break;

					aPoint =  PointD();
					aPoint.X = a3x;
					aPoint.Y = a3y;
					pointList.push_back(aPoint);
					if (abs(a3y - sy) < 0.000001 && abs(a3x - sx) < 0.000001)
						break;

					a2x = a3x;
					a2y = a3y;
					i1 = i2;
					j1 = j2;
					i2 = i3;
					j2 = j3;
					if (i2 == m - 1 || j2 == n - 1)
						break;

				}
				H[i][j] = -2;
				if (pointList.size() > 4)
				{
					aLine =  PolyLine();
					aLine.Value = W;
					aLine.Type = "Close";
					aLine.PointList = pointList;
					//m_LineList.Add(aLine)
					cLineList.push_back(aLine);
				}
			}
		}
	}

	for (i = 1; i < m - 1; i++)
	{
		for (j = 1; j < n - 2; j++)
		{
			if (S[i][ j] != -2)
			{
				vector<PointD> pointList;;
				i2 = i;
				j2 = j;
				a2x = X[j2] + S[i][ j] * nx;
				a2y = Y[i];
				j1 = j2;
				i1 = 0;
				sx = a2x;
				sy = a2y;
				aPoint =  PointD();
				aPoint.X = a2x;
				aPoint.Y = a2y;
				pointList.push_back(aPoint);
				while (true)
				{
					returnVal = TraceIsoline(i1, i2,  H,  S, j1, j2, X, Y, nx, ny, a2x);
					
					i3 = std::get<0>(returnVal);// (int)returnVal[0];
					j3 = std::get<1>(returnVal);// (int)returnVal[1];
					a3x = std::get<2>(returnVal);// (double)returnVal[2];
					a3y = std::get<3>(returnVal);// (double)returnVal[3];
					aPoint =  PointD();
					aPoint.X = a3x;
					aPoint.Y = a3y;
					pointList.push_back(aPoint);
					if (abs(a3y - sy) < 0.000001 && abs(a3x - sx) < 0.000001)
						break;

					a2x = a3x;
					a2y = a3y;
					i1 = i2;
					j1 = j2;
					i2 = i3;
					j2 = j3;
					if (i2 == m - 1 || j2 == n - 1)
						break;
				}
				S[i][ j] = -2;
				if (pointList.size() > 4)
				{
					aLine =  PolyLine();
					aLine.Value = W;
					aLine.Type = "Close";
					aLine.PointList = pointList;
					//m_LineList.Add(aLine)
					cLineList.push_back(aLine);
				}
			}
		}
	}

	return cLineList;
}


vector<WPolygon> Contour::TracingPolygons(vector<PolyLine> LineList, vector<BorderPoint> borderList, Extent bBound, double* contour,int contoursNum)
{
	if (LineList.size() == 0)
		return vector<WPolygon>();

	vector<WPolygon> aPolygonList;
	//List<WPolygon> newPolygonlist = new List<WPolygon>();
	vector<PolyLine> aLineList;
	PolyLine aLine;
	PointD aPoint;
	WPolygon aPolygon;
	Extent aBound;
	int i, j;

	aLineList = LineList;

	//---- Tracing border polygon
	vector<PointD> aPList;
	vector<PointD> newPList;
	BorderPoint bP;
	int* timesArray = new int[borderList.size() - 1]();
	int timesArrayLength = borderList.size() - 1;
	for (i = 0; i < timesArrayLength; i++)
		timesArray[i] = 0;

	int pIdx, pNum, vNum;
	double aValue = 0, bValue = 0;
	vector<BorderPoint> lineBorderList;

	pNum = borderList.size() - 1;
	for (i = 0; i < pNum; i++)
	{
		if ((borderList[i]).Id == -1)
			continue;

		pIdx = i;
		aPList = vector<PointD>();
		lineBorderList.push_back(borderList[i]);

		//---- Clockwise traceing
		if (timesArray[pIdx] < 2)
		{
			aPList.push_back((borderList[pIdx]).Point);
			pIdx += 1;
			if (pIdx == pNum)
				pIdx = 0;

			vNum = 0;
			while (true)
			{
				bP = borderList[pIdx];
				if (bP.Id == -1)    //---- Not endpoint of contour
				{
					if (timesArray[pIdx] == 1)
						break;

					aPList.push_back(bP.Point);
					timesArray[pIdx] += +1;
				}
				else    //---- endpoint of contour
				{
					if (timesArray[pIdx] == 2)
						break;

					timesArray[pIdx] += +1;
					aLine = aLineList[bP.Id];
					if (vNum == 0)
					{
						aValue = aLine.Value;
						bValue = aLine.Value;
						vNum += 1;
					}
					else
					{
						if (aValue == bValue)
						{
							if (aLine.Value > aValue)
								bValue = aLine.Value;
							else if (aLine.Value < aValue)
								aValue = aLine.Value;

							vNum += 1;
						}
					}
					newPList = vector<PointD>(aLine.PointList);
					aPoint = newPList[0];
					//If Not (Math.Abs(bP.point.X - aPoint.X) < 0.000001 And _
					//  Math.Abs(bP.point.Y - aPoint.Y) < 0.000001) Then    '---- Start point
					if (!(bP.Point.X == aPoint.X && bP.Point.Y == aPoint.Y))    //---- Start point
						std::reverse(newPList.begin(), newPList.end());
					//newPList.Reverse();

				//aPList.AddRange(newPList);
					aPList.insert(aPList.end(), newPList.begin(), newPList.end());
					for (j = 0; j < borderList.size() - 1; j++)
					{
						if (j != pIdx)
						{
							if ((borderList[j]).Id == bP.Id)
							{
								pIdx = j;
								timesArray[pIdx] += +1;
								break;
							}
						}
					}
				}

				if (pIdx == i)
				{
					if (aPList.size() > 0)
					{
						aPolygon = WPolygon();
						aPolygon.LowValue = aValue;
						aPolygon.HighValue = bValue;
						aBound = Extent();
						aPolygon.Area = GetExtentAndArea(aPList, aBound);
						aPolygon.IsClockWise = true;
						aPolygon.StartPointIdx = lineBorderList.size() - 1;
						aPolygon.Extent = aBound;
						aPolygon.OutLine.PointList = aPList;
						aPolygon.OutLine.Value = aValue;
						aPolygon.IsHighCenter = true;
						aPolygon.OutLine.Type = "Border";
						aPolygonList.push_back(aPolygon);
					}
					break;
				}
				pIdx += 1;
				if (pIdx == pNum)
					pIdx = 0;

			}
		}


		//---- Anticlockwise traceing
		pIdx = i;
		if (timesArray[pIdx] < 2)
		{
			aPList = vector<PointD>();
			aPList.push_back((borderList[pIdx]).Point);
			pIdx += -1;
			if (pIdx == -1)
				pIdx = pNum - 1;

			vNum = 0;
			while (true)
			{
				bP = borderList[pIdx];
				if (bP.Id == -1)    //---- Not endpoint of contour
				{
					if (timesArray[pIdx] == 1)
						break;
					aPList.push_back(bP.Point);
					timesArray[pIdx] += +1;
				}
				else    //---- endpoint of contour
				{
					if (timesArray[pIdx] == 2)
						break;

					timesArray[pIdx] += +1;
					aLine = aLineList[bP.Id];
					if (vNum == 0)
					{
						aValue = aLine.Value;
						bValue = aLine.Value;
						vNum += 1;
					}
					else
					{
						if (aValue == bValue)
						{
							if (aLine.Value > aValue)
								bValue = aLine.Value;
							else if (aLine.Value < aValue)
								aValue = aLine.Value;

							vNum += 1;
						}
					}
					newPList = vector<PointD>(aLine.PointList);
					aPoint = newPList[0];
					//If Not (Math.Abs(bP.point.X - aPoint.X) < 0.000001 And _
					//  Math.Abs(bP.point.Y - aPoint.Y) < 0.000001) Then    '---- Start point
					if (!(bP.Point.X == aPoint.X && bP.Point.Y == aPoint.Y))    //---- Start point
						std::reverse(newPList.begin(), newPList.end());
					//newPList.Reverse();
					aPList.insert(aPList.end(), newPList.begin(), newPList.end());
					//aPList.AddRange(newPList);
					for (j = 0; j < borderList.size() - 1; j++)
					{
						if (j != pIdx)
						{
							if ((borderList[j]).Id == bP.Id)
							{
								pIdx = j;
								timesArray[pIdx] += +1;
								break;
							}
						}
					}
				}

				if (pIdx == i)
				{
					if (aPList.size() > 0)
					{
						aPolygon = WPolygon();
						aPolygon.LowValue = aValue;
						aPolygon.HighValue = bValue;
						aBound = Extent();
						aPolygon.Area = GetExtentAndArea(aPList, aBound);
						aPolygon.IsClockWise = false;
						aPolygon.StartPointIdx = lineBorderList.size() - 1;
						aPolygon.Extent = aBound;
						aPolygon.OutLine.PointList = aPList;
						aPolygon.OutLine.Value = aValue;
						aPolygon.IsHighCenter = true;
						aPolygon.OutLine.Type = "Border";
						aPolygonList.push_back(aPolygon);
					}
					break;
				}
				pIdx += -1;
				if (pIdx == -1)
					pIdx = pNum - 1;

			}
		}
	}

	//---- tracing close polygons
	vector<WPolygon> cPolygonlist;
	bool isInserted;
	for (i = 0; i < aLineList.size(); i++)
	{
		aLine = aLineList[i];
		if (aLine.Type == "Close" && aLine.PointList.size() > 0)
		{
			aPolygon = WPolygon();
			aPolygon.LowValue = aLine.Value;
			aPolygon.HighValue = aLine.Value;
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			aPolygon.IsHighCenter = true;

			//---- Sort from big to small
			isInserted = false;
			for (j = 0; j < cPolygonlist.size(); j++)
			{
				if (aPolygon.Area > (cPolygonlist[j]).Area)
				{
					//cPolygonlist.Insert(j, aPolygon);
					cPolygonlist.insert(cPolygonlist.begin() + j, aPolygon);
					isInserted = true;
					break;
				}
			}
			if (!isInserted)
				cPolygonlist.push_back(aPolygon);

		}
	}

	//---- Juge isHighCenter for border polygons
	Extent cBound1, cBound2;
	if (aPolygonList.size() > 0)
	{
		int outPIdx;
		bool IsSides = true;
		bool IfSameValue = false;    //---- If all boder polygon lines have same value
		aPolygon = aPolygonList[0];
		if (aPolygon.LowValue == aPolygon.HighValue)
		{
			IsSides = false;
			outPIdx = aPolygon.StartPointIdx;
			while (true)
			{
				if (aPolygon.IsClockWise)
				{
					outPIdx = outPIdx - 1;
					if (outPIdx == -1)
						outPIdx = lineBorderList.size() - 1;

				}
				else
				{
					outPIdx = outPIdx + 1;
					if (outPIdx == lineBorderList.size())
						outPIdx = 0;

				}
				bP = lineBorderList[outPIdx];
				aLine = aLineList[bP.Id];
				if (aLine.Value == aPolygon.LowValue)
				{
					if (outPIdx == aPolygon.StartPointIdx)
					{
						IfSameValue = true;
						break;
					}
					else
						continue;

				}
				else
				{
					IfSameValue = false;
					break;
				}
			}
		}

		if (IfSameValue)
		{
			if (cPolygonlist.size() > 0)
			{
				WPolygon cPolygon;
				cPolygon = cPolygonlist[0];
				cBound1 = cPolygon.Extent;
				for (i = 0; i < aPolygonList.size(); i++)
				{
					aPolygon = aPolygonList[i];
					cBound2 = aPolygon.Extent;
					if (cBound1.xMin > cBound2.xMin && cBound1.yMin > cBound2.yMin &&
						cBound1.xMax < cBound2.xMax && cBound1.yMax < cBound2.yMax)
					{
						aPolygon.IsHighCenter = false;
					}
					else
					{
						aPolygon.IsHighCenter = true;
					}
					//aPolygonList[i] = aPolygon;
				}
			}
			else
			{
				bool tf = true;    //---- Temperal solution, not finished
				for (i = 0; i < aPolygonList.size(); i++)
				{
					aPolygon = aPolygonList[i];
					tf = !tf;
					aPolygon.IsHighCenter = tf;
					//aPolygonList[i] = aPolygon;                            
				}
			}
		}
		else
		{
			for (i = 0; i < aPolygonList.size(); i++)
			{
				aPolygon = aPolygonList[i];
				if (aPolygon.LowValue == aPolygon.HighValue)
				{
					IsSides = false;
					outPIdx = aPolygon.StartPointIdx;
					while (true)
					{
						if (aPolygon.IsClockWise)
						{
							outPIdx = outPIdx - 1;
							if (outPIdx == -1)
								outPIdx = lineBorderList.size() - 1;
						}
						else
						{
							outPIdx = outPIdx + 1;
							if (outPIdx == lineBorderList.size())
								outPIdx = 0;

						}
						bP = lineBorderList[outPIdx];
						aLine = aLineList[bP.Id];
						if (aLine.Value == aPolygon.LowValue)
						{
							if (outPIdx == aPolygon.StartPointIdx)
								break;
							else
							{
								IsSides = !IsSides;
								continue;
							}
						}
						else
						{
							if (IsSides)
							{
								if (aLine.Value < aPolygon.LowValue)
								{
									aPolygon.IsHighCenter = false;
									//aPolygonList.Insert(i, aPolygon);
									//aPolygonList.RemoveAt(i + 1);
								}
							}
							else
							{
								if (aLine.Value > aPolygon.LowValue)
								{
									aPolygon.IsHighCenter = false;
									//aPolygonList.Insert(i, aPolygon);
									//aPolygonList.RemoveAt(i + 1);
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	else    //Add border polygon
	{
		//Get max & min contour values
		double max = aLineList[0].Value, min = aLineList[0].Value;
		for (PolyLine aPLine : aLineList)
		{
			if (aPLine.Value > max)
				max = aPLine.Value;
			if (aPLine.Value < min)
				min = aPLine.Value;
		}
		aPolygon = WPolygon();
		aLine = PolyLine();
		aLine.Type = "Border";
		aLine.Value = contour[0];
		aPolygon.IsHighCenter = false;
		if (cPolygonlist.size() > 0)
		{
			if ((cPolygonlist[0].LowValue == max))
			{
				int contourLength = contoursNum;
				aLine.Value = contour[contourLength - 1];
				aPolygon.IsHighCenter = true;
			}
		}
		newPList.clear();
		aPoint = PointD();
		aPoint.X = bBound.xMin;
		aPoint.Y = bBound.yMin;
		newPList.push_back(aPoint);
		aPoint = PointD();
		aPoint.X = bBound.xMin;
		aPoint.Y = bBound.yMax;
		newPList.push_back(aPoint);
		aPoint = PointD();
		aPoint.X = bBound.xMax;
		aPoint.Y = bBound.yMax;
		newPList.push_back(aPoint);
		aPoint = PointD();
		aPoint.X = bBound.xMax;
		aPoint.Y = bBound.yMin;
		newPList.push_back(aPoint);
		newPList.push_back(newPList[0]);
		aLine.PointList = vector<PointD>(newPList);

		if (aLine.PointList.size() > 0)
		{
			aPolygon.LowValue = aLine.Value;
			aPolygon.HighValue = aLine.Value;
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			//aPolygon.IsHighCenter = false;
			aPolygonList.push_back(aPolygon);
		}
	}

	//---- Add close polygons to form total polygons list
	//aPolygonList.AddRange(cPolygonlist);
	aPolygonList.insert(aPolygonList.end(), cPolygonlist.begin(), cPolygonlist.end());

	//---- Juge IsHighCenter for close polygons
	int polygonNum = aPolygonList.size();
	WPolygon bPolygon;
	for (i = polygonNum - 1; i >= 0; i--)
	{
		aPolygon = aPolygonList[i];
		if (aPolygon.OutLine.Type == "Close")
		{
			cBound1 = aPolygon.Extent;
			aValue = aPolygon.LowValue;
			aPoint = aPolygon.OutLine.PointList[0];
			for (j = i - 1; j >= 0; j--)
			{
				bPolygon = aPolygonList[j];
				cBound2 = bPolygon.Extent;
				bValue = bPolygon.LowValue;
				newPList = vector<PointD>(bPolygon.OutLine.PointList);
				if (PointInPolygon(newPList, aPoint))
				{
					if (cBound1.xMin > cBound2.xMin && cBound1.yMin > cBound2.yMin &&
						cBound1.xMax < cBound2.xMax && cBound1.yMax < cBound2.yMax)
					{
						if (aValue < bValue)
						{
							aPolygon.IsHighCenter = false;
							//aPolygonList[i] = aPolygon;                                    
						}
						else if (aValue == bValue)
						{
							if (bPolygon.IsHighCenter)
							{
								aPolygon.IsHighCenter = false;
								//aPolygonList[i] = aPolygon;
							}
						}
						break;
					}
				}
			}
		}
	}

	delete[] timesArray;

	return aPolygonList;
}

vector<WPolygon> Contour::TracingPolygons(vector<PolyLine> LineList, vector<BorderPoint> borderList)
{
	if (LineList.size() == 0)
		return vector<WPolygon>();

	vector<WPolygon> aPolygonList;
	vector<PolyLine> aLineList;
	PolyLine aLine;
	PointD aPoint;
	WPolygon aPolygon;
	Extent aBound;
	int i, j;

	aLineList = LineList;

	//---- Tracing border polygon
	vector<PointD> aPList;
	vector<PointD> newPList;
	BorderPoint bP;
	int* timesArray = new int[borderList.size() - 1]();
	int timesArrayLength = borderList.size() - 1;
	for (i = 0; i < timesArrayLength; i++)
		timesArray[i] = 0;

	int pIdx, pNum, vNum, cvNum;
	double aValue = 0, bValue = 0, cValue = 0;
	vector<BorderPoint> lineBorderList;

	pNum = borderList.size() - 1;
	for (i = 0; i < pNum; i++)
	{
		if ((borderList[i]).Id == -1)
			continue;

		pIdx = i;
		aPList = vector<PointD>();
		lineBorderList.push_back(borderList[i]);

		//---- Clockwise traceing
		if (timesArray[pIdx] < 2)
		{
			aPList.push_back((borderList[pIdx]).Point);
			pIdx += 1;
			if (pIdx == pNum)
				pIdx = 0;

			vNum = 0;
			cvNum = 0;
			while (true)
			{
				bP = borderList[pIdx];
				if (bP.Id == -1)    //---- Not endpoint of contour
				{
					if (timesArray[pIdx] == 1)
						break;

					if (cvNum < 5)
						cValue = bP.Value;
					cvNum += 1;
					aPList.push_back(bP.Point);
					timesArray[pIdx] += +1;
				}
				else    //---- endpoint of contour
				{
					if (timesArray[pIdx] == 2)
						break;

					timesArray[pIdx] += +1;
					aLine = aLineList[bP.Id];
					if (vNum == 0)
					{
						aValue = aLine.Value;
						bValue = aLine.Value;
						vNum += 1;
					}
					else
					{
						if (aValue == bValue)
						{
							if (aLine.Value > aValue)
								bValue = aLine.Value;
							else if (aLine.Value < aValue)
								aValue = aLine.Value;

							vNum += 1;
						}
					}
					newPList = vector<PointD>(aLine.PointList);
					aPoint = newPList[0];
					//If Not (Math.Abs(bP.point.X - aPoint.X) < 0.000001 And _
					//  Math.Abs(bP.point.Y - aPoint.Y) < 0.000001) Then    '---- Start point
					if (!(bP.Point.X == aPoint.X && bP.Point.Y == aPoint.Y))    //---- Start point
						std::reverse(newPList.begin(), newPList.end());
					//newPList.Reverse();

				//aPList.AddRange(newPList);
					aPList.insert(aPList.end(), newPList.begin(), newPList.end());
					for (j = 0; j < borderList.size() - 1; j++)
					{
						if (j != pIdx)
						{
							if ((borderList[j]).Id == bP.Id)
							{
								pIdx = j;
								timesArray[pIdx] += +1;
								break;
							}
						}
					}
				}

				if (pIdx == i)
				{
					if (aPList.size() > 0)
					{
						aPolygon = WPolygon();
						aPolygon.IsBorder = true;
						aPolygon.LowValue = aValue;
						aPolygon.HighValue = bValue;
						aBound = Extent();
						aPolygon.Area = GetExtentAndArea(aPList, aBound);
						aPolygon.IsClockWise = true;
						aPolygon.StartPointIdx = lineBorderList.size() - 1;
						aPolygon.Extent = aBound;
						aPolygon.OutLine.PointList = aPList;
						aPolygon.OutLine.Value = aValue;
						aPolygon.IsHighCenter = true;
						aPolygon.HoleLines = vector<PolyLine>();
						if (aValue == bValue)
						{
							if (cValue < aValue)
								aPolygon.IsHighCenter = false;
						}
						aPolygon.OutLine.Type = "Border";
						aPolygonList.push_back(aPolygon);
					}
					break;
				}
				pIdx += 1;
				if (pIdx == pNum)
					pIdx = 0;

			}
		}


		//---- Anticlockwise traceing
		pIdx = i;
		if (timesArray[pIdx] < 2)
		{
			aPList = vector<PointD>();
			aPList.push_back((borderList[pIdx]).Point);
			pIdx += -1;
			if (pIdx == -1)
				pIdx = pNum - 1;

			vNum = 0;
			cvNum = 0;
			while (true)
			{
				bP = borderList[pIdx];
				if (bP.Id == -1)    //---- Not endpoint of contour
				{
					if (timesArray[pIdx] == 1)
						break;

					if (cvNum < 5)
						cValue = bP.Value;
					cvNum += 1;
					aPList.push_back(bP.Point);
					timesArray[pIdx] += +1;
				}
				else    //---- endpoint of contour
				{
					if (timesArray[pIdx] == 2)
						break;

					timesArray[pIdx] += +1;
					aLine = aLineList[bP.Id];
					if (vNum == 0)
					{
						aValue = aLine.Value;
						bValue = aLine.Value;
						vNum += 1;
					}
					else
					{
						if (aValue == bValue)
						{
							if (aLine.Value > aValue)
								bValue = aLine.Value;
							else if (aLine.Value < aValue)
								aValue = aLine.Value;

							vNum += 1;
						}
					}
					newPList = vector<PointD>(aLine.PointList);
					aPoint = newPList[0];
					//If Not (Math.Abs(bP.point.X - aPoint.X) < 0.000001 And _
					//  Math.Abs(bP.point.Y - aPoint.Y) < 0.000001) Then    '---- Start point
					if (!(bP.Point.X == aPoint.X && bP.Point.Y == aPoint.Y))    //---- Start point
						std::reverse(newPList.begin(), newPList.end());
					//newPList.Reverse();

				//aPList.AddRange(newPList);
					aPList.insert(aPList.end(), newPList.begin(), newPList.end());
					for (j = 0; j < borderList.size() - 1; j++)
					{
						if (j != pIdx)
						{
							if ((borderList[j]).Id == bP.Id)
							{
								pIdx = j;
								timesArray[pIdx] += +1;
								break;
							}
						}
					}
				}

				if (pIdx == i)
				{
					if (aPList.size() > 0)
					{
						aPolygon = WPolygon();
						aPolygon.IsBorder = true;
						aPolygon.LowValue = aValue;
						aPolygon.HighValue = bValue;
						aBound = Extent();
						aPolygon.Area = GetExtentAndArea(aPList, aBound);
						aPolygon.IsClockWise = false;
						aPolygon.StartPointIdx = lineBorderList.size() - 1;
						aPolygon.Extent = aBound;
						aPolygon.OutLine.PointList = aPList;
						aPolygon.OutLine.Value = aValue;
						aPolygon.IsHighCenter = true;
						aPolygon.HoleLines = vector<PolyLine>();
						if (aValue == bValue)
						{
							if (cValue < aValue)
								aPolygon.IsHighCenter = false;
						}
						aPolygon.OutLine.Type = "Border";
						aPolygonList.push_back(aPolygon);
					}
					break;
				}
				pIdx += -1;
				if (pIdx == -1)
					pIdx = pNum - 1;

			}
		}
	}

	//---- tracing close polygons
	vector<WPolygon> cPolygonlist;
	bool isInserted;
	for (i = 0; i < aLineList.size(); i++)
	{
		aLine = aLineList[i];
		if (aLine.Type == "Close" && aLine.PointList.size() > 0)
		{
			aPolygon = WPolygon();
			aPolygon.IsBorder = false;
			aPolygon.LowValue = aLine.Value;
			aPolygon.HighValue = aLine.Value;
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			aPolygon.IsHighCenter = true;
			aPolygon.HoleLines = vector<PolyLine>();

			//---- Sort from big to small
			isInserted = false;
			for (j = 0; j < cPolygonlist.size(); j++)
			{
				if (aPolygon.Area > (cPolygonlist[j]).Area)
				{
					//cPolygonlist.Insert(j, aPolygon);
					cPolygonlist.insert(cPolygonlist.begin() + j, aPolygon);
					isInserted = true;
					break;
				}
			}
			if (!isInserted)
				cPolygonlist.push_back(aPolygon);

		}
	}

	//---- Juge isHighCenter for border polygons
	aPolygonList = JudgePolygonHighCenter(aPolygonList, cPolygonlist, aLineList, borderList);

	delete[] timesArray;

	return aPolygonList;
}

vector<WPolygon> Contour::TracingClipPolygons(WPolygon inPolygon, vector<PolyLine> LineList, vector<BorderPoint> borderList)
{
	if (LineList.size() == 0)
		return vector<WPolygon>();

	vector<WPolygon> aPolygonList;
	vector<PolyLine> aLineList;
	PolyLine aLine;
	PointD aPoint;
	WPolygon aPolygon;
	Extent aBound;
	int i, j;

	aLineList = LineList;

	//---- Tracing border polygon
	vector<PointD> aPList;
	vector<PointD> newPList;
	BorderPoint bP;
	int* timesArray = new int[borderList.size() - 1]();
	int timesArrayLength = borderList.size() - 1;
	for (i = 0; i < timesArrayLength; i++)
		timesArray[i] = 0;

	int pIdx, pNum;
	vector<BorderPoint> lineBorderList;

	pNum = borderList.size() - 1;
	PointD bPoint, b1Point;
	for (i = 0; i < pNum; i++)
	{
		if ((borderList[i]).Id == -1)
			continue;

		pIdx = i;
		lineBorderList.push_back(borderList[i]);
		bP = borderList[pIdx];
		b1Point = borderList[pIdx].Point;

		//---- Clockwise traceing
		if (timesArray[pIdx] < 1)
		{
			aPList = vector<PointD>();
			aPList.push_back((borderList[pIdx]).Point);
			pIdx += 1;
			if (pIdx == pNum)
				pIdx = 0;

			bPoint = (PointD)borderList[pIdx].Point.Clone();
			if (borderList[pIdx].Id == -1)
			{
				int aIdx = pIdx + 10;
				for (int o = 1; o <= 10; o++)
				{
					if (borderList[pIdx + o].Id > -1)
					{
						aIdx = pIdx + o - 1;
						break;
					}
				}
				bPoint = (PointD)borderList[aIdx].Point.Clone();
			}
			else
			{
				bPoint.X = (bPoint.X + b1Point.X) / 2;
				bPoint.Y = (bPoint.Y + b1Point.Y) / 2;
			}
			if (PointInPolygon(inPolygon, bPoint))
			{
				while (true)
				{
					bP = borderList[pIdx];
					if (bP.Id == -1)    //---- Not endpoint of contour
					{
						if (timesArray[pIdx] == 1)
							break;

						aPList.push_back(bP.Point);
						timesArray[pIdx] += 1;
					}
					else    //---- endpoint of contour
					{
						if (timesArray[pIdx] == 1)
							break;

						timesArray[pIdx] += 1;
						aLine = aLineList[bP.Id];

						newPList = vector<PointD>(aLine.PointList);
						aPoint = newPList[0];

						if (!(DoubleEquals(bP.Point.X, aPoint.X) && DoubleEquals(bP.Point.Y, aPoint.Y)))    //---- Start point
							//if (!IsClockwise(newPList))
							//newPList.Reverse();
							std::reverse(newPList.begin(), newPList.end());

						//aPList.AddRange(newPList);
						aPList.insert(aPList.end(), newPList.begin(), newPList.end());
						for (j = 0; j < borderList.size() - 1; j++)
						{
							if (j != pIdx)
							{
								if ((borderList[j]).Id == bP.Id)
								{
									pIdx = j;
									timesArray[pIdx] += 1;
									break;
								}
							}
						}
					}

					if (pIdx == i)
					{
						if (aPList.size() > 0)
						{
							aPolygon = WPolygon();
							aPolygon.IsBorder = true;
							aPolygon.LowValue = inPolygon.LowValue;
							aPolygon.HighValue = inPolygon.HighValue;
							aBound = Extent();
							aPolygon.Area = GetExtentAndArea(aPList, aBound);
							aPolygon.IsClockWise = true;
							aPolygon.StartPointIdx = lineBorderList.size() - 1;
							aPolygon.Extent = aBound;
							aPolygon.OutLine.PointList = aPList;
							aPolygon.OutLine.Value = inPolygon.LowValue;
							aPolygon.IsHighCenter = inPolygon.IsHighCenter;
							aPolygon.OutLine.Type = "Border";
							aPolygon.HoleLines = vector<PolyLine>();
							aPolygonList.push_back(aPolygon);
						}
						break;
					}
					pIdx += 1;
					if (pIdx == pNum)
						pIdx = 0;
				}
			}
		}

		//---- Anticlockwise traceing
		pIdx = i;
		if (timesArray[pIdx] < 1)
		{
			aPList = vector<PointD>();
			aPList.push_back((borderList[pIdx]).Point);
			pIdx += -1;
			if (pIdx == -1)
				pIdx = pNum - 1;

			bPoint = (PointD)borderList[pIdx].Point.Clone();
			if (borderList[pIdx].Id == -1)
			{
				int aIdx = pIdx + 10;
				for (int o = 1; o <= 10; o++)
				{
					if (borderList[pIdx + o].Id > -1)
					{
						aIdx = pIdx + o - 1;
						break;
					}
				}
				bPoint = (PointD)borderList[aIdx].Point.Clone();
			}
			else
			{
				bPoint.X = (bPoint.X + b1Point.X) / 2;
				bPoint.Y = (bPoint.Y + b1Point.Y) / 2;
			}
			if (PointInPolygon(inPolygon, bPoint))
			{
				while (true)
				{
					bP = borderList[pIdx];
					if (bP.Id == -1)    //---- Not endpoint of contour
					{
						if (timesArray[pIdx] == 1)
							break;

						aPList.push_back(bP.Point);
						timesArray[pIdx] += 1;
					}
					else    //---- endpoint of contour
					{
						if (timesArray[pIdx] == 1)
							break;

						timesArray[pIdx] += 1;
						aLine = aLineList[bP.Id];

						newPList = vector<PointD>(aLine.PointList);
						aPoint = newPList[0];

						if (!(DoubleEquals(bP.Point.X, aPoint.X) && DoubleEquals(bP.Point.Y, aPoint.Y)))    //---- Start point
							//if (IsClockwise(newPList))
							//newPList.Reverse();
							std::reverse(newPList.begin(), newPList.end());

						//aPList.AddRange(newPList);
						aPList.insert(aPList.end(), newPList.begin(), newPList.end());
						for (j = 0; j < borderList.size() - 1; j++)
						{
							if (j != pIdx)
							{
								if ((borderList[j]).Id == bP.Id)
								{
									pIdx = j;
									timesArray[pIdx] += 1;
									break;
								}
							}
						}
					}

					if (pIdx == i)
					{
						if (aPList.size() > 0)
						{
							aPolygon = WPolygon();
							aPolygon.IsBorder = true;
							aPolygon.LowValue = inPolygon.LowValue;
							aPolygon.HighValue = inPolygon.HighValue;
							aBound = Extent();
							aPolygon.Area = GetExtentAndArea(aPList, aBound);
							aPolygon.IsClockWise = false;
							aPolygon.StartPointIdx = lineBorderList.size() - 1;
							aPolygon.Extent = aBound;
							aPolygon.OutLine.PointList = aPList;
							aPolygon.OutLine.Value = inPolygon.LowValue;
							aPolygon.IsHighCenter = inPolygon.IsHighCenter;
							aPolygon.OutLine.Type = "Border";
							aPolygon.HoleLines = vector<PolyLine>();
							aPolygonList.push_back(aPolygon);
						}
						break;
					}
					pIdx += -1;
					if (pIdx == -1)
						pIdx = pNum - 1;

				}
			}
		}
	}

	delete[] timesArray;

	return aPolygonList;
}

vector<WPolygon> Contour::JudgePolygonHighCenter(vector<WPolygon> borderPolygons, vector<WPolygon> closedPolygons,
	vector<PolyLine> aLineList, vector<BorderPoint> borderList)
{
	int i, j;
	WPolygon aPolygon;
	PolyLine aLine;
	vector<PointD> newPList;
	Extent aBound;
	double aValue = 0;
	double bValue = 0;
	PointD aPoint;

	if (borderPolygons.size() == 0)    //Add border polygon
	{

		//Get max & min contour values
		double max = aLineList[0].Value, min = aLineList[0].Value;
		for (PolyLine aPLine : aLineList)
		{
			if (aPLine.Value > max)
				max = aPLine.Value;
			if (aPLine.Value < min)
				min = aPLine.Value;
		}
		aPolygon = WPolygon();
		aLine = PolyLine();
		aLine.Type = "Border";
		aLine.Value = min;
		newPList.clear();
		for (BorderPoint aP : borderList)
		{
			newPList.push_back(aP.Point);
		}
		aLine.PointList = vector<PointD>(newPList);
		if (aLine.PointList.size() > 0)
		{
			aPolygon.IsBorder = true;
			aPolygon.LowValue = min;
			aPolygon.HighValue = max;
			if (max > min)
			{
				aPolygon.IsHighCenter = true;
			}
			else
			{
				if (borderList[0].Value > min)
				{
					aPolygon.IsHighCenter = true;
				}
				else
				{
					aPolygon.IsHighCenter = false;
				}
			}
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			aPolygon.HoleLines = vector<PolyLine>();
			borderPolygons.push_back(aPolygon);
		}
	}

	//---- Add close polygons to form total polygons list
	//borderPolygons.AddRange(closedPolygons);
	borderPolygons.insert(borderPolygons.end(), closedPolygons.begin(), closedPolygons.end());

	//---- Juge IsHighCenter for close polygons
	Extent cBound1, cBound2;
	int polygonNum = borderPolygons.size();
	WPolygon bPolygon;
	for (i = 1; i < polygonNum; i++)
	{
		aPolygon = borderPolygons[i];
		if (aPolygon.OutLine.Type == "Close")
		{
			cBound1 = aPolygon.Extent;
			aValue = aPolygon.LowValue;
			aPoint = aPolygon.OutLine.PointList[0];
			for (j = i - 1; j >= 0; j--)
			{
				bPolygon = borderPolygons[j];
				cBound2 = bPolygon.Extent;
				bValue = bPolygon.LowValue;
				newPList = vector<PointD>(bPolygon.OutLine.PointList);
				if (PointInPolygon(newPList, aPoint))
				{
					if (cBound1.xMin > cBound2.xMin && cBound1.yMin > cBound2.yMin &&
						cBound1.xMax < cBound2.xMax && cBound1.yMax < cBound2.yMax)
					{
						if (aValue < bValue)
						{
							aPolygon.IsHighCenter = false;
							//borderPolygons[i] = aPolygon;
						}
						else if (aValue == bValue)
						{
							if (bPolygon.IsHighCenter)
							{
								aPolygon.IsHighCenter = false;
								//borderPolygons[i] = aPolygon;
							}
						}
						break;
					}
				}
			}
		}
	}

	return borderPolygons;
}

vector<WPolygon> Contour::JudgePolygonHighCenter_old(vector<WPolygon> borderPolygons, vector<WPolygon> closedPolygons,
	vector<PolyLine> aLineList, vector<BorderPoint> borderList)
{
	int i, j;
	WPolygon aPolygon;
	PolyLine aLine;
	vector<PointD> newPList;
	Extent aBound;
	double aValue = 0;
	double bValue = 0;
	PointD aPoint;

	if (borderPolygons.size() == 0)    //Add border polygon
	{

		//Get max & min contour values
		double max = aLineList[0].Value, min = aLineList[0].Value;
		for (PolyLine aPLine : aLineList)
		{
			if (aPLine.Value > max)
				max = aPLine.Value;
			if (aPLine.Value < min)
				min = aPLine.Value;
		}
		aPolygon = WPolygon();
		aLine = PolyLine();
		aLine.Type = "Border";
		aLine.Value = min;
		aPolygon.IsHighCenter = false;
		if (closedPolygons.size() > 0)
		{
			if (borderList[0].Value >= closedPolygons[0].LowValue)
			{
				aLine.Value = max;
				aPolygon.IsHighCenter = true;
			}
		}
		newPList.clear();
		for (BorderPoint aP : borderList)
		{
			newPList.push_back(aP.Point);
		}
		aLine.PointList = vector<PointD>(newPList);

		if (aLine.PointList.size() > 0)
		{
			aPolygon.IsBorder = true;
			aPolygon.LowValue = aLine.Value;
			aPolygon.HighValue = aLine.Value;
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			aPolygon.HoleLines = vector<PolyLine>();
			//aPolygon.IsHighCenter = false;
			borderPolygons.push_back(aPolygon);
		}
	}

	//---- Add close polygons to form total polygons list
	//borderPolygons.AddRange(closedPolygons);
	borderPolygons.insert(borderPolygons.end(), closedPolygons.begin(), closedPolygons.end());

	//---- Juge IsHighCenter for close polygons
	Extent cBound1, cBound2;
	int polygonNum = borderPolygons.size();
	WPolygon bPolygon;
	for (i = 1; i < polygonNum; i++)
	{
		aPolygon = borderPolygons[i];
		if (aPolygon.OutLine.Type == "Close")
		{
			cBound1 = aPolygon.Extent;
			aValue = aPolygon.LowValue;
			aPoint = aPolygon.OutLine.PointList[0];
			for (j = i - 1; j >= 0; j--)
			{
				bPolygon = borderPolygons[j];
				cBound2 = bPolygon.Extent;
				bValue = bPolygon.LowValue;
				newPList = vector<PointD>(bPolygon.OutLine.PointList);
				if (PointInPolygon(newPList, aPoint))
				{
					if (cBound1.xMin > cBound2.xMin && cBound1.yMin > cBound2.yMin &&
						cBound1.xMax < cBound2.xMax && cBound1.yMax < cBound2.yMax)
					{
						if (aValue < bValue)
						{
							aPolygon.IsHighCenter = false;
							//borderPolygons[i] = aPolygon;
						}
						else if (aValue == bValue)
						{
							if (bPolygon.IsHighCenter)
							{
								aPolygon.IsHighCenter = false;
								//borderPolygons[i] = aPolygon;
							}
						}
						break;
					}
				}
			}
		}
	}

	return borderPolygons;
}


vector<WPolygon> Contour::TracingPolygons_Ring(vector<PolyLine> LineList, vector<BorderPoint> borderList, Border aBorder,
	double* contour, int* pNums)
{
	vector<WPolygon> aPolygonList;
	vector<PolyLine> aLineList;
	PolyLine aLine;
	PointD aPoint;
	WPolygon aPolygon;
	Extent aBound;
	int i = 0;
	int j = 0;

	aLineList = LineList;

	//---- Tracing border polygon
	vector<PointD> aPList;
	vector<PointD> newPList;
	BorderPoint bP;
	BorderPoint bP1;
	int* timesArray = new int[borderList.size()]();
	for (i = 0; i < borderList.size(); i++)
	{
		timesArray[i] = 0;
	}
	int pIdx = 0;
	int pNum = 0;
	int vNum = 0;
	double aValue = 0;
	double bValue = 0;
	double cValue = 0;
	vector<BorderPoint> lineBorderList;
	int borderIdx1 = 0;
	int borderIdx2 = 0;
	int innerIdx = 0;

	pNum = borderList.size();
	for (i = 0; i < pNum; i++)
	{
		if ((borderList[i]).Id == -1)
		{
			continue;
		}
		pIdx = i;
		lineBorderList.push_back(borderList[i]);

		bool sameBorderIdx = false;    //The two end points of the contour line are on same inner border
		//---- Clockwise traceing
		if (timesArray[pIdx] < 2)
		{
			bP = borderList[pIdx];
			innerIdx = bP.BInnerIdx;
			aPList = vector<PointD>();
			vector<int> bIdxList;
			aPList.push_back(bP.Point);
			bIdxList.push_back(pIdx);
			borderIdx1 = bP.BorderIdx;
			borderIdx2 = borderIdx1;
			pIdx += 1;
			innerIdx += 1;
			//If pIdx = pNum Then
			//    pIdx = 0
			//End If
			if (innerIdx == pNums[borderIdx1] - 1)
			{
				pIdx = pIdx - (pNums[borderIdx1] - 1);
			}
			vNum = 0;
			do
			{
				bP = borderList[pIdx];
				//---- Not endpoint of contour
				if (bP.Id == -1)
				{
					if (timesArray[pIdx] == 1)
					{
						break;
					}
					cValue = bP.Value;
					aPList.push_back(bP.Point);
					timesArray[pIdx] += 1;
					bIdxList.push_back(pIdx);
					//---- endpoint of contour
				}
				else
				{
					if (timesArray[pIdx] == 2)
					{
						break;
					}
					timesArray[pIdx] += 1;
					bIdxList.push_back(pIdx);
					aLine = aLineList[bP.Id];
					//---- Set high and low value of the polygon
					if (vNum == 0)
					{
						aValue = aLine.Value;
						bValue = aLine.Value;
						vNum += 1;
					}
					else
					{
						if (aValue == bValue)
						{
							if (aLine.Value > aValue)
							{
								bValue = aLine.Value;
							}
							else if (aLine.Value < aValue)
							{
								aValue = aLine.Value;
							}
							vNum += 1;
						}
					}
					newPList = aLine.PointList;
					aPoint = newPList[0];
					//If Not (Math.Abs(bP.point.x - aPoint.x) < 0.000001 And _
					//  Math.Abs(bP.point.y - aPoint.y) < 0.000001) Then    '---- Not start point
					//---- Not start point
					if (!(bP.Point.X == aPoint.X && bP.Point.Y == aPoint.Y))
					{
						//newPList.Reverse();
						std::reverse(newPList.begin(), newPList.end());
					}
					//aPList.AddRange(newPList);
					aPList.insert(aPList.end(), newPList.begin(), newPList.end());
					//---- Find corresponding border point
					for (j = 0; j < borderList.size(); j++)
					{
						if (j != pIdx)
						{
							bP1 = borderList[j];
							if (bP1.Id == bP.Id)
							{
								pIdx = j;
								innerIdx = bP1.BInnerIdx;
								timesArray[pIdx] += 1;
								bIdxList.push_back(pIdx);
								borderIdx2 = bP1.BorderIdx;
								if (bP.BorderIdx > 0 && bP.BorderIdx == bP1.BorderIdx)
								{
									sameBorderIdx = true;
								}
								break;
							}
						}
					}
				}

				//---- Return to start point, tracing finish
				if (pIdx == i)
				{
					if (aPList.size() > 0)
					{
						if (sameBorderIdx)
						{
							bool isTooBig = false;
							int baseNum = 0;
							for (int idx = 0; idx < bP.BorderIdx; idx++)
							{
								baseNum += pNums[idx];
							}
							int sIdx = baseNum;
							int eIdx = baseNum + pNums[bP.BorderIdx];
							int theIdx = sIdx;
							for (int idx = sIdx; idx < eIdx; idx++)
							{
								if (std::find(bIdxList.begin(), bIdxList.end(), idx) != bIdxList.end())
								{
									theIdx = idx;
									break;
								}

								//if (!bIdxList.contains(idx))
								//{
								//	theIdx = idx;
								//	break;
								//}
							}
							if (PointInPolygon(aPList, borderList[theIdx].Point))
							{
								isTooBig = true;
							}

							if (isTooBig)
							{
								break;
							}
						}

						aPolygon = WPolygon();
						aPolygon.IsBorder = true;
						aPolygon.IsInnerBorder = sameBorderIdx;
						aPolygon.LowValue = aValue;
						aPolygon.HighValue = bValue;
						aBound = Extent();
						aPolygon.Area = GetExtentAndArea(aPList, aBound);
						aPolygon.IsClockWise = true;
						aPolygon.StartPointIdx = lineBorderList.size() - 1;
						aPolygon.Extent = aBound;
						aPolygon.OutLine.PointList = aPList;
						aPolygon.OutLine.Value = aValue;
						aPolygon.IsHighCenter = true;
						if (aValue == bValue)
						{
							if (cValue < aValue)
								aPolygon.IsHighCenter = false;
						}
						aPolygon.OutLine.Type = "Border";
						aPolygon.HoleLines = vector<PolyLine>();
						aPolygonList.push_back(aPolygon);
					}
					break;
				}
				pIdx += 1;
				innerIdx += 1;
				if (borderIdx1 != borderIdx2)
				{
					borderIdx1 = borderIdx2;
				}

				//if (pIdx == pNum)
				//    pIdx = 0;

				if (innerIdx == pNums[borderIdx1] - 1)
				{
					pIdx = pIdx - (pNums[borderIdx1] - 1);
					innerIdx = 0;
				}
			} while (true);
		}

		sameBorderIdx = false;
		//---- Anticlockwise traceing
		pIdx = i;
		if (timesArray[pIdx] < 2)
		{
			aPList = vector<PointD>();
			vector<int> bIdxList;
			bP = borderList[pIdx];
			innerIdx = bP.BInnerIdx;
			aPList.push_back(bP.Point);
			bIdxList.push_back(pIdx);
			borderIdx1 = bP.BorderIdx;
			borderIdx2 = borderIdx1;
			pIdx += -1;
			innerIdx += -1;
			//If pIdx = -1 Then
			//    pIdx = pNum - 1
			//End If
			if (innerIdx == -1)
			{
				pIdx = pIdx + (pNums[borderIdx1] - 1);
			}
			vNum = 0;
			do
			{
				bP = borderList[pIdx];
				//---- Not endpoint of contour
				if (bP.Id == -1)
				{
					if (timesArray[pIdx] == 1)
					{
						break;
					}
					cValue = bP.Value;
					aPList.push_back(bP.Point);
					timesArray[pIdx] += 1;
					bIdxList.push_back(pIdx);
					//---- endpoint of contour
				}
				else
				{
					if (timesArray[pIdx] == 2)
					{
						break;
					}
					timesArray[pIdx] += 1;
					bIdxList.push_back(pIdx);
					aLine = aLineList[bP.Id];
					if (vNum == 0)
					{
						aValue = aLine.Value;
						bValue = aLine.Value;
						vNum += 1;
					}
					else
					{
						if (aValue == bValue)
						{
							if (aLine.Value > aValue)
							{
								bValue = aLine.Value;
							}
							else if (aLine.Value < aValue)
							{
								aValue = aLine.Value;
							}
							vNum += 1;
						}
					}
					newPList = aLine.PointList;
					aPoint = newPList[0];
					//If Not (Math.Abs(bP.point.x - aPoint.x) < 0.000001 And _
					//  Math.Abs(bP.point.y - aPoint.y) < 0.000001) Then    '---- Start point
					//---- Start point
					if (!(bP.Point.X == aPoint.X && bP.Point.Y == aPoint.Y))
					{
						//newPList.Reverse();
						std::reverse(newPList.begin(), newPList.end());
					}
					//aPList.AddRange(newPList);
					aPList.insert(aPList.end(), newPList.begin(), newPList.end());
					for (j = 0; j < borderList.size(); j++)
					{
						if (j != pIdx)
						{
							bP1 = borderList[j];
							if (bP1.Id == bP.Id)
							{
								pIdx = j;
								innerIdx = bP1.BInnerIdx;
								timesArray[pIdx] += 1;
								bIdxList.push_back(pIdx);
								borderIdx2 = bP1.BorderIdx;
								if (bP.BorderIdx > 0 && bP.BorderIdx == bP1.BorderIdx)
								{
									sameBorderIdx = true;
								}
								break;
							}
						}
					}
				}

				if (pIdx == i)
				{
					if (aPList.size() > 0)
					{
						if (sameBorderIdx)
						{
							bool isTooBig = false;
							int baseNum = 0;
							for (int idx = 0; idx < bP.BorderIdx; idx++)
							{
								baseNum += pNums[idx];
							}
							int sIdx = baseNum;
							int eIdx = baseNum + pNums[bP.BorderIdx];
							int theIdx = sIdx;
							for (int idx = sIdx; idx < eIdx; idx++)
							{
								if (std::find(bIdxList.begin(), bIdxList.end(), idx) != bIdxList.end())
								{
									theIdx = idx;
									break;
								}

								//if (!bIdxList.Contains(idx))
								//{
								//	theIdx = idx;
								//	break;
								//}
							}
							if (PointInPolygon(aPList, borderList[theIdx].Point))
							{
								isTooBig = true;
							}

							if (isTooBig)
							{
								break;
							}
						}

						aPolygon = WPolygon();
						aPolygon.IsBorder = true;
						aPolygon.IsInnerBorder = sameBorderIdx;
						aPolygon.LowValue = aValue;
						aPolygon.HighValue = bValue;
						aBound = Extent();
						aPolygon.Area = GetExtentAndArea(aPList, aBound);
						aPolygon.IsClockWise = false;
						aPolygon.StartPointIdx = lineBorderList.size() - 1;
						aPolygon.Extent = aBound;
						aPolygon.OutLine.PointList = aPList;
						aPolygon.OutLine.Value = aValue;
						aPolygon.IsHighCenter = true;
						if (aValue == bValue)
						{
							if (cValue < aValue)
								aPolygon.IsHighCenter = false;
						}
						aPolygon.OutLine.Type = "Border";
						aPolygon.HoleLines = vector<PolyLine>();
						aPolygonList.push_back(aPolygon);
					}
					break;
				}
				pIdx += -1;
				innerIdx += -1;
				if (borderIdx1 != borderIdx2)
				{
					borderIdx1 = borderIdx2;
				}
				//If pIdx = -1 Then
				//    pIdx = pNum - 1
				//End If
				if (innerIdx == -1)
				{
					pIdx = pIdx + pNums[borderIdx1];
					innerIdx = pNums[borderIdx1] - 1;
				}
			} while (true);
		}
	}

	//---- tracing close polygons
	vector<WPolygon> cPolygonlist;
	bool isInserted = false;
	for (i = 0; i < aLineList.size(); i++)
	{
		aLine = aLineList[i];
		if (aLine.Type == "Close")
		{
			aPolygon = WPolygon();
			aPolygon.IsBorder = false;
			aPolygon.LowValue = aLine.Value;
			aPolygon.HighValue = aLine.Value;
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			aPolygon.IsHighCenter = true;
			aPolygon.HoleLines = vector<PolyLine>();

			//---- Sort from big to small
			isInserted = false;
			for (j = 0; j < cPolygonlist.size(); j++)
			{
				if (aPolygon.Area > (cPolygonlist[j]).Area)
				{
					//cPolygonlist.Insert(j, aPolygon);
					cPolygonlist.insert(cPolygonlist.begin() + j, aPolygon);
					isInserted = true;
					break;
				}
			}
			if (!isInserted)
			{
				cPolygonlist.push_back(aPolygon);
			}
		}
	}

	//---- Juge isHighCenter for border polygons
	if (aPolygonList.size() == 0)
	{
		aLine = PolyLine();
		aLine.Type = "Border";
		aLine.Value = contour[0];
		aLine.PointList = aBorder.LineList[0].pointList;

		if (aLine.PointList.size() > 0)
		{
			aPolygon = WPolygon();
			aPolygon.LowValue = aLine.Value;
			aPolygon.HighValue = aLine.Value;
			aBound = Extent();
			aPolygon.Area = GetExtentAndArea(aLine.PointList, aBound);
			aPolygon.IsClockWise = IsClockwise(aLine.PointList);
			aPolygon.Extent = aBound;
			aPolygon.OutLine = aLine;
			aPolygon.IsHighCenter = false;
			aPolygonList.push_back(aPolygon);
		}
	}

	//---- Add close polygons to form total polygons list
	//aPolygonList.AddRange(cPolygonlist);
	aPolygonList.insert(aPolygonList.end(), cPolygonlist.begin(), cPolygonlist.end());

	//---- Juge siHighCenter for close polygons
	Extent cBound1;
	Extent cBound2;
	int polygonNum = aPolygonList.size();
	WPolygon bPolygon;
	for (i = polygonNum - 1; i >= 0; i += -1)
	{
		aPolygon = aPolygonList[i];
		if (aPolygon.OutLine.Type == "Close")
		{
			cBound1 = aPolygon.Extent;
			aValue = aPolygon.LowValue;
			aPoint = aPolygon.OutLine.PointList[0];
			for (j = i - 1; j >= 0; j += -1)
			{
				bPolygon = aPolygonList[j];
				cBound2 = bPolygon.Extent;
				bValue = bPolygon.LowValue;
				newPList = bPolygon.OutLine.PointList;
				if (PointInPolygon(newPList, aPoint))
				{
					if (cBound1.xMin > cBound2.xMin & cBound1.yMin > cBound2.yMin & cBound1.xMax < cBound2.xMax & cBound1.yMax < cBound2.yMax)
					{
						if (aValue < bValue)
						{
							aPolygon.IsHighCenter = false;
							//aPolygonList.Insert(i, aPolygon);
							//aPolygonList.RemoveAt(i + 1);
						}
						else if (aValue == bValue)
						{
							if (bPolygon.IsHighCenter)
							{
								aPolygon.IsHighCenter = false;
								//aPolygonList.Insert(i, aPolygon);
								//aPolygonList.RemoveAt(i + 1);
							}
						}
						break;
					}
				}
			}
		}
	}

	delete[] timesArray;

	return aPolygonList;
}

vector<WPolygon> Contour::AddPolygonHoles(vector<WPolygon> polygonList)
{
	vector<WPolygon> holePolygons;
	int i, j;
	for (i = 0; i < polygonList.size(); i++)
	{
		WPolygon aPolygon = polygonList[i];
		if (!aPolygon.IsBorder)
		{
			aPolygon.HoleIndex = 1;
			holePolygons.push_back(aPolygon);
		}
	}

	if (holePolygons.size() == 0)
		return polygonList;
	else
	{
		vector<WPolygon> newPolygons;
		for (i = 1; i < holePolygons.size(); i++)
		{
			WPolygon aPolygon = holePolygons[i];
			for (j = i - 1; j >= 0; j--)
			{
				WPolygon bPolygon = holePolygons[j];
				if (bPolygon.Extent.Include(aPolygon.Extent))
				{
					if (PointInPolygon(bPolygon.OutLine.PointList, aPolygon.OutLine.PointList[0]))
					{
						aPolygon.HoleIndex = bPolygon.HoleIndex + 1;
						bPolygon.AddHole(aPolygon);
						//holePolygons[i] = aPolygon;
						//holePolygons[j] = bPolygon;
						break;
					}
				}
			}
		}
		vector<WPolygon> hole1Polygons;
		for (i = 0; i < holePolygons.size(); i++)
		{
			if (holePolygons[i].HoleIndex == 1)
				hole1Polygons.push_back(holePolygons[i]);
		}

		for (i = 0; i < polygonList.size(); i++)
		{
			WPolygon aPolygon = polygonList[i];
			if (aPolygon.IsBorder == true)
			{
				for (j = 0; j < hole1Polygons.size(); j++)
				{
					WPolygon bPolygon = hole1Polygons[j];
					if (aPolygon.Extent.Include(bPolygon.Extent))
					{
						if (PointInPolygon(aPolygon.OutLine.PointList, bPolygon.OutLine.PointList[0]))
						{
							aPolygon.AddHole(bPolygon);
						}
					}
				}
				newPolygons.push_back(aPolygon);
			}
		}
		//newPolygons.AddRange(holePolygons);
		newPolygons.insert(newPolygons.end(), holePolygons.begin(), holePolygons.end());

		return newPolygons;
	}
}

vector<WPolygon> Contour::AddPolygonHoles_Ring(vector<WPolygon> polygonList)
{
	vector<WPolygon> holePolygons;
	int i, j;
	for (i = 0; i < polygonList.size(); i++)
	{
		WPolygon aPolygon = polygonList[i];
		if (!aPolygon.IsBorder || aPolygon.IsInnerBorder)
		{
			aPolygon.HoleIndex = 1;
			holePolygons.push_back(aPolygon);
		}
	}

	if (holePolygons.size() == 0)
		return polygonList;
	else
	{
		vector<WPolygon> newPolygons;
		for (i = 1; i < holePolygons.size(); i++)
		{
			WPolygon aPolygon = holePolygons[i];
			for (j = i - 1; j >= 0; j--)
			{
				WPolygon bPolygon = holePolygons[j];
				if (bPolygon.Extent.Include(aPolygon.Extent))
				{
					if (PointInPolygon(bPolygon.OutLine.PointList, aPolygon.OutLine.PointList[0]))
					{
						aPolygon.HoleIndex = bPolygon.HoleIndex + 1;
						bPolygon.AddHole(aPolygon);
						//holePolygons[i] = aPolygon;
						//holePolygons[j] = bPolygon;
						break;
					}
				}
			}
		}
		vector<WPolygon> hole1Polygons;
		for (i = 0; i < holePolygons.size(); i++)
		{
			if (holePolygons[i].HoleIndex == 1)
				hole1Polygons.push_back(holePolygons[i]);
		}

		for (i = 0; i < polygonList.size(); i++)
		{
			WPolygon aPolygon = polygonList[i];
			if (aPolygon.IsBorder && !aPolygon.IsInnerBorder)
			{
				for (j = 0; j < hole1Polygons.size(); j++)
				{
					WPolygon bPolygon = hole1Polygons[j];
					if (aPolygon.Extent.Include(bPolygon.Extent))
					{
						if (PointInPolygon(aPolygon.OutLine.PointList, bPolygon.OutLine.PointList[0]))
						{
							aPolygon.AddHole(bPolygon);
						}
					}
				}
				newPolygons.push_back(aPolygon);
			}
		}
		//newPolygons.AddRange(holePolygons);
		newPolygons.insert(newPolygons.end(), holePolygons.begin(), holePolygons.end());

		return newPolygons;
	}
}

void Contour::AddHoles_Ring(vector<WPolygon>& polygonList, vector<vector<PointD>> holeList)
{
	int i, j;
	for (i = 0; i < holeList.size(); i++)
	{
		vector<PointD> holePs = holeList[i];
		Extent aExtent = GetExtent(holePs);
		for (j = 0; j < polygonList.size(); j++)
		{
			WPolygon aPolygon = polygonList[j];
			if (aPolygon.Extent.Include(aExtent))
			{
				bool isHole = true;
				for (PointD aP : holePs)
				{
					if (!PointInPolygon(aPolygon.OutLine.PointList, aP))
					{
						isHole = false;
						break;
					}
				}
				if (isHole)
				{
					aPolygon.AddHole(holePs);
					//polygonList[j] = aPolygon;
					break;
				}
			}
		}
	}
}

vector<PolyLine> Contour::CutPolyline(PolyLine inPolyline, vector<PointD> clipPList)
{
	vector<PolyLine> newPolylines;
	vector<PointD> aPList = inPolyline.PointList;
	Extent plExtent = GetExtent(aPList);
	Extent cutExtent = GetExtent(clipPList);

	if (!IsExtentCross(plExtent, cutExtent))
		return newPolylines;

	int i, j;

	if (!IsClockwise(clipPList))    //---- Make cut polygon clockwise
		//clipPList.Reverse();
		std::reverse(clipPList.begin(), clipPList.end());

	//Judge if all points of the polyline are in the cut polygon
	if (PointInPolygon(clipPList, aPList[0]))
	{
		bool isAllIn = true;
		int notInIdx = 0;
		for (i = 0; i < aPList.size(); i++)
		{
			if (!PointInPolygon(clipPList, aPList[i]))
			{
				notInIdx = i;
				isAllIn = false;
				break;
			}
		}
		//if (!isAllIn && inPolyline.Type == "Close")   //Put start point outside of the cut polygon
		if (!isAllIn)
		{
			if (inPolyline.Type == "Close")
			{
				vector<PointD> bPList;
				bPList.insert(bPList.end(), aPList.begin() + notInIdx, aPList.end());
				bPList.insert(bPList.end(), aPList.begin() + 1, aPList.begin() + notInIdx);
				//bPList.AddRange(aPList.GetRange(notInIdx, aPList.Count - notInIdx));
				//bPList.AddRange(aPList.GetRange(1, notInIdx - 1));
				//for (i = notInIdx; i < aPList.Count; i++)
				//    bPList.Add(aPList[i]);

				//for (i = 1; i < notInIdx; i++)
				//    bPList.Add(aPList[i]);

				bPList.push_back(bPList[0]);
				aPList = bPList;
			}
			else
			{
				//aPList.Reverse();
				std::reverse(aPList.begin(), aPList.end());
			}
		}
		else    //the input polygon is inside the cut polygon
		{
			newPolylines.push_back(inPolyline);
			return newPolylines;
		}
	}

	//Cutting            
	bool isInPolygon = PointInPolygon(clipPList, aPList[0]);
	PointD q1, q2, p1, p2, IPoint;
	Line lineA, lineB;
	vector<PointD> newPlist;
	PolyLine bLine;
	p1 = aPList[0];
	for (i = 1; i < aPList.size(); i++)
	{
		p2 = aPList[i];
		if (PointInPolygon(clipPList, p2))
		{
			if (!isInPolygon)
			{
				IPoint = PointD();
				lineA = Line();
				lineA.P1 = p1;
				lineA.P2 = p2;
				q1 = clipPList[clipPList.size() - 1];
				for (j = 0; j < clipPList.size(); j++)
				{
					q2 = clipPList[j];
					lineB = Line();
					lineB.P1 = q1;
					lineB.P2 = q2;
					if (IsLineSegmentCross(lineA, lineB))
					{
						IPoint = GetCrossPoint(lineA, lineB);
						break;
					}
					q1 = q2;
				}
				newPlist.push_back(IPoint);
				//aType = "Border";
			}
			newPlist.push_back(aPList[i]);
			isInPolygon = true;
		}
		else
		{
			if (isInPolygon)
			{
				IPoint = PointD();
				lineA = Line();
				lineA.P1 = p1;
				lineA.P2 = p2;
				q1 = clipPList[clipPList.size() - 1];
				for (j = 0; j < clipPList.size(); j++)
				{
					q2 = clipPList[j];
					lineB = Line();
					lineB.P1 = q1;
					lineB.P2 = q2;
					if (IsLineSegmentCross(lineA, lineB))
					{
						IPoint = GetCrossPoint(lineA, lineB);
						break;
					}
					q1 = q2;
				}
				newPlist.push_back(IPoint);

				bLine = PolyLine();
				bLine.Value = inPolyline.Value;
				bLine.Type = inPolyline.Type;
				bLine.PointList = newPlist;
				newPolylines.push_back(bLine);
				isInPolygon = false;
				newPlist = vector<PointD>();
				//aType = "Border";
			}
		}
		p1 = p2;
	}

	if (isInPolygon && newPlist.size() > 1)
	{
		bLine = PolyLine();
		bLine.Value = inPolyline.Value;
		bLine.Type = inPolyline.Type;
		bLine.PointList = newPlist;
		newPolylines.push_back(bLine);
	}

	return newPolylines;
}

vector<WPolygon> Contour::CutPolygon_Hole(WPolygon inPolygon, vector<PointD> clipPList)
{
	vector<WPolygon> newPolygons;
	vector<PolyLine> newPolylines;
	vector<PointD> aPList = inPolygon.OutLine.PointList;
	Extent plExtent = GetExtent(aPList);
	Extent cutExtent = GetExtent(clipPList);

	if (!IsExtentCross(plExtent, cutExtent))
		return newPolygons;

	int i, j;

	if (!IsClockwise(clipPList))    //---- Make cut polygon clockwise
		//clipPList.Reverse();
		std::reverse(clipPList.begin(), clipPList.end());

	//Judge if all points of the polyline are in the cut polygon - outline   
	vector<vector<PointD>> newLines;
	if (PointInPolygon(clipPList, aPList[0]))
	{
		bool isAllIn = true;
		int notInIdx = 0;
		for (i = 0; i < aPList.size(); i++)
		{
			if (!PointInPolygon(clipPList, aPList[i]))
			{
				notInIdx = i;
				isAllIn = false;
				break;
			}
		}
		if (!isAllIn)   //Put start point outside of the cut polygon
		{
			vector<PointD> bPList;
			bPList.insert(bPList.end(), aPList.begin() + notInIdx, aPList.end());
			bPList.insert(bPList.end(), aPList.begin() + 1, aPList.begin() + notInIdx);
			//bPList.AddRange(aPList.GetRange(notInIdx, aPList.Count - notInIdx));
			//bPList.AddRange(aPList.GetRange(1, notInIdx - 1));
			//for (i = notInIdx; i < aPList.Count; i++)
			//    bPList.Add(aPList[i]);

			//for (i = 1; i < notInIdx; i++)
			//    bPList.Add(aPList[i]);

			bPList.push_back(bPList[0]);
			//if (!IsClockwise(bPList))
			//    bPList.Reverse();
			newLines.push_back(bPList);
		}
		else    //the input polygon is inside the cut polygon
		{
			newPolygons.push_back(inPolygon);
			return newPolygons;
		}
	}
	else
	{
		newLines.push_back(aPList);
	}

	//Holes
	vector<vector<PointD>> holeLines;
	for (int h = 0; h < inPolygon.HoleLines.size(); h++)
	{
		vector<PointD> holePList = inPolygon.HoleLines[h].PointList;
		plExtent = GetExtent(holePList);
		if (!IsExtentCross(plExtent, cutExtent))
			continue;

		if (PointInPolygon(clipPList, holePList[0]))
		{
			bool isAllIn = true;
			int notInIdx = 0;
			for (i = 0; i < holePList.size(); i++)
			{
				if (!PointInPolygon(clipPList, holePList[i]))
				{
					notInIdx = i;
					isAllIn = false;
					break;
				}
			}
			if (!isAllIn)   //Put start point outside of the cut polygon
			{
				vector<PointD> bPList;

				bPList.insert(bPList.end(), holePList.begin() + notInIdx, holePList.end());
				bPList.insert(bPList.end(), holePList.begin() + 1, holePList.begin() + notInIdx);
				//bPList.AddRange(holePList.GetRange(notInIdx, holePList.Count - notInIdx));
				//bPList.AddRange(holePList.GetRange(1, notInIdx - 1));
				//for (i = notInIdx; i < aPList.Count; i++)
				//    bPList.Add(aPList[i]);

				//for (i = 1; i < notInIdx; i++)
				//    bPList.Add(aPList[i]);

				bPList.push_back(bPList[0]);
				newLines.push_back(bPList);
			}
			else    //the hole is inside the cut polygon
			{
				holeLines.push_back(holePList);
			}
		}
		else
			newLines.push_back(holePList);
	}

	//Prepare border point list
	vector<BorderPoint> borderList;
	BorderPoint aBP = BorderPoint();
	for (PointD aP : clipPList)
	{
		aBP = BorderPoint();
		aBP.Point = aP;
		aBP.Id = -1;
		borderList.push_back(aBP);
	}

	//Cutting                     
	for (int l = 0; l < newLines.size(); l++)
	{
		aPList = newLines[l];
		bool isInPolygon = false;
		PointD q1, q2, p1, p2, IPoint;
		Line lineA, lineB;
		vector<PointD> newPlist;
		PolyLine bLine;
		p1 = aPList[0];
		int inIdx = -1, outIdx = -1;
		bool newLine = true;
		int a1 = 0;
		for (i = 1; i < aPList.size(); i++)
		{
			p2 = aPList[i];
			if (PointInPolygon(clipPList, p2))
			{
				if (!isInPolygon)
				{
					lineA = Line();
					lineA.P1 = p1;
					lineA.P2 = p2;
					q1 = borderList[borderList.size() - 1].Point;
					IPoint = PointD();
					for (j = 0; j < borderList.size(); j++)
					{
						q2 = borderList[j].Point;
						lineB = Line();
						lineB.P1 = q1;
						lineB.P2 = q2;
						if (IsLineSegmentCross(lineA, lineB))
						{
							IPoint = GetCrossPoint(lineA, lineB);
							aBP = BorderPoint();
							aBP.Id = newPolylines.size();
							aBP.Point = IPoint;
							//borderList.Insert(j, aBP);
							borderList.insert(borderList.begin() + j, aBP);
							inIdx = j;
							break;
						}
						q1 = q2;
					}
					newPlist.push_back(IPoint);
				}
				newPlist.push_back(aPList[i]);
				isInPolygon = true;
			}
			else
			{
				if (isInPolygon)
				{
					lineA = Line();
					lineA.P1 = p1;
					lineA.P2 = p2;
					q1 = borderList[borderList.size() - 1].Point;
					IPoint = PointD();
					for (j = 0; j < borderList.size(); j++)
					{
						q2 = borderList[j].Point;
						lineB = Line();
						lineB.P1 = q1;
						lineB.P2 = q2;
						if (IsLineSegmentCross(lineA, lineB))
						{
							if (!newLine)
							{
								if (inIdx - outIdx >= 1 && inIdx - outIdx <= 10)
								{
									if (!TwoPointsInside(a1, outIdx, inIdx, j))
									{
										borderList.erase(borderList.begin() + inIdx);
										borderList.insert(borderList.begin() + outIdx, aBP);
										//borderList.RemoveAt(inIdx);
										//borderList.Insert(outIdx, aBP);
									}
								}
								else if (inIdx - outIdx <= -1 && inIdx - outIdx >= -10)
								{
									if (!TwoPointsInside(a1, outIdx, inIdx, j))
									{
										borderList.erase(borderList.begin() + inIdx);
										borderList.insert(borderList.begin() + outIdx + 1, aBP);
										//borderList.RemoveAt(inIdx);
										//borderList.Insert(outIdx + 1, aBP);
									}
								}
								else if (inIdx == outIdx)
								{
									if (!TwoPointsInside(a1, outIdx, inIdx, j))
									{
										//borderList.RemoveAt(inIdx);
										//borderList.Insert(inIdx + 1, aBP);
										borderList.erase(borderList.begin() + inIdx);
										borderList.insert(borderList.begin() + inIdx + 1, aBP);
									}
								}
							}
							IPoint = GetCrossPoint(lineA, lineB);
							aBP = BorderPoint();
							aBP.Id = newPolylines.size();
							aBP.Point = IPoint;
							//borderList.Insert(j, aBP);
							borderList.insert(borderList.begin() + j, aBP);
							outIdx = j;
							a1 = inIdx;

							newLine = false;
							break;
						}
						q1 = q2;
					}
					newPlist.push_back(IPoint);

					bLine = PolyLine();
					bLine.Value = inPolygon.OutLine.Value;
					bLine.Type = inPolygon.OutLine.Type;
					bLine.PointList = newPlist;
					newPolylines.push_back(bLine);

					isInPolygon = false;
					newPlist = vector<PointD>();
				}
			}
			p1 = p2;
		}
	}

	if (newPolylines.size() > 0)
	{
		//Tracing polygons
		newPolygons = TracingClipPolygons(inPolygon, newPolylines, borderList);
	}
	else
	{
		if (PointInPolygon(aPList, clipPList[0]))
		{
			Extent aBound = Extent();
			WPolygon aPolygon = WPolygon();
			aPolygon.IsBorder = true;
			aPolygon.LowValue = inPolygon.LowValue;
			aPolygon.HighValue = inPolygon.HighValue;
			aPolygon.Area = GetExtentAndArea(clipPList, aBound);
			aPolygon.IsClockWise = true;
			//aPolygon.StartPointIdx = lineBorderList.Count - 1;
			aPolygon.Extent = aBound;
			aPolygon.OutLine.PointList = clipPList;
			aPolygon.OutLine.Value = inPolygon.LowValue;
			aPolygon.IsHighCenter = inPolygon.IsHighCenter;
			aPolygon.OutLine.Type = "Border";
			aPolygon.HoleLines = vector<PolyLine>();

			newPolygons.push_back(aPolygon);
		}
	}

	if (holeLines.size() > 0)
	{
		AddHoles_Ring(newPolygons, holeLines);
	}

	return newPolygons;
}

vector<WPolygon> Contour::CutPolygon(WPolygon inPolygon, vector<PointD> clipPList)
{
	vector<WPolygon> newPolygons;
	vector<PolyLine> newPolylines;
	vector<PointD> aPList = inPolygon.OutLine.PointList;
	Extent plExtent = GetExtent(aPList);
	Extent cutExtent = GetExtent(clipPList);

	if (!IsExtentCross(plExtent, cutExtent))
		return newPolygons;

	int i, j;

	if (!IsClockwise(clipPList))    //---- Make cut polygon clockwise
		//clipPList.Reverse();
		std::reverse(clipPList.begin(), clipPList.end());

	//Judge if all points of the polyline are in the cut polygon            
	if (PointInPolygon(clipPList, aPList[0]))
	{
		bool isAllIn = true;
		int notInIdx = 0;
		for (i = 0; i < aPList.size(); i++)
		{
			if (!PointInPolygon(clipPList, aPList[i]))
			{
				notInIdx = i;
				isAllIn = false;
				break;
			}
		}
		if (!isAllIn)   //Put start point outside of the cut polygon
		{
			vector<PointD> bPList;
			bPList.insert(bPList.end(), aPList.begin() + notInIdx, aPList.end());
			bPList.insert(bPList.end(), aPList.begin() + 1, aPList.begin() + notInIdx);

			//bPList.AddRange(aPList.GetRange(notInIdx, aPList.Count - notInIdx));
			//bPList.AddRange(aPList.GetRange(1, notInIdx - 1));
			//for (i = notInIdx; i < aPList.Count; i++)
			//    bPList.Add(aPList[i]);

			//for (i = 1; i < notInIdx; i++)
			//    bPList.Add(aPList[i]);

			bPList.push_back(bPList[0]);
			aPList = vector<PointD>(bPList);
		}
		else    //the input polygon is inside the cut polygon
		{
			newPolygons.push_back(inPolygon);
			return newPolygons;
		}
	}

	//Prepare border point list
	vector<BorderPoint> borderList;
	BorderPoint aBP = BorderPoint();
	for (PointD aP : clipPList)
	{
		aBP = BorderPoint();
		aBP.Point = aP;
		aBP.Id = -1;
		borderList.push_back(aBP);
	}

	//Cutting            
	bool isInPolygon = false;
	PointD q1, q2, p1, p2, IPoint;
	Line lineA, lineB;
	vector<PointD> newPlist;
	PolyLine bLine;
	p1 = aPList[0];
	int inIdx = -1, outIdx = -1;
	int a1 = 0;
	bool isNewLine = true;
	for (i = 1; i < aPList.size(); i++)
	{
		p2 = (PointD)aPList[i];
		if (PointInPolygon(clipPList, p2))
		{
			if (!isInPolygon)
			{
				lineA = Line();
				lineA.P1 = p1;
				lineA.P2 = p2;
				q1 = borderList[borderList.size() - 1].Point;
				IPoint = PointD();
				for (j = 0; j < borderList.size(); j++)
				{
					q2 = borderList[j].Point;
					lineB = Line();
					lineB.P1 = q1;
					lineB.P2 = q2;
					if (IsLineSegmentCross(lineA, lineB))
					{
						IPoint = GetCrossPoint(lineA, lineB);
						aBP = BorderPoint();
						aBP.Id = newPolylines.size();
						aBP.Point = IPoint;
						//borderList.Insert(j, aBP);
						borderList.insert(borderList.begin() + j, aBP);
						inIdx = j;
						break;
					}
					q1 = q2;
				}
				newPlist.push_back(IPoint);
			}
			newPlist.push_back(aPList[i]);
			isInPolygon = true;
		}
		else
		{
			if (isInPolygon)
			{
				lineA = Line();
				lineA.P1 = p1;
				lineA.P2 = p2;
				q1 = borderList[borderList.size() - 1].Point;
				IPoint = PointD();
				for (j = 0; j < borderList.size(); j++)
				{
					q2 = borderList[j].Point;
					lineB = Line();
					lineB.P1 = q1;
					lineB.P2 = q2;
					if (IsLineSegmentCross(lineA, lineB))
					{
						if (!isNewLine)
						{
							if (inIdx - outIdx >= 1 && inIdx - outIdx <= 10)
							{
								if (!TwoPointsInside(a1, outIdx, inIdx, j))
								{
									//borderList.RemoveAt(inIdx);
									//borderList.Insert(outIdx, aBP);
									borderList.erase(borderList.begin() + inIdx);
									borderList.insert(borderList.begin() + outIdx, aBP);
								}
							}
							else if (inIdx - outIdx <= -1 && inIdx - outIdx >= -10)
							{
								if (!TwoPointsInside(a1, outIdx, inIdx, j))
								{
									//borderList.RemoveAt(inIdx);
									//borderList.Insert(outIdx + 1, aBP);
									borderList.erase(borderList.begin() + inIdx);
									borderList.insert(borderList.begin() + outIdx + 1, aBP);
								}
							}
							else if (inIdx == outIdx)
							{
								if (!TwoPointsInside(a1, outIdx, inIdx, j))
								{
									//borderList.RemoveAt(inIdx);
									//borderList.Insert(inIdx + 1, aBP);
									borderList.erase(borderList.begin() + inIdx);
									borderList.insert(borderList.begin() + inIdx + 1, aBP);
								}
							}
						}
						IPoint = GetCrossPoint(lineA, lineB);
						aBP = BorderPoint();
						aBP.Id = newPolylines.size();
						aBP.Point = IPoint;
						//borderList.Insert(j, aBP);
						borderList.insert(borderList.begin() + j, aBP);
						outIdx = j;
						a1 = inIdx;
						isNewLine = false;
						break;
					}
					q1 = q2;
				}
				newPlist.push_back(IPoint);

				bLine = PolyLine();
				bLine.Value = inPolygon.OutLine.Value;
				bLine.Type = inPolygon.OutLine.Type;
				bLine.PointList = newPlist;
				newPolylines.push_back(bLine);

				isInPolygon = false;
				newPlist = vector<PointD>();
			}
		}
		p1 = p2;
	}

	if (newPolylines.size() > 0)
	{
		//Tracing polygons
		newPolygons = TracingClipPolygons(inPolygon, newPolylines, borderList);
	}
	else
	{
		if (PointInPolygon(aPList, clipPList[0]))
		{
			Extent aBound = Extent();
			WPolygon aPolygon = WPolygon();
			aPolygon.IsBorder = true;
			aPolygon.LowValue = inPolygon.LowValue;
			aPolygon.HighValue = inPolygon.HighValue;
			aPolygon.Area = GetExtentAndArea(clipPList, aBound);
			aPolygon.IsClockWise = true;
			//aPolygon.StartPointIdx = lineBorderList.Count - 1;
			aPolygon.Extent = aBound;
			aPolygon.OutLine.PointList = clipPList;
			aPolygon.OutLine.Value = inPolygon.LowValue;
			aPolygon.IsHighCenter = inPolygon.IsHighCenter;
			aPolygon.OutLine.Type = "Border";
			aPolygon.HoleLines = vector<PolyLine>();

			newPolygons.push_back(aPolygon);
		}
	}

	return newPolygons;
}

bool Contour::TwoPointsInside(int a1, int a2, int b1, int b2)
{
	if (a2 < a1)
		a1 += 1;
	if (b1 < a1)
		a1 += 1;
	if (b1 < a2)
		a2 += 1;


	if (a2 < a1)
	{
		int c = a1;
		a1 = a2;
		a2 = c;
	}

	if (b1 > a1 && b1 <= a2)
	{
		if (b2 > a1 && b2 <= a2)
			return true;
		else
			return false;
	}
	else
	{
		if (!(b2 > a1 && b2 <= a2))
			return true;
		else
			return false;
	}
}

vector<PointD> Contour::BSplineScanning(vector<PointD> pointList, int sum)
{
	float t;
	int i;
	double X = 0, Y = 0;
	PointD aPoint;
	vector<PointD> newPList;

	if (sum < 4)
	{
		return vector<PointD>();
	}

	bool isClose = false;
	aPoint = (PointD)pointList[0];
	PointD bPoint = (PointD)pointList[sum - 1];
	//if (aPoint.X == bPoint.X && aPoint.Y == bPoint.Y)
	if (DoubleEquals(aPoint.X, bPoint.X) && DoubleEquals(aPoint.Y, bPoint.Y))
	{
		//pointList.RemoveAt(0);
		pointList.erase(pointList.begin());
		pointList.push_back(pointList[0]);
		pointList.push_back(pointList[1]);
		pointList.push_back(pointList[2]);
		pointList.push_back(pointList[3]);
		pointList.push_back(pointList[4]);
		pointList.push_back(pointList[5]);
		pointList.push_back(pointList[6]);
		//pointList.Add(pointList[7]);
		//pointList.Add(pointList[8]);
		isClose = true;
	}

	sum = pointList.size();
	for (i = 0; i < sum - 3; i++)
	{
		for (t = 0; t <= 1; t += 0.05F)
		{
			BSpline(pointList, t, i, X, Y);
			if (isClose)
			{
				if (i > 3)
				{
					aPoint = PointD();
					aPoint.X = X;
					aPoint.Y = Y;
					newPList.push_back(aPoint);
				}
			}
			else
			{
				aPoint = PointD();
				aPoint.X = X;
				aPoint.Y = Y;
				newPList.push_back(aPoint);
			}
		}
	}

	if (isClose)
		newPList.push_back(newPList[0]);
	else
	{
		//newPList.Insert(0, pointList[0]);
		newPList.insert(newPList.begin(), pointList[0]);
		newPList.push_back(pointList[pointList.size() - 1]);
	}

	return newPList;
}

void Contour::BSpline(vector<PointD> pointList, double t, int i, double& X, double& Y)
{
	double* f = new double[4]();
	fb(t, f);
	int j;
	X = 0;
	Y = 0;
	PointD aPoint;
	for (j = 0; j < 4; j++)
	{
		aPoint = pointList[i + j];
		X = X + f[j] * aPoint.X;
		Y = Y + f[j] * aPoint.Y;
	}
	delete[] f;
}

double Contour::f0(double t)
{
	return 1.0 / 6 * (-t + 1) * (-t + 1) * (-t + 1);
}

double Contour::f1(double t)
{
	return 1.0 / 6 * (3 * t * t * t - 6 * t * t + 4);
}

double Contour::f2(double t)
{
	return 1.0 / 6 * (-3 * t * t * t + 3 * t * t + 3 * t + 1);
}

double Contour::f3(double t)
{
	return 1.0 / 6 * t * t * t;
}

void Contour::fb(double t, double* &fs)
{
	fs[0] = f0(t);
	fs[1] = f1(t);
	fs[2] = f2(t);
	fs[3] = f3(t);
}

Extent Contour::GetExtent(vector<PointD> pList)
{
	double  minX, minY, maxX, maxY;
	int i;
	PointD aPoint;
	aPoint = pList[0];
	minX = aPoint.X;
	maxX = aPoint.X;
	minY = aPoint.Y;
	maxY = aPoint.Y;
	for (i = 1; i < pList.size(); i++)
	{
		aPoint = pList[i];
		if (aPoint.X < minX)
			minX = aPoint.X;

		if (aPoint.X > maxX)
			maxX = aPoint.X;

		if (aPoint.Y < minY)
			minY = aPoint.Y;

		if (aPoint.Y > maxY)
			maxY = aPoint.Y;
	}

	Extent aExtent;
	aExtent.xMin = minX;
	aExtent.yMin = minY;
	aExtent.xMax = maxX;
	aExtent.yMax = maxY;

	return aExtent;
}

bool Contour::IsLineSegmentCross(Line lineA, Line lineB)
{
	Extent boundA, boundB;
	vector<PointD> PListA, PListB;
	PListA.push_back(lineA.P1);
	PListA.push_back(lineA.P2);
	PListB.push_back(lineB.P1);
	PListB.push_back(lineB.P2);
	GetExtentAndArea(PListA, boundA);
	GetExtentAndArea(PListB, boundB);

	if (!IsExtentCross(boundA, boundB))
		return false;
	else
	{
		double XP1 = (lineB.P1.X - lineA.P1.X) * (lineA.P2.Y - lineA.P1.Y) -
			(lineA.P2.X - lineA.P1.X) * (lineB.P1.Y - lineA.P1.Y);
		double XP2 = (lineB.P2.X - lineA.P1.X) * (lineA.P2.Y - lineA.P1.Y) -
			(lineA.P2.X - lineA.P1.X) * (lineB.P2.Y - lineA.P1.Y);
		if (XP1 * XP2 > 0)
			return false;
		else
			return true;
	}
}

bool Contour::IsExtentCross(Extent aBound, Extent bBound)
{
	if (aBound.xMin > bBound.xMax || aBound.xMax < bBound.xMin || aBound.yMin > bBound.yMax ||
		aBound.yMax < bBound.yMin)
		return false;
	else
		return true;

}

PointD Contour::GetCrossPoint(Line lineA, Line lineB)
{
	PointD IPoint;
	PointD p1, p2, q1, q2;
	double tempLeft, tempRight;

	double XP1 = (lineB.P1.X - lineA.P1.X) * (lineA.P2.Y - lineA.P1.Y) -
		(lineA.P2.X - lineA.P1.X) * (lineB.P1.Y - lineA.P1.Y);
	double XP2 = (lineB.P2.X - lineA.P1.X) * (lineA.P2.Y - lineA.P1.Y) -
		(lineA.P2.X - lineA.P1.X) * (lineB.P2.Y - lineA.P1.Y);
	if (XP1 == 0)
		IPoint = lineB.P1;
	else if (XP2 == 0)
		IPoint = lineB.P2;
	else
	{
		p1 = lineA.P1;
		p2 = lineA.P2;
		q1 = lineB.P1;
		q2 = lineB.P2;

		tempLeft = (q2.X - q1.X) * (p1.Y - p2.Y) - (p2.X - p1.X) * (q1.Y - q2.Y);
		tempRight = (p1.Y - q1.Y) * (p2.X - p1.X) * (q2.X - q1.X) + q1.X * (q2.Y - q1.Y) * (p2.X - p1.X) - p1.X * (p2.Y - p1.Y) * (q2.X - q1.X);
		IPoint.X = tempRight / tempLeft;

		tempLeft = (p1.X - p2.X) * (q2.Y - q1.Y) - (p2.Y - p1.Y) * (q1.X - q2.X);
		tempRight = p2.Y * (p1.X - p2.X) * (q2.Y - q1.Y) + (q2.X - p2.X) * (q2.Y - q1.Y) * (p1.Y - p2.Y) - q2.Y * (q1.X - q2.X) * (p2.Y - p1.Y);
		IPoint.Y = tempRight / tempLeft;
	}

	return IPoint;
}

vector<BorderPoint> Contour::InsertPoint2Border(vector<BorderPoint> bPList, vector<BorderPoint> aBorderList)
{
	BorderPoint aBPoint, bP;
	int i, j;
	PointD p1, p2, p3;
	//ArrayList aEPList = new ArrayList(), temEPList = new ArrayList(), dList = new ArrayList();
	vector<BorderPoint> BorderList = aBorderList;

	for (i = 0; i < bPList.size(); i++)
	{
		bP = bPList[i];
		p3 = bP.Point;
		aBPoint = BorderList[0];
		p1 = aBPoint.Point;
		for (j = 1; j < BorderList.size(); j++)
		{
			aBPoint = BorderList[j];
			p2 = aBPoint.Point;
			if ((DoubleEquals(p3.X, p1.X) && DoubleEquals(p3.Y, p1.Y)) ||
				(DoubleEquals(p3.X, p2.X) && DoubleEquals(p3.Y, p2.Y)))
			{
				//BorderList.Insert(j, bP);
				BorderList.insert(BorderList.begin() + j, bP);
				break;
			}
			else
			{
				if ((p3.X - p1.X) * (p3.X - p2.X) <= 0 || (DoubleEquals(p3.X, p1.X) || DoubleEquals(p3.X, p2.X)))
					//if ((p3.X - p1.X) * (p3.X - p2.X) <= 0)
				{
					if ((p3.Y - p1.Y) * (p3.Y - p2.Y) <= 0 || (DoubleEquals(p3.Y, p1.Y) || DoubleEquals(p3.Y, p2.Y)))
						//if ((p3.Y - p1.Y) * (p3.Y - p2.Y) <= 0)
					{
						if ((p3.X - p1.X) * (p2.Y - p1.Y) - (p2.X - p1.X) * (p3.Y - p1.Y) <= 0.001)
						{
							//BorderList.Insert(j, bP);
							BorderList.insert(BorderList.begin() + j, bP);
							break;
						}
					}
				}
			}
			p1 = p2;
		}
	}

	return BorderList;
}

vector<BorderPoint> Contour::InsertPoint2RectangleBorder(vector<PolyLine> LineList, Extent aBound)
{
	BorderPoint bPoint, bP;
	PolyLine aLine;
	PointD aPoint;
	int i, j, k;
	vector<BorderPoint> LBPList, TBPList;
	vector<BorderPoint> RBPList, BBPList;
	vector<BorderPoint> BorderList;
	vector<PointD> aPointList;
	bool IsInserted;

	//---- Get four border point list
	for (i = 0; i < LineList.size(); i++)
	{
		aLine = LineList[i];
		if (aLine.Type != "Close")
		{
			aPointList = aLine.PointList;
			bP = BorderPoint();
			bP.Id = i;
			for (k = 0; k <= 1; k++)
			{
				if (k == 0)
					aPoint = aPointList[0];
				else
					aPoint = aPointList[aPointList.size() - 1];

				bP.Point = aPoint;
				IsInserted = false;
				if (aPoint.X == aBound.xMin)
				{
					for (j = 0; j < LBPList.size(); j++)
					{
						bPoint = LBPList[j];
						if (aPoint.Y < bPoint.Point.Y)
						{
							//LBPList.Insert(j, bP);
							LBPList.insert(LBPList.begin() + j, bP);
							IsInserted = true;
							break;
						}
					}
					if (!IsInserted)
						LBPList.push_back(bP);

				}
				else if (aPoint.X == aBound.xMax)
				{
					for (j = 0; j < RBPList.size(); j++)
					{
						bPoint = RBPList[j];
						if (aPoint.Y > bPoint.Point.Y)
						{
							//RBPList.Insert(j, bP);
							RBPList.insert(RBPList.begin() + j, bP);
							IsInserted = true;
							break;
						}
					}
					if (!IsInserted)
						RBPList.push_back(bP);

				}
				else if (aPoint.Y == aBound.yMin)
				{
					for (j = 0; j < BBPList.size(); j++)
					{
						bPoint = BBPList[j];
						if (aPoint.X > bPoint.Point.X)
						{
							//BBPList.Insert(j, bP);
							BBPList.insert(RBPList.begin() + j, bP);
							IsInserted = true;
							break;
						}
					}
					if (!IsInserted)
						BBPList.push_back(bP);

				}
				else if (aPoint.Y == aBound.yMax)
				{
					for (j = 0; j < TBPList.size(); j++)
					{
						bPoint = TBPList[j];
						if (aPoint.X < bPoint.Point.X)
						{
							//TBPList.Insert(j, bP);
							TBPList.insert(TBPList.begin() + j, bP);
							IsInserted = true;
							break;
						}
					}
					if (!IsInserted)
						TBPList.push_back(bP);

				}
			}
		}
	}

	//---- Get border list
	bP = BorderPoint();
	bP.Id = -1;

	aPoint = PointD();
	aPoint.X = aBound.xMin;
	aPoint.Y = aBound.yMin;
	bP.Point = aPoint;
	BorderList.push_back(bP);

	//BorderList.AddRange(LBPList);
	BorderList.insert(BorderList.end(), LBPList.begin(), LBPList.end());

	bP = BorderPoint();
	bP.Id = -1;
	aPoint = PointD();
	aPoint.X = aBound.xMin;
	aPoint.Y = aBound.yMax;
	bP.Point = aPoint;
	BorderList.push_back(bP);

	//BorderList.AddRange(TBPList);
	BorderList.insert(BorderList.end(), TBPList.begin(), TBPList.end());

	bP = BorderPoint();
	bP.Id = -1;
	aPoint = PointD();
	aPoint.X = aBound.xMax;
	aPoint.Y = aBound.yMax;
	bP.Point = aPoint;
	BorderList.push_back(bP);

	//BorderList.AddRange(RBPList);
	BorderList.insert(BorderList.end(), RBPList.begin(), RBPList.end());

	bP = BorderPoint();
	bP.Id = -1;
	aPoint = PointD();
	aPoint.X = aBound.xMax;
	aPoint.Y = aBound.yMin;
	bP.Point = aPoint;
	BorderList.push_back(bP);

	//BorderList.AddRange(BBPList);
	BorderList.insert(BorderList.end(), BBPList.begin(), BBPList.end());

	BorderList.push_back(BorderList[0]);

	return BorderList;
}

vector<BorderPoint> Contour::InsertEndPoint2Border(vector<EndPoint> EPList, vector<BorderPoint> aBorderList)
{
	BorderPoint aBPoint, bP;
	int i, j, k;
	PointD p1, p2;
	vector<EndPoint> aEPList;
	vector<EndPoint> temEPList;
	//ArrayList dList = new ArrayList();
	std::vector<std::tuple<double, int>> dList;
	EndPoint aEP;
	double dist;
	bool IsInsert;
	vector<BorderPoint> BorderList;

	aEPList = EPList;

	aBPoint = aBorderList[0];
	p1 = aBPoint.Point;
	BorderList.push_back(aBPoint);
	for (i = 1; i < aBorderList.size(); i++)
	{
		aBPoint = aBorderList[i];
		p2 = aBPoint.Point;
		temEPList.clear();
		for (j = 0; j < aEPList.size(); j++)
		{
			if (j == aEPList.size())
				break;

			aEP = aEPList[j];
			if (abs(aEP.sPoint.X - p1.X) < 0.000001 && abs(aEP.sPoint.Y - p1.Y) < 0.000001)
			{
				temEPList.push_back(aEP);
				//aEPList.RemoveAt(j);
				aEPList.erase(aEPList.begin() + j);
				j -= 1;
			}
		}
		if (temEPList.size() > 0)
		{
			dList.clear();
			if (temEPList.size() > 1)
			{
				for (j = 0; j < temEPList.size(); j++)
				{
					aEP = temEPList[j];
					dist = pow(aEP.Point.X - p1.X, 2) + pow(aEP.Point.Y - p1.Y, 2);
					if (j == 0)
						//dList.Add(new object[]{ dist, j });
						dList.push_back(std::tuple<double, int>(dist, j));
					else
					{
						IsInsert = false;
						for (k = 0; k < dList.size(); k++)
						{

							if (dist < std::get<0>(dList.at(k)))
							{
								//dList.Insert(k, new object[]{ dist, j });
								dList.insert(dList.begin() + k, std::tuple<double, int>(dist, j));
								IsInsert = true;
								break;
							}
						}
						if (!IsInsert)
							//dList.Add(new object[]{ dist, j });
							dList.push_back(std::tuple<double, int>(dist, j));

					}
				}
				for (j = 0; j < dList.size(); j++)
				{
					//aEP = temEPList[(int)((object[])dList[j])[1]];
					aEP = temEPList[std::get<1>(dList.at(j))];
					bP = BorderPoint();
					bP.Id = aEP.Index;
					bP.Point = aEP.Point;
					BorderList.push_back(bP);
				}
			}
			else
			{
				aEP = temEPList[0];
				bP = BorderPoint();
				bP.Id = aEP.Index;
				bP.Point = aEP.Point;
				BorderList.push_back(bP);
			}
		}

		BorderList.push_back(aBPoint);

		p1 = p2;
	}

	return BorderList;
}

vector<BorderPoint> Contour::InsertPoint2Border_Ring(double** S0, vector<BorderPoint> bPList, Border aBorder, int* &pNums)
{
	BorderPoint aBPoint, bP;
	int i, j, k;
	PointD p1, p2, p3;
	//ArrayList aEPList = new ArrayList(), temEPList = new ArrayList(), dList = new ArrayList();
	BorderLine aBLine;
	vector<BorderPoint> newBPList, tempBPList, tempBPList1;

	pNums = new int[aBorder.LineNum()];
	for (k = 0; k < aBorder.LineNum(); k++)
	{
		aBLine = aBorder.LineList[k];
		tempBPList.clear();
		for (i = 0; i < aBLine.pointList.size(); i++)
		{
			aBPoint = BorderPoint();
			aBPoint.Id = -1;
			aBPoint.BorderIdx = k;
			aBPoint.Point = aBLine.pointList[i];
			aBPoint.Value = S0[aBLine.ijPointList[i].I][aBLine.ijPointList[i].J];
			tempBPList.push_back(aBPoint);
		}
		for (i = 0; i < bPList.size(); i++)
		{
			bP = (BorderPoint)bPList[i].Clone();
			bP.BorderIdx = k;
			p3 = bP.Point;
			//aBPoint = (BorderPoint)tempBPList[0];
			p1 = (PointD)tempBPList[0].Point.Clone();
			for (j = 1; j < tempBPList.size(); j++)
			{
				//aBPoint = (BorderPoint)tempBPList[j];
				p2 = (PointD)tempBPList[j].Point.Clone();
				if ((p3.X - p1.X) * (p3.X - p2.X) <= 0)
				{
					if ((p3.Y - p1.Y) * (p3.Y - p2.Y) <= 0)
					{
						if ((p3.X - p1.X) * (p2.Y - p1.Y) - (p2.X - p1.X) * (p3.Y - p1.Y) == 0)
						{
							//tempBPList.Insert(j, bP);
							tempBPList.insert(tempBPList.begin() + j, bP);
							break;
						}
					}
				}
				p1 = p2;
			}
		}
		tempBPList1.clear();
		for (i = 0; i < tempBPList.size(); i++)
		{
			bP = tempBPList[i];
			bP.BInnerIdx = i;
			tempBPList1.push_back(bP);
		}
		pNums[k] = tempBPList1.size();
		//newBPList.AddRange(tempBPList1);
		newBPList.insert(newBPList.end(), tempBPList1.begin(), tempBPList1.end());
	}

	return newBPList;
}

double Contour::GetExtentAndArea(vector<PointD> pList, Extent& aExtent)
{
	double bArea, minX, minY, maxX, maxY;
	int i;
	PointD aPoint;
	aPoint = pList[0];
	minX = aPoint.X;
	maxX = aPoint.X;
	minY = aPoint.Y;
	maxY = aPoint.Y;
	for (i = 1; i < pList.size(); i++)
	{
		aPoint = pList[i];
		if (aPoint.X < minX)
			minX = aPoint.X;

		if (aPoint.X > maxX)
			maxX = aPoint.X;

		if (aPoint.Y < minY)
			minY = aPoint.Y;

		if (aPoint.Y > maxY)
			maxY = aPoint.Y;
	}

	aExtent.xMin = minX;
	aExtent.yMin = minY;
	aExtent.xMax = maxX;
	aExtent.yMax = maxY;
	bArea = (maxX - minX) * (maxY - minY);

	return bArea;
}