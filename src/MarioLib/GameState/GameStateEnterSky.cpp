#include "GameStateEnterSky.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "Vine.h"

GameStateEnterSky::GameStateEnterSky(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_ENTERSKY;
}

GameStateEnterSky::~GameStateEnterSky()
{
}

void GameStateEnterSky::Init()
{
	DeleteOldState();
}

void GameStateEnterSky::Process()
{
	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->OnAfterProcess();

	if (m_nStateFrame == 60)
	{
		// Normal Pipe (Bonus stage)
		m_pGame->LoadSkyStage();
		m_pGame->InitPlayerObject();
		m_pGame->InitCamera();

		switch (CUR_STAGE->m_nEntranceType)
		{
		default:
			m_pGame->ChangeState(GAMESTATE_PLAY);
			break;
		case STAGEENTRANCE_GROUNDTOSKY:
			{
				// Find Live Vine
				Vine *pVine = (Vine*)FindVine();
				if (pVine)
				{
					pVine->m_nMaxLength = 6 * 16;
					pVine->SetState(STATE_VINEGROW);
					pVine->m_nStateFrame = 0;
				}

				// Attach Player
				CUR_PLAYER_OBJ->m_bFlip = false;
				CUR_PLAYER_OBJ->m_fX = pVine->m_fX;
				CUR_PLAYER_OBJ->m_fY = pVine->m_fY + 15;

				ChangeStateInfo info;
				info.pFromObject = pVine;
				CUR_PLAYER_OBJ->ChangeState(STATE_CLIMBIDLE, &info);
				CUR_PLAYER_OBJ->OnChangeState(STATE_CLIMBIDLE);
				
				m_pGame->ChangeState(GAMESTATE_SKYINTRO);
			}
			break;
		}

		CUR_STAGE->PlayBGM();

		DeleteOldState();
	}

	CUR_STAGE->ProcessEffects();
}

void GameStateEnterSky::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->Render();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}

GameObjectBase * GameStateEnterSky::FindVine()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecItem) {
		if (pObj->m_nType == ITEM_VINE)
			return pObj;
	} END_VECTOR_LOOP();

	return nullptr;
}
