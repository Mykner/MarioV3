//
// Url(or Directory Path) parser
// 2016.06.15
//

#include "NaUrl.h"

#include "NaCommon.h"

#include <list>

NaUrl::NaUrl()
{
}

NaUrl::~NaUrl()
{
}

void NaUrl::SetBase(NaString base)
{
	m_strBase = NaString(base);
}

void NaUrl::SetUrl(NaString url)
{
	m_strUrl = NaString(url);
}

NaString NaUrl::GetFullUrl()
{
	bool bRelative = false;
	int nIdx = m_strUrl.Find(L":");
	if (nIdx > 0)
	{
		// TODO
	}
	else
	{
		bRelative = true;

		NaString strUrl = m_strUrl;
		NaString strBase = m_strBase;
		strUrl.ReplaceAll(L"\\", L"/");
		strBase.ReplaceAll(L"\\", L"/");

		NaStrArray arUrl = strUrl.Split(L"/");
		NaStrArray arBase = strBase.Split(L"/");
		if (strBase.GetLast() != L'/')
		{
			// Maybe filename
			arBase.Pop();
		}

		for (int i = 0; i < arUrl.GetCount(); i++)
		{
			if (arUrl[i] == "/" && i == 0)
			{
				// TODO this is root
				continue;
			}
			if (arUrl[i] == ".")
				continue;
			if (arUrl[i] == "..")
			{
				arBase.Pop();
				continue;
			}

			arBase.Add(arUrl[i]);
		}

		return arBase.Join(L"/").wstr();
	}

	return NaString(L"");
}
