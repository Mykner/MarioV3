#include "Game.h"

#include "FModManager.h"
#include "SpriteManager.h"
#include "InputManager.h"
#include "ThemeManager.h"
#include "UIManager.h"
#include "ResourceHolder.h"
#include "DirectXManager.h"
#include "OpenGLManager.h"
#include "SoundManager.h"

#include "GameStatePlay.h"
#include "GameStateTest.h"

#include "Player.h"
#include "Stage.h"
#include "Camera.h"
#include "UserProfile.h"
#include "TypeNames.h"
#include "NaPlatformAPI.h"
#include "GameConfig.h"

#include "Mario.h"
#include "Yoshi.h"

Game::Game()
{
	m_pRenderManager = nullptr;
	m_pFMod = nullptr;
	m_pSpriteManager = nullptr;
	m_pInputManager = nullptr;
	m_pUIManager = nullptr;
	m_pSoundManager = nullptr;

	m_pResourceHolder = nullptr;

	m_pLoadSoundThread = nullptr;
	m_pLoadBGMThread = nullptr;

	m_FrameSkipInfo.m_fFPS = GameDefaults::fFPS;
	m_FrameSkipInfo.m_fValue = 0.0f;
	m_FrameSkipInfo.m_nRenderCnt = 0;
	m_FrameSkipInfo.m_fTimer = 0.0f;

	m_pGameState = nullptr;
	m_pCurPlayer = nullptr;
	m_pCurStage = nullptr;
	m_pCamera = nullptr;

	m_nNextState = -1;
	m_nGameFrame = 0;
	m_bModernStyle = false;
	m_nSpecialEvents = 0;
	m_nStagePackAbilityFlag = ABILITY_NONE;

	m_bIsDesignGame = false;
    m_bExit = false;

	m_nStagePackType = STAGEPACK_CLASSIC;
	m_strStagePack = L"Classic";
	m_strInitialStage = L"1-1";
	m_strLastStage = L"";

	m_strGameRootDir = "";
	m_strDocumentDir = "";
}

Game::~Game()
{
}

void Game::BeginFrameSkip()
{
	m_FrameSkipInfo.m_fOldTime = NaPlatform::GetTickCount();
}

void Game::EndFrameSkip()
{
	m_FrameSkipInfo.m_fCurTime = NaPlatform::GetTickCount();

	m_FrameSkipInfo.m_fDt = (m_FrameSkipInfo.m_fCurTime - m_FrameSkipInfo.m_fOldTime) / 1000.0f;
	if (m_FrameSkipInfo.m_fDt < 0)
		m_FrameSkipInfo.m_fDt = 0.0f;

	// Test Code
	/*
	float f = m_FrameSkipInfo.m_fCurTime - m_FrameSkipInfo.m_fOldTime;
	m_FrameSkipInfo.m_fTimer += f;

	float fCurFPS = m_FrameSkipInfo.m_nRenderCnt * 1000.0f / m_FrameSkipInfo.m_fTimer;

	NaString str;
	str.Format(L"FPS: %.2f", fCurFPS);
	SetWindowText(m_hWnd, str.wstr());
	//*/
}

bool Game::CanUpdateProcess()
{
	// Add last process, render time
	m_FrameSkipInfo.m_fValue += m_FrameSkipInfo.m_fDt;

	// If too fast, skip process
	// (Under zero -> too fast)
	if (m_FrameSkipInfo.m_fValue < 0)
		return false;

	// Subtract FPS
	m_FrameSkipInfo.m_fValue -= (1.0f / m_FrameSkipInfo.m_fFPS);
	return true;
}

bool Game::IsSkipFrame()
{
	// Process,render time - fps > 0
	// -> too slow
	// -> skip frame
	return m_FrameSkipInfo.m_fValue >= 0;
}

