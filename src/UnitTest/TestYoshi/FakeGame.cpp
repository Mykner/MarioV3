#include "FakeGame.h"

#include "RenderManager.h"
#include "SpriteManager.h"
#include "InputManager.h"

#include "OpenGLManager.h"
#include "GameObjectBase.h"

#include "Stage.h"
#include "Player.h"
#include "Camera.h"
#include "NaPlatformAPI.h"

extern FakeGame g_Game[2];

FakeGame::FakeGame()
{
	m_pRenderManager = nullptr;
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

	m_pRenderManager = new OpenGLManager(this);
	m_pRenderManager->Init();

	m_pInputManager = new InputManager(this);

	m_pSpriteManager = new SpriteManager(this);
	m_pSpriteManager->Load(L"Vehicle");
	m_pSpriteManager->Load(L"Font");
	m_pSprite = m_pSpriteManager->Find(L"Vehicle");

	m_nCurEdit = 0;
	m_nBody = SPRIDX_VEHICLE_YOSHI_BODY_SIT;
	m_nHead = SPRIDX_VEHICLE_YOSHI_HEAD_IDLE;
	m_ptOffset = { 6, -16 };

	NaPlatform::ShowWindow(m_hWindow, true);
}

void FakeGame::ProcessGameLoop()
{
	m_pInputManager->Process();

	if (m_pInputManager->IsKeyPressed(VK_TAB))
	{
		if (m_nCurEdit == 0)
			m_nCurEdit = 1;
		else
			m_nCurEdit = 0;
	}
	if (m_pInputManager->IsKeyPressed(VK_ADD))
	{
		if (m_nCurEdit == 0)
			m_nBody++;
		else
			m_nHead++;
	}
	if (m_pInputManager->IsKeyPressed(VK_SUBTRACT))
	{
		if (m_nCurEdit == 0)
			m_nBody--;
		else
			m_nHead--;
	}
	if (m_pInputManager->IsKeyPressed(VK_LEFT))
		m_ptOffset.x--;
	if (m_pInputManager->IsKeyPressed(VK_RIGHT))
		m_ptOffset.x++;
	if (m_pInputManager->IsKeyPressed(VK_UP))
		m_ptOffset.y--;
	if (m_pInputManager->IsKeyPressed(VK_DOWN))
		m_ptOffset.y++;

	m_pSprite->RenderToQueue(128, 120, m_nBody,
		false, false, -1, 1, 1, 10);
	m_pSprite->RenderToQueue(128 + m_ptOffset.x, 120 + m_ptOffset.y, m_nHead,
		false, false, -1, 1, 1, 11);

	m_pInputManager->OnAfterProcess();
	Render();
}

void FakeGame::Render()
{
	if (m_pRenderManager == nullptr)
		return;

	m_pRenderManager->BeginRenderFrame(0xff0040ff);
	m_pSpriteManager->RenderQueue();

	NaString str;
	str.Format(L"%s Body Frame %d",
		m_nCurEdit == 0 ? L">" : L" ",
		m_nBody
	);
	m_pRenderManager->Text(0, 0, str);

	str.Format(L"%s Head Frame %d (Offset: %d, %d)",
		m_nCurEdit == 1 ? L">" : L" ",
		m_nHead,
		m_ptOffset.x, m_ptOffset.y
	);
	m_pRenderManager->Text(0, 10, str);

	m_pRenderManager->EndRenderFrame();
}
