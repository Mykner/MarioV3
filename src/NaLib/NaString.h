// 2012.11.24 neoarc first draft
// 2016.06.02 neoarc remake 

#pragma once

#include <string>
#include "NaCommon.h"

#include "fmt/include/fmt/printf.h"

#define NASTRING_FORMAT_BUFFER_SIZE	65535

#if defined(USE_V8)
#include "v8.h"
using namespace v8;
#endif

#include <list>

class NaStrArray;
class NaString
{
private:
	union {
		wchar_t* m_pDebugBuf; // for debug
		unsigned char *m_pBuf;
	};
	int m_nBufLen;
	int m_nLen;
	char *m_pCstrBuf;

public:
	// constructor
	NaString();
	NaString(const char *lpsz);
	NaString(const wchar_t *lpsz);
	NaString(const NaString &nstr);
#if defined(USE_V8)
	NaString(Local<String> &str);
	NaString(String::Value &str);
#endif
	// destructor
	~NaString();

	// operator overloading
	const NaString& operator=(const wchar_t *lpsz);
	const NaString& operator=(const NaString &str);

	const NaString& operator+=(const wchar_t *lpsz);
	const NaString& operator+=(NaString &str);

	bool operator==(const char *lpsz);
	bool operator==(const wchar_t *lpsz);
	bool operator==(NaString &str);

	bool operator<(const NaString &str) const;

	wchar_t operator[](int index);

	operator const char* () const;
	operator const wchar_t* () const;

    // format
    template <typename... Args>
    const NaString & Format(const wchar_t * fmt, const Args&... args)
    {
        SetBuf(fmt::sprintf(fmt, args...).c_str());
        return *this;
    }

    template <typename... Args>
    const NaString & Format(const char * fmt, const Args&... args)
    {
        SetBuf(fmt::sprintf(fmt, args...).c_str());
        return *this;
    }

    template <typename... Args>
    const NaString & AppendFormat(const wchar_t * fmt, const Args&... args)
    {
        Format(L"%ls%ls", (wchar_t*)m_pBuf, fmt::sprintf(fmt, args...).c_str());
        return *this;
    }

    template <typename... Args>
    const NaString & AppendFormat(const char * fmt, const Args&... args)
    {
        Format("%s%s", cstr(), fmt::sprintf(fmt, args...).c_str());
        return *this;
    }

	// functions
	void ToLower();
	void ToUpper();
	//const NaString& Format(const wchar_t *fmt, ...);
	//const NaString& Format(const char *fmt, ...);
	int GetLength();
	int GetBufferSize();
	int Compare(const wchar_t *lpsz);
	int CompareNoCase(const wchar_t *lpsz);
	
	int Find(const wchar_t* ch, int begin = 0);
	NaString Left(int len);
	NaString Mid(int index, int len = -1);
	NaString Right(int len);
	NaStrArray Split(wchar_t *ch);
	int ReplaceAll(wchar_t* from, wchar_t* to);

	// utility
	//const NaString& AppendFormat(const wchar_t *fmt, ...);
	//const NaString& AppendFormat(const char *fmt, ...);
	const wchar_t* wstr() const;
	const char* cstr();
	wchar_t GetLast();
	int ToInt() const;

	static int ConvertWCharToChar(const wchar_t* wstr, char** str);
	static int ConvertCharToWChar(const char* str, wchar_t** wstr);
	
protected:
	// internal function
	const NaString& SetBuf(const wchar_t *wsz, int len = -1);
	const NaString& SetBuf(const char *sz, int len = -1);
	const wchar_t *GetBuf();
	void AllocBuf(int len);
	void DeallocBuf(unsigned char *pBuf = nullptr);
};

class NaStrArray
{
public:
	NaStrArray();
	~NaStrArray();

    NaStrArray& operator=(NaStrArray &str);
	NaString operator [](int nIndex);

	void Set(int nIndex, NaString str);

	int Add(NaString str);
	int Remove(int nIndex);
	int GetCount();
	int Find(NaString str);

	NaString Join(wchar_t* ch);
	NaString Pop(); // remove last element

protected:
	std::list<NaString> m_Array;
};