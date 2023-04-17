#include "DesignGame.h"

#include "DirectXManager.h"
#include "OpenGLManager.h"
#include "FModManager.h"
#include "SpriteManager.h"
#include "InputManager.h"
#include "ThemeManager.h"
#include "UIManager.h"
#include "ResourceHolder.h"

#include "GameStatePlay.h"
#include "GameStateTest.h"

#include "Player.h"
#include "Stage.h"
#include "Camera.h"

#include "GameStateDesign.h"
#include "DesignStage.h"
#include "NaPlatformAPI.h"
#include "PathManager.h"

#include "TypeNames.h"

DesignGame::DesignGame()
{
	m_bIsDesignGame = true;
	m_bModernStyle = true;
	m_nStagePackType = STAGEPACK_SINGLE;
}

DesignGame::~DesignGame()
{	
}

void DesignGame::Init(NaPlatformWindowHandle hWindow, const wchar_t *szCurrentDir)
{
	NaDebugOut(L"Init DesignGame\n");

	if (szCurrentDir)
		m_strGameRootDir = szCurrentDir;
	else
	{
        m_strGameRootDir = NaPlatform::GetCurrentPath();
	}

	m_nWidth = GameDefaults::nPageWidth;
	m_nHeight = GameDefaults::nPageHeight;

	// Init Screen
	m_hWindow = hWindow;

	if (m_pResourceHolder)
	{
		m_pResourceHolder->LoadResources(this, true);
	}
	else
	{
		m_pRenderManager = new OpenGLManager(this);
		m_pRenderManager->Init();

		m_pFMod = new FModManager(this);
		m_pFMod->Init();

		m_pSpriteManager = new SpriteManager(this);

		m_pThemeManager = new ThemeManager(this);

		m_pInputManager = new InputManager(this);

		m_pUIManager = new UIManager(this);

		LoadResources();

		m_pSpriteManager->Load(L"Design");
	}	

	m_pCurStage = new DesignStage(this);

	Player *pPlayer = new Player(this);
	m_vecPlayers.push_back(pPlayer);
	m_pCurPlayer = m_vecPlayers[0];

	m_pCamera = new Camera(this);

	InitState(GAMESTATE_DESIGN);

	// Load GameTheme List
	NaString strPath;
	strPath.Format(L"%ls\\Resources\\GameThemes", PathManager::GetGameRootPath(this));
	m_arGameTheme = NaPlatform::GetSubDirectoryList(strPath);
}

void DesignGame::InitState(int nState)
{
	GameStateBase *pState = nullptr;
	switch (nState)
	{
	case GAMESTATE_DESIGN:
		pState = new GameStateDesign(this);
		break;
	}

	m_pGameState = pState;
	m_pGameState->Init();
}
