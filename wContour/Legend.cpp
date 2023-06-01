#include "pch.h"
#include "Legend.h"
#include "ArryUtils.h"

Legend::Legend()
{

}

vector<lPolygon> Legend::CreateLegend(legendPara aLegendPara)
{
	vector<lPolygon> polygonList;
	vector<PointD> pList;
	lPolygon aLPolygon;
	PointD aPoint;
	int i, pNum;
	double aLength;
	bool ifRectangle;
	int length = aLegendPara.contourValues.size();//ArryUtils::GetArrayLength(aLegendPara.contourValues);
	pNum = length + 1;
	aLength = aLegendPara.length / pNum;
	if (aLegendPara.isVertical)
	{
		for (i = 0; i < pNum; i++)
		{
			pList = vector<PointD>();
			ifRectangle = true;
			if (i == 0)
			{
				aLPolygon.value = aLegendPara.contourValues[0];
				aLPolygon.isFirst = true;
				if (aLegendPara.isTriangle)
				{
					aPoint = PointD();
					aPoint.X = aLegendPara.startPoint.X + aLegendPara.width / 2;
					aPoint.Y = aLegendPara.startPoint.Y;
					pList.push_back(aPoint);
					aPoint = PointD();
					aPoint.X = aLegendPara.startPoint.X + aLegendPara.width;
					aPoint.Y = aLegendPara.startPoint.Y + aLength;
					pList.push_back(aPoint);
					aPoint = PointD();
					aPoint.X = aLegendPara.startPoint.X;
					aPoint.Y = aLegendPara.startPoint.Y + aLength;
					pList.push_back(aPoint);
					ifRectangle = false;
				}
			}
			else
			{
				aLPolygon.value = aLegendPara.contourValues[i - 1];
				aLPolygon.isFirst = false;
				if (i == pNum - 1)
				{
					if (aLegendPara.isTriangle)
					{
						aPoint = PointD();
						aPoint.X = aLegendPara.startPoint.X;
						aPoint.Y = aLegendPara.startPoint.Y + i * aLength;
						pList.push_back(aPoint);
						aPoint = PointD();
						aPoint.X = aLegendPara.startPoint.X + aLegendPara.width;
						aPoint.Y = aLegendPara.startPoint.Y + i * aLength;
						pList.push_back(aPoint);
						aPoint = PointD();
						aPoint.X = aLegendPara.startPoint.X + aLegendPara.width / 2;
						aPoint.Y = aLegendPara.startPoint.Y + (i + 1) * aLength;
						pList.push_back(aPoint);
						ifRectangle = false;
					}
				}
			}

			if (ifRectangle)
			{
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X;
				aPoint.Y = aLegendPara.startPoint.Y + i * aLength;
				pList.push_back(aPoint);
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X + aLegendPara.width;
				aPoint.Y = aLegendPara.startPoint.Y + i * aLength;
				pList.push_back(aPoint);
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X + aLegendPara.width;
				aPoint.Y = aLegendPara.startPoint.Y + (i + 1) * aLength;
				pList.push_back(aPoint);
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X;
				aPoint.Y = aLegendPara.startPoint.Y + (i + 1) * aLength;
				pList.push_back(aPoint);
			}

			pList.push_back(pList[0]);
			aLPolygon.pointList = pList;

			polygonList.push_back(aLPolygon);
		}
	}
	else
	{
		for (i = 0; i < pNum; i++)
		{
			pList = vector<PointD>();
			ifRectangle = true;
			if (i == 0)
			{
				aLPolygon.value = aLegendPara.contourValues[0];
				aLPolygon.isFirst = true;
				if (aLegendPara.isTriangle)
				{
					aPoint = PointD();
					aPoint.X = aLegendPara.startPoint.X;
					aPoint.Y = aLegendPara.startPoint.Y + aLegendPara.width / 2;
					pList.push_back(aPoint);
					aPoint = PointD();
					aPoint.X = aLegendPara.startPoint.X + aLength;
					aPoint.Y = aLegendPara.startPoint.Y;
					pList.push_back(aPoint);
					aPoint = PointD();
					aPoint.X = aLegendPara.startPoint.X + aLength;
					aPoint.Y = aLegendPara.startPoint.Y + aLegendPara.width;
					pList.push_back(aPoint);
					ifRectangle = false;
				}
			}
			else
			{
				aLPolygon.value = aLegendPara.contourValues[i - 1];
				aLPolygon.isFirst = false;
				if (i == pNum - 1)
				{
					if (aLegendPara.isTriangle)
					{
						aPoint = PointD();
						aPoint.X = aLegendPara.startPoint.X + i * aLength;
						aPoint.Y = aLegendPara.startPoint.Y;
						pList.push_back(aPoint);
						aPoint = PointD();
						aPoint.X = aLegendPara.startPoint.X + (i + 1) * aLength;
						aPoint.Y = aLegendPara.startPoint.Y + aLegendPara.width / 2;
						pList.push_back(aPoint);
						aPoint = PointD();
						aPoint.X = aLegendPara.startPoint.X + i * aLength;
						aPoint.Y = aLegendPara.startPoint.Y + aLegendPara.width;
						pList.push_back(aPoint);
						ifRectangle = false;
					}
				}
			}

			if (ifRectangle)
			{
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X + i * aLength;
				aPoint.Y = aLegendPara.startPoint.Y;
				pList.push_back(aPoint);
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X + (i + 1) * aLength;
				aPoint.Y = aLegendPara.startPoint.Y;
				pList.push_back(aPoint);
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X + (i + 1) * aLength;
				aPoint.Y = aLegendPara.startPoint.Y + aLegendPara.width;
				pList.push_back(aPoint);
				aPoint = PointD();
				aPoint.X = aLegendPara.startPoint.X + i * aLength;
				aPoint.Y = aLegendPara.startPoint.Y + aLegendPara.width;
				pList.push_back(aPoint);
			}

			pList.push_back(pList[0]);
			aLPolygon.pointList = pList;

			polygonList.push_back(aLPolygon);
		}
	}

	return polygonList;
}