void Game::Init(NaPlatformWindowHandle hWindow, const wchar_t* szCurrentDir)
{
	NaDebugOut(L"Init Game\n");

	if (szCurrentDir)
		m_strGameRootDir = szCurrentDir;
	else
	{
        m_strGameRootDir = NaPlatform::GetCurrentPath();
	}

	m_strDocumentDir = NaPlatform::GetDocumentPath();

#if !defined(NDEBUG)
	m_nWidth = GameWindowDefaults::nInnerWidth;
	m_nHeight = GameWindowDefaults::nInnerHeight;
#else
	m_nWidth = GameWindowDefaults::nInnerWidth;
	m_nHeight = GameWindowDefaults::nInnerHeight;
#endif

	// Init Screen
	m_hWindow = hWindow;

	GameConfig *pConfig = GameConfig::GetGameConfig(this);

	int nZoomFactor = GameWindowDefaults::nZoomFactor;
#if defined(NDEBUG)
	nZoomFactor = pConfig->m_nZoomFactor;
#endif

    NaRect rcWindow;
    rcWindow.left = 0;
    rcWindow.top = 0;
    rcWindow.right = Width()*nZoomFactor;
    rcWindow.bottom = Height()*nZoomFactor;
	NaPlatform::AdjustWindowRect(rcWindow);

    SIZE s = NaPlatform::GetScreenSize();
	int nWidth = rcWindow.right - rcWindow.left;
	int nHeight = rcWindow.bottom - rcWindow.top;
	int nX = s.cx / 2 - nWidth / 2;
	int nY = s.cy / 2 - nHeight / 2;

#if !defined(NDEBUG) && defined(WIN32)
	nX = 0;
	nY = 0;
#endif

	NaPlatform::MoveWindow(m_hWindow, nX, nY, nWidth, nHeight);
	NaPlatform::ShowWindow(m_hWindow, true);

	m_pResourceHolder = new ResourceHolder;
	m_pResourceHolder->m_bUseDirectX = m_Option.m_bUseDirectX;
	m_pResourceHolder->LoadResources(this, false);
	
	// Init Stage
	Stage *pStage = new Stage(this);
	m_pCurStage = pStage;

	// Init Player
	Player *pPlayer = new Player(this);
	pPlayer->BindInputs(pConfig->m_nInputKeyCode);

	// Default Player Object is Mario
#if !defined(NDEBUG)
	Mario *pPlayerObj = new Mario(this, m_pCurStage);
	//GameObjectBase *pPlayerObj = GameObjectBase::CreateInstance(this, pStage, ENEMY_GOOMBA);
#else
	Mario *pPlayerObj = new Mario(this, m_pCurStage);
#endif
	pPlayer->SetControl(pPlayerObj);
	m_vecPlayers.push_back(pPlayer);
	m_pCurPlayer = m_vecPlayers[0];

	pStage->Load(m_strInitialStage);

	pPlayerObj->x = pStage->m_ptStartPoint.x;
	pPlayerObj->y = pStage->m_ptStartPoint.y;

	// Init Camera
	m_pCamera = new Camera(this);
	m_pCamera->LookAt(pPlayerObj);

#if !defined(NDEBUG)
	pPlayer->LoadUserProfile(L"Profile0");
	pPlayer->m_pProfile->m_strName = L"DEBUG";

	//InitState(GAMESTATE_APPUPDATE);
	//InitState(GAMESTATE_LOGO);
	//InitState(GAMESTATE_TEST);

	if (m_Option.m_bTestMode)
		InitState(GAMESTATE_PLAY);
	else if (m_strInitialStage.Compare(L"1-1") != 0)
	{
		InitState(GAMESTATE_STAGEINTRO);
	}
	else
	{
		if (m_pInputManager->IsKeyHold(VK_SPACE))
			InitState(GAMESTATE_TITLE);
		else
			InitState(GAMESTATE_LOGO);
	}
#else
	if (m_Option.m_bTestMode)
		InitState(GAMESTATE_PLAY);
	else
		InitState(GameDefaults::nInitialState);
#endif
}

void Game::Release()
{
	ClearSceneEffect();
	ProcessSceneEffect();

	if (m_pResourceHolder)
	{
		if (m_pResourceHolder->Release(this))
		{
			delete m_pResourceHolder;
		}
		m_pResourceHolder = nullptr;
	}

	// #TODO Make this code simple
	std::vector<Player*>::iterator it = m_vecPlayers.begin();
	for (; it != m_vecPlayers.end(); ++it)
	{
		Player *pPlayer = *it;
		SAFE_DELETE(pPlayer->m_pControlObj);
		SAFE_DELETE(pPlayer);
	}
	m_vecPlayers.clear();

	SAFE_DELETE(m_pGameState);

	SAFE_DELETE(m_pCamera);
	SAFE_DELETE(m_pCurStage);
}

void Game::SetInitialStage(NaString strStage)
{
	m_strInitialStage = strStage;
}

