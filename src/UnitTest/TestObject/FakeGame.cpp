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

	m_pResourceHolder = new ResourceHolder();
	m_pResourceHolder->m_bUseDirectX = false;
	m_pResourceHolder->LoadResources(this);
	
	// Init Stage
	Stage *pStage = new Stage(this);
	pStage->Load(L"");
	m_pCurStage = pStage;

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

	int nType;
	//nType = ENEMY_HAMMERBROTHERS;
	nType = ENEMY_FIREPIRANHAPLANT;
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
		m_pObj->OnChangeShape(SHAPE_MINI);
	}
	if (m_pInputManager->IsKeyPressed(VK_2))
	{
		m_pObj->OnChangeShape(SHAPE_BIG);
	}
	if (m_pInputManager->IsKeyPressed(VK_4))
	{
		m_pObj->OnChangeShape(SHAPE_GIANT);
	}
	if (m_pInputManager->IsKeyPressed(VK_ADD))
	{
		m_pObj->m_nStateFrame++;
	}
	if (m_pInputManager->IsKeyPressed(VK_SUBTRACT))
	{
		m_pObj->m_nStateFrame--;
	}

	m_pInputManager->OnAfterProcess();
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
		GameObjectBase *pObj = m_pObj;

		NaString str = L"";
		NaString strType;
		strType.Format(L"%ls (0x%08x)\n\n",
			g_szEnemyTypes[pObj->m_nType - ENEMY_BEGIN], pObj);
		str += strType;

		NaString str2;
		float fScaleX, fScaleY;
		pObj->GetScale(fScaleX, fScaleY);

		str2.Format(L"X,Y:%.2f,%.2f\nSpeed:%.2f,%.2f\nScale:%.1f,%.1f\n",
			pObj->m_fX, pObj->m_fY,
			pObj->m_fXS, pObj->m_fYS,
			fScaleX, fScaleY);
		str += str2;

		str2.Format(L"State:%ls (%d) (StateFrame:%d)\nCollision:\n__%ls\n%ls_*_%ls\n__%ls\n",
			pObj->GetStateName().wstr(),
			pObj->m_nState,
			pObj->m_nStateFrame,
			pObj->m_bCollision[COLLISION_TOP] ? L"O" : L"X",
			pObj->m_bCollision[COLLISION_LEFT] ? L"O" : L"X",
			pObj->m_bCollision[COLLISION_RIGHT] ? L"O" : L"X",
			pObj->m_bCollision[COLLISION_BOTTOM] ? L"O" : L"X");
		str += str2;

		str2.Format(L"Combo:%d", pObj->m_nCombo);
		str += str2;

		if (str.GetLength() > 0)
		{
			m_pRenderManager->Text(22, 34, str, 0xffffffff,
				0.5f, Z_TEXT, true);
		}
	}
}
