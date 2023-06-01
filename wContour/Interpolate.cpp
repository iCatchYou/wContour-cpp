#include "pch.h"
#include "Interpolate.h"
int Interpolate::rows = 0;
int Interpolate::cols = 0;

void Interpolate::setRowCol(int r, int c)
{
	rows = r;
	cols = c;
}

void Interpolate::CreateGridXY_Delt(double Xlb, double Ylb, double Xrt, double Yrt,
	double XDelt, double YDelt, double* &X, double* &Y)
{
	int i, Xnum, Ynum;
	Xnum = (int)((Xrt - Xlb) / XDelt + 1);
	Ynum = (int)((Yrt - Ylb) / YDelt + 1);
	X = new double[Xnum];
	Y = new double[Ynum];
	for (i = 0; i < Xnum; i++)
	{
		X[i] = Xlb + i * XDelt;
	}
	for (i = 0; i < Ynum; i++)
	{
		Y[i] = Ylb + i * YDelt;
	}
}


void Interpolate::CreateGridXY_Num(double Xlb, double Ylb, double Xrt, double Yrt,
	int Xnum, int Ynum, double* &X, double* &Y)
{
	int i;
	double XDelt, YDelt;
	X = new double[Xnum];
	Y = new double[Ynum];
	XDelt = (Xrt - Xlb) / Xnum;
	YDelt = (Yrt - Ylb) / Ynum;
	for (i = 0; i < Xnum; i++)
	{
		X[i] = Xlb + i * XDelt;
	}
	for (i = 0; i < Ynum; i++)
	{
		Y[i] = Ylb + i * YDelt;
	}
}


double** Interpolate::Interpolation_IDW_Neighbor(double** SCoords, double* X, double* Y,
	int NumberOfNearestNeighbors)
{
	int rowNum = 0, colNum = 0, pNum = cols;
	colNum = cols;
	rowNum = rows;
	//pNum = SCoords.GetLength(1); 
	//ArryUtils::Get2DArryRowCol(SCoords, pNum2, pNum);
	double** GCoords = ArryUtils::create2DArray<double>(rowNum, colNum,0.0);
	//double[, ] GCoords = new double[rowNum, colNum];
	int i, j, p, l, aP;
	double w, SV, SW, aMin;
	int points;
	points = NumberOfNearestNeighbors;
	//object[, ] NW = new object[2, points];
	double** NW = ArryUtils::create2DArray<double>(2, points,0.0);

	//---- Do interpolation
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			GCoords[i][j] = -999.0;
			SV = 0;
			SW = 0;
			for (p = 0; p < points; p++)
			{
				if (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2) == 0)
				{
					GCoords[i, j] = SCoords[2, p];
					break;
				}
				else
				{
					w = 1 / (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2));
					NW[0][p] = w;
					NW[1][p] = p;
				}
			}
			if (GCoords[i][j] == -999.0)
			{
				for (p = points; p < pNum; p++)
				{
					if (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2) == 0)
					{
						GCoords[i, j] = SCoords[2, p];
						break;
					}
					else
					{
						w = 1 / (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2));
						aMin = NW[0][0];
						aP = 0;
						for (l = 1; l < points; l++)
						{
							if (NW[0][l] < aMin)
							{
								aMin = NW[0][l];
								aP = l;
							}
						}
						if (w > aMin)
						{
							NW[0][aP] = w;
							NW[1][aP] = p;
						}
					}
				}
				if (GCoords[i][j] == -999.0)
				{
					for (p = 0; p < points; p++)
					{
						SV += NW[0][p] * SCoords[2][(int)NW[1][p]];
						SW += NW[0][p];
					}
					GCoords[i][j] = SV / SW;
				}
			}
		}
	}

	//---- Smooth with 5 points
	double s = 0.5;
	for (i = 1; i < rowNum - 1; i++)
	{
		for (j = 1; j < colNum - 1; j++)
		{
			GCoords[i][j] = GCoords[i][j] + s / 4 * (GCoords[i + 1][j] + GCoords[i - 1][j] +
				GCoords[i][j + 1] + GCoords[i][j - 1] - 4 * GCoords[i][j]);
		}
	}
	ArryUtils::delete2DArray(NW, 2, points);

	return GCoords;
}


