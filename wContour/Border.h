#pragma once

#include "defines.h"
#include <vector>
#include "BorderLine.h"
using namespace std;
class WCONTOUR_API Border
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

