#include "GameStateSingleStageClear.h"

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
#include "GameConfig.h"

GameStateSingleStageClear::GameStateSingleStageClear(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_SINGLESTAGECLEAR;
	
	m_bGoalByCastle = false;
	m_bBestTime = false;
	m_bBestScore = false;

	m_strTime = L"";
	m_strBestTime = L"<No Profile>";
	m_strBestScore = L"<No Profile>";

	m_Transaction.Init(pGame);
	m_pCurMetaInfo = nullptr;
}

GameStateSingleStageClear::~GameStateSingleStageClear()
{
	SAFE_DELETE(m_pCurMetaInfo);
}

void GameStateSingleStageClear::Init()
{
	NaString strGoalBy = m_pGame->m_paramNextState.m_strOption;
	if (strGoalBy.Compare(L"Castle") == 0)
		m_bGoalByCastle = true;

	DeleteOldState();

	UI_MANAGER->Clear();

	CUR_PLAYER->LockInput();
	CUR_PLAYER->UnholdInput(INPUT_L);
	CUR_PLAYER->UnholdInput(INPUT_R);

	Stage *pTopStage = CUR_STAGE->GetTopStage();
	if (m_bGoalByCastle)
	{
		CUR_PLAYER->HoldInput(INPUT_L);
	}
	
	CUR_PLAYER_OBJ->m_nImmuneTime = 30000;

	// Check Record
	DWORD dwTime = pTopStage->m_dwPlayTime;
	m_strTime.Format(L"%02d:%02d:%02d.%02d",
		(dwTime / 1000 / 60 / 60) % 24, // hour
		(dwTime / 1000 / 60) % 60, // min
		(dwTime / 1000) % 60, // sec
		(dwTime / 10) % 10 // milli sec
	);

	if (CUR_PLAYER->m_pProfile)
	{
		int nScore = CUR_PLAYER->m_nScore;

		int nOldBestScore = CUR_PLAYER->m_pProfile->GetSingleStageBestScore(pTopStage->m_strName);
		int nOldBestTime = CUR_PLAYER->m_pProfile->GetSingleStageBestTime(pTopStage->m_strName);
		if (nOldBestScore < nScore)
			m_bBestScore = true;
		if (nOldBestTime > dwTime || nOldBestTime == 0)
			m_bBestTime = true;

		CUR_PLAYER->m_pProfile->AddClearedSingleStage(
			pTopStage->m_strName,
			dwTime,
			nScore);

		m_strBestTime.Format(L"%02d:%02d:%02d.%02d",
			(dwTime / 1000 / 60 / 60) % 24, // hour
			(dwTime / 1000 / 60) % 60, // min
			(dwTime / 1000) % 60, // sec
			(dwTime / 10) % 10 // milli sec
		);

		m_strBestScore.Format(L"   %8d", nScore);
		
		CUR_PLAYER->SaveUserProfile();
	}
}

