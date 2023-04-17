#pragma once

#include "Common.h"
#include "Sprite.h"

#include <vector>

class Game;
class SpriteManager
{
public:
	SpriteManager(Game *pGame);
	virtual ~SpriteManager();

	void Release();

	void Load(const wchar_t* name, int nMode = 0);
	void LoadMarioSprite(const wchar_t* name);
	void LoadThemeSprite(const wchar_t* name);

	Sprite* Find(const wchar_t* name, bool bWarn = true);
	Sprite* Request(const wchar_t* name, const wchar_t* bitmap_name = nullptr, int nMode = 0);
	Sprite* RequestBase(const wchar_t* name, const wchar_t* bitmap_name, bool bWithGameTheme, int nMode = 0);
	Sprite* RequestThemed(const wchar_t* name, const wchar_t* theme, bool bWithGameTheme, int nMode = 0);
	Sprite* RequestCostume(const wchar_t* name, const wchar_t* bitmap_name = nullptr);
	
	void RenderQueue();

	Game *m_pGame;
	std::vector<Sprite*> m_vecSprites;
	std::vector<SpriteRenderInfo> m_vecRenderQueue;
};