void Game::OnExit()
{
	m_bExit = true;

	if (m_pCurPlayer && m_pCurPlayer->m_pProfile)
		m_pCurPlayer->SaveUserProfile();

	if (m_pLoadBGMThread)
	{
		m_pLoadBGMThread->join();
		delete m_pLoadBGMThread;
		m_pLoadBGMThread = nullptr;
	}

	if (m_pLoadSoundThread)
	{
		m_pLoadSoundThread->join();
		delete m_pLoadSoundThread;
		m_pLoadSoundThread = nullptr;
	}
}

void Game::AttachWindow(NaPlatformWindowHandle hWindow)
{
	m_hWindow = hWindow;

	if (m_pRenderManager)
		m_pRenderManager->Init();
}

void Game::ProcessGameLoop()
{
	if (m_Option.m_bSoftwareFrameSkip)
	{
		BeginFrameSkip();

		if (CanUpdateProcess())
		{
			ProcessGame();
		}

		if (!IsSkipFrame())
		{
			Render();
			NaPlatform::Sleep(15);
		}

		EndFrameSkip();
	}
	else
	{
		ProcessGame();
		Render();
	}
}

void Game::ProcessGame()
{
	// ProcessKey
	m_pInputManager->Process();

	// Run StateMachine
	if (m_pGameState)
	{
		m_pGameState->Process();
		m_pGameState->m_nStateFrame++;
	}

	ProcessSceneEffect();

	// #TODO #CHECK move into state?
	m_pFMod->Process();

	OnChangeState();

	m_pInputManager->OnAfterProcess();
	m_nGameFrame++;
}

void Game::LoadResources()
{
	LoadPreloadResources();

	m_pRenderManager->BeginRenderFrame(0);
	m_pRenderManager->Text((Width() / 2) - 40, 120 - 4, L"Loading...");
	m_pSpriteManager->RenderQueue();
	m_pRenderManager->EndRenderFrame();

	LoadSprites();
	
	// Load Sound,BGM using thread
	//LoadSounds();
	//LoadBGMs();
	m_pLoadBGMThread = new std::thread(LoadBGMThread, this);
	m_pLoadSoundThread = new std::thread(LoadSoundThead, this);
}

void Game::LoadSoundThead(void *pParam)
{
	// #TODO to check lifecycle.. make function logger (instance)
	NaDebugOut(L"LoadSoundThead begin\n");
	Game *pThis = (Game*)pParam;
	pThis->LoadSounds();

	NaDebugOut(L"LoadSoundThead end\n");
}

void Game::LoadBGMThread(void *pParam)
{
	// #TODO to check lifecycle.. make function logger (instance)
	NaDebugOut(L"LoadBGMThread begin\n");
	Game *pThis = (Game*)pParam;	
	pThis->LoadBGMs();

	NaDebugOut(L"LoadBGMThread end\n");
}

void Game::LoadPreloadResources()
{
	// For Loading Screen.
	m_pSpriteManager->Load(L"Font", SPRITE_LOAD_TILE8);

	// For Title Screen
	m_pFMod->LoadBGM(L"Title");
	m_pFMod->LoadBGM(L"Overworld");

	// For Logo Screen
	m_pFMod->LoadSound(L"Coin", L"wav");
}

void Game::LoadSprites()
{
	wchar_t* szSprites[] =
	{
		L"Bullet",
		L"MapObject",
		L"Etc",
		L"Vehicle",
	};
	
	// Load Sprite
	NaDebugOut(L"// Load Sprites...\n");
	int nLen = sizeof(szSprites) / sizeof(wchar_t*);
	for (int i = 0; i < nLen; i++)
	{
		m_pSpriteManager->Load(szSprites[i]);
		NaPlatform::Sleep(10);
	}

	// Load Background Tile Sprite
	m_pSpriteManager->Load(L"TileBackground", SPRITE_LOAD_TILE16);

	// Load Mario Parts Sprite, Theme Sprite
	// -> Changed to load dynamically
}

