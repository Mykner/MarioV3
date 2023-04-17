#include "GameStatePlay.h"

#include "Stage.h"
#include "Player.h"
#include "InputManager.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UIBase.h"
#include "Camera.h"
#include "SoundManager.h"

GameStatePlay::GameStatePlay(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_PLAY;
}

GameStatePlay::~GameStatePlay()
{
}

void GameStatePlay::Init()
{
	DeleteOldState();

	CUR_PLAYER->UnlockInput();
	CUR_PLAYER->UnholdInputAll();
}

void GameStatePlay::Process()
{
	if (UI_MANAGER->m_pCurPanel->m_vecUI.size() == 0)
	{
		if (KE(VK_ESCAPE))
		{
			InitPauseMenuUI();

			PLAYSOUND(L"Pause");
			SOUND_MANAGER->PauseBGM(true);
		}

		CUR_STAGE->OnBeforeProcess();
		CUR_PLAYER->OnBeforeProcess();

		CUR_STAGE->Process();
		CUR_PLAYER->Process();

		CUR_STAGE->OnAfterProcess();
		CUR_PLAYER->OnAfterProcess();

		CUR_PLAYER->ProcessTime();
		CUR_STAGE->ProcessEffects();
		
		CAMERA->Process();
		CAMERA->ProcessPlayerBound();
	}
	else
	{
		UI_MANAGER->Process();

		if (KE(VK_ESCAPE))
		{
			UI_MANAGER->Clear();
			SOUND_MANAGER->PauseBGM(false);
		}

		if (UI_MANAGER->m_pClicked)
		{
			int nIDUI = UI_MANAGER->m_pClicked->m_nID;
			switch (nIDUI)
			{
			case UI_GOTO_TITLE:
				UI_MANAGER->Clear();
				m_pGame->ChangeState(GAMESTATE_TITLE);
				break;
			case UI_CONTINUE:
				UI_MANAGER->Clear();
				SOUND_MANAGER->PauseBGM(false);
				break;
			case UI_GIVEUP:
				UI_MANAGER->Clear();
				SOUND_MANAGER->PauseBGM(false);

				CUR_PLAYER_OBJ->ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
				break;
			case UI_GIVEUP_ALL:
				UI_MANAGER->Clear();
				SOUND_MANAGER->PauseBGM(false);

				CUR_PLAYER->m_nLife = 1;
				CUR_PLAYER_OBJ->ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
				break;
			}
		}
	}
}

void GameStatePlay::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();

	UI_MANAGER->Render();

	UIBase* pFocusedUI = UI_MANAGER->GetFocus();
	if (pFocusedUI)
	{
		NaRect rc = pFocusedUI->GetScreenRect();
		DRAWTEXT(
			rc.left - 12,
			rc.top,
			L"\x08");
	}
}

void GameStatePlay::InitPauseMenuUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int i = 0, y = 0;
	NaString str;
	UIBase *pUI;
	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"--- PAUSE ---");

	if (IS_ONLINESTAGE)
	{
		pUI = UI_MANAGER->CreateButton(
			UI_GOTO_TITLE, 0, y + ((i++) * 16), L"GO TO TITLE");

		pUI = UI_MANAGER->CreateButton(
			UI_GIVEUP, 0, y + ((i++) * 16), L"GIVE UP 1 LIFE");

		pUI = UI_MANAGER->CreateButton(
			UI_GIVEUP_ALL, 0, y + ((i++) * 16), L"GIVE UP ALL LIVES");
	}
	else
	{
		pUI = UI_MANAGER->CreateButton(
			UI_GOTO_TITLE, 0, y + ((i++) * 16), L"GO TO TITLE");
	}

	pUI = UI_MANAGER->CreateButton(
		UI_CONTINUE, 0, y + ((i++) * 16), L"CONTINUE GAME");

	pPanel->m_pFocused = pUI;
}
