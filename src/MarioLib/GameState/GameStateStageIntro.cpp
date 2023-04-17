#include "GameStateStageIntro.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "Sprite.h"
#include "SpeechBubble.h"

GameStateStageIntro::GameStateStageIntro(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_STAGEINTRO;

	m_pLifeObject = nullptr;
}

GameStateStageIntro::~GameStateStageIntro()
{
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecLifeObject);

	delete m_pLifeObject;
}

long GameStateStageIntro::GetBackgroundColor()
{
	return 0;
}

void GameStateStageIntro::Init()
{
	//m_pGame->InitPlayerObject();
	// #TODO ResetPlayerObject Position
	{
		GameObjectBase *pObj = m_pGame->m_pCurPlayer->GetLastControlObject();
		pObj->ChangeState(STATE_STAND);
		pObj->SetPosition(0, 0);
	}
	
	// #TODO ResetCamera
	CAMERA->m_ptPos.x = 0;
	CAMERA->m_ptPos.y = 0;

	CUR_PLAYER->m_nTime = -1;
	CUR_PLAYER->LockInput();

	int nType = CUR_PLAYER_OBJ->m_nType;
	int nScreenWidth = m_pGame->Width();

	m_pLifeObject = GameObjectBase::CreateInstance(m_pGame, CUR_STAGE, nType);
	m_pLifeObject->m_fX = nScreenWidth / 2 - 24;
	m_pLifeObject->m_fY = 121;
	m_pLifeObject->m_nState = STATE_STAND;
	m_pLifeObject->RequestSprites(CUR_STAGE);

	if (m_pOldState &&
		(m_pOldState->m_nType == GAMESTATE_DIE ||
			m_pOldState->m_nType == GAMESTATE_TIMEUP))
	{
		CUR_STAGE->m_nTryCount++;
	}

	if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
	{
		m_pLifeObject->m_fX = nScreenWidth / 2;
		m_pLifeObject->m_fY = 120;
		m_pLifeObject->m_fScaleX = 2.0f;
		m_pLifeObject->m_fScaleY = 2.0f;

		int nLife = CUR_PLAYER->m_nLife - 1;
		int nRow = 0;
		while (nLife > 0)
		{
			for (int i=0; i<10 && nLife > 0; i++)
			{
				GameObjectBase* pObj = GameObjectBase::CreateInstance(m_pGame, CUR_STAGE, nType);

				int nTargetX = ((nScreenWidth / 2) - 72) + ((i % 10) * 16.0f);
				if (CUR_STAGE->m_nTryCount == 0)
					pObj->m_fX = 0 - (rand() % 128);
				else
					pObj->m_fX = nTargetX;
				pObj->m_fY = 148.0f + (nRow * 20);
				pObj->SetState(STATE_WALK);
				pObj->m_nStateFrame = rand() % 10;

				pObj->m_pController = CUR_PLAYER;
				pObj->RequestSprites(CUR_STAGE);

				m_vecLifeObject.push_back(pObj);
				--nLife;
			}
			if (nLife == 0)
				break;
			nRow++;
		}

		CUR_PLAYER->HoldInput(INPUT_F);
		CUR_PLAYER->HoldInput(INPUT_R);
	}

	SOUND_MANAGER->StopBGM();
}

