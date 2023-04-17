#include "NaString.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <cassert>
#include <iostream>

#include "NaLeakDetector.h"

NaString::NaString() : m_pBuf(nullptr), m_nBufLen(0), m_nLen(0), m_pCstrBuf(nullptr)
{
}

NaString::NaString(const char * lpsz)
{
	m_pBuf = nullptr;
	m_pCstrBuf = nullptr;
	m_nBufLen = 0;
	m_nLen = 0;

	SetBuf(lpsz);
}

NaString::NaString(const wchar_t * lpsz)
{
	m_pBuf = nullptr;
	m_pCstrBuf = nullptr;
	m_nBufLen = 0;
	m_nLen = 0;

	SetBuf(lpsz);
}

NaString::NaString(const NaString & nstr)
{
	m_pBuf = nullptr;
	m_pCstrBuf = nullptr;
	m_nBufLen = 0;
	m_nLen = 0;

	SetBuf((wchar_t*)nstr.m_pBuf);
}

#if defined(USE_V8)
NaString::NaString(Local<String>& str)
{
	m_pBuf = nullptr;
	m_pCstrBuf = nullptr;
	m_nBufLen = 0;
	m_nLen = 0;

	String::Utf8Value value(str);
	SetBuf(*value);
}

NaString::NaString(String::Value & str)
{
	m_pBuf = nullptr;
	m_pCstrBuf = nullptr;
	m_nBufLen = 0;
	m_nLen = 0;

	SetBuf((const wchar_t*)*str);
}
#endif

NaString::~NaString()
{
	DeallocBuf();
	if (m_pCstrBuf != nullptr)
	{
		delete[] m_pCstrBuf;
		m_pCstrBuf = nullptr;
	}
}

const NaString& NaString::operator=(const wchar_t *lpsz)
{
	SetBuf((wchar_t*)lpsz);
	return *this;
}

const NaString& NaString::operator=(const NaString &str)
{
	SetBuf((wchar_t*)str.m_pBuf);
	return *this;
}

const NaString& NaString::operator+=(const wchar_t *lpsz)
{
	if (m_pBuf == nullptr || m_nLen == 0)
		SetBuf(lpsz);
	else
		Format(L"%ls%ls", (wchar_t*)m_pBuf, lpsz);
	return *this;
}

const NaString& NaString::operator+=(NaString &str)
{
	if (m_pBuf == nullptr || m_nLen == 0)
		SetBuf(str.wstr());
	else
		Format(L"%ls%ls", (wchar_t*)m_pBuf, str.wstr());
	return *this;
}

bool NaString::operator==(const char * lpsz)
{
	return Compare(NaString(lpsz).wstr()) == 0;
}

bool NaString::operator==(const wchar_t * lpsz)
{
	return Compare(lpsz) == 0;
}

bool NaString::operator==(NaString & str)
{
	return Compare(str.wstr()) == 0;
}

bool NaString::operator<(const NaString & str) const
{
	return wcscmp((wchar_t*)m_pBuf, (wchar_t*)str.m_pBuf) < 0;
}

wchar_t NaString::operator[](int index)
{
	if (index >= 0 && index < m_nLen)
		return *((wchar_t*)m_pBuf + index);

	return L'\0';
}

NaString::operator const char* () const
{
	char *sz = nullptr;
	ConvertWCharToChar((wchar_t*)m_pBuf, &sz);

	return sz;
}

NaString::operator const wchar_t* () const
{
	if (m_pBuf == nullptr)
		return L"";
	return (const wchar_t*)m_pBuf;
}

void NaString::ToLower()
{
#ifdef WIN32
	_wcslwr_s((wchar_t*)m_pBuf, m_nLen+1);
#else
#endif
}

void NaString::ToUpper()
{
#ifdef WIN32
	_wcsupr_s((wchar_t*)m_pBuf, m_nLen+1);
#else
#endif
}

int NaString::GetLength()
{
	return m_nLen;
}

int NaString::GetBufferSize()
{
	return m_nBufLen;
}

int NaString::Compare(const wchar_t * lpsz)
{
	int arglen = wcslen(lpsz);
	int minlen = m_nLen < arglen ? m_nLen : arglen;

	if (m_pBuf == nullptr)
		return wcscmp(L"", lpsz);

	return wcscmp((wchar_t*)m_pBuf, lpsz);
}

int NaString::CompareNoCase(const wchar_t * lpsz)
{
	NaString str(*this);
	str.ToLower();
	
	NaString str2(lpsz);
	str2.ToLower();

	return str.Compare(str2);
}

int NaString::Find(const wchar_t * ch, int begin /*= 0*/)
{
	int itemlen = wcslen(ch);
	if (m_nLen < itemlen)
		return -1;

	for (int i = begin; i < m_nLen - itemlen + 1; i++)
	{
		bool bMatch = true;
		for (int j = 0; j < itemlen; j++)
		{
			if (*((wchar_t*)m_pBuf + i + j) != *(ch + j))
			{
				bMatch = false;
				break;
			}
		}

		if (bMatch == true)
			return i;
	}
	return -1;
}

