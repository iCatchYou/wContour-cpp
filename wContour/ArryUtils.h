#pragma once
/************************************************************************************
*  __________  _________.____														*
*  \______   \/   _____/|    |				  wCountour  v1.0.0.0					*
*   |     ___/\_____  \ |    |			Designed for generate contour lines			*
*   |    |    /        \|    |___				on Windows (c++)					*
*   |____|   /_______  /|_______ \													*
*                    \/         \/		(C) 2020-2023. All rights reserved.			*
*																					*
*  @file        ArryUtils.h															*
*  @author      YangZe(709102202@qq.com)											*
*  @date        2023/06/01 09:14													*
*																					*
*  @brief       C++ Array Manipulator Class											*
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
#include <vector>
#include <tuple>
#include "PointD.h"
#include <iostream>

using namespace std;
class WCONTOUR_API ArryUtils
{
public:	
	template <typename T>
	static T* create1DArray(int x, T v)
	{
		T* arr = new T[x];
		for (int i = 0; i < x; i++) {
			arr[i] = v;
		}
		return arr;
	}
	template <typename T>
	static void delete1DArray(T* arr)
	{
		if (arr != NULL)
		{
			delete[] arr;
			arr = NULL;
		}
	}

	//创建三维数组
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
	
	//释放三维数组
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

	//创建二维数组
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
	
	//释放二维数组
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
	
	//克隆二维数组
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

	//打印二维数组
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

