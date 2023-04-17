#include "GameStateDesign.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"

GameStateDesign::GameStateDesign(Game *pGame)
	: GameStateBase(pGame)
{
	m_pGame = pGame;
}


GameStateDesign::~GameStateDesign()
{
}

void GameStateDesign::Init()
{
}

void GameStateDesign::Process()
{
	// Will not called
	CUR_STAGE->OnBeforeProcess();
	CUR_PLAYER->OnBeforeProcess();

	CUR_STAGE->Process();
	CUR_PLAYER->Process();

	CUR_STAGE->OnAfterProcess();
	CUR_PLAYER->OnAfterProcess();
}

void GameStateDesign::Render()
{
	CUR_STAGE->Render();
	CUR_PLAYER->RenderDesignStatus();
}
