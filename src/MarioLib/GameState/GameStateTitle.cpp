#include "GameStateTitle.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "UIManager.h"
#include "UIBase.h"
#include "UIEdit.h"
#include "UIButton.h"
#include "UIPanel.h"
#include "NaPlatformAPI.h"
#include "StagePack.h"
#include "UIList.h"
#include "UIImageButton.h"
#include "PathManager.h"
#include "UISingleStageButton.h"
#include "UIWorldStageButton.h"

#include "ServerRequest.h"
#include "NaException.h"
#include "JsonManager.h"
#include "GameConfig.h"
#include "UIRectangle.h"
#include "Macros.h"

#include <functional>

GameStateTitle::GameStateTitle(Game *pGame) :
#if defined(_DEBUG)
	m_nFirstProfileIdx(0),
#else
	m_nFirstProfileIdx(1),
#endif
	m_nProfileCnt(4),
	GameStateBase(pGame)

{
	m_nType = GAMESTATE_TITLE;

	m_pSpriteEtc = SPR_MANAGER->Find(L"Etc");
	m_pCurProfile = nullptr;

	m_Transaction.Init(pGame);
	m_pCurMetaInfo = nullptr;

	m_nCurSortMethod = 0;
	m_nCurDiffFilter = 0;
	m_nCurThemeFilter = 0;
}

GameStateTitle::~GameStateTitle()
{
	for (int i = m_nFirstProfileIdx; i < m_nProfileCnt; i++)
	{
		delete m_pProfile[i];
		m_pProfile[i] = nullptr;
	}
}

long GameStateTitle::GetBackgroundColor()
{
	//return 0xff086a9a;
	return GameStateBase::GetBackgroundColor();
}

void GameStateTitle::Init()
{
	DeleteOldState();

	auto strLastStagePack = m_pGame->m_strStagePack;

    CUR_PLAYER_OBJ->SetState(STATE_DEAD); //Mykner> Prevent title mario from getting items
	CUR_STAGE->CreatePresetStage(L"Title");

	for (int i = m_nFirstProfileIdx; i < m_nProfileCnt; i++)
	{
		m_pProfile[i] = new UserProfile(m_pGame);
		m_pProfile[i]->m_strFileName.Format(L"Profile%d", i);
		m_pProfile[i]->BuildPropertyMap();
	}

	LoadProfiles();

	// Find Current Profile
	if (CUR_PLAYER->m_pProfile)
	{
		for (int i = m_nFirstProfileIdx; i < m_nProfileCnt; i++)
		{
			if (CUR_PLAYER->m_pProfile->m_strFileName == m_pProfile[i]->m_strFileName)
			{
				m_pCurProfile = m_pProfile[i];
				break;
			}
		}
	}

	CreateParameter param;
	param.nDuration = 60;
	m_pGame->CreateSceneEffect(
		m_pGame->m_nWidth / 2,
		m_pGame->m_nHeight / 2,
		EFFECT_SCENE_CIRCLEIN, &param
	);

	InitGameSelectUI();

	if (m_pCurProfile == nullptr)
	{
		InitProfileSelectUI();
	}

	CAMERA->SetPosition(0, 0);
	CUR_PLAYER->LockInput();

	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlayBGM(L"Title");

	auto pConfig = GameConfig::GetGameConfig(m_pGame);
	m_nCurSortMethod = pConfig->m_nStageSortMethod;
	m_nCurDiffFilter = pConfig->m_nStageDifficultyFilter;
	m_nCurThemeFilter = pConfig->m_nStageThemeFilter;

	StageMetaInfo::s_nStageMataInfoSortOption = m_nCurSortMethod;
	StageMetaInfo::s_nStageMataInfoDifficultyFilter = m_nCurDiffFilter;
	StageMetaInfo::s_nStageMataInfoThemeFilter = m_nCurThemeFilter;

	// Init from stage world,
	if (strLastStagePack.Compare(L"Download") == 0)
	{
		InitWorldStageSelectUI();
	}
}

