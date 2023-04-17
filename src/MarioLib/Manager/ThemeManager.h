#pragma once

#include "Common.h"

class Game;
class Sprite;
class Stage;
class ThemeManager
{
public:
	ThemeManager(Game *pGame);
	virtual ~ThemeManager();

	void LoadSettings(Stage *pStage);
	void ApplyThemeSet(Stage *pStage);
	
	Sprite* FindSprite(wchar_t* szName, int nTheme);
	Sprite* RequestSprite(wchar_t* szName, int nTheme);
	Sprite* RequestCustomSprite(wchar_t* szName, NaString strCustomTheme);
	Sprite* RequestLayerSprite(wchar_t* szName, int nTheme);
	Sprite* RequestCustomLayerSprite(wchar_t* szName, NaString strCustomTheme);
	Sprite* RequestSpriteWithPostfix(wchar_t* szName, NaString strThemePostfix, int nMode = 0);
	void RequestTileSprite(wchar_t* szName, int nTheme, Sprite **ppSprite, Sprite **ppPieceSprite);
	void RequestCustomTileSprite(wchar_t* szName, NaString strCustomTheme, Sprite **ppSprite, Sprite **ppPieceSprite);
	void RequestTileSpriteWithPostfix(wchar_t* szName, NaString strThemePostfix, Sprite **ppSprite, Sprite **ppPieceSprite);

	void ApplyBackgroundColor(int nTheme, int nBgTheme, long &lBgColor);
	NaString GetThemePostfix(int nTheme);

	Game *m_pGame;
};

