#include "GameStateGoal.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "GoalPoint.h"
#include "FModManager.h"
#include "EventObjectBase.h"
#include "UserProfile.h"

GameStateGoal::GameStateGoal(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_GOAL;

    m_pCastle = nullptr;
    
	m_nFireworksCount = 0;
	m_nFireworksFired = 0;
	m_nTimeStamp = 0;
}

GameStateGoal::~GameStateGoal()
{
}

void GameStateGoal::Init()
{
	DeleteOldState();

	Stage *pTopStage = CUR_STAGE->GetTopStage();
	pTopStage->EndStopWatch();
	CUR_PLAYER->LockInput();
	CUR_PLAYER->HoldInput(INPUT_R);

	// Calc BoomCount
	switch (CUR_PLAYER->m_nTime % 10)
	{
	case 1:
	case 3:
	case 6:
		m_nFireworksCount = CUR_PLAYER->m_nTime % 10;
		break;
	default:
		m_nFireworksCount = 0;
		break;
	}

	// Find Castle
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEventObject.begin();
	for (; it != CUR_STAGE->m_vecEventObject.end(); ++it)
	{
		EventObjectBase *pEvent = (EventObjectBase*)*it;
		if (pEvent->m_nType == EVENTOBJECT_GOALPOINT && pEvent->CanApplied())
		{
			m_pCastle = *it;
			break;
		}
	}
}

void GameStateGoal::Process()
{
	if (KE(VK_SPACE))
	{
		CUR_PLAYER->AddScore(CUR_PLAYER->m_nTime * 50);
		CUR_PLAYER->m_nTime = 0;

		m_nStateFrame = 900;
	}

	if (m_nStateFrame == 16)//24)
	{
		CUR_PLAYER->UnholdInput(INPUT_R);

		CUR_PLAYER_OBJ->ChangeState(STATE_STAND);
	}

	if (CUR_PLAYER->m_nTime > 0)
	{
		if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
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
		}
		else
		{
			if (CUR_PLAYER->m_nTime > 0)
			{
				CUR_PLAYER->m_nTime -= 1;
				CUR_PLAYER->AddScore(50);
			}
		}

		PLAYSOUND(L"Score");

		if (CUR_PLAYER->m_nTime == 0)
		{
			m_nTimeStamp = m_nStateFrame;
			if (m_pCastle)
				m_pCastle->ChangeState(STATE_FLAGUP);
		}
	}
	else
	{
		if (m_nFireworksCount > 0 && m_nFireworksFired < m_nFireworksCount)
		{
			if ((m_nStateFrame - m_nTimeStamp) % 30 == 0)
			{
				FireBoom();
			}
		}

		int nDelayBeforeStateChange;
		if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
			nDelayBeforeStateChange = 210;
		else if (m_pGame->m_nStagePackType == STAGEPACK_CUSTOM)
			nDelayBeforeStateChange = 210;
		else
			nDelayBeforeStateChange = 16;

		if ((m_nStateFrame >= 120 + m_nTimeStamp + (m_nFireworksCount * 30) &&
			!FMOD_MANAGER->IsPlayingBGM()) || 
			(m_nStateFrame >= 120 + m_nTimeStamp + (nDelayBeforeStateChange)))
		{
			if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
			{
				// Stage Clear!
				GameStateChangeParameter param;
				param.m_strOption = L"Castle";
				m_pGame->ChangeState(GAMESTATE_SINGLESTAGECLEAR, &param);
			}
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

void GameStateGoal::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();

	if (m_pGame->m_bModernStyle)
	{
		CUR_PLAYER->RenderControlObj();
	}
	else if (m_nStateFrame < 8)
	{
		CUR_PLAYER->RenderControlObj();
	}

	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}

void GameStateGoal::FireBoom()
{
	int nBoomX[6] = { 126, 94, 190, 190, 142, 94 };
	int nBoomY[6] = { 53, 101, 69, 118, 69, 101 };

	CreateParameter param;
	param.nType = EFFECT_FIREWORKS;
	CUR_STAGE->CreateEffect(
		CAMERA->m_ptPos.x + nBoomX[m_nFireworksFired],
		CAMERA->m_ptPos.y + nBoomY[m_nFireworksFired],
		EFFECT_EXPLOSION,
		&param
	);
	m_nFireworksFired++;
}
