#include "FakeGame.h"

#include "RenderManager.h"
#include "SpriteManager.h"
#include "InputManager.h"

#include "ResourceHolder.h"
#include "GameObjectBase.h"

#include "Stage.h"
#include "Player.h"
#include "Camera.h"
#include "NaPlatformAPI.h"
#include "TypeNames.h"

#include "Mario.h"
#include "PathManager.h"
#include "JsonManager.h"

extern FakeGame g_Game[2];

FakeGame::FakeGame()
{
	m_pRenderManager = nullptr;
	m_pObj = nullptr;
}

FakeGame::~FakeGame()
{
}

void FakeGame::Init(NaPlatformWindowHandle hWindow, const wchar_t * szCurrentDir)
{
	NaDebugOut(L"Init Game\n");

	if (szCurrentDir)
		m_strGameRootDir = szCurrentDir;
	else
	{
		m_strGameRootDir = NaPlatform::GetCurrentPath();
	}

#if defined(_DEBUG)
	m_nWidth = GameWindowDefaults::nInnerWidth;
	m_nHeight = GameWindowDefaults::nInnerHeight;
#else
	m_nWidth = GameWindowDefaults::nInnerWidth;
	m_nHeight = GameWindowDefaults::nInnerHeight;
#endif

	// Init Screen
	m_hWindow = hWindow;

	int nZoomFactor = GameWindowDefaults::nZoomFactor;
	RECT rcWindow = { 0, 0, Width()*nZoomFactor, Height()*nZoomFactor };
	AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE);

	int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	int nWidth = rcWindow.right - rcWindow.left;
	int nHeight = rcWindow.bottom - rcWindow.top;
	int nX = nScreenWidth / 2 - nWidth / 2;
	int nY = nScreenHeight / 2 - nHeight / 2;

#if defined(_DEBUG)
	nX = 0;
	nY = 0;
#endif
	NaPlatform::MoveWindow(m_hWindow, nX, nY, nWidth, nHeight);

	m_Option.m_bPlayBGM = false;

	m_pResourceHolder = new ResourceHolder();
	m_pResourceHolder->m_bUseDirectX = false;
	m_pResourceHolder->LoadResources(this);
	
	// Init Stage
	Stage *pStage = new Stage(this);
	//pStage->Load(L"");
	pStage->CreateDefaultGround();
	m_pCurStage = pStage;
	pStage->InitTheme();

	// Init Player
	Player *pPlayer = new Player(this);
	pPlayer->BindInputs(VK_LEFT, VK_RIGHT, VK_UP, VK_DOWN, VK_X, VK_Z);
	
	GameObjectBase *pPlayerObj = GameObjectBase::CreateInstance(this, m_pCurStage, PLAYEROBJECT_MARIO);
	pPlayerObj->m_fX = m_nWidth / 8;
	pPlayerObj->m_fY = 180;
	pPlayerObj->m_nState = STATE_STAND;
	pPlayerObj->m_bFlip = false;

	pPlayer->SetControl(pPlayerObj);
	m_vecPlayers.push_back(pPlayer);
	m_pCurPlayer = m_vecPlayers[0];

	// Some setups
	InitMarioPhysicalProps();
	m_nStagePackType = STAGEPACK_CUSTOM;

	int nType;
	nType = ENEMY_PIRANHA_PLANT;
	//nType = ENEMY_HAMMERBROTHERS;
	//nType = ENEMY_FIREPIRANHAPLANT;
	//nType = ENEMY_BOBOMB;
	//nType = ENEMY_DRYBONE;
	//nType = ENEMY_GRINDER;
	//nType = ENEMY_THWOMP;
	//nType = ENEMY_SLEDGEBROTHERS;

	for (int i = 10; i < 16; i++)
	{
		m_pCurStage->SetTileData(i, 5, TILE_BRICK);
		m_pCurStage->SetTileData(i, 9, TILE_BRICK);
	}

	m_pObj = GameObjectBase::CreateInstance(this, m_pCurStage, nType);
	m_pObj->m_fX = m_nWidth / 2;
	m_pObj->m_fY = 100;
	m_pObj->m_nType = nType;
	m_pObj->m_bPlayerSide = false;

	m_pObj->m_bFlip = true;	

	// Init Camera
	m_pCamera = new Camera(this);
	m_pCamera->LookAt(m_pObj);

	m_bModernStyle = true;

	NaPlatform::ShowWindow(m_hWindow, true);
}