void GameStateTitle::Process()
{
	// for catch memory leak
	//if (m_nStateFrame == 100)
	//	m_pGame->ChangeState(GAMESTATE_CREDITS);
#if !defined(NDEBUG)
	if (KE(VK_F1))
	{
		CUR_STAGE->CreatePresetStage(L"Title");
	}
#endif

 	// Mykner> Process stage on title screen
 	CUR_STAGE->OnBeforeProcess(); 
 	CUR_STAGE->Process();
 	CUR_STAGE->OnAfterProcess();
	CUR_STAGE->ProcessEffects();

	UI_MANAGER->Process();

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();

		switch (m_Transaction.m_nJobType)
		{
		case JOB_LOAD_WORLDSTAGE_LIST:
			break;
		case JOB_LOAD_WORLDSTAGE_LIST_END:
			OnLoadWorldStageList(
				m_Transaction.m_Response.vecMetaInfo, 
				(UIList*)m_Transaction.m_Response.pUI
			);
			break;
		case JOB_LOAD_WORLDSTAGE_LIST_FAIL:
			OnLoadWorldStageListFail(
				m_Transaction.m_Response.strReason,
				(UIList*)m_Transaction.m_Response.pUI
			);
			break;
		case JOB_DOWNLOAD_WORLDSTAGE:
			break;
		case JOB_DOWNLOAD_WORLDSTAGE_END:
			OnDownloadWorldStage();
			break;
		case JOB_LOAD_WORLDSTAGE_DETAIL_END:
			OnLoadWorldStageDetail(
				m_Transaction.m_Response.vecCommentInfo,
				(UIList*)m_Transaction.m_Response.pUI
			);
			break;
		case JOB_LOAD_WORLDSTAGE_RECORD_END:
			OnLoadWorldStageRecord();
			break;
		case JOB_LOAD_WORLDSTAGE_RECORD_FAIL:
			OnPlayWorldStage();
			break;
		}

		// #TODO Handling download fail

		m_Transaction.ChangeJob(JOB_NONE);
	}

	if (!m_Transaction.IsWorking() && UI_MANAGER->m_pClicked)
	{
		int nIDUI = UI_MANAGER->m_pClicked->m_nID;
		switch (nIDUI)
		{
		case UI_PROFILE0:
		case UI_PROFILE1:
		case UI_PROFILE2:
		case UI_PROFILE3:
			OnSelectProfile(nIDUI - UI_PROFILE0);
			break;
		case UI_PROFILE:
			InitProfileSelectUI();
			break;
		case UI_CUSTOMGAME:
			OnSelectCustomGame();
			break;
		case UI_CUSTOMSTAGE:
			OnSelectCustomStage();
			break;
		case UI_WORLDSTAGE:
			OnSelectWorldStage();
			break;
		case UI_CREDITS:
			m_pGame->ChangeState(GAMESTATE_CREDITS);
			break;
		case UI_CONTINUE:
			OnSelectContinue();
			break;
		case UI_STAGESELECT:
			OnSelectStageSelect();			
			break;
		case UI_NEWGAME:
			OnSelectNewGame();
			break;
		case UI_USERNAME:
			OnCompleteUserName();
			break;
		case UI_WORLDSTAGE_DEVELOPERRATING:
			{
				m_Transaction.m_pJobThread = new std::thread(RateWorldStage, this);
				/*
				m_Transaction.m_pJobThread = 
					new std::thread(
						//RateWorldStage, 
						RateStage<GameStateTitle>,
						this);
						*/
			}
			break;
		case UI_WORLDSTAGE_PLAY:
			OnSelectPlayCurrentWorldStage();
			break;
		case UI_WORLDSTAGE_LIKE:
			{
				SOUND_MANAGER->PlaySoundEffect(L"1UP");
			}
			break;
		case UI_WORLDSTAGE_CREATORCOMMENT:
			{
				BEGIN_VECTOR_LOOP(UIBase*, UI_MANAGER->m_pCurPanel->m_vecUI) {
					if (pObj->m_nID == UI_WORLDSTAGE_CREATORCOMMENT ||
						pObj->m_nID == UI_WORLDSTAGE_CREATORCOMMENT_CONTENT)
					{
						pObj->m_bEnable = false;
						pObj->m_bVisible = false;
					}
					if (pObj->m_nID == UI_WORLDSTAGE_PLAYERCOMMENT ||
						pObj->m_nID == UI_WORLDSTAGE_PLAYERCOMMENT_CONTENT)
					{
						pObj->m_bEnable = true;
						pObj->m_bVisible = true;
					}
					if (pObj->m_nID == UI_WORLDSTAGE_PLAYERCOMMENT)
					{
						((UIPanel*)pObj->m_pParent)->m_pFocused = pObj;
					}
				} END_VECTOR_LOOP();
			}
			break;
		case UI_WORLDSTAGE_PLAYERCOMMENT:
			{
				BEGIN_VECTOR_LOOP(UIBase*, UI_MANAGER->m_pCurPanel->m_vecUI) {
					if (pObj->m_nID == UI_WORLDSTAGE_CREATORCOMMENT ||
						pObj->m_nID == UI_WORLDSTAGE_CREATORCOMMENT_CONTENT)
					{
						pObj->m_bEnable = true;
						pObj->m_bVisible = true;
					}
					if (pObj->m_nID == UI_WORLDSTAGE_PLAYERCOMMENT ||
						pObj->m_nID == UI_WORLDSTAGE_PLAYERCOMMENT_CONTENT)
					{
						pObj->m_bEnable = false;
						pObj->m_bVisible = false;
					}
					if (pObj->m_nID == UI_WORLDSTAGE_CREATORCOMMENT)
					{
						((UIPanel*)pObj->m_pParent)->m_pFocused = pObj;
					}
				} END_VECTOR_LOOP();
			}
			break;
		case UI_WORLDSTAGE_FILTER_DIFFICULTY:
			{
				auto pList = UI_MANAGER->GetControl(UI_WORLDSTAGE_LIST, UI_MANAGER->m_pCurPanel);
				if (pList)
					OnChangeWorldStageDifficultyFilter(pList);
			}
			break;
		case UI_WORLDSTAGE_FILTER_GAMETHEME:
			{
				auto pList = UI_MANAGER->GetControl(UI_WORLDSTAGE_LIST, UI_MANAGER->m_pCurPanel);
				if (pList)
					OnChangeWorldStageThemeFilter(pList);
			}
			break;
		case UI_WORLDSTAGE_SORT_0:
		case UI_WORLDSTAGE_SORT_1:
		case UI_WORLDSTAGE_SORT_2:
		case UI_WORLDSTAGE_SORT_3:
		case UI_WORLDSTAGE_SORT_4:
		case UI_WORLDSTAGE_SORT_5:
			{
				auto pList = UI_MANAGER->GetControl(UI_WORLDSTAGE_LIST, UI_MANAGER->m_pCurPanel);				
				if (pList)
					OnChangeWorldStageSortMethod(pList, nIDUI - UI_WORLDSTAGE_SORT_0 + 1);

				UIRectangle* pInd = (UIRectangle*)UI_MANAGER->GetControl(UI_WORLDSTAGE_SORT_INDICATOR, UI_MANAGER->m_pCurPanel);
				if (pInd)
				{
					pInd->m_nX = UI_MANAGER->m_pClicked->m_nX + 4;
					pInd->m_nY = UI_MANAGER->m_pClicked->m_nY + 4;
					if (StageMetaInfo::s_nStageMataInfoSortOption < 0)
						pInd->m_strText = WSZ_DOWNARROW;
					else
						pInd->m_strText = WSZ_UPARROW;
				}
			}
			break;
		case UI_CANCEL:
		case UI_BACK:
			OnCancel();
			break;
		case UI_EXIT:
#ifdef WIN32
			::PostQuitMessage(0);
#else
            m_pGame->m_bExit = true;
#endif
			break;
		}

		if (nIDUI >= UI_STAGE_000 && nIDUI <= UI_STAGE_099)
		{
			OnSelectStage();
		}
		else if (nIDUI >= UI_CUSTOMGAME_000 && nIDUI <= UI_CUSTOMGAME_099)
		{
			OnSelectSomeCustomGame(nIDUI - UI_CUSTOMGAME_000);
		}
		else if (nIDUI >= UI_CUSTOMSTAGE_000 && nIDUI <= UI_CUSTOMSTAGE_999)
		{
			UISingleStageButton *pButton = (UISingleStageButton*)UI_MANAGER->m_pClicked;
			OnSelectSomeCustomStage(pButton->m_strPathName);
		}
		else if (nIDUI >= UI_WORLDSTAGE_000 && nIDUI <= UI_WORLDSTAGE_999)
		{
			UIWorldStageButton *pButton = (UIWorldStageButton*)UI_MANAGER->m_pClicked;
			
			OnSelectSomeWorldStage(pButton->m_pMetaInfo);
		}
	}
}

void GameStateTitle::Render()
{
 	CUR_STAGE->RenderBackground();
 	CUR_STAGE->Render();

	/*
	// For fade background
	int nFadeAlpha = 0xe0;
	if (m_nStateFrame < 30)
		nFadeAlpha *= (m_nStateFrame / 30.0f);

	m_pSpriteEtc->RenderToQueue(0, 0, SPRIDX_ETC_DOT,
		false, false, 
		(nFadeAlpha * 0x1000000) + 0x10,
		m_pGame->m_nWidth, m_pGame->m_nHeight, Z_EFFECT);
		*/

	// SUPER fanmade
	// MARIO BROS.
	int nTitleBoxX = (m_pGame->Width()) / 2 - 24;
	int nTitleBoxY = 56;
	m_pSpriteEtc->RenderToQueue(
		nTitleBoxX - 73, nTitleBoxY, SPRIDX_ETC_TITLE_SUPER,
		false, false, 0xffffa000, 1.0f, 1.0f, Z_UI_TITLEBOX, true
	);

	m_pSpriteEtc->RenderToQueue(
		nTitleBoxX - 73, nTitleBoxY + 38, SPRIDX_ETC_TITLE_MARIO,
		false, false, 0xfff73910, 1.0f, 1.0f, Z_UI_TITLEBOX, true
	);

	m_pSpriteEtc->RenderToQueue(
		nTitleBoxX + 75, nTitleBoxY + 38, SPRIDX_ETC_TITLE_BROS,
		false, false, 0xff01db01, 1.0f, 1.0f, Z_UI_TITLEBOX, true
	);

	m_pSpriteEtc->RenderToQueue(
		nTitleBoxX + 3, nTitleBoxY, SPRIDX_ETC_TITLE_FANMADE,
		false, false, -1L, 1.0f, 1.0f, Z_UI_TITLEBOX, true
	);

	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth / 2, m_pGame->m_nHeight / 2);
	}

	NaString strLic;
	strLic.Format(L"Made by neoarc. Original Game by %c1985 NINTENDO.", 0x09);
	RENDER_MANAGER->Text(16, m_pGame->Height() - 20, strLic, 0xffffffff, 1.0f, Z_UI_TITLEBOX, true);

	//RENDER_MANAGER->Text(nTitleBoxX + 64, 120, L"\x091985 NINTENDO", 0xFFFFCBBD, 1.0f, Z_UI_TITLEBOX);

	// Original UI
	/*
	DRAWTEXT(72, 144, L"\x08");
	DRAWTEXT(88, 144, L"1 PLAYER GAME");
	DRAWTEXTC(88, 160, L"2 PLAYER GAME", 0xFFc0c0c0);
	DRAWTEXT(96, 184, L"TOP- 000000");
	*/

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

			if (rc.Height() > 24)
			{
				Sprite *pSprite = SPR_MANAGER->Find(L"Etc");

				int nIndicatorColor = 0xfffc5755;
				pSprite->RenderToQueue(rc.left, rc.top, SPRIDX_ETC_SELECTINDICATOR_LT, false, false, nIndicatorColor, 1.0f, 1.0f, Z_UI_INDICATOR);
				pSprite->RenderToQueue(rc.right, rc.top, SPRIDX_ETC_SELECTINDICATOR_RT, false, false, nIndicatorColor, 1.0f, 1.0f, Z_UI_INDICATOR);
				pSprite->RenderToQueue(rc.left, rc.bottom, SPRIDX_ETC_SELECTINDICATOR_LB, false, false, nIndicatorColor, 1.0f, 1.0f, Z_UI_INDICATOR);
				pSprite->RenderToQueue(rc.right, rc.bottom, SPRIDX_ETC_SELECTINDICATOR_RB, false, false, nIndicatorColor, 1.0f, 1.0f, Z_UI_INDICATOR);
			}
			else
			{
				DRAWTEXT(
					rc.left - 12,
					rc.top + rc.Height() / 2 - 4,
					L"\x08");
			}		
		}
	}
}

