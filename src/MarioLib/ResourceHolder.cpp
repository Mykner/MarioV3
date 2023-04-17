#include "ResourceHolder.h"

#include "Game.h"
#include "DirectXManager.h"
#include "OpenGLManager.h"
#include "FModManager.h"
#include "SpriteManager.h"
#include "InputManager.h"
#include "ThemeManager.h"
#include "UIManager.h"
#include "SoundManager.h"

#include "NaException.h"

ResourceHolder::ResourceHolder()
{
	m_nRef = 0;

	m_pRenderManager = nullptr;
	m_pFMod = nullptr;
	m_pSpriteManager = nullptr;
	m_pInputManager = nullptr;
	m_pThemeManager = nullptr;
	m_pUIManager = nullptr;
	m_pSoundManager = nullptr;

	m_pActiveGame = nullptr;
	m_bUseDirectX = false;
}

ResourceHolder::~ResourceHolder()
{
}

void ResourceHolder::LoadResources(Game * pGame, bool bIsDesign)
{
	if (m_nRef == 0)
	{
		NaDebugOut(L"Load Resources\n");

		try
		{
#ifdef WIN32
			if (m_bUseDirectX)
				m_pRenderManager = new DirectXManager(pGame);
			else
				m_pRenderManager = new OpenGLManager(pGame);
#else
			m_pRenderManager = new OpenGLManager(pGame);
#endif
			m_pRenderManager->Init();
			pGame->m_pRenderManager = m_pRenderManager;

			m_pFMod = new FModManager(pGame);
			m_pFMod->Init();
			pGame->m_pFMod = m_pFMod;

			m_pSpriteManager = new SpriteManager(pGame);
			pGame->m_pSpriteManager = m_pSpriteManager;
			if (bIsDesign)
			{
				m_pSpriteManager->Load(L"Design");
			}

			pGame->LoadResources();

			m_pThemeManager = new ThemeManager(pGame);
			pGame->m_pThemeManager = m_pThemeManager;

			m_pInputManager = new InputManager(pGame);
			pGame->m_pInputManager = m_pInputManager;

			m_pUIManager = new UIManager(pGame);
			pGame->m_pUIManager = m_pUIManager;

			m_pSoundManager = new SoundManager(pGame);
			pGame->m_pSoundManager = m_pSoundManager;
		}
		catch (NaException &ex)
		{
			throw NaException(ex.m_szFunc, ex.m_nLine, ex.m_szWhat);
		}
	}
	else
	{
		AttachResources(pGame);
	}

	m_pActiveGame = pGame;

	AddRef();
}

void ResourceHolder::AttachResources(Game * pGame)
{
	NaDebugOut(L"Attach Resources to game 0x%08x\n", pGame);

	if (m_nRef > 0)
	{
		pGame->m_pRenderManager = m_pRenderManager;
		pGame->m_pRenderManager->m_pGame = pGame;

		pGame->m_pFMod = m_pFMod;
		pGame->m_pFMod->m_pGame = pGame;

		pGame->m_pSpriteManager = m_pSpriteManager;
		pGame->m_pSpriteManager->m_pGame = pGame;

		pGame->m_pThemeManager = m_pThemeManager;
		pGame->m_pThemeManager->m_pGame = pGame;

		pGame->m_pInputManager = m_pInputManager;
		pGame->m_pInputManager->m_pGame = pGame;

		pGame->m_pUIManager = m_pUIManager;
		pGame->m_pUIManager->m_pGame = pGame;

		pGame->m_pSoundManager = m_pSoundManager;
		pGame->m_pSoundManager->m_pGame = pGame;

		BEGIN_VECTOR_LOOP(Sprite*, m_pSpriteManager->m_vecSprites) {
			pObj->m_pGame = pGame;
			if (pObj->m_pSpritePiece)
				pObj->m_pSpritePiece->m_pGame = pGame;
		} END_VECTOR_LOOP();
	}
}

void ResourceHolder::AddRef()
{
	m_nRef++;
}

bool ResourceHolder::Release(Game *pGame)
{
	NaDebugOut(L"Release Resources from game 0x%08x (Current RefCnt: %d)\n", pGame, m_nRef);

	// #TODO Only attached game can release resource

	if (m_nRef == 0)
	{
		NaDebugOut(L"-> Error!\n");
		return false;
	}

	m_nRef--;
	if (m_nRef == 0)
	{
		if (m_pActiveGame == pGame)
			m_pActiveGame = nullptr;

		SAFE_DELETE(m_pSoundManager);
		SAFE_DELETE(m_pUIManager);
		SAFE_DELETE(m_pInputManager);
		SAFE_DELETE(m_pThemeManager);
		
		SAFE_RELEASE(m_pSpriteManager);
		SAFE_RELEASE(m_pRenderManager);
		SAFE_RELEASE(m_pFMod);
		
		SAFE_DELETE(m_pSpriteManager);
		SAFE_DELETE(m_pRenderManager);
		SAFE_DELETE(m_pFMod);

		// Detach destoryed managers
		pGame->m_pSoundManager = nullptr;
		pGame->m_pUIManager = nullptr;
		pGame->m_pInputManager = nullptr;
		pGame->m_pThemeManager = nullptr;

		pGame->m_pSpriteManager = nullptr;
		pGame->m_pRenderManager = nullptr;
		pGame->m_pFMod = nullptr;
		
		return true;
	}

	return false;
}
