#include "GameStateEnterPipe.h"

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

GameStateEnterPipe::GameStateEnterPipe(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_ENTERPIPE;
}

GameStateEnterPipe::~GameStateEnterPipe()
{
}

void GameStateEnterPipe::Init()
{
	CUR_STAGE->m_pCurPipe = m_pGame->m_paramNextState.m_pCurPipe;

	PLAYSOUND(L"PowerDown");
}

void GameStateEnterPipe::Process()
{
	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->OnAfterProcess();

	if (m_nStateFrame == 32)
	{
		CUR_PLAYER_OBJ->ResetSpeed();
		CUR_PLAYER_OBJ->m_bFlip = false;

		if (CUR_PLAYER_OBJ->m_pVehicle)
		{
			CUR_PLAYER_OBJ->m_pVehicle->ResetSpeed();
			CUR_PLAYER_OBJ->m_pVehicle->m_bFlip = false;
		}

		// Fade Out
		CreateParameter param;
		param.nDuration = 28;
		m_pGame->CreateSceneEffect(0, 0, EFFECT_SCENE_FADEOUT, &param);
	}

	if (m_nStateFrame == 60)
	{
		// Apply all following keys
		BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecItem) {
			if (pObj->m_nType == ITEM_DOORKEY &&
				pObj->m_nState == STATE_FOLLOWPREPARE)
			{
				CUR_PLAYER_OBJ->OnApplyItem(pObj);
				pObj->SetState(STATE_DEAD);

				CUR_PLAYER->AddDoorKey(pObj);
			}
		} END_VECTOR_LOOP();

		// Stage Entrance Pipe
		if (m_pOldState->m_nType == GAMESTATE_ENTERSTAGE)
		{
			ProcessOnAfterEnterStage();
			return;
		}

		// Go into pipe
		PipeInfo *pPipeInfo = CUR_STAGE->m_pCurPipe;
		if (pPipeInfo == nullptr)
		{
			if (CUR_STAGE->m_pParentStage)
				ProcessOnAfterBonusStage();
			return;
		}

		switch (pPipeInfo->m_nWarpType)
		{
		case PIPE_WARP_TO_BONUSZONE:
			{
				// Normal Pipe (Bonus stage)
				if (CUR_STAGE->m_pParentStage)
				{
					// This is inside of pipe -> turn back to old stage
					ProcessExitBonusStage(pPipeInfo);
				}
				else
				{
					ProcessEnterBonusStage(pPipeInfo);
				}

				if (CUR_STAGE->m_bSwapCoinBlock == false)
					CUR_STAGE->PlayBGM();
			}
			break;
		case PIPE_WARP_TO_CURRENTSTAGE:
			// Warp in current stage
			ProcessWarpToCurrentStage(pPipeInfo);
			break;
		case PIPE_WARPZONE:
			// Warp to new stage
			ProcessWarpZone(pPipeInfo);
			break;
		case PIPE_WARP_TO_SUBZONE:
			// Ignore entrance type,.. find indexed exit of pipe
			ProcessWarpToSubZone(pPipeInfo);
			break;
		}

		DeleteOldState();
	}

	CUR_STAGE->ProcessEffects();
}

void GameStateEnterPipe::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->Render();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}

void GameStateEnterPipe::ProcessOnAfterEnterStage()
{
	DeleteOldState();

	CUR_PLAYER->UnlockInput();
	CUR_PLAYER->UnholdInput(INPUT_R);
	CUR_PLAYER->UnholdInput(INPUT_F);

	m_pGame->ClearSceneEffect();
	m_pGame->PopStage();
	m_pGame->InitPlayerObject();
	m_pGame->InitCamera();
	m_pGame->ChangeState(GAMESTATE_PLAY);

	CUR_STAGE->PlayBGM();
	return;
}

void GameStateEnterPipe::ProcessOnAfterBonusStage()
{
	DeleteOldState();

	// This is inside of pipe -> turn back to old stage
	m_pGame->PopStage();

	PipeInfo *pPipeInfo = CUR_STAGE->m_pCurPipe;
	PipeExitInfo *pPipeExitInfo = pPipeInfo ? FindPipeExit(pPipeInfo->m_nExitIndex) : nullptr;
	if (pPipeInfo && pPipeExitInfo)
	{
		MovePlayerObjectToExit(pPipeExitInfo);
		m_pGame->InitCamera();
		m_pGame->ChangeState(GAMESTATE_EXITPIPE);
	}
	else
	{
		NaDebugOut(L"Error: PipeInfo or PipeExitInfo is missing!\n");
		m_pGame->ClearSceneEffect();
		m_pGame->ChangeState(GAMESTATE_PLAY);
	}
	CUR_STAGE->PlayBGM();
}

