#include "FakeGame.h"

#include "RenderManager.h"
#include "SpriteManager.h"
#include "InputManager.h"
#include "FModManager.h"
#include "UIManager.h"

#include "ResourceHolder.h"
#include "GameObjectBase.h"

#include "Stage.h"
#include "Player.h"
#include "Camera.h"
#include "NaPlatformAPI.h"

#include "UIPanel.h"
#include "UIBase.h"
#include "UIEdit.h"

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
	NaPlatform::MoveWindow(m_hWindow, nX, nY, nWidth, nHeight);

	m_pResourceHolder = new ResourceHolder();
	m_pResourceHolder->m_bUseDirectX = false;
	m_pResourceHolder->LoadResources(this);

	NaPlatform::ShowWindow(m_hWindow, true);

	InitUI();
}

void FakeGame::ProcessGameLoop()
{
	m_pInputManager->Process();
	
	m_pUIManager->Process();

	if (m_pUIManager->m_pClicked)
	{
		int nIDUI = m_pUIManager->m_pClicked->m_nID;
		switch (nIDUI)
		{
		case UI_FREQUENCY:
			{
				UIEdit *pUI = (UIEdit*)m_pUIManager->GetFocus();

				int nFreq = atoi(pUI->m_strText.cstr());
				m_pFMod->m_pChannel[CHANNEL_BGM]->setFrequency(nFreq);

				/*
				FMOD::DSP *pDsp;
				m_pFMod->m_pSystem->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &pDsp);
				m_pFMod->m_pChannel[CHANNEL_BGM]->addDSP(pDsp, nullptr);
				//*/
			}
			break;				
		case UI_PLAY_BGM:
			{
				m_pFMod->StopBGM();
				m_pFMod->PlayBGM(L"Overworld");
			}
			break;
		}
	}

	m_pInputManager->OnAfterProcess();

	Render();
}

void FakeGame::Render()
{
	if (m_pRenderManager == nullptr)
		return;

	m_pRenderManager->BeginRenderFrame(0xff0040ff);

	m_pUIManager->Render();

	UIBase* pFocusedUI = m_pUIManager->GetFocus();
	if (pFocusedUI)
	{
		NaRect rc = pFocusedUI->GetScreenRect();
		m_pRenderManager->Text(
			rc.left - 12,
			rc.top,
			L"&");
	}

	m_pSpriteManager->RenderQueue();

	m_pRenderManager->EndRenderFrame();
}

void FakeGame::InitUI()
{
	UIPanel *pPanel = m_pUIManager->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int i = 0, y = 0;
	NaString str;
	UIBase *pUI;
	pUI = m_pUIManager->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"--- SOUND TEST ---");

	pUI = m_pUIManager->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"FREQUENCY:");

	pUI = m_pUIManager->CreateEdit(
		UI_FREQUENCY, 0, y + ((i++) * 16), L"1000");

	pUI = m_pUIManager->CreateButton(
		UI_PLAY_BGM, 0, y + ((i++) * 16), L"PLAY BGM");

	pPanel->m_pFocused = pUI;
}