double** Interpolate::Interpolation_IDW_Neighbor(double** SCoords, double* X, double* Y,
	int NumberOfNearestNeighbors, double unDefData)
{
	int rowNum, colNum, pNum = cols;
	//int xLen = ArryUtils::GetArrayLength(X);
	//int yLen = ArryUtils::GetArrayLength(Y);
	colNum = cols;
	rowNum = rows;
	//pNum = SCoords.GetLength(1);
	//ArryUtils::Get2DArryRowCol(SCoords, pNum2, pNum);
	double** GCoords = ArryUtils::create2DArray<double>(rowNum, colNum,0.0);
	//double[, ] GCoords = new double[rowNum, colNum];
	int i, j, p, l, aP;
	double w, SV, SW, aMin;
	int points;
	points = NumberOfNearestNeighbors;
	double* AllWeights = new double[pNum]();
	//object[, ] NW = new object[2, points];
	double** NW = ArryUtils::create2DArray<double>(2, points,0.0);
	int NWIdx;

	//---- Do interpolation with IDW method 
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			GCoords[i][j] = unDefData;
			SV = 0;
			SW = 0;
			NWIdx = 0;
			for (p = 0; p < pNum; p++)
			{
				if (SCoords[2][p] == unDefData)
				{
					AllWeights[p] = -1;
					continue;
				}
				if (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2) == 0)
				{
					GCoords[i][j] = SCoords[2][p];
					break;
				}
				else
				{
					w = 1 / (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2));
					AllWeights[p] = w;
					if (NWIdx < points)
					{
						NW[0][NWIdx] = w;
						NW[1][NWIdx] = p;
					}
					NWIdx += 1;
				}
			}

			if (GCoords[i][j] == unDefData)
			{
				for (p = 0; p < pNum; p++)
				{
					w = AllWeights[p];
					if (w == -1)
						continue;

					aMin = (double)NW[0][0];
					aP = 0;
					for (l = 1; l < points; l++)
					{
						if ((double)NW[0][l] < aMin)
						{
							aMin = (double)NW[0][l];
							aP = l;
						}
					}
					if (w > aMin)
					{
						NW[0][aP] = w;
						NW[1][aP] = p;
					}
				}
				for (p = 0; p < points; p++)
				{
					SV += (double)NW[0][p] * SCoords[2][(int)NW[1][p]];
					SW += (double)NW[0][p];
				}
				GCoords[i][j] = SV / SW;
			}
		}
	}

	//---- Smooth with 5 points
	double s = 0.5;
	for (i = 1; i < rowNum - 1; i++)
	{
		for (j = 1; j < colNum - 1; j++)
			GCoords[i][j] = GCoords[i][j] + s / 4 * (GCoords[i + 1][j] + GCoords[i - 1][j] + GCoords[i][j + 1] +
				GCoords[i][j - 1] - 4 * GCoords[i][j]);

	}

	ArryUtils::delete2DArray(NW, 2, points);

	delete[] AllWeights;

	return GCoords;
}


double** Interpolate::Interpolation_IDW_Radius(double** SCoords, double* X, double* Y,
	int NeededPointNum, double radius, double unDefData)
{
	int rowNum, colNum, pNum = cols;
	//int xLen = ArryUtils::GetArrayLength(X);
	//int yLen = ArryUtils::GetArrayLength(Y);
	colNum = cols;
	rowNum = rows;
	//pNum = SCoords.GetLength(1);
	//ArryUtils::Get2DArryRowCol(SCoords, pNum2, pNum);
	double** GCoords = ArryUtils::create2DArray<double>(rowNum, colNum,0.0);

	//colNum = X.Length;
	//rowNum = Y.Length;
	//pNum = SCoords.GetLength(1);
	//double[, ] GCoords = new double[rowNum, colNum];
	int i, j, p, vNum;
	double w, SV, SW;
	bool ifPointGrid;

	//---- Do interpolation
	for (i = 0; i < rowNum; i++)
	{
		for (j = 0; j < colNum; j++)
		{
			GCoords[i][j] = unDefData;
			ifPointGrid = false;
			SV = 0;
			SW = 0;
			vNum = 0;
			for (p = 0; p < pNum; p++)
			{
				if (SCoords[2][p] == unDefData)
					continue;

				if (SCoords[0][p] < X[j] - radius || SCoords[0][p] > X[j] + radius || SCoords[1][p] < Y[i] - radius ||
					SCoords[1][p] > Y[i] + radius)
					continue;

				if (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2) == 0)
				{
					GCoords[i, j] = SCoords[2, p];
					ifPointGrid = true;
					break;
				}
				else if (sqrt(pow(X[j] - SCoords[0][p], 2) +
					pow(Y[i] - SCoords[1][p], 2)) <= radius)
				{
					w = 1 / (pow(X[j] - SCoords[0][p], 2) + pow(Y[i] - SCoords[1][p], 2));
					SW = SW + w;
					SV = SV + SCoords[2][p] * w;
					vNum += 1;
				}
			}

			if (!ifPointGrid)
			{
				if (vNum >= NeededPointNum)
				{
					GCoords[i][j] = SV / SW;
				}
			}
		}
	}

	//---- Smooth with 5 points
	double s = 0.5;
	for (i = 1; i < rowNum - 1; i++)
	{
		for (j = 1; j < colNum - 2; j++)
		{
			if (GCoords[i][j] == unDefData || GCoords[i + 1][j] == unDefData || GCoords[i - 1][j] ==
				unDefData || GCoords[i][j + 1] == unDefData || GCoords[i][j - 1] == unDefData)
			{
				continue;
			}
			GCoords[i][j] = GCoords[i][j] + s / 4 * (GCoords[i + 1][j] + GCoords[i - 1][j] + GCoords[i][j + 1] +
				GCoords[i][j - 1] - 4 * GCoords[i][j]);
		}
	}

	return GCoords;
}