void GameStateTitle::LoadProfiles()
{
	for (int i = m_nFirstProfileIdx; i < m_nProfileCnt; i++)
	{
		m_pProfile[i]->Load();
	}
}

//
// CONTINUE LAST
// -> Default title menu when no profile. (Development)
void GameStateTitle::InitJustNewGameUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 144);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;
	
	UI_MANAGER->CreateButton(UI_NEWGAME, 0, 0, L"CONTINUE LAST");
}

//
// SELECT PROFILE
// <PROFILE1>
// <PROFILE2>
// <PROFILE3>
// EXIT
// -> Default title menu on Release build.
void GameStateTitle::InitProfileSelectUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int _y = 0;
	UI_MANAGER->CreateText(UI_UNDEFINED, 0, _y, L"Select Profile");
	_y += 16;

	UIList *pList = (UIList*)UI_MANAGER->CreateList(0, 0, _y, 172, 16 * 4 + 8);
	_y += 8;

	for (int i = m_nFirstProfileIdx; i < m_nProfileCnt; i++)
	{
		if (m_pProfile[i]->m_strName.GetLength() == 0)
		{
			UI_MANAGER->CreateButton(UI_PROFILE0 + i, 8, 8 + (i - m_nFirstProfileIdx) * 16, L"<Create new Profile>", pList);
		}
		else
		{
			UI_MANAGER->CreateButton(UI_PROFILE0 + i, 8, 8 + (i - m_nFirstProfileIdx) * 16, m_pProfile[i]->m_strName, pList);
		}
		
	}

	_y += 64;
	_y += 8;

	UI_MANAGER->CreateButton(UI_BACK, 0, _y, L"Back");
}

//
// TYPE YOURNAME
// _
// CANCEL
// -> Creating profile menu
void GameStateTitle::InitProfileCreateUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 144);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	UI_MANAGER->CreateText(UI_TYPENAME, 0, 0, L"TYPE YOURNAME");
	UI_MANAGER->CreateEdit(UI_USERNAME, 0, 16, L"");
	UI_MANAGER->CreateButton(UI_CANCEL, 0, 32, L"CANCEL");
}

//
// CLASSIC GAME
// CUSTOM GAME
// CHALLENGE MODE
//-> When profile selected, user must select game.
void GameStateTitle::InitGameSelectUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = false;
	pPanel->m_bAutoAlignY = false;
	pPanel->m_nPanelType = UIPanel::PANEL_NORMAL;

	UIBase *pUI = nullptr;
	Sprite *pSprite = SPR_MANAGER->Find(L"Etc");

	NaString str;
	if (m_pCurProfile)
	{
		str.Format(L"%ls", m_pCurProfile->m_strName.wstr());
	}
	else
	{
		str = L"<No Profile>";
	}

	pUI = UI_MANAGER->CreateButton(UI_PROFILE, 8, 6, str);
	UIButton* pButton = (UIButton*)pUI;
	pButton->m_bRenderBox = true;
	pButton->m_bShadow = true;

	int _y = 128;
	pUI = UI_MANAGER->CreateImageButton(UI_CUSTOMGAME, 40, _y, 100, 64, L"Stage Pack", pSprite, SPRIDX_ETC_TITLE_STAGEPACK);
	((UIImageButton*)pUI)->m_bText = false;
	((UIImageButton*)pUI)->m_bSpeechBubble = true;
	UI_MANAGER->m_pCurPanel->m_pFocused = pUI;

	pUI = UI_MANAGER->CreateImageButton(UI_CUSTOMSTAGE, 150, _y, 100, 64, L"Single Stage", pSprite, SPRIDX_ETC_TITLE_SINGLESTAGE);
	((UIImageButton*)pUI)->m_bText = false;
	((UIImageButton*)pUI)->m_bSpeechBubble = true;

	pUI = UI_MANAGER->CreateImageButton(UI_WORLDSTAGE, 260, _y, 100, 64, L"Stage World", pSprite, SPRIDX_ETC_TITLE_WORLDSTAGE);
	((UIImageButton*)pUI)->m_bText = false;
	((UIImageButton*)pUI)->m_bSpeechBubble = true;
	pUI->m_pParent->m_pFocused = pUI;

	pUI = UI_MANAGER->CreateButton(UI_CREDITS, m_pGame->m_nWidth - 32, 6, L"?");
	pButton = (UIButton*)pUI;
	pButton->m_bRenderBox = true;
	pButton->m_bShadow = true;

	// #TODO Help & Thanks to, etc.
	/*
	pUI = UI_MANAGER->CreateButton(UI_HELP, m_pGame->m_nWidth - 56, 6, L"HELP?");
	pButton = (UIButton*)pUI;
	pButton->m_bRenderBox = true;
	pButton->m_bShadow = true;
	*/
}

//
// NEW GAME
// or
// CONTINUE (X-X)
// STAGE SELECT
// BACK
// -> Classic game menu.
void GameStateTitle::InitClassicGameUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 144);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int i = 0;
	UI_MANAGER->CreateButton(UI_NEWGAME, 0, 0 + (i++ * 16), L"NEW GAME");

	if (m_pCurProfile && m_pCurProfile->GetLastStage().GetLength() > 0)
	{
		NaString str;
		str.Format(L"CONTINUE (%ls)", m_pCurProfile->GetLastStage().wstr());
		UI_MANAGER->CreateButton(UI_CONTINUE, 0, 0 + (i++ * 16), str);
		UI_MANAGER->CreateButton(UI_STAGESELECT, 0, 0 + (i++ * 16), L"STAGE SELECT");
	}
	
	UI_MANAGER->CreateButton(UI_BACK, 0, 0 + (i++ * 16), L"BACK");
}

//
// 1-1 1-2 1-3 1-4
// ...
// BACK
// -> Stage selection menu. Player can select already cleared stage.
void GameStateTitle::InitStageSelectUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int nUIID = UI_STAGE_000;

	if (m_pCurProfile && m_pCurProfile->GetLastStage().GetLength() > 0)
	{
		for (int nWorld=1; nWorld<=8; nWorld++)
		{
			for (int nStage=1; nStage<=4; nStage++)
			{
				NaString str;
				str.Format(L"%d-%d", nWorld, nStage);
				UIBase *pUI = UI_MANAGER->CreateButton(
					nUIID, 
					(nStage - 1) * 44,
					(nWorld - 1) * 16,
					str);

				NaStrArray ar = m_pCurProfile->GetSelectableStages().Split(L",");
				if (ar.Find(str) < 0)
					pUI->m_bEnable = false;
				nUIID++;
			}
		}
	}
	
	UI_MANAGER->CreateButton(UI_BACK, 0, 8 * 16, L"BACK");
}

