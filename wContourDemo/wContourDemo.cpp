// wContourDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "Border.h"
#include "Polygon.h"
#include "PolyLine.h"
#include "EndPoint.h"
#include <fstream>
#include <algorithm>
#include "Contour.h"

#include "ArryUtils.h"

using namespace std;


std::vector<std::string> vStringSplit(const std::string& s, const std::string& delim)
{
	std::vector<std::string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}

std::vector<std::vector<std::string>> parseCSV(const std::string& fileName, int startIndex, std::string splitCtr)
{
	std::vector<std::vector<std::string>> result;
	if (startIndex < 0)
	{
		return result;
	}

	ifstream inFile(fileName);
	if (inFile)
	{
		string strLine;
		int count = 0;
		//跳过
		while (count < startIndex)
		{
			if (!getline(inFile, strLine))
			{
				return result;
			}
			count++;
		}

		while (getline(inFile, strLine)) // line中不包括每行的换行符
		{
			std::vector<std::string> v_str;
			v_str = vStringSplit(strLine, splitCtr);
			result.push_back(v_str);
		}
	}
	return result;
}

std::vector<double> GetDiffData(string filePath, int startLine = 0)
{
	std::vector<double> datas;

	std::vector<std::vector<std::string>> ds = parseCSV(filePath, startLine, ",");
	for (std::vector<std::string> line : ds)
	{
		if (line.size() >= 4)
		{
			datas.push_back( stod( line.at(3)));
		}
	}	

	return datas;
}

std::vector<double> ConvertGrids(std::vector<double> grid, int n)
{
	int m = grid.size() / n;
	//Console.WriteLine(n + ":" + m);
	for (int i = 0; i < m / 2; i++)
	{
		for (int j = 0; j < n; j++)
		{
			int k = i * n + j;
			int l = (m - i - 1) * n + j;
			double temp = grid[k];
			grid[k] = grid[l];
			grid[l] = temp;
		}
	}
	return grid;
}

void GetRowColById(int gridId, int totalCols, int &row, int &col)
{
	row = gridId / totalCols; // 计算行号
	col = gridId % totalCols; // 计算列号
}


void ClearObjects()
{
	//_discreteData = null;
	//_gridData = NULL;
	//_borders.clear();
	//_contourLines.clear();
	//_contourPolygons = new List<Polygon>();
	//_clipLines.clear();
	//_clipContourLines.clear();
	//_clipContourPolygons = new List<Polygon>();
	//_mapLines = new List<List<PointD>>();
	//_legendPolygons = new List<Legend.lPolygon>();
	//_streamLines = new List<PolyLine>();
}

void DoContour()
{
	double filterValue = 1E-1;
	int bandCont = 20;
	int contourNum = 20;
	int cols = 101;
	int rows = 101;
	int allPt = cols * rows;

	double** _gridData = NULL;
	double* _X = NULL;
	double* _Y = NULL;
	double* _CValues = NULL;

	vector<PolyLine> _contourLines;
	vector<Border> _borders;
	vector<WPolygon> _contourPolygons;
	//vector<WPolygon> _clipContourPolygons;
	//vector<PolyLine> _clipContourLines;
	//vector<vector<PointD>> _clipLines;

	//double _minX = 0;
	//double _minY = 0;
	//double _maxX = 0;
	//double _maxY = 0;
	//double _scaleX = 1.0;
	//double _scaleY = 1.0;

	double _min = 0;
	double _max = 0;

	for (int f = 1; f <= bandCont; f++)
	{
		//ClearObjects();

		//----------------------------------------------Init
		_gridData = ArryUtils::create2DArray<double>(rows, cols, 0.0);

		vector<double> datas1 = GetDiffData("E:/code/Cluster2Redis/data/rf/diff/map/" + std::to_string(f) + ".csv", 1);
		vector<double> datas = ConvertGrids(datas1, cols);

		_min = *(std::min_element(datas.begin(), datas.end()));
		_max = *(std::max_element(datas.begin(), datas.end()));
		for (int index = 0; index < datas.size(); index++)
		{
			int r = 0, c = 0;
			GetRowColById(index, cols, r, c);
			_gridData[r][c] = datas[index];
		}
				
		_X = ArryUtils::create1DArray<double>(cols,0.0);
		_Y = ArryUtils::create1DArray<double>(rows, 0.0);
		for (int i = 0; i < cols; i++)
		{
			_X[i] = i * 1;
		}

		for (int i = 0; i < rows; i++)
		{
			_Y[i] = i * 1;
		}

		_CValues = ArryUtils::create1DArray<double>(contourNum, 0.0);
		//_colors = new Color[contourNum];
		double step = (_max - _min) / contourNum;
		for (int i = 0; i < contourNum; i++)
		{
			_CValues[i] = _min + step * i;
			//_colors[i] = ColorTranslator.FromHtml(GetColor(_CValues[i], _min, _max));
		}

		double XDelt = 0;
		double YDelt = 0;
		XDelt = _X[1] - _X[0];
		YDelt = _Y[1] - _Y[0];

		//int[, ] S1 = new int[1, 1];
		int** S1 = ArryUtils::create2DArray<int>(1, 1, 0);
		double _undefData = -9999.0;

		Contour::setRowCol(rows, cols);

		_borders = Contour::TracingBorders(_gridData, _X, _Y, S1, _undefData);
		//----------------------------------------------TracingContourLines
		_contourLines = Contour::TracingContourLines(_gridData, _X, _Y, contourNum, _CValues, _undefData, _borders, S1);
		//---------------------------------------------------------SmoothLines
		_contourLines = Contour::SmoothLines(_contourLines);
		//---------------------------------------------------------TracingPolygons
		_contourPolygons = Contour::TracingPolygons(_gridData, _contourLines, _borders, _CValues, contourNum);		

		//-------------------------------------Clear
		ArryUtils::delete2DArray(_gridData, rows, cols);
		ArryUtils::delete1DArray(_X);
		ArryUtils::delete1DArray(_Y);
		ArryUtils::delete1DArray(_CValues);
		ArryUtils::delete2DArray(S1, rows, cols);
		_borders.clear();
		_contourLines.clear();
		_contourPolygons.clear();
	}
}

int main()
{
	DoContour();

    std::cout << "Hello World!\n";
}

