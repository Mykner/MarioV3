#include "ThemeManager.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Themes.h"
#include "JsonManager.h"
#include "PathManager.h"

ThemeManager::ThemeManager(Game *pGame)
{
	m_pGame = pGame;
}

ThemeManager::~ThemeManager()
{
}

void ThemeManager::LoadSettings(Stage * pStage)
{
	if (pStage->m_strGameTheme.GetLength() == 0)
		return;

	// Load GameTheme settings
	NaDebugOut(L"Load GameTheme settings...\n");

	NaString strName;
	strName.Format(L"%ls\\%ls\\ThemeSettings.json", 
		PathManager::GetGameThemesPath(m_pGame),
		pStage->m_strGameTheme.wstr());
	Json::Value *j = JsonManager::LoadFile(strName.cstr(), true);
	if (j)
	{
		JsonManager::LoadJsonObjectProperty(&pStage->m_ThemeSetting, *j);
		
		// For ecnrypt before distribute
		//JsonManager::SaveToEncryptFile(strName.cstr(), *j);

		delete j;
		j = nullptr;

		pStage->m_ThemeSetting.OnLoad();

	}
}

void ThemeManager::ApplyThemeSet(Stage * pStage)
{
	if (pStage->m_strCustomThemeName.GetLength() > 0)
	{
		RequestCustomTileSprite(L"Tile", pStage->m_strCustomThemeName, &pStage->m_pSprite, &pStage->m_pSpritePiece);
		RequestCustomTileSprite(L"TileBackground", pStage->m_strCustomThemeName, &pStage->m_pSpriteBackground, nullptr);

		pStage->m_pSpriteItem = RequestCustomSprite(L"Item", pStage->m_strCustomThemeName);
		pStage->m_pSpriteEnemy = RequestCustomSprite(L"Enemy", pStage->m_strCustomThemeName);
		pStage->m_pSpriteEffect = RequestCustomSprite(L"Effect", pStage->m_strCustomThemeName);
		pStage->m_pSpriteMapObject = RequestCustomSprite(L"MapObject", pStage->m_strCustomThemeName);
		pStage->m_pSpriteFarBackground = RequestCustomLayerSprite(L"FarBackground", pStage->m_strCustomThemeName);

		return;
	}

	switch (pStage->m_nThemeSet)
	{
	case STAGETHEMESET_OVERWORLD:
		pStage->m_nTheme = STAGETHEME_OVERWORLD;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_MOUNTAIN;
		pStage->m_lBackgroundColor = 0xff7686ff;
		break;
	case STAGETHEMESET_UNDERGROUND:
		pStage->m_nTheme = STAGETHEME_UNDERGROUND;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_UNDERGROUND;
		pStage->m_lBackgroundColor = 0xff000000;
		break;
	case STAGETHEMESET_CASTLE:
		pStage->m_nTheme = STAGETHEME_CASTLE;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_CASTLE;
		pStage->m_lBackgroundColor = 0xff000000;
		break;
	case STAGETHEMESET_UNDERWATER:
		pStage->m_nTheme = STAGETHEME_UNDERWATER;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_UNDERWATER;
		pStage->m_lBackgroundColor = 0xff4040ff;
		break;
	case STAGETHEMESET_GHOSTHOUSE:
		pStage->m_nTheme = STAGETHEME_GHOSTHOUSE;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_GHOSTHOUSE;
		pStage->m_lBackgroundColor = 0xff000000;
		break;
	case STAGETHEMESET_AIRSHIP:
		pStage->m_nTheme = STAGETHEME_AIRSHIP;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_CLOUD;// AIRSHIP;
		pStage->m_lBackgroundColor = 0xff7686ff;

		// I need background with 2 level layer...
		// 1) cloud (far layer)
		// 2) pipe, flag, .. (not far)

		break;
	case STAGETHEMESET_SNOW:
		pStage->m_nTheme = STAGETHEME_SNOW;
		pStage->m_nBackgroundTheme = STAGEBGTHEME_SNOW;
		pStage->m_lBackgroundColor = 0xff7686ff;
		break;
	default:
		break;
	}

	//pStage->m_pSprite = RequestSprite(L"Tile", pStage->m_nTheme);

	// Oops, we need switching. :(
	Stage *pCurStage = m_pGame->m_pCurStage;
	m_pGame->m_pCurStage = pStage;
	RequestTileSprite(L"Tile", pStage->m_nTheme, &pStage->m_pSprite, &pStage->m_pSpritePiece);
	RequestTileSprite(L"TileBackground", pStage->m_nTheme, &pStage->m_pSpriteBackground, nullptr);
	m_pGame->m_pCurStage = pCurStage;

	pStage->m_pSpriteItem = RequestSprite(L"Item", pStage->m_nTheme);
	pStage->m_pSpriteEnemy = RequestSprite(L"Enemy", pStage->m_nTheme);
	pStage->m_pSpriteEffect = RequestSprite(L"Effect", pStage->m_nTheme);
	pStage->m_pSpriteMapObject = RequestSprite(L"MapObject", pStage->m_nTheme);
	pStage->m_pSpriteFarBackground = RequestLayerSprite(L"FarBackground", pStage->m_nTheme);
}

