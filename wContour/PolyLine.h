#pragma once
#include "defines.h"
#include <string>
#include <vector>
#include "PointD.h"
using namespace std;

class WCONTOUR_API PolyLine
{
public:
	double Value;
	string Type;
	int BorderIdx;
	vector<PointD> PointList;
public:
	PolyLine();
};