double** Interpolate::Interpolation_Grid(double** GridData, double* X, double* Y, double unDefData,
	double* &nX, double* &nY)
{
	int xLen = cols;
	int yLen = rows;

	int xNum = xLen;
	int yNum = yLen;
	int nxNum = xLen * 2 - 1;
	int nyNum = yLen * 2 - 1;
	nX = new double[nxNum]();
	nY = new double[nyNum]();
	//double[, ] nGridData = new double[nyNum, nxNum];
	double** nGridData = ArryUtils::create2DArray<double>(nyNum, nxNum,0.0);
	int i, j;
	double a, b, c, d;
	vector<double> dList;
	for (i = 0; i < nxNum; i++)
	{
		if (i % 2 == 0)
			nX[i] = X[i / 2];
		else
			nX[i] = (X[(i - 1) / 2] + X[(i - 1) / 2 + 1]) / 2;
	}
	for (i = 0; i < nyNum; i++)
	{
		if (i % 2 == 0)
			nY[i] = Y[i / 2];
		else
			nY[i] = (Y[(i - 1) / 2] + Y[(i - 1) / 2 + 1]) / 2;
		for (j = 0; j < nxNum; j++)
		{
			if (i % 2 == 0 && j % 2 == 0)
				nGridData[i, j] = GridData[i / 2, j / 2];
			else if (i % 2 == 0 && j % 2 != 0)
			{
				a = GridData[i / 2][(j - 1) / 2];
				b = GridData[i / 2][(j - 1) / 2 + 1];
				dList = vector<double>();
				if (a != unDefData)
					dList.push_back(a);
				if (b != unDefData)
					dList.push_back(b);

				if (dList.size() == 0)
					nGridData[i][j] = unDefData;
				else if (dList.size() == 1)
					nGridData[i][j] = dList[0];
				else
					nGridData[i][j] = (a + b) / 2;
			}
			else if (i % 2 != 0 && j % 2 == 0)
			{
				a = GridData[(i - 1) / 2][j / 2];
				b = GridData[(i - 1) / 2 + 1][j / 2];
				dList = vector<double>();
				if (a != unDefData)
					dList.push_back(a);
				if (b != unDefData)
					dList.push_back(b);

				if (dList.size() == 0)
					nGridData[i][j] = unDefData;
				else if (dList.size() == 1)
					nGridData[i][j] = dList[0];
				else
					nGridData[i][j] = (a + b) / 2;
			}
			else
			{
				a = GridData[(i - 1) / 2][(j - 1) / 2];
				b = GridData[(i - 1) / 2][(j - 1) / 2 + 1];
				c = GridData[(i - 1) / 2 + 1][(j - 1) / 2 + 1];
				d = GridData[(i - 1) / 2 + 1][(j - 1) / 2];
				dList = vector<double>();
				if (a != unDefData)
					dList.push_back(a);
				if (b != unDefData)
					dList.push_back(b);
				if (c != unDefData)
					dList.push_back(c);
				if (d != unDefData)
					dList.push_back(d);

				if (dList.size() == 0)
					nGridData[i][j] = unDefData;
				else if (dList.size() == 1)
					nGridData[i][j] = dList[0];
				else
				{
					double aSum = 0;
					for (double dd : dList)
						aSum += dd;
					nGridData[i][j] = aSum / dList.size();
				}
			}
		}
	}

	return nGridData;
}

