#pragma once

#include "GameStateBase.h"

#include "ServerTransaction.h"

class GameStateTest : public GameStateBase
{
public:
	GameStateTest(Game *pGame);
	virtual ~GameStateTest();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	void ChangeZoomFactor(int nZoomFactor);
	void InitTestMenuUI();
	void InitRateMenuUI();

	static void RateStage(void *pParam);

	int m_nStatusMode;
	bool m_bFrameCheck;
	int m_nObserveIdx;

	ServerTransaction m_Transaction;
	StageMetaInfo *m_pCurMetaInfo;

	enum GameStateTestUI
	{
		UI_UNDEFINED = 0,
		UI_GOTO_TITLE,
		
		UI_RATE_STAGE, // Developer's rating
			UI_RATE_INPUT,

		UI_RESET_CURRENT_STAGE,
		UI_RESET_POWERUPS,
		UI_RESET_TIME,
		UI_SELECT_STAGE,
			UI_CUSTOMSTAGE_000,
			UI_CUSTOMSTAGE_999 = UI_CUSTOMSTAGE_000 + 999,
			UI_BACK,
		UI_RELOAD_TEXTURES,
		UI_LOAD_INITIAL_STAGE,
		UI_TEST_STATE,
		UI_TEST_FUNC,
		UI_TOGGLE_MODERNSTYLE,
		UI_GIVEUP,
		UI_GIVEUP_ALL,
		UI_CHANGE_GAMETHEME,
		UI_CHANGE_THEMESET,
		UI_CLOSE_MENU,
	};
};