void GameStateStageIntro::Process()
{
	CUR_STAGE->ProcessEffects();

	if (KE(VK_SPACE))
		m_nStateFrame = 150;

	// LifeObjects
	{
		int nWidth = m_pGame->Width();
		int i = 0;
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecLifeObject) {
			pObj->m_bCollision[COLLISION_BOTTOM] = true;
			pObj->m_ptCollision[COLLISION_BOTTOM] = { pObj->m_fX, pObj->m_fY };

			if (pObj->m_nState == STATE_WALK)
			{
				if (pObj->m_nStateFrame % 10 == 9)
				{
					SOUND_MANAGER->PlaySoundEffect(L"ShoeStep");
				}
				pObj->OnWalk();

				int nTargetX = ((nWidth / 2) - 72) + ((i % 10) * 16.0f);
				if (pObj->m_fX >= nTargetX)
				{
					pObj->m_fX = nTargetX;
					pObj->ChangeState(STATE_STAND);
				}
			}

			pObj->OnAfterProcess();
			i++;
		} END_VECTOR_LOOP();
	}

	if (m_nStateFrame == 150)
	{
		CUR_PLAYER->m_nDoorKeyCoin = 0;
		CUR_PLAYER->m_nPSwitchTime = 0;
		CUR_PLAYER->m_nTime = 400;

		if (m_pOldState &&
			(m_pOldState->m_nType == GAMESTATE_DIE ||
			m_pOldState->m_nType == GAMESTATE_TIMEUP))
		{
			CUR_PLAYER->UnholdInputAll();

			int nTryCount = CUR_STAGE->m_nTryCount;

			NaString strLastCheckPoint = CUR_STAGE->m_strLastCheckPoint;
			POINT ptLastCheckPoint = CUR_STAGE->m_ptLastCheckPoint;

			CUR_STAGE->Clear();
			CUR_STAGE->Load(m_pGame->m_pCurStage->m_strName);
			CUR_STAGE->m_nTryCount = nTryCount;

			CUR_PLAYER->m_nTime = CUR_STAGE->m_nTime;
			CUR_STAGE->m_strLastCheckPoint = strLastCheckPoint;
			CUR_STAGE->m_ptLastCheckPoint = ptLastCheckPoint;

			if (strLastCheckPoint.Compare(L"") != 0)
			{
				m_pGame->m_paramNextState.m_strOption = strLastCheckPoint;
				m_pGame->LoadSubStage();
				CUR_STAGE->InitTheme();
			}

			m_pGame->InitPlayerObject(ptLastCheckPoint);
			m_pGame->InitCamera();
			m_pGame->ChangeState(GAMESTATE_PLAY);

			if (ptLastCheckPoint.x != -1 && 
				m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
				ClearEnemyInViewport();

			CUR_STAGE->BeginStopWatch();
			CUR_STAGE->PlayBGM();
		}
		else
		{
			CUR_PLAYER->UnholdInputAll();
			CUR_STAGE->BeginStopWatch();

			CUR_PLAYER->m_nTime = CUR_STAGE->m_nTime;

			bool bInitPlayer = true;

			switch (CUR_STAGE->m_nEntranceType)
			{
			default:
			case STAGEENTRANCE_NONE:
				m_pGame->ChangeState(GAMESTATE_PLAY);

				CUR_STAGE->PlayBGM();
				break;
			case STAGEENTRANCE_UNDERGROUND:
				{
					m_pGame->LoadEntranceStage();
					m_pGame->InitPlayerObject();
					m_pGame->InitCamera();

					m_pGame->ChangeState(GAMESTATE_ENTERSTAGE);

					CUR_STAGE->PlayBGM();
				}
				break;
			case STAGEENTRANCE_FROMPIPE:
				{
					// #TODO pStage->GetFirstPipe()
					//pStage->GetFirstPipe();

					PipeExitInfo *pPipeExitInfo = FindPipeExit(0);
					if (pPipeExitInfo)
					{
						MovePlayerObjectToExit(pPipeExitInfo);
						bInitPlayer = false;

						// Trick for first render frame.
						CUR_PLAYER_OBJ->OnAfterProcess();
						CUR_PLAYER_OBJ->m_fY -= CUR_PLAYER_OBJ->m_fYS;

						m_pGame->ChangeState(GAMESTATE_EXITPIPE);
					}
					else
					{
						m_pGame->ChangeState(GAMESTATE_PLAY);
					}

					CUR_STAGE->PlayBGM();
				}
				break;
			}

			if (bInitPlayer)
				m_pGame->InitPlayerObject();
			m_pGame->InitCamera();
		}

		DeleteOldState();	
	}
}

void GameStateStageIntro::Render()
{
	// Render Stage Info
	int nWidth = m_pGame->Width();
	NaString str;

	if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
	{
		str.Format(L"WORLD %ls", CUR_STAGE->m_strName.wstr());
		DRAWTEXT((nWidth / 2) - 40, 80, str);

		// Display Lifes
		m_pLifeObject->Render();

		DRAWTEXT((nWidth / 2) - 8, 111, L"*");

		str.Format(L"%d", CUR_PLAYER->m_nLife);
		DRAWTEXT((nWidth / 2) + 15, 111, str);

		CUR_PLAYER->RenderStatus();
	}
	else
	{
		NaString strName = CUR_STAGE->m_strTitle;
		if (strName.GetLength() == 0)
			strName = CUR_STAGE->m_strName;
		int nIdx = strName.Find(L"\\");
		if (nIdx > 0)
		{
			strName = strName.Mid(nIdx + 1);
		}
		
		str.Format(L"%ls", strName.wstr());
		RENDER_MANAGER->Text(
			(nWidth / 2) - (str.GetLength() * (4 * 1.5f)), 36,
			str, 0xffffffff, 1.5f, Z_TEXT);

		str.Format(L"Created by %ls", CUR_STAGE->m_strCreator.wstr());
		RENDER_MANAGER->Text(
			(nWidth / 2) - (str.GetLength() * (4 * 1.0f)),
			56, 
			str, 0xff909090, 1.0f, Z_TEXT);

		// Display Lifes
		m_pLifeObject->Render();

		/*
		if (m_nStateFrame > 100)
		{
			SpeechBubble b(m_pGame);
			b.RenderWithTextAutoSized(
				m_pLifeObject->m_fX + 8,
				m_pLifeObject->m_fY - m_pLifeObject->GetSize().cy - 1,
				L"Let's go!"
			);
		}
		*/

		str.Format(L"*");
		DRAWTEXT((nWidth / 2) + 16, 111, str);

		str.Format(L"%d", CUR_PLAYER->m_nLife);
		RENDER_MANAGER->Text((nWidth / 2) + 24, 103, str, 0xffffffff, 2.0f, Z_TEXT);

		// LifeObjects
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecLifeObject) {
			pObj->Render();
		} END_VECTOR_LOOP();

#if defined(_DEBUG)
		/*
		str.Format(L"Try: %d", CUR_STAGE->m_nTryCount);
		RENDER_MANAGER->Text(0, 0, str);
		*/
#endif
	}	
}