double** Interpolate::Cressman(double** stationData, double* X, double* Y, double unDefData)
{
	vector<double> radList{ 10, 7, 4, 2, 1 };
	//radList.AddRange(new double[] { 10, 7, 4, 2, 1 });

	return Cressman(stationData, X, Y, unDefData, radList);
}

double** Interpolate::Cressman(double** stationData, double* X, double* Y, double unDefData, vector<double> radList)
{
	//double** stData = (double[, ])stationData.Clone();
	double** stData = ArryUtils::clone2DArray(stationData,rows,cols);
	int xNum = cols;
	int yNum = rows;
	//int r = 0, c = 0;
	//ArryUtils::Get2DArryRowCol(stData, r, c);
	int pNum = cols;
	//double[, ] gridData = new double[yNum, xNum];
	double** gridData = ArryUtils::create2DArray<double>(yNum, xNum,0.0);
	int irad = radList.size();
	int i, j;

	//Loop through each stn report and convert stn lat/lon to grid coordinates
	double xMin = X[0];
	double xMax = X[xNum - 1];
	double yMin = Y[0];
	double yMax = Y[yNum - 1];
	double xDelt = X[1] - X[0];
	double yDelt = Y[1] - Y[0];
	double x, y;
	double sum = 0, total = 0;
	int stNum = 0;
	for (i = 0; i < pNum; i++)
	{
		x = stData[0][i];
		y = stData[1][i];
		stData[0][i] = (x - xMin) / xDelt;
		stData[1][i] = (y - yMin) / yDelt;
		if (stData[2][i] != unDefData)
		{
			total += stData[2][i];
			stNum += 1;
		}
	}
	total = total / stNum;

	////Initial grid values are average of station reports
	//for (i = 0; i < yNum; i++)
	//{
	//    for (j = 0; j < xNum; j++)
	//    {
	//        gridData[i, j] = sum;
	//    }
	//}

	//Initial the arrays
	double HITOP = -999900000000000000000.0;
	double HIBOT = 999900000000000000000.0;
	//double[, ] TOP = new double[yNum, xNum];
	//double[, ] BOT = new double[yNum, xNum];

	double** TOP = ArryUtils::create2DArray<double>(yNum, xNum,0.0);
	double** BOT = ArryUtils::create2DArray<double>(yNum, xNum,0.0);

	//double[,] GRID = new double[yNum, xNum];
	//int[,] NG = new int[yNum, xNum];
	for (i = 0; i < yNum; i++)
	{
		for (j = 0; j < xNum; j++)
		{
			TOP[i][j] = HITOP;
			BOT[i][j] = HIBOT;
			//GRID[i, j] = 0;
			//NG[i, j] = 0;
		}
	}

	//Initial grid values are average of station reports within the first radius
	double rad;
	if (radList.size() > 0)
		rad = radList[0];
	else
		rad = 4;
	for (i = 0; i < yNum; i++)
	{
		y = (double)i;
		yMin = y - rad;
		yMax = y + rad;
		for (j = 0; j < xNum; j++)
		{
			x = (double)j;
			xMin = x - rad;
			xMax = x + rad;
			stNum = 0;
			sum = 0;
			for (int s = 0; s < pNum; s++)
			{
				double val = stData[2][s];
				double sx = stData[0][s];
				double sy = stData[1][s];
				if (sx < 0 || sx >= xNum - 1 || sy < 0 || sy >= yNum - 1)
					continue;

				if (val == unDefData || sx < xMin || sx > xMax || sy < yMin || sy > yMax)
					continue;

				double dis = sqrt(pow(sx - x, 2) + pow(sy - y, 2));
				if (dis > rad)
					continue;

				sum += val;
				stNum += 1;
				if (TOP[i][j] < val)
					TOP[i][j] = val;
				if (BOT[i][j] > val)
					BOT[i][j] = val;
			}
			if (stNum == 0)
			{
				gridData[i][j] = unDefData;
				//gridData[i, j] = total;
			}
			else
				gridData[i][j] = sum / stNum;
		}
	}

	//Perform the objective analysis
	for (int p = 0; p < irad; p++)
	{
		rad = radList[p];
		for (i = 0; i < yNum; i++)
		{
			y = (double)i;
			yMin = y - rad;
			yMax = y + rad;
			for (j = 0; j < xNum; j++)
			{
				if (gridData[i][j] == unDefData)
					continue;

				x = (double)j;
				xMin = x - rad;
				xMax = x + rad;
				sum = 0;
				double wSum = 0;
				for (int s = 0; s < pNum; s++)
				{
					double val = stData[2][s];
					double sx = stData[0][s];
					double sy = stData[1][s];
					if (sx < 0 || sx >= xNum - 1 || sy < 0 || sy >= yNum - 1)
						continue;

					if (val == unDefData || sx < xMin || sx > xMax || sy < yMin || sy > yMax)
						continue;

					double dis = sqrt(pow(sx - x, 2) + pow(sy - y, 2));
					if (dis > rad)
						continue;

					int i1 = (int)sy;
					int j1 = (int)sx;
					int i2 = i1 + 1;
					int j2 = j1 + 1;
					double a = gridData[i1][j1];
					double b = gridData[i1][j2];
					double c = gridData[i2][j1];
					double d = gridData[i2][j2];
					vector<double> dList;
					if (a != unDefData)
						dList.push_back(a);
					if (b != unDefData)
						dList.push_back(b);
					if (c != unDefData)
						dList.push_back(c);
					if (d != unDefData)
						dList.push_back(d);

					double calVal;
					if (dList.size() == 0)
						continue;
					else if (dList.size() == 1)
						calVal = dList[0];
					else if (dList.size() <= 3)
					{
						double aSum = 0;
						for (double dd : dList)
							aSum += dd;
						calVal = aSum / dList.size();
					}
					else
					{
						double x1val = a + (c - a) * (sy - i1);
						double x2val = b + (d - b) * (sy - i1);
						calVal = x1val + (x2val - x1val) * (sx - j1);
					}
					double eVal = val - calVal;
					double w = (rad * rad - dis * dis) / (rad * rad + dis * dis);
					sum += eVal * w;
					wSum += w;
				}
				if (wSum < 0.000001)
				{
					gridData[i][j] = unDefData;
				}
				else
				{
					double aData = gridData[i][j] + sum / wSum;
					gridData[i][j] = max(BOT[i][j], min(TOP[i][j], aData));
				}
			}
		}
	}

	ArryUtils::delete2DArray(stData, yNum, xNum);
	ArryUtils::delete2DArray(TOP, yNum, xNum);
	ArryUtils::delete2DArray(BOT, yNum, xNum);
	//Return
	return gridData;
}

