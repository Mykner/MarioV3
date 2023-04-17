//
// Url(or Directory Path) parser
// 2016.06.15
//
#pragma once

#include "NaString.h"

class NaUrl
{
public:
	NaUrl();
	~NaUrl();

	void SetBase(NaString base);
	void SetUrl(NaString url);
	NaString GetFullUrl();

protected:
	NaString m_strBase;
	NaString m_strUrl;
};

