#include "FakeGame.h"

#include "RenderManager.h"
#include "SpriteManager.h"

#include "OpenGLManager.h"
#include "DirectXManager.h"
#include "NaPlatformAPI.h"
#include "InputManager.h"

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
	m_nWidth = 256;// GameWindowDefaults::nInnerWidth;
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

	if (this == &g_Game[0])
	{
		m_pRenderManager = new OpenGLManager(this);
		::MoveWindow(m_hWindow, nX, nY, nWidth, nHeight, TRUE);
	}
	else
	{
		m_pRenderManager = new DirectXManager(this);
		::MoveWindow(m_hWindow, nX + 530, nY, nWidth, nHeight, TRUE);
	}
	m_pRenderManager->Init();

	m_pInputManager = new InputManager(this);

	// Load Sprite
	m_pSpriteManager = new SpriteManager(this);
	m_pSpriteManager->Load(L"Font");
	m_pSprite = m_pSpriteManager->Request(L"Mario");
	m_nFrame = 1;

	NaPlatform::ShowWindow(m_hWindow, true);
}

void FakeGame::ProcessGameLoop()
{
	m_pInputManager->Process();

	if (m_pInputManager->IsKeyPressed(VK_ADD))
	{
		m_nFrame++;
	}
	if (m_pInputManager->IsKeyPressed(VK_SUBTRACT))
	{
		m_nFrame--;
	}

	m_pInputManager->OnAfterProcess();
	Render();
}

void FakeGame::Render()
{
	if (m_pRenderManager == nullptr)
		return;

	m_pRenderManager->BeginRenderFrame(0xff0040ff);

	static float fAngle = 0.0f;

	/*
	// 1. Normal
	int y = 0;
	m_pRenderManager->Text(0, y, L"Normal Sprite Sheet", 0xffffffff);
	for (int i=0; i<10; i++)
	{
		m_pSprite->Render(10 + i*20, y + 24, i);
	}

	// 2. Flip
	y += 26;
	m_pRenderManager->Text(0, y, L"FlipH", 0xffffffff);
	for (int i = 0; i < 10; i++)
	{
		m_pSprite->Render(10 + i * 20, y + 24, i, true);
	}

	// 3. FlipV
	y += 26;
	m_pRenderManager->Text(0, y, L"FlipV", 0xffffffff);
	for (int i = 0; i < 10; i++)
	{
		m_pSprite->Render(10 + i * 20, y + 24, i, false, true);
	}

	// 4. ScaleXY
	y += 39;
	m_pRenderManager->Text(0, y, L"ScaleXY", 0xffffffff);
	for (int i = 0; i < 10; i++)
	{
		m_pSprite->Render(10 + i * 20, y + 24, i, false, false, -1L, 
			1.1f + (0.1f * i),
			1.1f + (0.1f * i));
	}

	// 5. ScaleXY
	y += 39;
	m_pRenderManager->Text(0, y, L"ScaleXY + FlipH", 0xffffffff);
	for (int i = 0; i < 10; i++)
	{
		m_pSprite->Render(10 + i * 20, y + 24, i, true, false, -1L,
			1.1f + (0.1f * i),
			1.1f + (0.1f * i));
	}

	// 6. Rotation
	y += 26;
	m_pRenderManager->Text(0, y, L"Rotate", 0xffffffff);
	for (int i = 0; i < 10; i++)
	{
		m_pSprite->Render(10 + i * 20, y + 24, i, true, false, -1L,
			1.0f, 1.0f, 0.0f, fAngle);
	}
	fAngle += 1.0f;
	//*/

	///*
	float fScale = 3.0f;
	m_pSprite->Render(128, 60, m_nFrame, 
		false, 
		false, 
		-1L, 
		fScale, fScale,
		0, 
		fAngle, 1);

	m_pSprite->Render(128, 120, m_nFrame,
		true,
		false,
		-1L,
		fScale, fScale,
		0,
		fAngle);

	m_pSprite->Render(128, 180, m_nFrame,
		false,
		true,
		-1L,
		fScale, fScale,
		0,
		fAngle);
	fAngle += 1.0f;
	//*/

	/*
	float fScale = 1.0f;
	m_pSprite->Render(128, 60, m_nFrame,
		false,
		false,
		-1L,
		fScale, fScale,
		0,
		fAngle);

	m_pSprite->Render(128, 180, m_nFrame,
		true,
		false,
		-1L,
		fScale, fScale,
		0,
		fAngle);
	//*/

	m_pSpriteManager->RenderQueue();

	//m_pRenderManager->EndRenderFrame();

	HDC hdc = GetDC(m_hWindow);
	::MoveToEx(hdc, 0, 120, NULL); ::LineTo(hdc, 512, 120);
	::MoveToEx(hdc, 0, 240, NULL); ::LineTo(hdc, 512, 240);
	::MoveToEx(hdc, 0, 360, NULL); ::LineTo(hdc, 512, 360);
	::MoveToEx(hdc, 256, 0, NULL); ::LineTo(hdc, 256, 480);

	::MoveToEx(hdc, 0, 120, NULL); ::LineTo(hdc, 512, 120);
	::MoveToEx(hdc, 0, 240, NULL); ::LineTo(hdc, 512, 240);
	::MoveToEx(hdc, 0, 360, NULL); ::LineTo(hdc, 512, 360);
	::MoveToEx(hdc, 256, 0, NULL); ::LineTo(hdc, 256, 480);

	::ReleaseDC(m_hWindow, hdc);
	Sleep(10);

	SpriteFrame &f = m_pSprite->m_vecFrames[m_nFrame];
	//f.m_ptOffset.x = 0;

	NaString str;
	str.Format(L"w/h: %d,%d\noff: %d,%d\n",
		f.m_rc.right - f.m_rc.left, f.m_rc.bottom - f.m_rc.top,
		f.m_ptOffset.x, f.m_ptOffset.y
	);
	m_pRenderManager->Text(0, 0, str);

	m_pRenderManager->EndRenderFrame();
}
