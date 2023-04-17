#include "GameStateRescuePrincess.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "GoalPoint.h"
#include "SoundManager.h"
#include "UserProfile.h"
#include "UIManager.h"
#include "UIPanel.h"

GameStateRescuePrincess::GameStateRescuePrincess(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_RESCUEPRINCESS;

	m_nTimeStamp = 0;
	m_pPrincess = nullptr;
	m_nStopPointX = 0;
	m_bThankWordComplete = false;
	m_bUseCustomMessage = false;
}

GameStateRescuePrincess::~GameStateRescuePrincess()
{
}

void GameStateRescuePrincess::Init()
{
	DeleteOldState();

	BEGIN_VECTOR_LOOP(GameObjectBase*, m_pGame->m_pCurStage->m_vecEventObject)
		if (pObj->m_nType == EVENTOBJECT_PRINCESS)
		{
			m_pPrincess = (Princess*)pObj;

			int nPixelPerPage = GameDefaults::nPageWidth;
			m_nStopPointX = ((((int)m_pPrincess->m_fX / nPixelPerPage) + 1) * nPixelPerPage);
			break;
		}
	END_VECTOR_LOOP();

	UIPanel *pPanel = UI_MANAGER->AddPanel(m_pGame->Width() - 256, 80);
	pPanel->m_bAutoSize = false;
	pPanel->m_rc.left = m_pGame->Width() - 256;
	pPanel->m_rc.top = 80;
	pPanel->m_rc.right = 256;
	pPanel->m_rc.bottom = 256;

	if (m_pPrincess)
	{
		if (m_pPrincess->m_strThankWord.GetLength() > 0)
		{
			NaString strCustomMessage = m_pPrincess->m_strThankWord;
			
			m_arCustomMessage = strCustomMessage.Split(L"\\n");
			m_bUseCustomMessage = true;
		}
	}

	Stage *pTopStage = CUR_STAGE->GetTopStage();
	pTopStage->EndStopWatch();
	CUR_PLAYER->LockInput();
	CUR_PLAYER->UnholdInput(INPUT_R);
}

void GameStateRescuePrincess::Process()
{
	if (KE(VK_SPACE))
	{
		CUR_PLAYER->AddScore(CUR_PLAYER->m_nTime * 50);
		CUR_PLAYER->m_nTime = 0;

		if (m_nTimeStamp == 0)
		{
			m_nTimeStamp = m_nStateFrame;
		}
		SOUND_MANAGER->StopBGM();
	}

	if (CUR_STAGE->m_ptStopPoint.x < m_nStopPointX)
	{
		CUR_STAGE->m_ptStopPoint.x++;
	}
	
	if (CAMERA->m_ptPos.x + m_pGame->Width() < m_nStopPointX)
	{
		CAMERA->m_ptPos.x++;

		// #TODO Improvement!
		/*
		if (CAMERA->m_ptPos.x + GameDefaults::nTilePerScreenWidth * TILE_XS ==
			CUR_STAGE->m_ptStopPoint.x)
			m_nTimeStamp = m_nStateFrame;
		*/
	}
	else if (m_nTimeStamp == 0)
	{
		m_nTimeStamp = m_nStateFrame;
	}

	if (m_nTimeStamp == 0 && m_nStateFrame >= 256)
	{
		m_nTimeStamp = m_nStateFrame;
	}

	if (m_nTimeStamp != 0)
	{
		ProcessThankWord(m_nStateFrame - m_nTimeStamp);

		if (m_bThankWordComplete)
		{
			if (m_pPrincess->m_bIsReal)
			{
				m_pGame->ChangeState(GAMESTATE_GAMEOVER);
				UI_MANAGER->Clear();
			}
			else
			{
				if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
				{
					GameStateChangeParameter param;
					param.m_strOption = L"Princess";
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
					UI_MANAGER->Clear();
				}
			}
		}
	}

	CUR_STAGE->OnBeforeProcess();
	CUR_PLAYER->OnBeforeProcess();

	CUR_STAGE->Process();
	CUR_PLAYER->Process();

	CUR_STAGE->OnAfterProcess();
	CUR_PLAYER->OnAfterProcess();

	CUR_STAGE->ProcessEffects();
}

void GameStateRescuePrincess::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();

	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();

	UI_MANAGER->Render();
}

void GameStateRescuePrincess::ProcessThankWord(int nStateFrame)
{
	if (m_bUseCustomMessage)
	{
		if (nStateFrame % 64 == 0)
		{
			int nIndex = nStateFrame / 64;
			if (nIndex < m_arCustomMessage.GetCount())
			{
				UI_MANAGER->CreateText(-1, 0, 0 + (nIndex * 16), m_arCustomMessage[nIndex]);
			}
			else
			{
				int nCnt = m_arCustomMessage.GetCount();
				if (m_pPrincess->m_bIsReal)
				{
					if (nIndex == nCnt + 2)
					{
						if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
						{
							GameStateChangeParameter param;
							param.m_strOption = L"Princess";
							m_pGame->ChangeState(GAMESTATE_SINGLESTAGECLEAR, &param);
						}
					}
					if (nIndex == nCnt + 5)
					{
						SOUND_MANAGER->StopBGM();
						SOUND_MANAGER->PlayBGM(L"Ending", false);
					}
					if (nIndex >= nCnt + 7)
					{
						if (!m_pGame->m_pFMod->IsPlayingBGM())
							m_bThankWordComplete = true;
					}
				}
				else
				{
					if (nIndex == nCnt + 2)
					{
						if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
							m_bThankWordComplete = true;
					}
					if (nIndex == nCnt + 3)
					{
						m_bThankWordComplete = true;
					}
				}
			}
		}
	}
	else
	{
		if (m_pPrincess->m_bIsReal)
		{
			switch (nStateFrame)
			{
			case 0:
				UI_MANAGER->CreateText(-1, 64, 0, L"THANK YOU MARIO!");
				break;
			case 128:
				UI_MANAGER->CreateText(-1, 56, 32, L"YOUR QUEST IS OVER.");
				break;
			case 240:
				if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
				{
					GameStateChangeParameter param;
					param.m_strOption = L"Princess";
					m_pGame->ChangeState(GAMESTATE_SINGLESTAGECLEAR, &param);
				}
				break;
			case 256:
				UI_MANAGER->CreateText(-1, 48, 48, L"ANOTHER QUEST COMING");
				UI_MANAGER->CreateText(-1, 110, 64, L"SOON...");
				break;
			case 512:
				SOUND_MANAGER->StopBGM();
				SOUND_MANAGER->PlayBGM(L"Ending", false);
				break;
			}

			if (nStateFrame >= 512 + 128)
			{
				if (!m_pGame->m_pFMod->IsPlayingBGM())
					m_bThankWordComplete = true;
			}
		}
		else
		{
			switch (nStateFrame)
			{
			case 0:
				UI_MANAGER->CreateText(-1, 64, 0, L"THANK YOU MARIO!");
				break;
			case 128:
				UI_MANAGER->CreateText(-1, 40, 32, L"BUT OUR PRINCESS IS IN");
				UI_MANAGER->CreateText(-1, 40, 48, L"ANOTHER CASTLE!");
				break;
			case 240:
				if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
					m_bThankWordComplete = true;
				break;
			case 256:
				m_bThankWordComplete = true;
				break;
			}
		}
	}
}