Sprite * ThemeManager::FindSprite(wchar_t * szName, int nTheme)
{
	NaString strThemePostfix = GetThemePostfix(nTheme);
	NaString strSpriteName;
	strSpriteName.Format(L"%ls%ls", szName, strThemePostfix.wstr());

	Sprite *pRet = m_pGame->m_pSpriteManager->Find(strSpriteName, false);
	if (pRet == nullptr)
	{
		strSpriteName.Format(L"%ls%ls", szName, L"Overworld");
		pRet = m_pGame->m_pSpriteManager->Find(strSpriteName, false);
		if (pRet == nullptr)
		{
			strSpriteName.Format(L"%ls", szName);
			pRet = m_pGame->m_pSpriteManager->Find(strSpriteName);
		}
	}

	return pRet;
}

Sprite * ThemeManager::RequestSprite(wchar_t * szName, int nTheme)
{
	NaString strThemePostfix = GetThemePostfix(nTheme);

	return RequestSpriteWithPostfix(szName, strThemePostfix);
}

Sprite * ThemeManager::RequestCustomSprite(wchar_t * szName, NaString strCustomTheme)
{
	return RequestSpriteWithPostfix(szName, strCustomTheme);
}

Sprite * ThemeManager::RequestLayerSprite(wchar_t * szName, int nTheme)
{
	NaString strThemePostfix = GetThemePostfix(nTheme);

	return RequestSpriteWithPostfix(szName, strThemePostfix, SPRITE_LOAD_LAYERIMAGE);
}

Sprite * ThemeManager::RequestCustomLayerSprite(wchar_t * szName, NaString strCustomTheme)
{
	return RequestSpriteWithPostfix(szName, strCustomTheme, SPRITE_LOAD_LAYERIMAGE);
}

Sprite * ThemeManager::RequestSpriteWithPostfix(wchar_t * szName, NaString strThemePostfix, int nMode)
{
	NaString strSpriteName;
	strSpriteName.Format(L"%ls", szName);

	Sprite *pRet = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, true, nMode);
	if (pRet == nullptr)
	{
		strThemePostfix = GetThemePostfix(STAGETHEME_OVERWORLD);
		pRet = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, true, nMode);
		if (pRet == nullptr)
		{
			strSpriteName.Format(L"%ls", szName);
			pRet = m_pGame->m_pSpriteManager->RequestBase(strSpriteName, nullptr, true, nMode);
		}
	}

	if (pRet == nullptr)
	{
		pRet = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, false, nMode);
		if (pRet == nullptr)
		{
			strThemePostfix = GetThemePostfix(STAGETHEME_OVERWORLD);
			pRet = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, false, nMode);
			if (pRet == nullptr)
			{
				strSpriteName.Format(L"%ls", szName);
				pRet = m_pGame->m_pSpriteManager->RequestBase(strSpriteName, nullptr, false, nMode);
			}
		}
	}

	return pRet;
}

void ThemeManager::RequestTileSprite(wchar_t * szName, int nTheme, Sprite ** ppSprite, Sprite ** ppPieceSprite)
{
	NaString strThemePostfix = GetThemePostfix(nTheme);
	
	RequestTileSpriteWithPostfix(szName, strThemePostfix, ppSprite, ppPieceSprite);
}

void ThemeManager::RequestCustomTileSprite(wchar_t * szName, NaString strCustomTheme, Sprite ** ppSprite, Sprite ** ppPieceSprite)
{
	RequestTileSpriteWithPostfix(szName, strCustomTheme, ppSprite, ppPieceSprite);
}