//
// SELECT CUSTOM GAME
// Game1
// Game2
// ...
// Back
// -> Custom game(=StageSet) selection menu.
void GameStateTitle::InitCustomGameSelectUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int _y = 0;
	UI_MANAGER->CreateText(0, 0, 0, L"Select Stage Pack");
	_y += 16;

	// Load Custom Game List
	{
		UIList *pList = (UIList*)UI_MANAGER->CreateList(0, 0, _y, m_pGame->m_nWidth - 64, 24 * 7);
		_y += (24 * 7);
		_y += 8;

		NaString strMapPath = PathManager::GetMapPath(m_pGame);
		NaStrArray ar = NaPlatform::GetSubDirectoryList(strMapPath);

		NaDebugOut(L"Load StagePack List, %d Found.\n", ar.GetCount());
		int nLen = ar.GetCount();
		m_vecStagePack.clear();
		for (int i=0, j=0; i<nLen; i++)
		{
			NaString strStagePack;
			strStagePack.Format(L"%ls", ar[i].wstr());

			StagePack sp(m_pGame);
			sp.BuildPropertyMap();
			sp.Load(strStagePack);
			if (sp.IsValid())
			{
				m_vecStagePack.push_back(sp);

				NaString strName = sp.m_strName;
				if (strName.GetLength() == 0)
					strName = strStagePack;

				UI_MANAGER->CreateCustomGameButton(
					UI_CUSTOMGAME_000 + j,
					0,
					j * 48,
					&sp, CUR_PLAYER, pList);

				j++;
			}
		}

		if (pList->m_vecUI.size() > 0)
		{
			UI_MANAGER->m_pCurPanel->m_pFocused = pList;
			pList->m_pFocused = pList->m_vecUI[0];
			pList->EnsureVisible(pList->m_pFocused);
		}
	}

	UI_MANAGER->CreateButton(UI_BACK, 0, _y, L"Back");
}

//
// NEW GAME
// or
// CONTINUE
// STAGE SELECT
// -> Custom game(=StageSet) menu.
void GameStateTitle::InitCustomGameUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 144);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int i = 0;

	UI_MANAGER->CreateButton(UI_NEWGAME, 0, 0 + (i++ * 16), L"NEW GAME");
	if (m_pCurProfile && m_pCurProfile->GetLastStage().GetLength() > 0)
	{
		NaString str;
		str.Format(L"CONTINUE (%ls)", m_pCurProfile->GetLastStage().wstr());
		UI_MANAGER->CreateButton(UI_CONTINUE, 0, 0 + (i++ * 16), str);

		UIBase *pUI = UI_MANAGER->CreateButton(UI_STAGESELECT, 0, 0 + (i++ * 16), L"STAGE SELECT");
		pUI->m_bEnable = false;
	}

	UI_MANAGER->CreateButton(UI_BACK, 0, 0 + (i++ * 16), L"BACK");
}

//
// SELECT CUSTOM STAGE
// Stage1
// Stage2
// ...
// Back
// -> Custom stage(=SingleStage) selection menu.
void GameStateTitle::InitCustomStageSelectUI()
{
	GameStateBase::InitCustomStageSelectUI(UI_CUSTOMSTAGE_000, UI_BACK);
}

void GameStateTitle::InitWorldStageSelectUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int _y = 0;
	UI_MANAGER->CreateText(0, 0, _y, L"Stage World");
	
	_y += 16;

	// Load World Stage List
	UIList *pList = (UIList*)UI_MANAGER->CreateList(UI_WORLDSTAGE_LIST, 0, _y, m_pGame->m_nWidth - 64, 24 * 7);
	pList->m_nItemHeight = 24;

	_y += (24 * 7);
	_y += 8;

	m_Transaction.m_pJobThread = new std::thread(LoadWorldStageList, this, pList);

	UI_MANAGER->CreateButton(UI_BACK, 0, _y, L"Back");

	int _x = 240;
	_y = -4;
	int _width = 24;
	UIBase *pUI;
	wchar_t* szText[] = {
		L"Newly added",
		L"Likes",
		L"Clear rate",
		L"Downloads",
		L"Developer's rating",
	};

	int nCurrentX = -1;
	_x = 192;

	// Difficulty
	pUI = UI_MANAGER->CreateImageButton(
		UI_WORLDSTAGE_FILTER_DIFFICULTY,
		_x, _y, 16, 16, L"", m_pSpriteEtc,
		SPRIDX_ETC_SORT_DIFF_ALL + m_nCurDiffFilter);
	_x += _width;

	((UIImageButton*)pUI)->m_bText = false;
	((UIImageButton*)pUI)->m_bRenderBox = false;
	((UIImageButton*)pUI)->m_bSpeechBubble = false;

	// GameTheme
	pUI = UI_MANAGER->CreateImageButton(
		UI_WORLDSTAGE_FILTER_GAMETHEME,
		_x, _y, 16, 16, L"", m_pSpriteEtc,
		SPRIDX_ETC_SORT_THEME_ALL + m_nCurThemeFilter);
	_x += _width;

	((UIImageButton*)pUI)->m_bText = false;
	((UIImageButton*)pUI)->m_bRenderBox = false;
	((UIImageButton*)pUI)->m_bSpeechBubble = false;

	for (int i = 0; i < 5; i++)
	{
		pUI = UI_MANAGER->CreateImageButton(
			UI_WORLDSTAGE_SORT_0 + i,
			_x, _y, 16, 16, szText[i], m_pSpriteEtc, 
			SPRIDX_ETC_SORT0 + i);
		if (i + 1 == abs(StageMetaInfo::s_nStageMataInfoSortOption))
			nCurrentX = _x;
		_x += _width;

		((UIImageButton*)pUI)->m_bText = false;
		((UIImageButton*)pUI)->m_bRenderBox = false;
		((UIImageButton*)pUI)->m_bSpeechBubble = false;
	}

	((UIImageButton*)pUI)->m_bText = false;
	((UIImageButton*)pUI)->m_bRenderBox = false;
	((UIImageButton*)pUI)->m_bSpeechBubble = false;

	// Up/Down arrow
	pUI = UI_MANAGER->CreateRectangle(
		UI_WORLDSTAGE_SORT_INDICATOR,
		nCurrentX + 4, 
		_y + 4,
		16, 16, 
		(StageMetaInfo::s_nStageMataInfoSortOption < 0 ? WSZ_DOWNARROW : WSZ_UPARROW),
		0xffffffff,
		nullptr, -1);
	((UIRectangle*)pUI)->m_dwTextColor = 0xffff0000;
}

void GameStateTitle::InitWorldStageDetailUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(32, 24);
	pPanel->m_bAutoAlignX = false;
	pPanel->m_bAutoAlignY = false;
	pPanel->m_bAutoSize = false;
	pPanel->m_rc.left = 32;
	pPanel->m_rc.top = 12;
	pPanel->m_rc.right = m_pGame->m_nWidth - 32;
	pPanel->m_rc.bottom = m_pGame->m_nHeight - 12;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	UIBase *pUI;
	NaString str;

	if (m_pCurMetaInfo->m_nLocalRevision == -1)
		str = L"Download + Play Stage";
	else if (m_pCurMetaInfo->m_nLocalRevision < m_pCurMetaInfo->m_nRevision)
		str = L"Update + Play Stage";
	else
		str = L"Play Stage";
	pUI = UI_MANAGER->CreateButton(UI_WORLDSTAGE_PLAY, 0, pPanel->m_rc.Height() - 24, str);
	pPanel->m_pFocused = pUI;

	UI_MANAGER->CreateButton(UI_BACK, 0, pPanel->m_rc.Height() - 8, L"Back");

	// Buttons
