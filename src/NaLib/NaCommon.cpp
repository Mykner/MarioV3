#include "NaCommon.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef WIN32
#include <Windows.h>
#else
#include <string.h>
#include <wchar.h>
#endif


void NaDebugOut(const wchar_t* pszFormat, ...)
{
//#if defined(NDEBUG)
//	return;
//#endif

#ifdef WIN32
	wchar_t str[NA_DEBUGOUT_TEMPBUFFER_SIZE];
	memset(str, 0, sizeof(wchar_t) * NA_DEBUGOUT_TEMPBUFFER_SIZE);

	va_list arglist;
	va_start(arglist, pszFormat);
	size_t size = sizeof(str);
	vswprintf(str, size, pszFormat, arglist);
	va_end(arglist);

	OutputDebugStringW(str);
#else
    // #TODO MultiPlatform DebugOut
    wchar_t str[NA_DEBUGOUT_TEMPBUFFER_SIZE];
    memset(str, 0, sizeof(wchar_t) * NA_DEBUGOUT_TEMPBUFFER_SIZE);
    
    va_list arglist;
    va_start(arglist, pszFormat);
    size_t size = sizeof(str);
    vswprintf(str, size, pszFormat, arglist);
    va_end(arglist);
    
    wprintf(str);
#endif
}