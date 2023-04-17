#pragma once

#include "NaString.h"

class Game;
class PathManager
{
public:	
	static void InitAll(Game *pGame);

	static const wchar_t* GetGameRootPath(Game *pGame);
	static const wchar_t* GetConfigPath(Game *pGame);
	static const wchar_t* GetResourcesPath(Game *pGame);
	static const wchar_t* GetGameThemesPath(Game *pGame);
	static const wchar_t* GetMapPath(Game *pGame);
	static const wchar_t* GetMapDownloadPath(Game *pGame);
	static const wchar_t* GetSpritePath(Game *pGame, const wchar_t* szGameTheme = nullptr);
	static const wchar_t* GetBGMPath(Game *pGame, const wchar_t* szGameTheme = nullptr);
	static const wchar_t* GetSoundPath(Game *pGame, const wchar_t* szGameTheme = nullptr);
	static const wchar_t* GetDocumentPath(Game *pGame);
	static const wchar_t* GetProfilePath(Game *pGame);
};