#if !defined(NDEBUG)
	NaString strRating = L"0";
	if (m_pCurMetaInfo)
		strRating.Format(L"%d", m_pCurMetaInfo->m_nDeveloperRating);

	NaString strRatingTitle;
	strRatingTitle.Format(L"%ls: ", WSZ_NEUTRAL);
	UI_MANAGER->CreateEdit(
		UI_WORLDSTAGE_DEVELOPERRATING, 52, pPanel->m_rc.Height() - 8, strRating, strRatingTitle);
#endif

	// Title
	if (m_pCurMetaInfo->m_strTitle.GetLength() > 25)
		str.Format(L"%ls...", m_pCurMetaInfo->m_strTitle.Left(25).wstr());
	else
		str.Format(L"%ls", m_pCurMetaInfo->m_strTitle.wstr());
	UI_MANAGER->CreateText(0, 0, 0, str);

	// Creator
	str.Format(L"by %ls", m_pCurMetaInfo->m_strCreator.wstr());
	pUI = UI_MANAGER->CreateText(0, 8, 12, str);
	((UIText*)pUI)->m_dwTextColor = 0xffa0a0a0;

	// Avatar
	pUI = UI_MANAGER->CreateRectangle(
		UI_WORLDSTAGE_CREATORAVATAR, 20 + str.GetLength() * 8, 16, 
		16, 16, 
		L"", 
		0x00000000,
		m_pSpriteEtc, SPRIDX_ETC_AVATAR0);
	pUI->m_bEnable = false;

	// Revision
	int nUpperRightY = 0;
	str.Format(L"Version %d", m_pCurMetaInfo->m_nRevision);
	pUI = UI_MANAGER->CreateText(0, m_pGame->m_nWidth - 64 - (str.GetLength() * 8), 0, str);
	((UIText*)pUI)->m_dwTextColor = 0xffa0a0a0;
	nUpperRightY += 12;

	NaString strUpdateAndLike = L"";
	if (m_pCurMetaInfo->m_nLocalRevision != -1 &&
		m_pCurMetaInfo->m_nLocalRevision < m_pCurMetaInfo->m_nRevision)
	{
		strUpdateAndLike += L" Updated!";
	}

	if (m_pCurProfile)
	{
		auto ar = m_pCurProfile->m_strLikedStages.Split(L",");
		if (ar.Find(m_pCurMetaInfo->m_strUniqueId) >= 0)
		{
			strUpdateAndLike += L" ";
			strUpdateAndLike += WSZ_LIKE;
		}
	}

	if (strUpdateAndLike.GetLength() > 0)
	{
		pUI = UI_MANAGER->CreateText(0, m_pGame->m_nWidth - 64 - (strUpdateAndLike.GetLength() * 8), nUpperRightY, strUpdateAndLike);
		nUpperRightY += 12;
	}


	// Creator's comment
	if (m_pCurMetaInfo->m_strCreatorComment.GetLength() == 0)
	{
		pUI = UI_MANAGER->CreateText(UI_WORLDSTAGE_CREATORCOMMENT_CONTENT, 16, 28, L"...");
		((UIText*)pUI)->m_dwTextColor = 0xffa0a0a0;
	}
	else
	{
		pUI = UI_MANAGER->CreateRectangle(
			0, 
			16 - 4, 28,
			(41 * 8), 40, 
			L"",
			0xc0000000, m_pSpriteEtc, -1
		);
		((UIRectangle*)pUI)->m_bRenderBox = true;

		NaString strSplit = L"";
		NaString strOriginal = m_pCurMetaInfo->m_strCreatorComment;
		int nMaxTextLength = 40;
		int nLine = 0;
		for ( ; ; )
		{
			auto strLeft = strOriginal.Left(nMaxTextLength);
			auto nNewLineIdx = strLeft.Find(L"\n");
			if (nNewLineIdx >= 0)
			{
				strSplit += strOriginal.Left(nNewLineIdx + 1);
				//strSplit += L"\n";
				strOriginal = strOriginal.Mid(nNewLineIdx + 1);
			}
			else
			{
				strSplit += strOriginal.Left(nMaxTextLength);
				strSplit += L"\n";
				strOriginal = strOriginal.Mid(nMaxTextLength);
			}

			nLine++;

			if (strOriginal.GetLength() <= 0)
				break;

			if (nLine == 4 && strOriginal.GetLength() > nMaxTextLength - 3)
			{
				strOriginal = strOriginal.Left(nMaxTextLength - 3);
				strOriginal += L"...";
			}

			if (nLine == 5)
				break;
		}

		str.Format(L"%ls", strSplit.wstr());
		UI_MANAGER->CreateText(UI_WORLDSTAGE_CREATORCOMMENT_CONTENT, 16, 28, str);
		((UIText*)pUI)->m_dwTextColor = 0xffa0a0a0;
	}

	// Players' comments
	str.Format(L"Players' comments", m_pCurMetaInfo->m_strCreator.wstr());
	pUI = UI_MANAGER->CreateText(0 /*UI_WORLDSTAGE_PLAYERCOMMENT*/, 0, 74, str);

	UIList *pList = (UIList*)UI_MANAGER->CreateList(UI_WORLDSTAGE_PLAYERCOMMENT_CONTENT, 8, 86, m_pGame->m_nWidth - 80, 16 * 6);
	pList->m_bFocusAccept = true;
	pList->m_nItemHeight = 16;
	pUI = pList;

	// GameTheme, Difficulty
	float fClearRate;
	if (m_pCurMetaInfo->m_nCleared == 0 && m_pCurMetaInfo->m_nFailed == 0)
		fClearRate = -1;
	else
		fClearRate = (m_pCurMetaInfo->m_nCleared * 100.0f) / (m_pCurMetaInfo->m_nCleared + m_pCurMetaInfo->m_nFailed);

	int nGameTheme = m_pCurMetaInfo->CalcGameTheme();
	pUI = UI_MANAGER->CreateRectangle(
		0,
		pPanel->m_rc.Width() - 24,
		pPanel->m_rc.Height() - 20,
		30, 16,
		L"",
		0x00000000,
		m_pSpriteEtc, SPRIDX_ETC_THEME_SMB1 + (nGameTheme - 1));
	pUI->m_bEnable = false;

	// Difficulty
	int nDiff = -1;
	if (fClearRate > 0 && fClearRate <= 10)
		nDiff = SPRIDX_ETC_HELL;
	else if (fClearRate > 10 && fClearRate <= 40)
		nDiff = SPRIDX_ETC_HARD;
	else if (fClearRate > 40 && fClearRate <= 90)
		nDiff = SPRIDX_ETC_NORMAL;
	else if (fClearRate > 90)
		nDiff = SPRIDX_ETC_EASY;

	pUI = UI_MANAGER->CreateRectangle(
		0,
		pPanel->m_rc.Width() - 60,
		pPanel->m_rc.Height() - 20,
		30, 16,
		L"",
		0x00000000,
		m_pSpriteEtc, nDiff);
	pUI->m_bEnable = false;

	// Ratings .. etc
	NaString strClearRate;
	if (fClearRate == -1)
		strClearRate.Format(L"-");
	else
		strClearRate.Format(L"%.0f%%", fClearRate);

	NaString strDevRating;
	if (m_pCurMetaInfo->m_nDeveloperRating == -1)
		strDevRating.Format(L"%ls?", WSZ_NEUTRAL, m_pCurMetaInfo->m_nDeveloperRating);
	else if (m_pCurMetaInfo->m_nDeveloperRating == 0)
		strDevRating.Format(L"%ls%d", WSZ_BAD, m_pCurMetaInfo->m_nDeveloperRating);
	else
		strDevRating.Format(L"%ls%d", WSZ_GOOD, m_pCurMetaInfo->m_nDeveloperRating);

	str.Format(L"%ls %ls%d %ls%ls %ls%d %ls",
		m_pCurMetaInfo->m_nCleared == 0 ? WSZ_NEW : L"",
		WSZ_DOWNLOAD, m_pCurMetaInfo->m_nDownload,
		WSZ_CLEAR, strClearRate.wstr(),
		WSZ_LIKE, m_pCurMetaInfo->m_nLike,
		strDevRating.wstr()
	);
	pUI = UI_MANAGER->CreateText(
		0, 
		pPanel->m_rc.Width() - str.GetLength() * 8, 
		pPanel->m_rc.Height() - 8,
		str);
	//((UIText*)pUI)->m_dwTextColor = 0xffa0a0a0;

	m_Transaction.m_pJobThread = new std::thread(LoadWorldStageDetail, this);
}

