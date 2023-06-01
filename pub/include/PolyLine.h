#pragma once
#include <string>
#include <vector>
#include "PointD.h"
using namespace std;

class  PolyLine
{
public:
	double Value;
	string Type;
	int BorderIdx;
	vector<PointD> PointList;
public:
	PolyLine();
};