NaString NaString::Left(int len)
{
	if (len >= m_nLen)
	{
		NaString strRet((wchar_t*)this->m_pBuf);
		return strRet;
	}

	NaString strRet;
	unsigned char *buf;
	int nNewLen = len;
	int nNewBufLen = sizeof(wchar_t) * (len + 1);
	buf = new unsigned char[nNewBufLen];

	memcpy(buf, this->m_pBuf, nNewBufLen);
	*((wchar_t*)buf + len) = 0;

	strRet.SetBuf((wchar_t*)buf);
	delete[] buf;

	return strRet;
}

NaString NaString::Mid(int index, int len /*= -1*/)
{
	if (index == 0 && len == -1)
	{
		NaString strRet((wchar_t*)this->m_pBuf);
		return strRet;
	}

	if (len == -1)
		len = m_nLen - index;
	if (index + len > m_nLen)
		len = m_nLen - index;
	if (len <= 0)
		return L"";

	NaString strRet;
	unsigned char *buf;
	int nNewBufLen = sizeof(wchar_t) * (len + 1);
	buf = new unsigned char[nNewBufLen];

	memcpy(buf, (wchar_t*)(this->m_pBuf) + index, nNewBufLen);
	*((wchar_t*)buf + len) = 0;

	strRet.SetBuf((wchar_t*)buf);
	delete[] buf;

	return strRet;
}

NaString NaString::Right(int len)
{
	if (len >= m_nLen)
	{
		NaString strRet((wchar_t*)this->m_pBuf);
		return strRet;
	}

	int index = m_nLen - len;

	NaString strRet;
	unsigned char *buf;
	int nNewBufLen = sizeof(wchar_t) * (len + 1);
	buf = new unsigned char[nNewBufLen];

	memcpy(buf, (wchar_t*)(this->m_pBuf) + index, nNewBufLen);
	*((wchar_t*)buf + len) = 0;

	strRet.SetBuf((wchar_t*)buf);
	delete[] buf;

	return strRet;
}

NaStrArray NaString::Split(wchar_t *ch)
{
	NaStrArray ar;

	int from = 0;
	bool bBreak = false;

	if (GetLength() == 0)
		return ar;

	while (!bBreak)
	{
		NaString token;
		int index = this->Find(ch, from);
		if (index < 0)
		{
			bBreak = true;
			token = this->Mid(from);
		}
		else
		{
			token = this->Mid(from, index - from);
		}

		ar.Add(token);
		from = (index + wcslen(ch));
	}

	return ar;
}

int NaString::ReplaceAll(wchar_t * from, wchar_t * to)
{
	NaString temp(L"");
	int begin = 0;

	while (true)
	{
		int index = Find(from, begin);
		if (index < 0)
		{
			temp += Mid(begin, m_nLen - begin);
			break;
		}

		temp += Mid(begin, index - begin);
		temp += to;

		begin = index + (int)wcslen(from);
	}

	SetBuf(temp.wstr());

	return 0;
}

const wchar_t * NaString::wstr() const
{
	if (m_pBuf == nullptr)
		return L"";
	return (const wchar_t*)m_pBuf;
}

const char * NaString::cstr()
{
	if (m_pCstrBuf != nullptr)
	{
		delete[] m_pCstrBuf;
		m_pCstrBuf = nullptr;
	}

	ConvertWCharToChar((wchar_t*)m_pBuf, &m_pCstrBuf);

	return m_pCstrBuf;
}

wchar_t NaString::GetLast()
{
	if (m_nLen <= 0)
		return 0;

	return *((wchar_t*)m_pBuf + m_nLen);
}

int NaString::ToInt() const
{
	return std::stoi(wstr());
}

int NaString::ConvertWCharToChar(const wchar_t* wstr, char** str)
{
#ifdef WIN32
	int nChars = WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wstr, -1, NULL, 0, 0, 0);
	if (nChars == 0)
	{
		*str = new char[1];
		*(*str) = '\0';
		return 0;
	}

	*str = new char[nChars];
	WideCharToMultiByte(CP_ACP, 0, (wchar_t*)wstr, -1, *str, nChars, 0, 0);
#else
    int nChars = wcstombs(nullptr, wstr, 0);
    *str = new char[nChars + 1];
    
    size_t szTest = wcstombs(*str, wstr, nChars);
    *((char*)(*str) + nChars) = '\0';
    
    //NaDebugOut(L"* ConvWCharToChar\n* (1) %ls\n", wstr);
    //printf("* (2) %s\n", *str);
#endif
    
	return nChars;
}