void Game::LoadBGMs()
{
	if (m_Option.m_bPlayBGM == false)
		return;

	wchar_t* szBGMs[] =
	{
		//L"Overworld",
		L"Underground",
		L"UnderWater",
		L"Castle",
		L"Starman",
		L"LevelComplete",
		L"Death",
		L"HurryUp",
		L"GameOver",
		L"Ending",
		L"CastleComplete",
		L"Giant",
		L"PSwitch",
		L"GhostHouse",
		L"AirShip",
		L"Snow",
		L"HitGoalBox",
		L"HitGoalPost",

		L"10MarioMap",
		L"100MarioMap",
		L"StageList",
	};

	// Load BGM
	NaDebugOut(L"// Load BGMs...\n");
	int nLen = sizeof(szBGMs) / sizeof(wchar_t*);
	for (int i = 0; i < nLen; i++)
	{
		m_pFMod->LoadBGM(szBGMs[i]);

		if (m_bExit)
			return;
		NaPlatform::Sleep(10);
	}
}

void Game::LoadSounds()
{
	if (m_Option.m_bPlaySound == false)
		return;

	wchar_t* szSounds[] =
	{
		// Re-newed
		L"1UP", L"wav",
		L"BreakBlock", L"wav",
		L"Cannon", L"wav",
		L"FlagDown", L"wav",
		L"HitBlock", L"wav",
		L"PowerUp", L"wav",
		L"JumpSmall", L"wav",
		L"JumpBig", L"wav",
		L"BowserFall", L"wav",
		L"BowserFire", L"wav",
		L"PowerDown", L"wav",
		L"ItemPop", L"wav",
		L"Pressed", L"wav",
		L"ThrowFireball", L"wav",
		L"Kicked", L"wav",
		L"BreakBridge", L"wav",
		L"MenuTok", L"wav",
		L"MenuTak", L"wav",
		L"MarioLetsGo", L"wav",
		L"Pause", L"wav",

		// Not Re-newed yet
		L"Score", L"mp3",
		L"Scores", L"mp3",
		L"Vine", L"mp3",
		L"Die", L"wav",
		L"RunBreak", L"wav",

		L"ShoeStep", L"wav",
		L"SceneCut", L"wav",
		L"ClapLong", L"wav",
		L"YoshiEggBreak", L"wav",
		L"YoshiRide", L"wav",
		L"YoshiSwallow", L"wav",
		L"YoshiStomp", L"wav",
		L"YoshiSpit", L"wav",
		L"YoshiTongue", L"wav",
		L"YoshiDamaged", L"wav",
		L"YoshiFire", L"wav",
		L"YoshiStompImmune", L"wav",

		L"DoorKey", L"wav",
		L"DoorKeyCoin", L"wav",
		L"DoorLocked", L"wav",
		L"DoorUnlock", L"wav",
		L"DoorOpen", L"wav",
		L"DoorKeyAppear", L"wav",
		L"CheckPoint", L"wav",
		L"MessageBlock", L"wav",
		L"Text", L"wav",
		L"JumpSpin", L"wav",
		L"BoomerangSpin", L"wav",
		L"PowerUpMega", L"wav",

		L"PassGate", L"wav",
		L"NotCorrect", L"wav",
		L"ShoeDead", L"wav",
		L"ShoeStiletto1", L"wav",
		L"ShoeStiletto2", L"wav",
		L"ShoeStiletto3", L"wav",
	};

	// Load Sounds
	NaDebugOut(L"// Load Sound Effects...\n");
	int nLen = sizeof(szSounds) / sizeof(wchar_t*);
	for (int i = 0; i < nLen; i += 2)
	{
		m_pFMod->LoadSound(szSounds[i], szSounds[i + 1]);

		if (m_bExit)
			return;
		NaPlatform::Sleep(10);
	}
}

void Game::InitState(int nState)
{
	NaDebugOut(L"Init GameState: %d:%ls\n", nState, g_szGameStates[nState]);

	GameStateBase *pState = GameStateBase::CreateInstance(this, nState);
	if (pState)
	{
		pState->m_pOldState = m_pGameState;

		m_pGameState = pState;
		m_pGameState->Init();
	}
	else
	{
		NaDebugOut(L"GameState not implemented: %d\n", nState);
	}
}

void Game::OnChangeState()
{
	if (m_nNextState != -1)
	{
		if (m_Option.m_bTestMode)
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
#ifdef WIN32
				::PostQuitMessage(0);
#else
				m_pGame->m_bExit = true;
#endif
			}
			m_nNextState = -1;
		}
		else
		{
			InitState(m_nNextState);
			m_nNextState = -1;
		}
	}
}

