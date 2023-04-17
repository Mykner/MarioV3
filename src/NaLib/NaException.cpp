#include "NaException.h"

NaException::NaException(const wchar_t * szFunc, int nLine, const char* szMsg)
{
	m_szFunc = szFunc;
	m_nLine = nLine;

	sprintf_s(m_szWhat, "%s", szMsg);
}

NaException::NaException(const wchar_t * szFunc, int nLine, const wchar_t * fmt, ...)
{
	m_szFunc = szFunc;
	m_nLine = nLine;

	const int nBufSize = 5 * NASTRING_FORMAT_BUFFER_SIZE;
	wchar_t *buf = new wchar_t[nBufSize];
	memset(buf, 0, sizeof(wchar_t) * nBufSize);

	va_list arglist;
	va_start(arglist, fmt);
#ifdef WIN32
	vswprintf_s(buf, nBufSize, fmt, arglist);
#else
	vswprintf(buf, nBufSize, fmt, arglist);
#endif
	va_end(arglist);

	// Oops.
	sprintf_s(m_szWhat, "%s", NaString(buf).cstr());
	delete[] buf;
}
