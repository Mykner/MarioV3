#pragma once

//#include <exception>

#include "NaString.h"

class NaException// : public std::exception
{
public:
	NaException(const wchar_t* szFunc, int nLine, const char* szMsg);
	NaException(const wchar_t* szFunc, int nLine, const wchar_t* fmt, ...);

	virtual const char* what() const throw()
	{
		return m_szWhat;
	}

	const wchar_t* m_szFunc;
	int m_nLine;
	char m_szWhat[256];
};