void ThemeManager::RequestTileSpriteWithPostfix(wchar_t * szName, NaString strThemePostfix, Sprite ** ppSprite, Sprite ** ppPieceSprite)
{
	NaString strSpriteName;
	strSpriteName.Format(L"%ls", szName);

	*ppSprite = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, true, SPRITE_LOAD_TILE16_WITH_PIECE);
	if (*ppSprite == nullptr)
	{
		strThemePostfix = GetThemePostfix(STAGETHEME_OVERWORLD);
		*ppSprite = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, true, SPRITE_LOAD_TILE16_WITH_PIECE);
		if (*ppSprite == nullptr)
		{
			strSpriteName.Format(L"%ls", szName);
			*ppSprite = m_pGame->m_pSpriteManager->RequestBase(strSpriteName, nullptr, true, SPRITE_LOAD_TILE16_WITH_PIECE);
		}
	}

	if (*ppSprite != nullptr && ppPieceSprite != nullptr)
	{
		*ppPieceSprite = (*ppSprite)->m_pSpritePiece;
	}

	if (*ppSprite == nullptr)
	{
		*ppSprite = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, false, SPRITE_LOAD_TILE16_WITH_PIECE);
		if (*ppSprite == nullptr)
		{
			strThemePostfix = GetThemePostfix(STAGETHEME_OVERWORLD);
			*ppSprite = m_pGame->m_pSpriteManager->RequestThemed(strSpriteName, strThemePostfix, false, SPRITE_LOAD_TILE16_WITH_PIECE);
			if (*ppSprite == nullptr)
			{
				strSpriteName.Format(L"%ls", szName);
				*ppSprite = m_pGame->m_pSpriteManager->RequestBase(strSpriteName, nullptr, false, SPRITE_LOAD_TILE16_WITH_PIECE);
			}
		}

		if (*ppSprite != nullptr && ppPieceSprite != nullptr)
		{
			*ppPieceSprite = (*ppSprite)->m_pSpritePiece;
		}
	}
}

void ThemeManager::ApplyBackgroundColor(int nTheme, int nBgTheme, long &lBgColor)
{
	/*
	switch (nBgTheme)
	{
	case STAGEBGTHEME_MOUNTAIN:
		lBgColor = 0xff5080ff;
		break;
	//case STAGEBGTHEME_SNOW:
	case STAGEBGTHEME_NONE:
		{
			switch (nTheme)
			{
			case STAGETHEME_UNDERGROUND:
			case STAGETHEME_CASTLE:
				lBgColor = 0xff000000;
				break;
			default:
				// lBgColor = 0xff5080ff;
				break;
			}
		}
		break;
	case STAGEBGTHEME_NIGHT:
	case STAGEBGTHEME_SNOWNIGHT:
		lBgColor = 0xff000000;
		break;
	case STAGEBGTHEME_UNDERWATER:
		lBgColor = 0xffff4040;
		break;
	}
	*/

	if (nBgTheme == STAGEBGTHEME_UNDERWATER)
		lBgColor = 0xff4040ff;
}

NaString ThemeManager::GetThemePostfix(int nTheme)
{
	NaString strThemePostfix = L"";
	switch (nTheme)
	{
	case STAGETHEME_OVERWORLD:
		strThemePostfix = L"Overworld";
		break;
	case STAGETHEME_UNDERGROUND:
		strThemePostfix = L"Underground";
		break;
	case STAGETHEME_CASTLE:
		strThemePostfix = L"Castle";
		break;
	case STAGETHEME_UNDERWATER:
		strThemePostfix = L"Underwater";
		break;
	case STAGETHEME_CASTLEUNDERWATER:
		strThemePostfix = L"CastleUnderwater";
		break;
	case STAGETHEME_CLASSICSNOW:
		strThemePostfix = L"ClassicSnow";
		break;
	case STAGETHEME_MUSHROOM:
		strThemePostfix = L"Mushroom";
		break;
	case STAGETHEME_GRAY:
		strThemePostfix = L"Gray";
		break;
	case STAGETHEME_GHOSTHOUSE:
		strThemePostfix = L"GhostHouse";
		break;
	case STAGETHEME_AIRSHIP:
		strThemePostfix = L"AirShip";
		break;
	case STAGETHEME_SNOW:
		strThemePostfix = L"Snow";
		break;
	}

	return strThemePostfix;
}