void Game::ChangeState(int nState, GameStateChangeParameter *pParam)
{
	m_nNextState = nState;
	if (pParam)
		m_paramNextState = *pParam;
}

int Game::GetStateFrame()
{
	if (m_pGameState)
		return m_pGameState->m_nStateFrame;
	
	return 0;
}

bool Game::IsActiveAbility(int nFlag)
{
	Stage *pTopStage = m_pCurStage->GetTopStage();

	return (m_nStagePackAbilityFlag & nFlag) || (pTopStage->m_nAbilityFlag & nFlag);
}

void Game::LoadEntranceStage()
{
	switch (m_pCurStage->m_nEntranceType)
	{
	case STAGEENTRANCE_UNDERGROUND:
		{
			Stage *pNewStage = new Stage(this);

			pNewStage->CreatePresetStage(L"CommonEntrance");
			pNewStage->m_pParentStage = m_pCurStage;

			m_pCurStage = pNewStage;
		}
		break;
	}
}

void Game::LoadNextStage()
{
	PopAllStage();

	NaString strNext = m_pCurStage->m_strNextStageName;

	m_pCurStage->Clear();
	m_pCurStage->Load(strNext);
}

void Game::LoadPipeStage()
{
	Stage *pNewStage = new Stage(this);
	pNewStage->Load(m_paramNextState.m_strOption);
	pNewStage->m_pParentStage = m_pCurStage;

	if (m_pCurStage->m_bSwapCoinBlock)
		pNewStage->SwapCoinAndBrick();

	m_pCurStage = pNewStage;
}

void Game::LoadSkyStage()
{
	Stage *pNewStage = new Stage(this);
	pNewStage->Load(m_pCurStage->m_strSkyStageName);
	pNewStage->m_pParentStage = m_pCurStage;

	m_pCurStage = pNewStage;
}

void Game::LoadWarpStage(NaString strStage)
{
	PopAllStage();

	m_pCurStage->Clear();
	m_pCurStage->Load(strStage);
}

void Game::LoadSubStage()
{
	NaDebugOut(L"LoadSubStage\n");

	NaString &strName = m_paramNextState.m_strOption;

	if (m_pCurStage->m_bIsSubStage)
		m_pCurStage = m_pCurStage->m_pParentStage;

	Stage *pNewStage = m_pCurStage->FindSubStage(strName);
	if (pNewStage == nullptr)
	{
		pNewStage = new Stage(this);

		pNewStage->Load(strName);
		pNewStage->m_pParentStage = m_pCurStage;
		pNewStage->m_bIsSubStage = true;

		if (pNewStage->m_nMaxDoorKey > 0)
			m_pCurStage->m_nMaxDoorKey += pNewStage->m_nMaxDoorKey;

		m_pCurStage->AddSubStage(pNewStage);

		if (m_pCurStage->m_bSwapCoinBlock)
			pNewStage->SwapCoinAndBrick();
	}
	
	NaDebugOut(L"-> Current: %s\n", pNewStage->m_strName.wstr());
	m_pCurStage = pNewStage;
}

void Game::PopStage()
{
	NaDebugOut(L"PopStage\n");

	Stage *pCurStage = m_pCurStage;
	Stage *pChildStage;
	if (pCurStage->m_pParentStage)
	{
		pChildStage = pCurStage;
		pCurStage = pCurStage->m_pParentStage;

		NaDebugOut(L"-> Removed: %s\n", pChildStage->m_strName.wstr());
		if (!pChildStage->m_bIsSubStage)
			delete pChildStage;
	}

	NaDebugOut(L"-> Current: %s\n", pCurStage->m_strName.wstr());
	m_pCurStage = pCurStage;
}

void Game::PopAllStage()
{
	NaDebugOut(L"PopAllStage\n");

	Stage *pCurStage = m_pCurStage;
	Stage *pChildStage;
	while (true)
	{
		if (pCurStage->m_pParentStage)
		{
			pChildStage = pCurStage;
			pCurStage = pCurStage->m_pParentStage;
			
			NaDebugOut(L"-> Removed: %s\n", pChildStage->m_strName.wstr());
			if (!pChildStage->m_bIsSubStage)
				delete pChildStage;
		}
		else
			break;
	}

	NaDebugOut(L"-> Current: %s\n", pCurStage->m_strName.wstr());
	m_pCurStage = pCurStage;
}

