#include "GameStateExitPipe.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "PipeInfo.h"
#include "VehicleObjectBase.h"
#include "PiranhaPlant.h"
#include "Camera.h"

GameStateExitPipe::GameStateExitPipe(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_EXITPIPE;
}

GameStateExitPipe::~GameStateExitPipe()
{
}

void GameStateExitPipe::Init()
{
	DeleteOldState();

	NaRect rc = CUR_PLAYER->GetLastControlObject()->GetRect();
	rc.top -= 32;

	PiranhaPlant *pPiranhaPlant = (PiranhaPlant*)FindPiranhaPlantAtExit(rc);
	if (pPiranhaPlant)
	{
		if (pPiranhaPlant->IsAlive())
			m_vecPiranhaPlant.push_back(pPiranhaPlant);
	}

	// Circle In
	m_pGame->ClearSceneEffect();

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	CreateParameter param;
	param.nStartDelay = 30;
	param.nDuration = 60;
	m_pGame->CreateSceneEffect(
		CUR_PLAYER_OBJ->m_fX - ptCamera.x,
		CUR_PLAYER_OBJ->m_fY - ptCamera.y - 32, 
		EFFECT_SCENE_CIRCLEIN, &param);
}

void GameStateExitPipe::Process()
{
	if (m_vecPiranhaPlant.size() > 0)
	{
		if (m_nStateFrame < 60)
		{
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPiranhaPlant) {
				pObj->OnBeforeProcess();
				pObj->Process();
				pObj->OnAfterProcess();
			} END_VECTOR_LOOP();
		}
	}

	if (m_nStateFrame == 30)
	{
		PLAYSOUND(L"PowerDown");
	}

	if (m_nStateFrame >= 30)
	{
		CUR_PLAYER->OnBeforeProcess();
		CUR_PLAYER->Process();
		CUR_PLAYER->OnAfterProcess();
	}

	if (m_nStateFrame == 61)
	{
		if (CUR_PLAYER_OBJ->m_pVehicle)
		{
			CUR_PLAYER_OBJ->m_pVehicle->ChangeState(STATE_STAND);
		}
		else
		{
			CUR_PLAYER_OBJ->ChangeState(STATE_STAND);
		}
		m_pGame->ChangeState(GAMESTATE_PLAY);
	}

	CUR_STAGE->ProcessEffects();

	CAMERA->Process();
}

void GameStateExitPipe::Render()
{
	CUR_STAGE->RenderBackground();
	if (m_nStateFrame >= 30)
		CUR_PLAYER->RenderControlObj();
	CUR_STAGE->Render();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}

GameObjectBase * GameStateExitPipe::FindPiranhaPlant()
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
