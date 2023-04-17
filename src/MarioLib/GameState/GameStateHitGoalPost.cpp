#include "GameStateHitGoalPost.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "PipeInfo.h"
#include "FModManager.h"
#include "UserProfile.h"

GameStateHitGoalPost::GameStateHitGoalPost(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_HITGOALPOST;
	
	m_nTimeStamp = -1;
}

GameStateHitGoalPost::~GameStateHitGoalPost()
{
}

void GameStateHitGoalPost::Init()
{
	DeleteOldState();

	ChangeEnemyInViewportToCoin();
	ChangeVehicleInViewportToCoin();

	Stage *pTopStage = CUR_STAGE->GetTopStage();
	pTopStage->EndStopWatch();
	CUR_PLAYER->LockInput();
	CUR_PLAYER->HoldInput(INPUT_R);

	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlayBGM(L"HitGoalPost", false);
}

void GameStateHitGoalPost::Process()
{
	if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
	{
		if (m_nStateFrame == 450)
		{
			CUR_PLAYER->UnholdInput(INPUT_R);
		}

		if (m_nStateFrame >= 450)
		{
			if (CUR_PLAYER->m_nTime >= 10)
			{
				CUR_PLAYER->m_nTime -= 10;
				CUR_PLAYER->AddScore(500);
			}
			else if (CUR_PLAYER->m_nTime > 0)
			{
				CUR_PLAYER->m_nTime -= 1;
				CUR_PLAYER->AddScore(50);
			}

			PLAYSOUND(L"Score");

			if (CUR_PLAYER->m_nTime == 0)
			{
				m_nTimeStamp = m_nStateFrame;
			}
		}
		
		if (m_nStateFrame > 256 && m_nTimeStamp != -1)
		{
			if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
				m_pGame->ChangeState(GAMESTATE_SINGLESTAGECLEAR);
			else
			{
				// Let's load next stage
				m_pGame->LoadNextStage();
				m_pGame->InitPlayerObject();
				m_pGame->InitCamera();

				if (CUR_PLAYER->m_pProfile)
				{
					CUR_PLAYER->m_pProfile->SetLastStage(CUR_STAGE->m_strName);
					CUR_PLAYER->m_pProfile->AddSelectableStage(CUR_STAGE->m_strName);
					CUR_PLAYER->SaveUserProfile();
				}

				m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
				SOUND_MANAGER->StopBGM();
			}
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
}

void GameStateHitGoalPost::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
