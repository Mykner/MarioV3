#include "GameStateSingleStageGameOver.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "FModManager.h"
#include "UserProfile.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "NaPlatformAPI.h"
#include "UIButton.h"
#include "UIList.h"

#include "ServerRequest.h"
#include "StageMetaInfo.h"
#include "UIEdit.h"

GameStateSingleStageGameOver::GameStateSingleStageGameOver(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_SINGLESTAGEGAMEOVER;

	m_Transaction.Init(pGame);
	m_pCurMetaInfo = nullptr;
}

GameStateSingleStageGameOver::~GameStateSingleStageGameOver()
{
	SAFE_DELETE(m_pCurMetaInfo);
}

void GameStateSingleStageGameOver::Init()
{
	DeleteOldState();

	UI_MANAGER->Clear();

	CUR_PLAYER->LockInput();
	CUR_PLAYER->UnholdInputAll();

	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlayBGM(L"GameOver", false);
}

void GameStateSingleStageGameOver::Process()
{
	switch (m_nStateFrame)
	{
	case 1:
		if (IS_ONLINESTAGE && m_pGame->m_Option.m_bTestMode == false)
		{
			m_Transaction.m_pJobThread = new std::thread(AddFailedCount, this);
		}
		break;
	case 25:
		{
			CUR_PLAYER->UnholdInputAll();
		}
		break;
	case 64 + 72:
		{
			UIPanel *pPanel = UI_MANAGER->AddPanel(0, 144);
			pPanel->m_bAutoAlignX = true;
			pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

			int i = 0;
			UI_MANAGER->CreateButton(UI_RETRY, 0, 0 + (i++ * 16), L"RETRY");

#if !defined(NDEBUG)
			NaString strRating = L"0";
			if (m_pCurMetaInfo)
				strRating.Format(L"%d", m_pCurMetaInfo->m_nDeveloperRating);
			
			UI_MANAGER->CreateEdit(
				UI_RATESTAGE, 0, 0 + ((i++) * 16), strRating, L"Developer's Rating: ");
#endif

			auto pUI = UI_MANAGER->CreateButton(0, 0, 0 + (i++ * 16), L"TRY ANOTHER STAGE");
			pUI->m_bEnable = false;

			UI_MANAGER->CreateButton(UI_BACKTOTITLE, 0, 0 + (i++ * 16), L"GIVE UP");
		}
		break;
	}

	UI_MANAGER->Process();

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();
		m_Transaction.ChangeJob(JOB_NONE);
	}

	if (!m_Transaction.IsWorking() && UI_MANAGER->m_pClicked)
	{
		int nIDUI = UI_MANAGER->m_pClicked->m_nID;
		switch (nIDUI)
		{
		case UI_RETRY:
			CUR_PLAYER->Reset();

			m_pGame->PopAllStage();
			CUR_STAGE->Load(m_pGame->m_strInitialStage.wstr());

			UI_MANAGER->Clear();
			m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
			return;
			break;
		case UI_RATESTAGE:
			m_Transaction.m_pJobThread = new std::thread(RateStage, this);
			break;
		case UI_BACKTOTITLE:
			m_pGame->ChangeState(GAMESTATE_TITLE);
			return;
			break;
		}
	}

	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->ProcessPSwitchTime();
	CUR_PLAYER->OnAfterProcess();

	CUR_STAGE->ProcessEffects();
}

void GameStateSingleStageGameOver::AddFailedCount(void *pParam)
{
	GameStateSingleStageGameOver *pThis = (GameStateSingleStageGameOver*)pParam;

	pThis->m_Transaction.BeginJob(JOB_ADD_FAILED);

	ServerRequest req;
	try
	{
		if (pThis->m_pCurMetaInfo == nullptr)
		{
			auto pTopStage = pThis->m_pGame->m_pCurStage->GetTopStage();
			auto pMetaInfo = req.GetMetaInfo(pTopStage->m_strUniqueId);
			pThis->m_pCurMetaInfo = pMetaInfo;
		}

		pThis->m_pCurMetaInfo->m_nFailed += 1;

		req.UpdateMetaInfo(pThis->m_pCurMetaInfo, UPDATE_METAINFO_FAILED);

		pThis->m_Transaction.ChangeJob(JOB_ADD_FAILED_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Error", MB_OK | MB_ICONERROR);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateSingleStageGameOver::RateStage(void * pParam)
{
	GameStateSingleStageGameOver *pThis = (GameStateSingleStageGameOver*)pParam;

	int nScore = -1;
	UIEdit *pUI = (UIEdit*)pThis->m_pGame->m_pUIManager->GetFocus();
	if (pUI->m_strText.GetLength() > 0)
	{
		nScore = pUI->m_strText.ToInt();
	}

	pThis->m_Transaction.BeginJob(JOB_RATE);

	ServerRequest req;
	try
	{
		if (pThis->m_pCurMetaInfo == nullptr)
		{
			auto pTopStage = pThis->m_pGame->m_pCurStage->GetTopStage();
			auto pMetaInfo = req.GetMetaInfo(pTopStage->m_strUniqueId);
			pThis->m_pCurMetaInfo = pMetaInfo;
		}

		pThis->m_pCurMetaInfo->m_nDeveloperRating = nScore;

		req.UpdateMetaInfo(pThis->m_pCurMetaInfo, UPDATE_METAINFO_DEVELOPERRATING);

	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Error", MB_OK | MB_ICONERROR);
	}

	pThis->m_Transaction.ChangeJob(JOB_RATE_END);
	pThis->m_Transaction.EndJob();
}

void GameStateSingleStageGameOver::Render()
{
	const int nWidth = m_pGame->Width();

	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();

	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();

	NaString str;
	if (m_nStateFrame >= 0)
	{
		str.Format(L"GAME OVER");

		RENDER_MANAGER->Text(
			(nWidth / 2) - (str.GetLength() * (4 * 1.5f)), 64,
			str, 0xffffffff, 1.5f, Z_STATUS, true);
	}

	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth - 25, m_pGame->m_nHeight - 25, 0.5f);
	}

	UI_MANAGER->Render();

	UIBase* pFocusedUI = UI_MANAGER->GetFocus();
	if (pFocusedUI)
	{
		bool bVisible = true;
		if (pFocusedUI->m_pParent && pFocusedUI->m_pParent->m_nType == UI_LIST)
		{
			bVisible = ((UIList*)(pFocusedUI->m_pParent))->IsVisibleChild(pFocusedUI);
		}

		if (bVisible)
		{
			NaRect rc = pFocusedUI->GetScreenRect();
			DRAWTEXT(
				rc.left - 12,
				rc.top + rc.Height() / 2 - 4,
				L"\x08");
		}
	}
}

