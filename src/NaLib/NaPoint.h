#pragma once

#include "NaCommon.h"


class NaPoint
{
public:
	NaPoint()
	{
		x = 0;
		y = 0;
	};
	NaPoint(int x2, int y2)
	{
		x = x2;
		y = y2;
	};
	NaPoint(const NaPoint &pt)
	{
		x = pt.x;
		y = pt.y;
	};
	~NaPoint() {};

	operator POINT ()
	{
		POINT pt;
		pt.x = x;
		pt.y = y;

		return pt;
	};
	operator LPPOINT ()
	{
		return (LPPOINT)this;
	};
	NaPoint& operator =(const POINT& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	};
	
	void Offset(int x2, int y2)
	{
		x += x2;
		y += y2;
	};
	
	int x;
	int y;
};

template <typename T>
class NaPointT
{
public:
	NaPointT()
	{
		x = 0;
		y = 0;
	};
	NaPointT(T x2, T y2)
	{
		x = x2;
		y = y2;
	};
	NaPointT(const NaPointT<T> &pt)
	{
		x = pt.x;
		y = pt.y;
	};
	~NaPointT() {};

	operator POINT ()
	{
		POINT pt;
		pt.x = x;
		pt.y = y;

		return pt;
	};
	operator LPPOINT ()
	{
		return (LPPOINT)this;
	};
	/*
	NaPointT& operator =(const POINT& pt)
	{
		x = pt.x;
		y = pt.y;
		return *this;
	};
	*/

	void Offset(int x2, int y2)
	{
		x += x2;
		y += y2;
	};

	T x;
	T y;
};