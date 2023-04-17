#pragma once

#include "NaCommon.h"

class NaRect
{
public:
	NaRect()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	};
	NaRect(int l, int t, int r, int b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	};
	NaRect(RECT &rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
	};
    NaRect(RECT rc)
    {
        left = rc.left;
        top = rc.top;
        right = rc.right;
        bottom = rc.bottom;
    };
	~NaRect() {};

	operator RECT ()
	{
		RECT rc;
		rc.left = left;
		rc.top = top;
		rc.right = right;
		rc.bottom = bottom;

		return rc;
	};
	operator LPRECT ()
	{
		return (LPRECT)this;
	};
	NaRect& operator =(const RECT& rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
		return *this;
	};
	int IsOverlapped(NaRect &rhs)
	{
		return !(rhs.left > this->right
			|| rhs.right < this->left
			|| rhs.top > this->bottom
			|| rhs.bottom < this->top);

		return false;
	};
	int Width()
	{
		return (right - left);
	};
	int Height() 
	{
		return (bottom - top);
	};
	int Center()
	{
		return (left + (Width() / 2));
	};
	int Middle()
	{
		return (top + (Height() / 2));
	};
	void Offset(int x, int y)
	{
		left += x;
		top += y;
		right += x;
		bottom += y;
	};
	void Scale(int x, int y)
	{
		left *= x;
		top *= y;
		right *= x;
		bottom *= y;
	};
	void Inflate(int amount)
	{
		left -= amount;
		top -= amount;
		right += amount;
		bottom += amount;
	};
	void Deflate(int amount)
	{
		left += amount;
		top += amount;
		right -= amount;
		bottom -= amount;
	};
	bool ptInRect(int x, int y)
	{
		if (x >= left && x <= right &&
			y >= top && y <= bottom)
			return true;
		return false;
	};
	int left;
	int top;
	int right;
	int bottom;
};

template <typename T>
class NaRectT
{
public:
	NaRectT()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	};
	NaRectT(T l, T t, T r, T b)
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	};
	NaRectT(RECT &rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
	};
	~NaRectT() {};

	operator RECT ()
	{
		RECT rc;
		rc.left = left;
		rc.top = top;
		rc.right = right;
		rc.bottom = bottom;

		return rc;
	};
	operator LPRECT ()
	{
		return (LPRECT)this;
	};
	NaRectT& operator =(const RECT& rc)
	{
		left = rc.left;
		top = rc.top;
		right = rc.right;
		bottom = rc.bottom;
		return *this;
	};
	int IsOverlapped(NaRectT<T> &rhs)
	{
		return !(rhs.left > this->right
			|| rhs.right < this->left
			|| rhs.top > this->bottom
			|| rhs.bottom < this->top);

		return false;
	};
	int Width()
	{
		return (right - left);
	};
	int Height()
	{
		return (bottom - top);
	};
	int Center()
	{
		return (left + (Width() / 2));
	};
	int Middle()
	{
		return (top + (Height() / 2));
	};
	void Offset(T x, T y)
	{
		left += x;
		top += y;
		right += x;
		bottom += y;
	};
	void Scale(T x, T y)
	{
		left *= x;
		top *= y;
		right *= x;
		bottom *= y;
	};
	void Inflate(T amount)
	{
		left -= amount;
		top -= amount;
		right += amount;
		bottom += amount;
	};
	void Deflate(T amount)
	{
		left += amount;
		top += amount;
		right -= amount;
		bottom -= amount;
	};
	bool ptInRect(T x, T y)
	{
		if (x >= left && x <= right &&
			y >= top && y <= bottom)
			return true;
		return false;
	};
	T left;
	T top;
	T right;
	T bottom;
};