#include "GameStateCutBridge.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "GameObjectBase.h"
#include "BossBridge.h"
#include "SoundManager.h"
#include "BossBridgeAxe.h"
#include "Camera.h"

GameStateCutBridge::GameStateCutBridge(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_CUTBRIDGE;

	m_pBridge = nullptr;
	m_nTimeStamp = 0;
	m_nFallTime = 64;
}

GameStateCutBridge::~GameStateCutBridge()
{
}

void GameStateCutBridge::Init()
{
	DeleteOldState();

	Stage *pTopStage = CUR_STAGE->GetTopStage();
	pTopStage->EndStopWatch();
	SOUND_MANAGER->StopBGM();

	BEGIN_VECTOR_LOOP(GameObjectBase*, m_pGame->m_pCurStage->m_vecEventObject)
		if (pObj->m_nType == EVENTOBJECT_BOSSBRIDGE)
		{
			m_pBridge = (BossBridge*)pObj;
		}
		else if (pObj->m_nType == EVENTOBJECT_BOSSBRIDGE_AXE)
		{
			m_pAxe = (BossBridgeAxe*)pObj;
		}
	END_VECTOR_LOOP();
	
	NaDebugOut(L"Bridge: 0x%08x\n", m_pBridge);
	NaDebugOut(L"Axe: 0x%08x\n", m_pAxe);

	if (m_pBridge)
	{
		std::vector<GameObjectBase*> vecGameObjects;
		vecGameObjects.insert(vecGameObjects.end(), m_pGame->m_pCurStage->m_vecItem.begin(), m_pGame->m_pCurStage->m_vecItem.end());
		vecGameObjects.insert(vecGameObjects.end(), m_pGame->m_pCurStage->m_vecEnemy.begin(), m_pGame->m_pCurStage->m_vecEnemy.end());

		NaRect rcBridge = m_pBridge->GetRect();
		rcBridge.top -= 8;
		rcBridge.bottom = rcBridge.top + 8;
		BEGIN_VECTOR_LOOP(GameObjectBase*, vecGameObjects) {
			NaRect rcObj = pObj->GetRect();
			if (!pObj->IsAlive())
				continue;
			if (rcBridge.IsOverlapped(rcObj))
			{
				if (pObj == m_pBridge)
					continue;
				m_vecOverBridge.push_back(pObj);
			}
		} END_VECTOR_LOOP();
	}
	else
	{
		m_nTimeStamp = 1;
		m_nFallTime = 1;
	}
}

void GameStateCutBridge::Process()
{
	if (m_pBridge)
	{
		if (m_pAxe != nullptr && m_nStateFrame < 18)
		{
			m_pAxe->OnBeforeProcess();
			m_pAxe->Process();
			m_pAxe->OnAfterProcess();
		}

		if (m_nStateFrame == 18)
		{
			m_pBridge->m_bAnchor = false;

			//
			// #TODO Check Enemy count in camera
			// if count == 0 -> pass break bridge
			//
		
			m_pBridge->ChangeState(STATE_DIE);
		}
		
		if (!m_pBridge->m_bAnchor && m_pBridge->m_nLength > 0)
		{
			m_pBridge->OnBeforeProcess();
			m_pBridge->Process();
			m_pBridge->OnAfterProcess();

			if (m_pBridge->m_nLength == 0)
			{
				m_nTimeStamp = m_nStateFrame;

				BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecOverBridge) {
					pObj->m_fXS = 0;
					pObj->ChangeState(STATE_FALL);
				} END_VECTOR_LOOP();

				SOUND_MANAGER->PlaySoundEffect(L"BowserFall");
			}
		}
	}

	if (m_nTimeStamp > 0)
	{
		BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecOverBridge) {
			pObj->OnBeforeProcess();
			pObj->Process();
			if (pObj->m_nState == STATE_DEAD)
			{
				it = m_vecOverBridge.erase(it);
				continue;
			}
			pObj->OnAfterProcess();

			++it;
		} END_VECTOR_LOOP();

		// Fall time = 64 frame (m_nFallTime)
		if (m_nStateFrame == m_nTimeStamp + m_nFallTime)
		{
			ChangeEnemyInViewportToCoin();
			ChangeVehicleInViewportToCoin();

			CUR_PLAYER->LockInput();
			CUR_PLAYER->HoldInput(INPUT_R);

			SOUND_MANAGER->PlayBGM(L"CastleComplete", false);
		}
		else if (m_nStateFrame > m_nTimeStamp + m_nFallTime)
		{
			CUR_STAGE->OnBeforeProcess();
			CUR_PLAYER->OnBeforeProcess();

			CUR_STAGE->Process();
			CUR_PLAYER->Process();

			CUR_STAGE->OnAfterProcess();
			CUR_PLAYER->OnAfterProcess();

			if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
			{
				NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
				if (rcPlayer.right + 16 > CUR_STAGE->m_ptStopPoint.x)
				{
					CUR_STAGE->m_ptStopPoint.x = rcPlayer.right + 16;
				}
			}
			else
			{
				int nCurFrame = m_nStateFrame - (m_nTimeStamp + m_nFallTime);
				if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
				{
					if (nCurFrame == 256)
					{
						CUR_PLAYER->UnholdInput(INPUT_R);
						CUR_PLAYER->HoldInput(INPUT_L);
					}

					if (nCurFrame == 257)
						CUR_PLAYER->UnholdInput(INPUT_L);

					if (nCurFrame == 300)
						m_pGame->ChangeState(GAMESTATE_SINGLESTAGECLEAR);
				}
			}
		}
	}

	CUR_PLAYER->ProcessPSwitchTime();
	CUR_STAGE->ProcessEffects();

	CAMERA->Process();
	CAMERA->ProcessPlayerBound();
}

void GameStateCutBridge::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}