void GameStateTitle::LoadWorldStageList(void* pParam, void* pList)
{
	GameStateTitle* pThis = (GameStateTitle*)pParam;

	pThis->m_Transaction.BeginJob(JOB_LOAD_WORLDSTAGE_LIST);

	ServerRequest req;
	ServerInfo *pInfo = nullptr;
	try
	{
		pInfo = req.GetServerInfo();
		if (pInfo == nullptr)
			throw NaException(__FUNCTIONW__, __LINE__, "Cannot get server information");
		
#if defined(NDEBUG)
		if (pInfo->m_nStatus != 1)
			throw NaException(__FUNCTIONW__, __LINE__, "The server is under maintenance break");

		if (pInfo->m_nVersion < ServerRequest::s_nCurrentVersion)
			throw NaException(__FUNCTIONW__, __LINE__, "The server is being updated");

		if (pInfo->m_nVersion > ServerRequest::s_nCurrentVersion)
			throw NaException(__FUNCTIONW__, __LINE__, "Game update required");
#endif

		auto vec = req.GetMetaInfoList();
		NaDebugOut(L"Load WorldStage List, %d Files Found.\n", vec.size());

#if !defined(NDEBUG)
		// Temporary Migration
		/*
		int j = 0;
		BEGIN_VECTOR_LOOP(StageMetaInfo*, vec) {
			if (pObj->m_strGameTheme.Compare(L"?") == 0)
			{
				NaDebugOut(L"Need Update: Stage #%03d: %ls\n", j, pObj->m_strTitle.wstr());

				// 
				pObj->CheckDownloaded(pThis->m_pGame);
				if (pObj->m_nLocalRevision < pObj->m_nRevision)
					req.DownloadStage(pObj->m_strUniqueId, pObj->m_strFilePath);
				 
				Stage *pTempStage = new Stage(pThis->m_pGame);
				pThis->m_pGame->m_strStagePack = L"Download";
				pThis->m_pGame->m_nStagePackType = STAGEPACK_SINGLE;
				pTempStage->Load(pObj->m_strUniqueId);
				pObj->m_strGameTheme = pTempStage->m_strGameTheme;
				NaDebugOut(L"-> %ls\n", pObj->m_strGameTheme.wstr());

				req.UpdateMetaInfo(pObj, UPDATE_METAINFO_GAMETHEME);
			}
			
			j++;
		} END_VECTOR_LOOP();
		//*/
#endif

		pThis->m_Transaction.m_Response.vecMetaInfo = vec;
		pThis->m_Transaction.m_Response.pUI = pList;

		pThis->m_Transaction.ChangeJob(JOB_LOAD_WORLDSTAGE_LIST_END);
	}
	catch (NaException &e)
	{
		pThis->m_Transaction.m_Response.strReason = NaString(e.m_szWhat).wstr();
		pThis->m_Transaction.m_Response.pUI = pList;

		pThis->m_Transaction.ChangeJob(JOB_LOAD_WORLDSTAGE_LIST_FAIL);

		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
	}

	pThis->m_Transaction.EndJob();
}

void GameStateTitle::OnLoadWorldStageList(std::vector<StageMetaInfo*> vec, UIList* pList)
{
	if (vec.size() == 0)
	{
		UI_MANAGER->CreateText(0, 8, 8, L"Stage not uploaded", pList);
		return;
	}

	std::sort(vec.begin(), vec.end(), StageMetaInfo::Compare);

	int j = 0;
	UIBase *pLastPlayed = nullptr;
	BEGIN_VECTOR_LOOP(StageMetaInfo*, vec) {
		NaDebugOut(L"Stage #%03d: %ls\n", j, pObj->m_strTitle.wstr());

		UIBase *pUI = UI_MANAGER->CreateWorldStageButton(
			UI_WORLDSTAGE_000 + j,
			0, j * pList->m_nItemHeight,
			pObj,
			CUR_PLAYER, pList);

		if (m_pGame->m_strLastStage.Compare(pObj->m_strUniqueId) == 0)
			pLastPlayed = pUI;

		j++;
	} END_VECTOR_LOOP();

	// Lifecycle of Metainfo == WorldStageButton
	//SAFE_DELETE_OBJPTR_VECTOR(StageMetaInfo*, vec);

	vec.clear();

	if (pList->m_vecUI.size() > 0)
	{
		UI_MANAGER->m_pCurPanel->m_pFocused = pList;
		if (pLastPlayed != nullptr)
			pList->m_pFocused = pLastPlayed;
		else
			pList->m_pFocused = pList->m_vecUI[0];
		pList->EnsureVisible(pList->m_pFocused);
	}
}

void GameStateTitle::OnLoadWorldStageListFail(NaString strReason, UIList * pList)
{
	UI_MANAGER->CreateText(0, 8, 8, strReason, pList);
}

