#pragma once
#include <vector>
#include <tuple>
#include "PointD.h"
#include <iostream>
using namespace std;
class ArryUtils
{
public:
	
	template <typename T>
	static T*** create3DArray(int x, int y, int z,T v) {
		T*** arr = new T** [x];
		for (int i = 0; i < x; i++) {
			arr[i] = new T* [y];
			for (int j = 0; j < y; j++) {
				arr[i][j] = new T[z];
				for (int k = 0; k < z; k++) {
					arr[i][j][k] = v;
				}
			}
		}
		return arr;
	}

	template <typename T>
	static void delete3DArray(T*** arr, int x, int y, int z) {
		for (int i = 0; i < x; i++) {
			for (int j = 0; j < y; j++) {
				delete[] arr[i][j];
			}
			delete[] arr[i];
		}
		delete[] arr;
	}

	template <typename T>
	static T** create2DArray(int x, int y,T v) {
		T** arr = new T*[x];
		for (int i = 0; i < x; i++) {
			arr[i] = new T[y];
			for (int j = 0; j < y; j++) {
				arr[i][j] = v;
			}
		}
		return arr;
	}
	
	template <typename T>
	static void delete2DArray(T** arr, int x, int y) {
		if (arr != NULL)
		{
			for (int i = 0; i < x; i++) {
				if (arr[i] != NULL)
				{
					delete[] arr[i];
					arr[i] = NULL;
				}
				
			}
		}
		
		delete[] arr;
		arr = NULL;
	}
	
	/*template <typename T>
	static int GetArrayLength(T* a)
	{
		return sizeof(a) / sizeof(a[0]);
	}

	template <typename T>
	static void Get2DArryRowCol(T ** a,int &rows,int &cols)
	{
		rows = sizeof(a) / sizeof(a[0]);  // 行数
		cols = sizeof(a[0]) / sizeof(a[0][0]);  // 列数
	}*/

	template <typename T>
	static T** clone2DArray(T **arr, int rows, int cols)
	{
		//int rows = 0, cols = 0;
		//Get2DArryRowCol(arr,rows,cols );
		T** a = new T* [rows];  // n 表示行数
		for (int i = 0; i < rows; i++) {
			a[i] = new T[cols];  // m 表示列数
			for (int j = 0; j < cols; j++) {
				a[i][j] = arr[i][j];  // b 表示要复制的二维数组
			}
		}
		return a;
	}

	template <typename T>
	static void print2DArray(T** arr, int rows, int cols)
	{
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				std::cout << arr[i][j] << " ";
			}
			std::cout << std::endl;
		}
	}

};