int NaString::ConvertCharToWChar(const char* str, wchar_t** wstr)
{
#ifdef WIN32
    // MBTWC = contains end byte(zero)
	int nWChars = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	if (nWChars == 0)
	{
		*wstr = new wchar_t[1];
		*(*wstr) = L'\0';
		return 0;
	}

	*wstr = new wchar_t[nWChars];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, (wchar_t*)*wstr, nWChars);
#else
    // strlen = just text length
    int nWChars = strlen(str) + 1;
    
    *wstr = new wchar_t[nWChars];
    mbstowcs((wchar_t*)*wstr, str, nWChars);
    
    *((*wstr) + nWChars - 1) = L'\0';
#endif
    
	return nWChars - 1;
}

const NaString & NaString::SetBuf(const wchar_t* lpsz, int len)
{
	unsigned char* pCurBuf = m_pBuf;

	if (lpsz == nullptr)
	{
		m_nLen = 0;
		m_nBufLen = (int)(sizeof(wchar_t) * (m_nLen + 1));
		AllocBuf(m_nBufLen);
		*((wchar_t*)m_pBuf + m_nLen) = 0;
	}
	else
	{
		if (len < 0)
		{
			m_nLen = wcslen(lpsz);
			m_nBufLen = (int)(sizeof(wchar_t) * (m_nLen + 1));
		}

		AllocBuf(m_nBufLen);
		memcpy(m_pBuf, lpsz, m_nBufLen);

		*((wchar_t*)m_pBuf + m_nLen) = 0;
	}
	
	delete pCurBuf;

	return *this;
}

const NaString & NaString::SetBuf(const char * sz, int len)
{
	DeallocBuf();

	// convert to wstr
	int nLen = ConvertCharToWChar(sz, (wchar_t**)&m_pBuf);
	m_nLen = nLen;
	m_nBufLen = sizeof(wchar_t) * (nLen + 1);

	return *this;
}

const wchar_t* NaString::GetBuf()
{
	return reinterpret_cast<wchar_t*>(m_pBuf);
}

void NaString::AllocBuf(int len)
{
	if (len < 0)
		assert(0);

	m_pBuf = new unsigned char[len];
}

void NaString::DeallocBuf(unsigned char *pBuf)
{
	if (pBuf == nullptr)
		pBuf = m_pBuf;

	if (pBuf)
	{
		// for debug
		//OutputDebugStringW(L"DeleteBuf: ");
		//OutputDebugStringW((wchar_t*)m_pBuf);
		//OutputDebugStringW(L"\n");

		delete[] pBuf;
		pBuf = nullptr;
	}
}

//
// String Array
// 2016.06.17
//
NaStrArray::NaStrArray()
{
}

NaStrArray::~NaStrArray()
{
	m_Array.clear();
}

NaStrArray & NaStrArray::operator=(NaStrArray &str)
{
    m_Array.clear();

    for (int i = 0; i < str.GetCount(); i++)
        m_Array.push_back(str[i]);

    return *this;
}

NaString NaStrArray::operator[](int nIndex)
{
	std::list<NaString>::iterator it = m_Array.begin();
	for (int i=0; it != m_Array.end(); ++it,++i)
	{
		if (i == nIndex)
			return (*it);
	}

	return NaString();
}

void NaStrArray::Set(int nIndex, NaString str)
{
	std::list<NaString>::iterator it = m_Array.begin();
	for (int i = 0; it != m_Array.end(); ++it, ++i)
	{
		if (i == nIndex)
			(*it) = str;
	}
}

int NaStrArray::Add(NaString str)
{
	m_Array.push_back(str);
	return GetCount();
}

int NaStrArray::Remove(int nIndex)
{
	std::list<NaString>::iterator it = m_Array.begin();
	for (int i = 0; it != m_Array.end(); ++it, ++i)
	{
		if (i == nIndex)
		{
			m_Array.erase(it);
			return i;
		}
	}
	return -1;
}

int NaStrArray::GetCount()
{
	return m_Array.size();
}

int NaStrArray::Find(NaString str)
{
	std::list<NaString>::iterator it = m_Array.begin();
	for (int i = 0; it != m_Array.end(); ++it, ++i)
	{
		if (str.Compare(*it) == 0)
			return i;
	}
	return -1;
}

NaString NaStrArray::Join(wchar_t * ch)
{
	int nCnt = GetCount();
	NaString str(L"");
	std::list<NaString>::iterator it = m_Array.begin();
	for (int i = 0; it != m_Array.end(); ++it, ++i)
	{
		str += *it;
		if (i < nCnt-1)
			str += ch;
	}

	return str;
}

NaString NaStrArray::Pop()
{
	NaString strRet(L"");
	int nCnt = GetCount();
	std::list<NaString>::iterator it = m_Array.begin();
	for (int i = 0; it != m_Array.end(); ++it, ++i)
	{
		if (i == nCnt - 1)
		{
			strRet = *it;
			m_Array.erase(it);
			break;
		}		
	}
	return strRet;
}