void GameStateTitle::LoadWorldStageDetail(void * pParam)
{
	GameStateTitle* pThis = (GameStateTitle*)pParam;

	pThis->m_Transaction.BeginJob(JOB_LOAD_WORLDSTAGE_DETAIL);

	try
	{
		ServerRequest req;

		// Get UserInfo (Creator)
		auto pUserInfo = req.GetUserInfo(pThis->m_pCurMetaInfo->m_strCreatorUniqueId);
		auto strAvatar = pUserInfo->m_strAvatar;
		auto pUI = pThis->m_pGame->m_pUIManager->GetControl(UI_WORLDSTAGE_CREATORAVATAR);
		UIRectangle *pRect = (UIRectangle*)pUI;
		pRect->m_nSpriteIndex = pThis->GetAvatarIndex(strAvatar);
		
		delete pUserInfo;

		// Get Comments
		auto vecComments = req.GetComments(pThis->m_pCurMetaInfo->m_strUniqueId);
		auto pList = pThis->m_pGame->m_pUIManager->GetControl(UI_WORLDSTAGE_PLAYERCOMMENT_CONTENT);
		
		pThis->m_Transaction.m_Response.vecCommentInfo = vecComments;
		pThis->m_Transaction.m_Response.pUI = pList;

		pThis->m_Transaction.ChangeJob(JOB_LOAD_WORLDSTAGE_DETAIL_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		pThis->m_Transaction.ChangeJob(JOB_LOAD_WORLDSTAGE_DETAIL_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateTitle::OnLoadWorldStageDetail(std::vector<StageCommentInfo*> vec, UIList * pList)
{
	UIBase *pUI = nullptr;
	if (pList)
	{
		((UIList*)pList)->m_nItemHeight = 6;
		int j = 0;
		BEGIN_VECTOR_LOOP(StageCommentInfo*, vec) {
			// 1 line = 37 character

			NaString strPlayerComment;
			strPlayerComment.Format(L"%ls: %ls", pObj->m_strName.wstr(), pObj->m_strComment.wstr());
			UIBase *pUI = m_pGame->m_pUIManager->CreateRectangle(
				0,
				16,
				8 + j * 8,
				16, 8, L"", 0xff000000,
				m_pSpriteEtc,
				GetAvatarIndex(pObj->m_strAvatar),
				(UIContainer*)pList);

			// Processing wordwrap
			NaString strSplit = L"";
			NaString strOriginal = strPlayerComment;
			int nMaxTextLength = 37;
			int nLine = 0;
			for (; ; )
			{
				strSplit += strOriginal.Left(nMaxTextLength);
				strSplit += L"\n";
				strOriginal = strOriginal.Mid(nMaxTextLength);
				if (strOriginal.GetLength() <= 0)
					break;
			}

			NaStrArray ar = strSplit.Split(L"\n");
			for (int k = 0; k<ar.GetCount(); k++)
			{
				pUI = m_pGame->m_pUIManager->CreateText(
					0,
					32, 4 + j * 8,
					ar[k], (UIContainer*)pList);
				j++;
			}

			NaDebugOut(L"Comment #%d: %ls: %ls\n", j, pObj->m_strName.wstr(), pObj->m_strComment.wstr());
		} END_VECTOR_LOOP();

		if (vec.size() == 0)
		{
			pUI = m_pGame->m_pUIManager->CreateText(0, 8, 4, L"<No comment yet>", (UIContainer*)pList);
			((UIText*)pUI)->m_dwTextColor = 0xffa0a0a0;
		}
	}

	SAFE_DELETE_OBJPTR_VECTOR(StageCommentInfo*, vec);
}

void GameStateTitle::DownloadWorldStage(void * pParam)
{
	GameStateTitle* pThis = (GameStateTitle*)pParam;

	pThis->m_Transaction.BeginJob(JOB_DOWNLOAD_WORLDSTAGE);

	try
	{
		ServerRequest req;

		// Download
		req.DownloadStage(
			pThis->m_pCurMetaInfo->m_strUniqueId, 
			pThis->m_pCurMetaInfo->m_strFilePath
		);

		// Update download count
		pThis->m_pCurMetaInfo->m_nDownload += 1;
		req.UpdateMetaInfo(((GameStateTitle*)pThis)->m_pCurMetaInfo);

		pThis->m_Transaction.ChangeJob(JOB_DOWNLOAD_WORLDSTAGE_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());

		pThis->m_Transaction.ChangeJob(JOB_DOWNLOAD_WORLDSTAGE_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateTitle::OnDownloadWorldStage()
{
	m_Transaction.m_pJobThread = new std::thread(LoadWorldStageRecord, this);
}

void GameStateTitle::LoadWorldStageRecord(void * pParam)
{
	GameStateTitle* pThis = (GameStateTitle*)pParam;

	pThis->m_Transaction.BeginJob(JOB_LOAD_WORLDSTAGE_RECORD);

	try
	{
		ServerRequest req;

		// Load
		auto pRecord = req.GetStageRecordInfo(pThis->m_pCurMetaInfo->m_strUniqueId);
		
		pThis->m_Transaction.m_Response.pStageRecordInfo = pRecord;

		pThis->m_Transaction.ChangeJob(JOB_LOAD_WORLDSTAGE_RECORD_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());

		pThis->m_Transaction.ChangeJob(JOB_LOAD_WORLDSTAGE_RECORD_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateTitle::OnLoadWorldStageRecord()
{
	m_pGame->m_LastStageRecord.m_strDeathPositions = m_Transaction.m_Response.pStageRecordInfo->m_strDeathPositions;

	OnPlayWorldStage();
}

void GameStateTitle::OnPlayWorldStage()
{
	CreateParameter param;
	param.nStartDelay = -10;
	param.nDuration = 60;
	m_pGame->CreateSceneEffect(m_pGame->Width() / 2, m_pGame->Height() / 2, EFFECT_SCENE_CIRCLEOUT, &param);

	// Transition & Callback
	UI_MANAGER->Transition([&]()
	{
		// Play..
		m_pGame->m_strStagePack = L"Download";
		m_pGame->m_nStagePackType = STAGEPACK_SINGLE;
		m_pGame->m_strInitialStage = m_pCurMetaInfo->m_strUniqueId.wstr();
		m_pGame->m_strLastStage = m_pGame->m_strInitialStage;

		CUR_PLAYER->ApplyUserProfile(m_pCurProfile);
		CUR_PLAYER->Reset();

		m_pGame->PopAllStage();
		CUR_STAGE->Load(m_pGame->m_strInitialStage.wstr());

		UI_MANAGER->Clear();
		m_pGame->ChangeState(GAMESTATE_STAGEINTRO);

		// Fade in effect
        CreateParameter param;
		param.nStartDelay = 0;
		param.nDuration = 40;
		m_pGame->CreateSceneEffect(m_pGame->Width() / 2, m_pGame->Height() / 2, EFFECT_SCENE_FADEIN, &param);
	});
}

void GameStateTitle::RateWorldStage(void * pParam)
{
	GameStateTitle *pThis = (GameStateTitle*)pParam;

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

void GameStateTitle::OnSelectProfile(int nIdx)
{
	m_pCurProfile = m_pProfile[nIdx];
	if (m_pCurProfile->m_strName.GetLength() == 0)
	{
		m_pCurBtnProfile = UI_MANAGER->m_pClicked;
		InitProfileCreateUI();
	}
	else
	{
		CUR_PLAYER->ApplyUserProfile(m_pCurProfile);
		InitGameSelectUI();
	}
}

void GameStateTitle::OnSelectCustomGame()
{
	InitCustomGameSelectUI();
}

void GameStateTitle::OnSelectCustomStage()
{
	InitCustomStageSelectUI();
}

void GameStateTitle::OnSelectWorldStage()
{
	InitWorldStageSelectUI();
}

void GameStateTitle::OnSelectSomeCustomGame(int nIdx)
{
	// Pop CustomGameSelection Panel
	UI_MANAGER->PopPanel();

	StagePack sp = m_vecStagePack[nIdx];

	m_pGame->m_strStagePack = sp.m_strFileName;
	if (sp.m_strFileName.CompareNoCase(L"Classic") == 0)
		m_pGame->m_nStagePackType = STAGEPACK_CLASSIC;
	else
		m_pGame->m_nStagePackType = STAGEPACK_CUSTOM;
	m_pGame->m_strInitialStage = sp.m_strInitialStage;
	
	CUR_PLAYER->ApplyUserProfile(m_pCurProfile);

	m_pGame->PopAllStage();
	CUR_STAGE->Load(m_pGame->m_strInitialStage.wstr());

	if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
		InitClassicGameUI();
	else
		InitCustomGameUI();
}

void GameStateTitle::OnSelectSomeCustomStage(NaString strName)
{
	CreateParameter param;
	param.nStartDelay = -10;
	param.nDuration = 60;
	m_pGame->CreateSceneEffect(m_pGame->Width() / 2, m_pGame->Height() / 2, EFFECT_SCENE_CIRCLEOUT, &param);

	// Transition & Callback
	UI_MANAGER->Transition([&, strName]()
	{
		// Play
		m_pGame->m_strStagePack = L".";
		m_pGame->m_nStagePackType = STAGEPACK_SINGLE;
		m_pGame->m_strInitialStage = strName.wstr();

		CUR_PLAYER->ApplyUserProfile(m_pCurProfile);

		CUR_PLAYER->Reset();

		m_pGame->PopAllStage();
		CUR_STAGE->Load(m_pGame->m_strInitialStage.wstr());

		UI_MANAGER->Clear();
		m_pGame->ChangeState(GAMESTATE_STAGEINTRO);

		// Fade in effect
        CreateParameter param;
		param.nStartDelay = 0;
		param.nDuration = 40;
		m_pGame->CreateSceneEffect(m_pGame->Width() / 2, m_pGame->Height() / 2, EFFECT_SCENE_FADEIN, &param);
	});
}

void GameStateTitle::OnSelectSomeWorldStage(StageMetaInfo *pMetaInfo)
{
	m_pCurMetaInfo = pMetaInfo;
	m_pCurMetaInfo->CheckDownloaded(m_pGame);

	InitWorldStageDetailUI();
}

void GameStateTitle::OnSelectPlayCurrentWorldStage()
{
	if (m_pCurMetaInfo->m_nLocalRevision < m_pCurMetaInfo->m_nRevision)
	{
		// Download & play
		m_Transaction.m_pJobThread = new std::thread(DownloadWorldStage, this);
	}
	else
	{
		OnDownloadWorldStage();
	}
}

void GameStateTitle::OnSelectContinue()
{
	CUR_PLAYER->ApplyUserProfile(m_pCurProfile);

	m_pGame->PopAllStage();
	CUR_STAGE->Load(m_pCurProfile->GetLastStage().wstr());

	UI_MANAGER->Clear();
	m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
}

void GameStateTitle::OnSelectStageSelect()
{
	InitStageSelectUI();
}

void GameStateTitle::OnSelectNewGame()
{
	if (m_pCurProfile)
	{
		m_pCurProfile->AddSelectableStage(CUR_STAGE->m_strName);
		m_pCurProfile->Save();

		CUR_PLAYER->ApplyUserProfile(m_pCurProfile);
	}

	// #TODO When InitStage, Set that as LastStage
	//m_pCurProfile->GetLastStage() = CUR_STAGE->m_strName;
	
	m_pGame->PopAllStage();
	CUR_STAGE->Load(CUR_STAGE->m_strName.wstr());
	CUR_PLAYER->Reset();

	UI_MANAGER->Clear();
	m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
}

void GameStateTitle::OnCompleteUserName()
{
	UIEdit *pUI = (UIEdit*)UI_MANAGER->GetFocus();
	if (pUI->m_strText.GetLength() > 0)
	{
		m_pCurProfile->m_strName = pUI->m_strText;
		m_pCurProfile->Save();
		CUR_PLAYER->ApplyUserProfile(m_pCurProfile);

		((UIButton*)m_pCurBtnProfile)->m_strText = pUI->m_strText;

		UI_MANAGER->PopPanel();
		UI_MANAGER->m_pCurPanel->RecalcBoxSize();
		UI_MANAGER->m_pCurPanel->RecalcAlignPos();
	}
}

void GameStateTitle::OnSelectStage()
{
	UIButton *pUI = (UIButton*)UI_MANAGER->m_pClicked;

	m_pGame->PopAllStage();
	CUR_STAGE->Load(pUI->m_strText.wstr());

	UI_MANAGER->Clear();
	m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
}

void GameStateTitle::OnCancel()
{
	UI_MANAGER->PopPanel();
}

void GameStateTitle::OnChangeWorldStageDifficultyFilter(UIBase * pUI)
{
	// 0 all
	// 1 easy
	// 2 normal
	// 3 hard
	// 4 hell
	m_nCurDiffFilter++;
	if (m_nCurDiffFilter > 4)
		m_nCurDiffFilter = 0;

	StageMetaInfo::s_nStageMataInfoDifficultyFilter = m_nCurDiffFilter;

 	auto pConfig = GameConfig::GetGameConfig(m_pGame);
 	pConfig->m_nStageDifficultyFilter = m_nCurDiffFilter;
 	pConfig->Save();

	std::vector<UIWorldStageButton*> vec;
	UIList *pList = (UIList*)pUI;

	BEGIN_VECTOR_LOOP(UIBase*, pList->m_vecUI) {
		vec.push_back((UIWorldStageButton*)pObj);
	} END_VECTOR_LOOP();

	std::sort(vec.begin(), vec.end(), UIWorldStageButton::Compare);

	pList->m_vecUI.clear();

	int j = 0;
	BEGIN_VECTOR_LOOP(UIWorldStageButton*, vec) {
		pObj->m_nY = j * pList->m_nItemHeight;
		pList->m_vecUI.push_back(pObj);

		j++;
	} END_VECTOR_LOOP();

	auto pButton = (UIImageButton*)UI_MANAGER->GetControl(UI_WORLDSTAGE_FILTER_DIFFICULTY, UI_MANAGER->m_pCurPanel);
	pButton->m_nSpriteIndex = SPRIDX_ETC_SORT_DIFF_ALL + m_nCurDiffFilter;
}

void GameStateTitle::OnChangeWorldStageThemeFilter(UIBase * pUI)
{
	// 0 all
	// 1 smb1
	// 2 smb3
	// 3 smw
	// 4 custom
	m_nCurThemeFilter++;
	if (m_nCurThemeFilter > 4)
		m_nCurThemeFilter = 0;

	StageMetaInfo::s_nStageMataInfoThemeFilter = m_nCurThemeFilter;

	auto pConfig = GameConfig::GetGameConfig(m_pGame);
	pConfig->m_nStageThemeFilter = m_nCurThemeFilter;
	pConfig->Save();

	std::vector<UIWorldStageButton*> vec;
	UIList *pList = (UIList*)pUI;

	BEGIN_VECTOR_LOOP(UIBase*, pList->m_vecUI) {
		vec.push_back((UIWorldStageButton*)pObj);
	} END_VECTOR_LOOP();

	std::sort(vec.begin(), vec.end(), UIWorldStageButton::Compare);

	pList->m_vecUI.clear();

	int j = 0;
	BEGIN_VECTOR_LOOP(UIWorldStageButton*, vec) {
		pObj->m_nY = j * pList->m_nItemHeight;
		pList->m_vecUI.push_back(pObj);

		j++;
	} END_VECTOR_LOOP();

	auto pButton = (UIImageButton*)UI_MANAGER->GetControl(UI_WORLDSTAGE_FILTER_GAMETHEME, UI_MANAGER->m_pCurPanel);
	pButton->m_nSpriteIndex = SPRIDX_ETC_SORT_THEME_ALL + m_nCurThemeFilter;
}

void GameStateTitle::OnChangeWorldStageSortMethod(UIBase * pUI, int nMethod)
{
	if (m_nCurSortMethod == nMethod)
		StageMetaInfo::s_nStageMataInfoSortOption = -nMethod;
	else
		StageMetaInfo::s_nStageMataInfoSortOption = nMethod;
	m_nCurSortMethod = StageMetaInfo::s_nStageMataInfoSortOption;

	auto pConfig = GameConfig::GetGameConfig(m_pGame);
	pConfig->m_nStageSortMethod = m_nCurSortMethod;
	pConfig->Save();

	std::vector<UIWorldStageButton*> vec;
	UIList *pList = (UIList*)pUI;

	BEGIN_VECTOR_LOOP(UIBase*, pList->m_vecUI) {
		vec.push_back((UIWorldStageButton*)pObj);
	} END_VECTOR_LOOP();

	std::sort(vec.begin(), vec.end(), UIWorldStageButton::Compare);
	
	pList->m_vecUI.clear();

	int j = 0;
	BEGIN_VECTOR_LOOP(UIWorldStageButton*, vec) {
		pObj->m_nY = j * pList->m_nItemHeight;
		pList->m_vecUI.push_back(pObj);

		j++;
	} END_VECTOR_LOOP();
}

int GameStateTitle::GetAvatarIndex(NaString str)
{
	if (str.CompareNoCase(L"Mario") == 0)
		return SPRIDX_ETC_AVATAR1;
	else if (str.CompareNoCase(L"BigMario") == 0)
		return SPRIDX_ETC_AVATAR2;
	else if (str.CompareNoCase(L"Goomba") == 0)
		return SPRIDX_ETC_AVATAR3;
	else if (str.CompareNoCase(L"Koopa") == 0)
		return SPRIDX_ETC_AVATAR4;
	else if (str.CompareNoCase(L"Spiny") == 0)
		return SPRIDX_ETC_AVATAR5;
	else if (str.CompareNoCase(L"FireFlower") == 0)
		return SPRIDX_ETC_AVATAR6;
	else if (str.CompareNoCase(L"Bloober") == 0)
		return SPRIDX_ETC_AVATAR7;
	else if (str.CompareNoCase(L"Bowser") == 0)
		return SPRIDX_ETC_AVATAR8;

	return SPRIDX_ETC_AVATAR0;
}
