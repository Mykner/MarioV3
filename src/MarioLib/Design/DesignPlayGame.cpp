#include "DesignPlayGame.h"

#include "RenderManager.h"
#include "SoundManager.h"
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
#include "GameConfig.h"

#include "GameStateDesign.h"
#include "DesignStage.h"

#include "NaPlatformAPI.h"
#include "DesignGame.h"
#include "DesignStage.h"

#include "Mario.h"

DesignPlayGame::DesignPlayGame() :
	Game()
{
	m_bIsDesignGame = true;
	m_bExitFlag = false;
	m_pGame = nullptr;

	m_nStagePackType = STAGEPACK_SINGLE;
}


DesignPlayGame::~DesignPlayGame()
{
}

void DesignPlayGame::Init(NaPlatformWindowHandle hWindow, const wchar_t *szCurrentDir)
{
	NaDebugOut(L"Init DesignPlayGame\n");

	if (szCurrentDir)
		m_strGameRootDir = szCurrentDir;
	else
        m_strGameRootDir = NaPlatform::GetCurrentPath();

	m_nWidth = GameDefaults::nTilePerScreenWidth * GameDefaults::nTileWidth;
	m_nHeight = GameDefaults::nTilePerScreenHeight * GameDefaults::nTileHeight;

	// Shared Resources: DirectX, FMod, Sprites, ..
	if (m_pResourceHolder)
		m_pResourceHolder->LoadResources(this);

	GameConfig *pConfig = GameConfig::GetGameConfig(this);

	AttachWindow(hWindow);

	// Init Stage
	Stage *pStage = new Stage(this);
	m_pCurStage = pStage;

	// Init Player
	Player *pPlayer = new Player(this);
	pPlayer->BindInputs(pConfig->m_nInputKeyCode);

	// Default Player Object is Mario
#if defined(_DEBUG)
	Mario *pMario = new Mario(this, m_pCurStage);
#else
	Mario *pMario = new Mario(this, m_pCurStage);
#endif
	pMario->x = pStage->m_ptStartPoint.x;
	pMario->y = pStage->m_ptStartPoint.y;

	pPlayer->SetControl(pMario);
	m_vecPlayers.push_back(pPlayer);
	m_pCurPlayer = m_vecPlayers[0];

	// Init Camera
	m_pCamera = new Camera(this);
	m_pCamera->LookAt(pMario);
}

void DesignPlayGame::OnChangeState()
{
	if (m_nNextState != -1)
	{
		bool bAllow = false;
		switch (m_nNextState)
		{
		case GAMESTATE_DIE:
		case GAMESTATE_SHAPECHANGE:
		case GAMESTATE_ENTERPIPE:
		case GAMESTATE_EXITPIPE:
		case GAMESTATE_ENTERSKY:
		case GAMESTATE_SKYINTRO:
		case GAMESTATE_FLAGDOWN:
		case GAMESTATE_GOAL:
		case GAMESTATE_CUTBRIDGE:
		case GAMESTATE_RESCUEPRINCESS:
		case GAMESTATE_ENTERDOOR:
		case GAMESTATE_EXITDOOR:
		case GAMESTATE_MESSAGEBLOCK:
		case GAMESTATE_HITGOALBOX:
		case GAMESTATE_HITGOALPOST:
			bAllow = true;
			break;
		case GAMESTATE_PLAY:
			m_nNextState = GAMESTATE_DESIGNPLAY;
			bAllow = true;
			break;
		}

		if (bAllow)
		{
			InitState(m_nNextState);
		}
		else
		{
			m_bExitFlag = true;
		}
		m_nNextState = -1;
	}
}

void DesignPlayGame::InitFromGame(Game * pGame)
{
	// Init random seed
	srand(time(nullptr));

	m_pGame = pGame;
	m_pResourceHolder = pGame->m_pResourceHolder;

	Init(pGame->m_hWindow, nullptr);
	pGame->m_hWindow = nullptr;
	((DesignStage*)((DesignGame*)pGame)->m_pCurStage)->m_vecPlayedShadow.clear();

	m_nWidth = pGame->m_nWidth;
	m_nHeight = pGame->m_nHeight;
	m_bModernStyle = pGame->m_bModernStyle;

	m_pCurStage->m_ptStopPoint.x = GameDefaults::nDefaultStopPointX;
	m_pCurStage->m_ptStopPoint.y = 0;
	m_pCurStage->m_ptEnemyStop.x = GameDefaults::nMaxCameraPosX;

	// Is SubStage?
	int nIdx = pGame->m_pCurStage->GetSubStageIndex(pGame->m_pCurStage);
	Stage* pSourceStage = pGame->m_pCurStage->GetTopStage();

	Json::Value j;
	pSourceStage->SaveJsonMap(j);
	m_pCurStage->LoadJsonMap(j);
	m_pCurStage->OnLoad();
	((ControllableObject*)(m_pCurPlayer->m_pControlObj))->RequestSprites();

	// #TODO check
	m_pCurStage->m_strName = m_strInitialStage = pSourceStage->m_strName;

	if (nIdx != -1)
	{
		pSourceStage = pSourceStage->m_vecSubStage[nIdx];
		m_pCurStage = m_pCurStage->m_vecSubStage[nIdx];
	}

	// Copy & Copy reference
	m_pCurStage->m_pSprite = pSourceStage->m_pSprite;
	m_pCurStage->m_pSpriteBackground = pSourceStage->m_pSpriteBackground;
	m_pCurStage->m_pSpriteEffect = pSourceStage->m_pSpriteEffect;
	m_pCurStage->m_pSpriteEnemy = pSourceStage->m_pSpriteEnemy;
	m_pCurStage->m_pSpriteItem = pSourceStage->m_pSpriteItem;

	InitState(GAMESTATE_DESIGNPLAY);

	m_pCamera->m_ptPos = pGame->m_pCamera->m_ptPos;	

	m_pCurPlayer->m_nTime = 500;
	
	m_pGame->m_pInputManager->ClearStatus();

	SOUND_MANAGER->PlaySoundEffect(L"SceneCut");
	m_pCurStage->PlayBGM();
}

void DesignPlayGame::RestoreGame()
{
	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->StopSoundEffect();

	// Change Camera Position to Current Position
	if (m_pCurStage->m_strName == m_strInitialStage)
	{
		int nOld = m_pGame->m_pCamera->m_ptPos.x;
		m_pGame->m_pCamera->m_ptPos = m_pCamera->m_ptPos;
		int nNew = m_pGame->m_pCamera->m_ptPos.x;
		
		GameObjectBase *pPlayBot = ((DesignStage*)m_pGame->m_pCurStage)->m_pPlayBotObject;
		pPlayBot->m_fX += (nNew - nOld);

		if (m_pCurPlayer->m_pControlObj->IsAlive())
		{
			pPlayBot->m_fX = m_pCurPlayer->m_pControlObj->m_fX;
			pPlayBot->m_fY = m_pCurPlayer->m_pControlObj->m_fY;
		}
		else
		{
			if (pPlayBot->m_fY >= GameDefaults::nPageHeight - (TILE_YS * 3))
				pPlayBot->m_fY = GameDefaults::nPageHeight - (TILE_YS * 3) - 1;
		}
	}

	m_pGame->AttachWindow(m_hWindow);
	m_pGame->m_pSpriteManager->m_vecRenderQueue.clear();
}
