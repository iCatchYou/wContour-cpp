#pragma once

#include <vector>
#include "BorderLine.h"
using namespace std;
class Border
{
public:
	
	/// <summary>
	/// Line list
	/// </summary>
	vector<BorderLine> LineList;
public:
	/// <summary>
	/// Constructor
	/// </summary>
	Border();

	/// <summary>
	/// Get line number
	/// </summary>
	int LineNum();

};