void GameStateSingleStageClear::Process()
{
	if (UI_MANAGER->m_pCurPanel->m_vecUI.size() == 0 && KE(VK_SPACE))
	{
		CUR_PLAYER->AddScore(CUR_PLAYER->m_nTime * 50);
		CUR_PLAYER->m_nTime = 0;

		SOUND_MANAGER->StopBGM();
	}

	switch (m_nStateFrame)
	{
	case 1:
		if (IS_ONLINESTAGE && m_pGame->m_Option.m_bTestMode == false)
		{
			m_Transaction.m_pJobThread = new std::thread(AddClearedCount, this);
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
			auto pUI = UI_MANAGER->CreateButton(UI_RETRY, 0, 0 + (i++ * 16), L"RETRY");

			if (IS_ONLINESTAGE)
			{
#if !defined(NDEBUG)
				NaString strRating = L"0";
				if (m_pCurMetaInfo)
					strRating.Format(L"%d", m_pCurMetaInfo->m_nDeveloperRating);

				pUI = UI_MANAGER->CreateEdit(
					UI_RATESTAGE, 0, 0 + ((i++) * 16), strRating, L"Developer's Rating: ");

				UIEdit* pEdit = (UIEdit*)pUI;
				pEdit->m_strEmptyText = L"";
#endif

				// Post Comment button
				auto pConfig = GameConfig::GetGameConfig(m_pGame);
				pUI = UI_MANAGER->CreateButton(UI_COMMENTSTAGE, 0, 0 + (i++ * 16), L"POST COMMENT");
				if (pConfig->m_strName.GetLength() == 0)
				{
					// Not registered user
					pUI->m_bEnable = false;
				}

				// Like button
				pUI = UI_MANAGER->CreateButton(UI_LIKESTAGE, 0, 0 + (i++ * 16), L"LIKE THIS STAGE");

				auto pTopStage = CUR_STAGE->GetTopStage();
				if (pTopStage->m_strUniqueId.GetLength() == 0)
					pUI->m_bEnable = false;
				else if (CUR_PLAYER->m_pProfile)
				{
					auto ar = CUR_PLAYER->m_pProfile->m_strLikedStages.Split(L",");
					if (ar.Find(pTopStage->m_strUniqueId) >= 0)
						pUI->m_bEnable = false;
				}
			}
			else
			{
				// Select stage button (disable)
				auto pUI = UI_MANAGER->CreateButton(UI_SELECTSTAGE, 0, 0 + (i++ * 16), L"SELECT STAGE");
			}

			UI_MANAGER->CreateButton(UI_BACKTOTITLE, 0, 0 + (i++ * 16), L"BACK TO TITLE");
		}
		break;
	}

	UI_MANAGER->Process();

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();

		switch (m_Transaction.m_nJobType)
		{
		case JOB_LIKE_END:
			if (CUR_PLAYER->m_pProfile)
			{
				auto pTopStage = CUR_STAGE->GetTopStage();
				CUR_PLAYER->m_pProfile->AddLikedStage(pTopStage->m_strUniqueId);
				CUR_PLAYER->m_pProfile->Save();

				auto pCurButton = UI_MANAGER->GetControl(UI_LIKESTAGE);
				pCurButton->m_pParent->MoveFocusNext();
				pCurButton->m_bEnable = false;

				SOUND_MANAGER->PlaySoundEffect(L"Coin");
			}
			break;
		case JOB_POST_COMMENT_END:
			{
				UI_MANAGER->PopPanel();

				SOUND_MANAGER->PlaySoundEffect(L"Coin");
			}
			break;
		}

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
		case UI_SELECTSTAGE:
			InitCustomStageSelectUI();
			break;
		case UI_COMMENTSTAGE:
			InitPostCommentUI();
			break;
		case UI_POSTCOMMENT:
			m_Transaction.m_pJobThread = new std::thread(PostComment, this);
			break;
		case UI_LIKESTAGE:
			if (UI_MANAGER->m_pClicked->m_bEnable)
			{
				m_Transaction.m_pJobThread = new std::thread(LikeStage, this);
			}
			break;
		case UI_RATESTAGE:
			m_Transaction.m_pJobThread = new std::thread(RateStage, this);
			break;
		case UI_BACKTOTITLE:
			m_pGame->ChangeState(GAMESTATE_TITLE);
			return;
			break;
		case UI_BACK:
			UI_MANAGER->PopPanel();
			break;
		}

		if (nIDUI >= UI_CUSTOMSTAGE_000 && nIDUI <= UI_CUSTOMSTAGE_099)
		{
			UIButton *pButton = (UIButton*)UI_MANAGER->m_pClicked;
			m_pGame->m_strInitialStage = pButton->m_strText.wstr();

			CUR_PLAYER->Reset();
			
			m_pGame->PopAllStage();
			CUR_STAGE->Load(m_pGame->m_strInitialStage.wstr());

			UI_MANAGER->Clear();
			m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
			return;
		}
	}

	CUR_STAGE->OnBeforeProcess();
	CUR_PLAYER->OnBeforeProcess();

	CUR_STAGE->Process();
	CUR_PLAYER->Process();
	CUR_PLAYER->ProcessPSwitchTime();

	CUR_STAGE->OnAfterProcess();
	CUR_PLAYER->OnAfterProcess();

	CUR_STAGE->ProcessEffects();
}