void Game::SwapCoinAndBrick(bool bIsInitial)
{
	// Swap Coin&Brick from all loaded Stage
	Stage *pStage = m_pCurStage;
	pStage->m_nReqSwapCoinBlock = bIsInitial ? 1 : -1;

	/*
	while (pStage)
	{
		BEGIN_VECTOR_LOOP(Stage*, pStage->m_vecSubStage) {
			if (pObj->m_bSwapCoinBlock != bIsInitial)
				pObj->SwapCoinAndBrick();
		} END_VECTOR_LOOP();

		if (pStage->m_bSwapCoinBlock != bIsInitial)
			pStage->SwapCoinAndBrick();

		pStage = pStage->m_pParentStage;
	}
	*/
}

void Game::InitPlayerObject(POINT ptLastCheckPoint)
{
	GameObjectBase *pPlayerObj = m_pCurPlayer->GetLastControlObject();
	pPlayerObj->ChangeState(STATE_STAND);

	if (ptLastCheckPoint.x != -1)
	{
		pPlayerObj->SetPosition(ptLastCheckPoint.x, ptLastCheckPoint.y);

		// Deactivate checkpoint flag
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_pCurStage->m_vecEventObject) {
			if (pObj->m_nType == EVENTOBJECT_CHECKPOINT_FLAG)
			{
				if (pPlayerObj->IsOverlapped(pObj))
				{
					pObj->m_nShape = 2;
				}
			}
		} END_VECTOR_LOOP();
	}
	else
	{
		pPlayerObj->SetPosition(
			m_pCurStage->m_ptStartPoint.x,
			m_pCurStage->m_ptStartPoint.y
		);
	}
	pPlayerObj->m_bFlip = false;
}

void Game::InitCamera()
{
	// #TODO need improvement
	//m_pCamera->m_ptPos.x = m_pCurStage->m_ptStartPoint.x - (TILE_XS * 3);
	int nCurPlayerObjX = m_pCurPlayer->m_pControlObj->m_fX;
	int nPageX = TO_PAGECOORD_X(nCurPlayerObjX);
	int nPage = TO_PAGE_X(nCurPlayerObjX);

	int x, y;
	x = m_pCamera->m_ptPos.x;
	y = m_pCamera->m_ptPos.y;

	// Set X
	if (m_pCurStage->m_bCanGoBack == false)
	{
		if (nPageX < CENTER_OF_PAGE)
		{
			x = TO_REALCOORD_X(nPage, 0);
		}
		else
		{
			x = TO_REALCOORD_X(nPage, 0);
		}
	}
	else
	{
		x = max(0, nCurPlayerObjX - (m_nWidth / 2));
	}

	// Set Y
	y = m_pCurPlayer->m_pControlObj->m_fY - (m_nHeight / 2);

	m_pCamera->SetPosition(x, y);
	//m_pCamera->m_ptPos.x = m_pCurPlayer->m_pControlObj->m_fX - (TILE_XS * 3) - (TILE_XS / 2);
}

void Game::Render()
{
	DWORD dwBG = m_pGameState->GetBackgroundColor();
	m_pRenderManager->BeginRenderFrame(dwBG);

	// Render Process
	m_pGameState->Render();
	RenderSceneEffect();

	m_pSpriteManager->RenderQueue();

	m_pRenderManager->EndRenderFrame();
	m_FrameSkipInfo.m_nRenderCnt++;
}

void Game::CreateSceneEffect(float fX, float fY, int nType, CreateParameter *pParam)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(this, m_pCurStage, nType);
	if (pObj)
	{
		pObj->m_fX = fX;
		pObj->m_fY = fY;
		pObj->ParseParameter(pParam);
		m_vecEffect.push_back(pObj);
	}
}

void Game::ProcessSceneEffect()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEffect) {
		pObj->OnBeforeProcess(); 
		pObj->Process();
		pObj->OnAfterProcess();
	} END_VECTOR_LOOP();

	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEffect) {
		if (pObj->m_nState == STATE_DEAD)
		{
			delete pObj;
			it = m_vecEffect.erase(it);
		}
		else
			++it;
	} END_VECTOR_LOOP();
}

void Game::ClearSceneEffect()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEffect) {
		pObj->m_nState = STATE_DEAD;
	} END_VECTOR_LOOP();
}

void Game::RenderSceneEffect()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEffect) {
		pObj->Render();
	} END_VECTOR_LOOP();
}

