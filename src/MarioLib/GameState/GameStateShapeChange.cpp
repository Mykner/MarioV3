#include "GameStateShapeChange.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "Camera.h"

#include "InputManager.h"

GameStateShapeChange::GameStateShapeChange(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_SHAPECHANGE;

	m_bDebugShapeChangeFrame = false;
}

GameStateShapeChange::~GameStateShapeChange()
{
}

void GameStateShapeChange::Process()
{
	if (!m_bDebugShapeChangeFrame || KE(VK_F))
	{
		if (KE(VK_F))
			m_bDebugShapeChangeFrame = true;

		CUR_PLAYER_OBJ->OnBeforeProcess();
		CUR_PLAYER_OBJ->ProcessShapeChange(m_nStateFrame);

		if (CUR_PLAYER_OBJ->m_nNextShape == -1)
		{
			PopState();
			delete this;

			return;
		}

		CUR_STAGE->ProcessEffects();
	}
	else
	{
		if (KE(VK_P))
			m_bDebugShapeChangeFrame = false;
		m_nStateFrame--;
	}
}

void GameStateShapeChange::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderShapeChange();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
