#include "GameStateEnterDoor.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "PipeInfo.h"
#include "PiranhaPlant.h"
#include "UserProfile.h"
#include "VehicleObjectBase.h"

GameStateEnterDoor::GameStateEnterDoor(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_ENTERDOOR;

	m_pDoorEntrance = nullptr;
	m_pDoorExit = nullptr;
}

GameStateEnterDoor::~GameStateEnterDoor()
{
}

void GameStateEnterDoor::Init()
{
	m_pDoorEntrance = m_pGame->m_paramNextState.pFromObject;
	m_pDoorExit = FindPairDoor(m_pDoorEntrance);

	// Fade Out
	CreateParameter param;
	param.nDuration = 32;
	m_pGame->CreateSceneEffect(0, 0, EFFECT_SCENE_FADEOUT, &param);

	CUR_PLAYER_OBJ->ResetSpeed();
	if (CUR_PLAYER_OBJ->m_pVehicle)
	{
		CUR_PLAYER_OBJ->m_pVehicle->ResetSpeed();
	}

	bool bHoldFire = CUR_PLAYER->IsInputHold(INPUT_F);
	CUR_PLAYER->LockInput();
	if (bHoldFire)
	{
		CUR_PLAYER->HoldInput(INPUT_F);
	}

	PLAYSOUND(L"DoorOpen");
}

void GameStateEnterDoor::Process()
{
	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->OnAfterProcess();

	m_pDoorEntrance->Process();
	m_pDoorEntrance->OnAfterProcess();

	if (m_pDoorExit)
	{
		m_pDoorExit->Process();
		m_pDoorExit->OnAfterProcess();
	}

	if (m_pDoorEntrance->m_nState == STATE_OPEN &&
		m_pDoorEntrance->m_nStateFrame == 32)
	{
		CUR_PLAYER_OBJ->m_bFlip = false;

		if (CUR_PLAYER_OBJ->m_pVehicle)
			CUR_PLAYER_OBJ->m_pVehicle->m_bFlip = false;

		MovePlayerObjectToPairDoor(m_pDoorEntrance);
		MoveCameraToPlayer();

		GameStateChangeParameter param;
		param.pFromObject = m_pDoorEntrance;
		m_pGame->ChangeState(GAMESTATE_EXITDOOR);
		
		DeleteOldState();
	}

	CUR_STAGE->ProcessEffects();
}

void GameStateEnterDoor::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->Render();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