double** Interpolate::AssignPointToGrid(double** SCoords, double* X, double* Y,
	double unDefData)
{
	int rowNum, colNum, pNum;
	colNum = cols;
	rowNum = rows;
	//int r = 0, c = 0;
	//ArryUtils::Get2DArryRowCol(SCoords, r, c);
	pNum = cols;
	//pNum = SCoords.GetLength(1);
	//double[, ] GCoords = new double[rowNum, colNum];
	double** GCoords = ArryUtils::create2DArray<double>(rowNum, colNum,0.0);
	double dX = X[1] - X[0];
	double dY = Y[1] - Y[0];
	//int[, ] pNums = new int[rowNum, colNum];
	int** pNums = ArryUtils::create2DArray<int>(rowNum, colNum,0);

	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < colNum; j++)
		{
			pNums[i][j] = 0;
			GCoords[i][j] = 0.0;
		}
	}

	for (int p = 0; p < pNum; p++)
	{
		if (DoubleEquals(SCoords[2][p], unDefData))
			continue;

		double x = SCoords[0][p];
		double y = SCoords[1][p];
		if (x < X[0] || x > X[colNum - 1])
			continue;
		if (y < Y[0] || y > Y[rowNum - 1])
			continue;

		int j = (int)((x - X[0]) / dX);
		int i = (int)((y - Y[0]) / dY);
		pNums[i][j] += 1;
		GCoords[i][j] += SCoords[2][p];
	}

	for (int i = 0; i < rowNum; i++)
	{
		for (int j = 0; j < colNum; j++)
		{
			if (pNums[i][j] == 0)
				GCoords[i][j] = unDefData;
			else
				GCoords[i][j] = GCoords[i][j] / pNums[i][j];
		}
	}
	ArryUtils::delete2DArray(GCoords, rowNum, colNum);
	ArryUtils::delete2DArray(pNums, rowNum, colNum);

	return GCoords;
}

bool Interpolate::DoubleEquals(double a, double b)
{
	//if (Math.Abs(a - b) < 0.000001)
	if (abs(a / b - 1) < 0.00000000001)
		return true;
	else
		return false;
}
	