#include "GameStateEnterStage.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "PipeInfo.h"

GameStateEnterStage::GameStateEnterStage(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_ENTERSTAGE;
}

GameStateEnterStage::~GameStateEnterStage()
{
}

void GameStateEnterStage::Init()
{
	DeleteOldState();

	CUR_PLAYER->LockInput();
	CUR_PLAYER->HoldInput(INPUT_R);
}

void GameStateEnterStage::Process()
{
	if (KE(VK_SPACE))
	{
		CUR_PLAYER->HoldInput(INPUT_F);
	}

	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->OnAfterProcess();

	CUR_STAGE->ProcessEffects();
}

void GameStateEnterStage::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
