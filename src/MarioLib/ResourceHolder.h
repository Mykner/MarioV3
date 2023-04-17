#pragma once

class Game;
class DirectXManager;
class FModManager;
class SpriteManager;
class InputManager;
class ThemeManager;
class UIManager; 
class OpenGLManager;
class RenderManager;
class SoundManager;
class ResourceHolder
{
public:
	ResourceHolder();
	virtual ~ResourceHolder();

	void LoadResources(Game* pGame, bool bIsDesign = false);
	void AttachResources(Game *pGame);

	void AddRef();
	bool Release(Game *pGame);

	int m_nRef;
	bool m_bUseDirectX;

	RenderManager *m_pRenderManager;
	FModManager *m_pFMod;
	SpriteManager *m_pSpriteManager;
	InputManager *m_pInputManager;
	ThemeManager *m_pThemeManager;
	UIManager *m_pUIManager;
	SoundManager *m_pSoundManager;
	Game *m_pActiveGame;
};