void GameStateSingleStageClear::InitCustomStageSelectUI()
{
	GameStateBase::InitCustomStageSelectUI(UI_CUSTOMSTAGE_000, UI_BACK);
}

void GameStateSingleStageClear::InitPostCommentUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int _y = 0;
	UI_MANAGER->CreateText(0, 0, _y, L"POST COMMENT");
	_y += 16;

	UI_MANAGER->CreateText(0, 0, _y, L"(You can leave only one comment on one stage)");
	_y += 16;

	UIEdit *pEdit = (UIEdit*)UI_MANAGER->CreateEdit(UI_POSTCOMMENT, 0, _y, L"", L"-> ");
	pEdit->m_bAllowAll = true;
	_y += 16;

	UI_MANAGER->CreateButton(UI_BACK, 0, _y, L"BACK");

	pPanel->m_pFocused = pEdit;
}

void GameStateSingleStageClear::LikeStage(void *pParam)
{
	GameStateSingleStageClear *pThis = (GameStateSingleStageClear*)pParam;

	pThis->m_Transaction.BeginJob(JOB_LIKE);

	ServerRequest req;
	try
	{
		if (pThis->m_pCurMetaInfo == nullptr)
		{
			auto pTopStage = pThis->m_pGame->m_pCurStage->GetTopStage();
			auto pMetaInfo = req.GetMetaInfo(pTopStage->m_strUniqueId);
			pThis->m_pCurMetaInfo = pMetaInfo;
		}

		pThis->m_pCurMetaInfo->m_nLike += 1;

		req.UpdateMetaInfo(pThis->m_pCurMetaInfo, UPDATE_METAINFO_LIKE);

		pThis->m_Transaction.ChangeJob(JOB_LIKE_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Error", MB_OK | MB_ICONERROR);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateSingleStageClear::AddClearedCount(void *pParam)
{
	GameStateSingleStageClear *pThis = (GameStateSingleStageClear*)pParam;

	pThis->m_Transaction.BeginJob(JOB_ADD_CLEARED);

	ServerRequest req;
	try
	{
		if (pThis->m_pCurMetaInfo == nullptr)
		{
			auto pTopStage = pThis->m_pGame->m_pCurStage->GetTopStage();
			auto pMetaInfo = req.GetMetaInfo(pTopStage->m_strUniqueId);
			pThis->m_pCurMetaInfo = pMetaInfo;
		}
		
		pThis->m_pCurMetaInfo->m_nCleared += 1;

		req.UpdateMetaInfo(pThis->m_pCurMetaInfo, UPDATE_METAINFO_CLEARED);

		pThis->m_Transaction.ChangeJob(JOB_ADD_CLEARED_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Error", MB_OK | MB_ICONERROR);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateSingleStageClear::RateStage(void * pParam)
{
	GameStateSingleStageClear *pThis = (GameStateSingleStageClear*)pParam;

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

void GameStateSingleStageClear::PostComment(void * pParam)
{
	GameStateSingleStageClear *pThis = (GameStateSingleStageClear*)pParam;

	pThis->m_Transaction.BeginJob(JOB_POST_COMMENT);

	ServerRequest req;
	try
	{
		if (pThis->m_pCurMetaInfo == nullptr)
		{
			auto pTopStage = pThis->m_pGame->m_pCurStage->GetTopStage();
			auto pMetaInfo = req.GetMetaInfo(pTopStage->m_strUniqueId);
			pThis->m_pCurMetaInfo = pMetaInfo;
		}

		auto pConfig = GameConfig::GetGameConfig(pThis->m_pGame);
		ServerUserInfo *pUserInfo = nullptr;

		if (pConfig->m_strName.GetLength() == 0)
			throw NaException(__FUNCTIONW__, __LINE__, L"You cannot register because never run MapEditor.");

		if (pConfig->m_strUniqueId.GetLength() == 0)
		{
			auto pUserInfo = req.RegisterUser(pConfig->m_strName);
			auto strUniqueId = pUserInfo->m_strUniqueId;
			if (strUniqueId.GetLength() == 0)
			{
				SAFE_DELETE(pUserInfo);
				throw NaException(__FUNCTIONW__, __LINE__, L"User registering failed");
			}

			pConfig->m_strUniqueId = strUniqueId;
			pConfig->Save();
		}
		else
		{
			pUserInfo = req.GetUserInfo(pConfig->m_strUniqueId);
		}

		StageCommentInfo *pComment = new StageCommentInfo;
		pComment->BuildPropertyMap();

		UIEdit *pEdit = (UIEdit*)pThis->m_pGame->m_pUIManager->GetControl(UI_POSTCOMMENT);
		if (pEdit)
		{
			pComment->m_strAvatar = pUserInfo ? pUserInfo->m_strAvatar : L"Bug";
			pComment->m_strName = pConfig->m_strName;
			pComment->m_strComment = pEdit->m_strText;
			pComment->m_strUniqueId = pConfig->m_strUniqueId;
		}

		if (pComment->m_strName.GetLength() > 0 &&
			pComment->m_strComment.GetLength() > 0 &&
			pComment->m_strUniqueId.GetLength() > 0)
		{
			req.UpdateComment(
				pThis->m_pCurMetaInfo->m_strUniqueId,
				pConfig->m_strUniqueId,
				pComment
			);
		}
		else
		{
			delete pUserInfo;
			delete pComment;

			throw NaException(__FUNCTIONW__, __LINE__, L"User registering failed");
		}

		delete pUserInfo;
		delete pComment;
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Error", MB_OK | MB_ICONERROR);
	}

	pThis->m_Transaction.ChangeJob(JOB_POST_COMMENT_END);
	pThis->m_Transaction.EndJob();
}

void GameStateSingleStageClear::Render()
{
	const int nWidth = m_pGame->Width();

	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();

	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();

	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth - 25, m_pGame->m_nHeight - 25, 0.5f);
	}

	NaString str;
	if (m_nStateFrame >= 0)
	{
		str.Format(L"STAGE CLEAR!");

		RENDER_MANAGER->Text(
			(nWidth / 2) - (str.GetLength() * (4 * 1.5f)), 64,
			str, 0xffffffff, 1.5f, Z_STATUS, true);
	}

	if (m_nStateFrame >= 64)
	{
		str.Format(L"CLEAR TIME  %ls", m_strTime.wstr());

		RENDER_MANAGER->Text((nWidth / 2) - 96, 92, str, 0xffffffff, 1.0f, Z_STATUS, true);
	}

	if (m_nStateFrame >= 64 + 24)
	{
		str.Format(L"BEST TIME   %ls", m_strBestTime.wstr());
		
		RENDER_MANAGER->Text((nWidth / 2) - 96, 108, str, 0xffffffff, 1.0f, Z_STATUS, true);

		if (m_bBestTime)
		{
			RENDER_MANAGER->Text((nWidth / 2) + 92, 108, L"NEW!", 0xffffff00, 1.0f, Z_STATUS, true);
		}
	}

	if (m_nStateFrame >= 64 + 48)
	{
		str.Format(L"BEST SCORE  %ls", m_strBestScore.wstr());
		
		RENDER_MANAGER->Text((nWidth / 2) - 96, 124, str, 0xffffffff, 1.0f, Z_STATUS, true);

		if (m_bBestScore)
		{
			RENDER_MANAGER->Text((nWidth / 2) + 92, 124, L"NEW!", 0xffffff00, 1.0f, Z_STATUS, true);
		}
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