void FakeGame::ProcessGameLoop()
{
	m_pInputManager->Process();

	static bool bContinue = true;
	if (bContinue || (!bContinue && m_pInputManager->IsKeyPressed(VK_F)))
	{
		m_pObj->OnBeforeProcess();
		m_pCurStage->OnBeforeProcess();
		m_pCurPlayer->OnBeforeProcess();

		m_pObj->Process();
		m_pCurStage->Process();
		m_pCurStage->ProcessEffects();
		m_pCurPlayer->Process();
	
		m_pObj->OnAfterProcess();
		m_pCurStage->OnAfterProcess();
		m_pCurPlayer->OnAfterProcess();

		m_pCamera->Process();
	}
	if (bContinue && m_pInputManager->IsKeyPressed(VK_F))
		bContinue = false;
	if (m_pInputManager->IsKeyPressed(VK_P))
		bContinue = true;

	if (m_pCurPlayer->m_pControlObj->m_nState == STATE_DIE)
	{
		GameObjectBase* pPlayerObj = m_pCurPlayer->m_pControlObj;
		pPlayerObj->m_fX = m_nWidth / 8;
		pPlayerObj->m_fY = 180;
		pPlayerObj->m_nState = STATE_STAND;
		pPlayerObj->m_bFlip = false;
		pPlayerObj->m_nDamageCooltime = 16 * 5;
	}

	SIZE s = m_pObj->GetSize();
	if (m_pObj->IsOutOfCamera())
	{
		m_pObj->m_fX = m_nWidth / 2;
		m_pObj->m_fY = 100;
		m_pObj->m_fXS = 0;
		m_pObj->m_fYS = 0;
		m_pObj->SetState(STATE_IDLE);
	}
	
	if (m_pInputManager->IsKeyPressed(VK_SPACE))
		m_pObj->m_bFlip = !m_pObj->m_bFlip;
	if (m_pInputManager->IsKeyPressed(VK_1))
	{
		LoadMarioPhysicalProps();
	}
	
	m_pInputManager->OnAfterProcess();
	m_nGameFrame++;

	Render();
}

void FakeGame::Render()
{
	if (m_pRenderManager == nullptr)
		return;

	m_pRenderManager->BeginRenderFrame(0xff0040ff);

	m_pCurStage->Render();
	m_pCurStage->RenderEffects();
	m_pObj->Render();
	m_pCurPlayer->RenderControlObj();

	m_pSpriteManager->RenderQueue();

	m_pRenderManager->EndRenderFrame();
	
	{
		Mario* pMario = (Mario*)(m_pCurPlayer->m_pControlObj);
		NaString str;
		str.Format(L"Press 1 to reload MarioPhysics.json\n"
			L"AccelWalk: %f\n"
			L"AccelRun: %f\n"
			L"MaxWalkSpeed: %f\n"
			L"MaxRunSpeed(Classic): %f\n"
			L"MaxRunSpeed(NonClassic): %f\n"
			L"MaxRunSpeed(Invinsible): %f\n"
			L"DecelWalk: %f\n"
			L"AccelSkid: %f\n",
			pMario->m_PhysicalProps.fAccelWalk,
			pMario->m_PhysicalProps.fAccelRun,
			pMario->m_PhysicalProps.fMaxWalkSpeed,
			pMario->m_PhysicalProps.fMaxRunSpeedClassic,
			pMario->m_PhysicalProps.fMaxRunSpeedNonClassic,
			pMario->m_PhysicalProps.fMaxRunSpeedInvinsible,
			pMario->m_PhysicalProps.fDecelWalk,
			pMario->m_PhysicalProps.fAccelSkid
		);		
		m_pRenderManager->Text(8, 8, str, 0xffffffff,
			0.5f, Z_TEXT, true);
	}
}

void FakeGame::InitMarioPhysicalProps()
{
	Mario* pMario = (Mario*)(m_pCurPlayer->m_pControlObj);

	pMario->m_PhysicalProps.BuildPropertyMap();
}

void FakeGame::LoadMarioPhysicalProps()
{
	Mario* pMario = (Mario*)(m_pCurPlayer->m_pControlObj);

	NaString strName;
	strName.Format(L"%ls\\TestMarioPhysics.json", PathManager::GetGameRootPath(this));
	Json::Value *j = JsonManager::LoadFile(strName.cstr());
	if (j)
	{
		JsonManager::LoadJsonObjectProperty(&pMario->m_PhysicalProps, *j);
		delete j;
		j = nullptr;
	}
	else
	{
		NaDebugOut(L"TestMarioPhysics.json not found.\n");

		Json::Value j2;
		if (JsonManager::SaveJsonObjectProperty(&pMario->m_PhysicalProps, j2))
			JsonManager::SaveFile(strName.cstr(), &pMario->m_PhysicalProps, j2);
	}
}
