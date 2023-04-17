#include "GameStateSkyIntro.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "PipeInfo.h"

GameStateSkyIntro::GameStateSkyIntro(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_SKYINTRO;
}

GameStateSkyIntro::~GameStateSkyIntro()
{
}

void GameStateSkyIntro::Init()
{
	DeleteOldState();

	CUR_PLAYER_OBJ->m_fYS = -1.0f;
	CUR_PLAYER_OBJ->SetState(STATE_VINEUP);

	PLAYSOUND(L"Vine");
}

void GameStateSkyIntro::Process()
{
	bool bProcessStage = true;
	bool bProcessPlayer = false;	

	if (m_nStateFrame == 6*24)
	{
		// Force Move Up
		CUR_PLAYER->LockInput();
		CUR_PLAYER->HoldInput(INPUT_U);
	}

	if (m_nStateFrame >= 6*24)
	{
		bProcessPlayer = true;
	}

	if (m_nStateFrame == 200)
	{
		CUR_PLAYER->UnholdInput(INPUT_U);

		CUR_PLAYER_OBJ->m_fYS = 0;
		CUR_PLAYER_OBJ->SetState(STATE_CLIMBIDLE);
	}

	if (m_nStateFrame == 224)
	{
		CUR_PLAYER->HoldInput(INPUT_R);
	}

	if (m_nStateFrame > 224)
	{
		if (CUR_PLAYER_OBJ->m_bCollision[COLLISION_BOTTOM])
		{
			CUR_PLAYER->UnholdInput(INPUT_R);
			CUR_PLAYER->UnlockInput();

			CUR_PLAYER_OBJ->ChangeState(STATE_STAND);
			m_pGame->ChangeState(GAMESTATE_PLAY);
		}		
	}

	CUR_STAGE->OnBeforeProcess();
	if (bProcessPlayer) CUR_PLAYER->OnBeforeProcess();

	CUR_STAGE->Process();
	if (bProcessPlayer) CUR_PLAYER->Process();

	CUR_STAGE->OnAfterProcess();
	if (bProcessPlayer) CUR_PLAYER->OnAfterProcess();

	CUR_STAGE->ProcessEffects();
}

void GameStateSkyIntro::Render()
{
	CUR_STAGE->RenderBackground();
	if (m_nStateFrame >= 6*24)
		CUR_PLAYER->RenderControlObj();
	CUR_STAGE->Render();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}

GameObjectBase * GameStateSkyIntro::FindPiranhaPlant()
{
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		if ((*it)->m_nType == ENEMY_PIRANHA_PLANT)
		{
			NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
			rcPlayer.top -= 60;

			NaRect rcPiranha = (*it)->GetRect();
			if (rcPlayer.IsOverlapped(rcPiranha))
				return (*it);
		}
	}

	return nullptr;
}
