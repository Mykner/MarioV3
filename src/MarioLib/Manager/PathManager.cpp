#include "PathManager.h"

#include "Game.h"
#include "NaPlatformAPI.h"

void PathManager::InitAll(Game * pGame)
{
	if (pGame)
	{
		GetGameRootPath(pGame);
		GetConfigPath(pGame);
		GetMapPath(pGame);
		GetSpritePath(pGame);
		GetBGMPath(pGame);
		GetSoundPath(pGame);
		GetDocumentPath(pGame);
		GetProfilePath(pGame);
	}
}

const wchar_t* PathManager::GetGameRootPath(Game * pGame)
{
	static NaString str = L"";
	static NaString strTemp = L"";
	if (str.GetLength() == 0)
	{
		if (pGame)
		{
			if (pGame->m_strGameRootDir.GetLength() == 0)
			{
				strTemp = NaPlatform::GetCurrentPath();
				return strTemp.wstr();
			}
			else
				str = pGame->m_strGameRootDir;
		}
		else
		{
			strTemp = NaPlatform::GetCurrentPath();
			return strTemp.wstr();
		}
	}
	
	return str.wstr();
}

const wchar_t * PathManager::GetConfigPath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0)
		str.Format(L"%ls\\UserData", GetGameRootPath(pGame));
	
	return str.wstr();
}

const wchar_t * PathManager::GetResourcesPath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0 && pGame)
		str.Format(L"%ls\\Resources", GetGameRootPath(pGame));

	return str.wstr();
}

const wchar_t * PathManager::GetGameThemesPath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0 && pGame)
		str.Format(L"%ls\\Resources\\GameThemes", GetGameRootPath(pGame));

	return str.wstr();
}

const wchar_t * PathManager::GetMapPath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0 && pGame)
		str.Format(L"%ls\\Resources\\Map", GetGameRootPath(pGame));

	return str.wstr();
}

const wchar_t * PathManager::GetMapDownloadPath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0 && pGame)
		str.Format(L"%ls\\Resources\\Map\\Download", GetGameRootPath(pGame));

	return str.wstr();
}

const wchar_t * PathManager::GetSpritePath(Game * pGame, const wchar_t* szGameTheme)
{
	static NaString str = L"";
	if (szGameTheme != nullptr)
	{
		if (pGame)
			str.Format(L"%ls\\Resources\\GameThemes\\%ls\\Sprite", GetGameRootPath(pGame), szGameTheme);
	}
	else
	{
		if (pGame)
			str.Format(L"%ls\\Resources\\Sprite", GetGameRootPath(pGame));
	}

	return str.wstr();
}

const wchar_t * PathManager::GetBGMPath(Game * pGame, const wchar_t* szGameTheme)
{
	static NaString str = L"";
	if (szGameTheme != nullptr)
	{
		if (pGame)
			str.Format(L"%ls\\Resources\\GameThemes\\%ls\\BGM", GetGameRootPath(pGame), szGameTheme);
	}
	else
	{
		if (pGame)
			str.Format(L"%ls\\Resources\\BGM", GetGameRootPath(pGame));
	}

	return str.wstr();
}

const wchar_t * PathManager::GetSoundPath(Game * pGame, const wchar_t* szGameTheme)
{
	static NaString str = L"";
	if (szGameTheme != nullptr)
	{
		if (pGame)
			str.Format(L"%ls\\Resources\\GameThemes\\%ls\\Sound", GetGameRootPath(pGame), szGameTheme);
	}
	else
	{
		if (pGame)
			str.Format(L"%ls\\Resources\\Sound", GetGameRootPath(pGame));
	}

	return str.wstr();
}

const wchar_t * PathManager::GetDocumentPath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0 && pGame)
		str.Format(L"%ls", pGame->m_strDocumentDir.wstr());

	return str.wstr();
}

const wchar_t * PathManager::GetProfilePath(Game * pGame)
{
	static NaString str = L"";
	if (str.GetLength() == 0 && pGame)
		str.Format(L"%ls\\Profile", GetDocumentPath(pGame));

	return str.wstr();
}
