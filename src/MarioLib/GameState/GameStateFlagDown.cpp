#include "GameStateFlagDown.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "PipeInfo.h"

GameStateFlagDown::GameStateFlagDown(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_FLAGDOWN;
}

GameStateFlagDown::~GameStateFlagDown()
{
}

void GameStateFlagDown::Init()
{
	DeleteOldState();

	ChangeEnemyInViewportToCoin();
	ChangeVehicleInViewportToCoin();

	Stage *pTopStage = CUR_STAGE->GetTopStage();
	pTopStage->EndStopWatch();
	CUR_PLAYER->LockInput();

	SOUND_MANAGER->StopBGM();
}

void GameStateFlagDown::Process()
{
	if (KE(VK_SPACE))
	{
		CUR_PLAYER_OBJ->m_fYS = 0;

		CUR_PLAYER->HoldInput(INPUT_R);

		m_nStateFrame = 121;
		SOUND_MANAGER->StopBGM();
	}

	if (m_nStateFrame == 120)
	{
		CUR_PLAYER->HoldInput(INPUT_R);

		SOUND_MANAGER->PlayBGM(L"LevelComplete", false);
	}

	if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
	{
		if (m_nStateFrame == 500)
		{
			CUR_PLAYER->UnholdInput(INPUT_R);
			CUR_PLAYER->HoldInput(INPUT_L);
		}

		if (m_nStateFrame == 501)
			CUR_PLAYER->UnholdInput(INPUT_L);

		if (m_nStateFrame == 640)
		{
			m_pGame->ChangeState(GAMESTATE_SINGLESTAGECLEAR);
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

	CAMERA->Process();
	CAMERA->ProcessPlayerBound();
}

void GameStateFlagDown::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