void GameStateEnterPipe::ProcessExitBonusStage(PipeInfo * pPipeInfo)
{
	m_pGame->PopStage();

	// Argument is just info that pipe in Bonus stage.

	// We need old stage's info. Re-take.
	pPipeInfo = CUR_STAGE->m_pCurPipe;

	PipeExitInfo *pPipeExitInfo = pPipeInfo ? FindPipeExit(pPipeInfo->m_nExitIndex) : nullptr;
	if (pPipeInfo && pPipeExitInfo)
	{
		MovePlayerObjectToExit(pPipeExitInfo);
		m_pGame->InitCamera();
		m_pGame->ChangeState(GAMESTATE_EXITPIPE);
	}
	else
	{
		NaDebugOut(L"Error: PipeInfo or PipeExitInfo is missing!\n");
		m_pGame->ClearSceneEffect();
		m_pGame->ChangeState(GAMESTATE_PLAY);
	}
}

void GameStateEnterPipe::ProcessEnterBonusStage(PipeInfo * pPipeInfo)
{
	m_pGame->LoadPipeStage();
	m_pGame->InitPlayerObject();
	m_pGame->InitCamera();

	switch (CUR_STAGE->m_nEntranceType)
	{
	default:
		m_pGame->ClearSceneEffect();
		m_pGame->ChangeState(GAMESTATE_PLAY);
		break;
	case STAGEENTRANCE_FROMPIPE:
		{
			PipeExitInfo *pPipeExitInfo = FindFirstExitOnlyPipe(); 
			if (pPipeInfo && pPipeExitInfo)
			{
				MovePlayerObjectToExit(pPipeExitInfo);

				GameObjectBase *pObj = CUR_PLAYER->GetLastControlObject();
				m_pGame->ChangeState(GAMESTATE_EXITPIPE);
			}
			else
			{
				m_pGame->ClearSceneEffect();
				m_pGame->ChangeState(GAMESTATE_PLAY);
			}
		}
		break;
	}
}

void GameStateEnterPipe::ProcessWarpToCurrentStage(PipeInfo * pPipeInfo)
{
	PipeExitInfo *pPipeExitInfo = pPipeInfo ? FindPipeExit(pPipeInfo->m_nExitIndex) : nullptr;
	if (pPipeExitInfo)
	{
		MovePlayerObjectToExit(pPipeExitInfo);
		MoveCameraToPlayer();
	}

	if (CUR_STAGE->m_bCanGoBack == false)
		MoveCameraToExit(pPipeExitInfo);

	m_pGame->ChangeState(GAMESTATE_EXITPIPE);
}

void GameStateEnterPipe::ProcessWarpZone(PipeInfo *pPipeInfo)
{
	NaString strNextStage = pPipeInfo->m_strStageName;

	m_pGame->PopAllStage();
	m_pGame->LoadWarpStage(strNextStage);
	m_pGame->InitPlayerObject();
	m_pGame->InitCamera();

	if (CUR_PLAYER->m_pProfile)
	{
		CUR_PLAYER->m_pProfile->SetLastStage(CUR_STAGE->m_strName);
		CUR_PLAYER->m_pProfile->AddSelectableStage(CUR_STAGE->m_strName);
		CUR_PLAYER->SaveUserProfile();

	}
	m_pGame->ClearSceneEffect();
	m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
}

void GameStateEnterPipe::ProcessWarpToSubZone(PipeInfo * pPipeInfo)
{
	if (CUR_STAGE->m_pParentStage == nullptr)
	{
		m_pGame->LoadSubStage();
		m_pGame->InitPlayerObject();
	}
	else if (pPipeInfo->m_strStageName.CompareNoCase(L"MainStage") == 0)
	{
		m_pGame->m_pCurStage = CUR_STAGE->m_pParentStage;
		m_pGame->InitPlayerObject();
	}
	else
	{
		m_pGame->LoadSubStage();
		m_pGame->InitPlayerObject();
	}

	m_pGame->m_pCurStage->InitTheme();

	PipeExitInfo *pExitPipeInfo = FindPipeExit(pPipeInfo->m_nExitIndex);
	if (pExitPipeInfo)
	{
		MovePlayerObjectToExit(pExitPipeInfo);
		MoveCameraToPlayer();

		m_pGame->ChangeState(GAMESTATE_EXITPIPE);
	}
	else
	{
		// #TODO How?
		NaDebugOut(L"Error: Cannot find SubZone Stage!\n");

		MoveCameraToPlayer();

		GameObjectBase *pObj = CUR_PLAYER->GetLastControlObject();
		pObj->ChangeState(STATE_PIPEUP);
		m_pGame->ChangeState(GAMESTATE_EXITPIPE);
	}

	if (CUR_STAGE->m_bSwapCoinBlock == false)
		CUR_STAGE->PlayBGM();
}
