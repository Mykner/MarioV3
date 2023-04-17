#include "GameStateExitDoor.h"

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

GameStateExitDoor::GameStateExitDoor(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_EXITDOOR;

	m_pDoorEntrance = nullptr;
	m_pDoorExit = nullptr;
}

GameStateExitDoor::~GameStateExitDoor()
{
}

void GameStateExitDoor::Init()
{
	m_pDoorEntrance = m_pGame->m_paramNextState.pFromObject;
	m_pDoorExit = FindPairDoor(m_pDoorEntrance);

	// Circle In
	m_pGame->ClearSceneEffect();

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	CreateParameter param;
	param.nDuration = 32;
	param.nStartDelay = 10;
	m_pGame->CreateSceneEffect(
		CUR_PLAYER_OBJ->m_fX - ptCamera.x, 
		CUR_PLAYER_OBJ->m_fY - ptCamera.y - 16,
		EFFECT_SCENE_CIRCLEIN, &param);

	if (m_pDoorExit)
	{
		// #TODO Activate this code when finished SceneAnimation
		//m_pDoorEntrance->SetState(STATE_IDLE);
		
		m_pDoorEntrance->SetState(STATE_CLOSE);
		m_pDoorExit->SetState(STATE_CLOSE);
	}
	else
	{
		m_pDoorEntrance->SetState(STATE_CLOSE);
	}

	DeleteOldState();
}

void GameStateExitDoor::Process()
{
	if (m_nStateFrame == 0)
	{
		PLAYSOUND(L"DoorOpen");
	}

	if (m_nStateFrame == 32)
	{
		if (CUR_PLAYER_OBJ->m_pVehicle)
		{
			CUR_PLAYER_OBJ->m_pVehicle->ChangeState(STATE_STAND);
		}
		else
		{
			CUR_PLAYER_OBJ->ChangeState(STATE_STAND);
		}

		CUR_PLAYER->UnholdInput(INPUT_F);
		CUR_PLAYER->UnlockInput();

		m_pGame->ChangeState(GAMESTATE_PLAY);
	}

	CUR_STAGE->ProcessEffects();

	CAMERA->Process();
}

void GameStateExitDoor::Render()
{
	if (m_nStateFrame == 0)
	{
		CUR_STAGE->RenderEffects();
		return;
	}

	CUR_STAGE->RenderBackground();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->Render();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
