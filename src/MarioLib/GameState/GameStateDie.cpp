#include "GameStateDie.h"

#include "Player.h"
#include "Stage.h"
#include "SoundManager.h"
#include "InputManager.h"

GameStateDie::GameStateDie(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_DIE;

	m_Transaction.Init(pGame);
	m_nCurSubStageIndex = -1;
}

GameStateDie::~GameStateDie()
{
}

void GameStateDie::Init()
{
	DeleteOldState();

	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlaySoundEffect(L"Die");

	if (IS_ONLINESTAGE)
	{
		Stage *pTopStage = CUR_STAGE->GetTopStage();
		m_nCurSubStageIndex = pTopStage->GetSubStageIndex(CUR_STAGE);

		m_Transaction.m_pJobThread = new std::thread(UpdateDeathPositions, this);
		m_arDeathPosition = m_pGame->m_LastStageRecord.m_strDeathPositions.Split(L",");
	}
}

void GameStateDie::Process()
{
	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->OnAfterProcess();

	if (KE(VK_SPACE))
		m_nStateFrame = 300;

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();
		m_Transaction.ChangeJob(JOB_NONE);
	}

	if (IS_ONLINESTAGE && 
		m_nStateFrame < 300 && m_nStateFrame > 60 &&
		m_nStateFrame % 2 == 0)
	{
		if (m_arDeathPosition.GetCount() > 0)
		{
			while (true)
			{
				auto strPosition = m_arDeathPosition.Pop();
				auto arXY = strPosition.Split(L":");
				if (arXY.GetCount() == 3)
				{
					int i = arXY[0].ToInt();
					if (i != m_nCurSubStageIndex)
						continue;

					int x = arXY[1].ToInt();
					int y = arXY[2].ToInt();

					CreateParameter param;
					param.fScaleX = 0.3f;
					param.fScaleY = 0.3f;
					CUR_STAGE->CreateEffect(x, y, EFFECT_DEATHPOSITION, &param);
				}
				break;
			}
		}
	}

	if (m_nStateFrame > 300)
	{
		CUR_PLAYER->OnLostLife();
		if (CUR_PLAYER->m_nLife == 0)
		{
			if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
				m_pGame->ChangeState(GAMESTATE_SINGLESTAGEGAMEOVER);
			else
				m_pGame->ChangeState(GAMESTATE_GAMEOVER);
		}
		else if (CUR_PLAYER->m_nTime == 0)
		{
			m_pGame->PopAllStage();

			m_pGame->ChangeState(GAMESTATE_TIMEUP);
		}
		else
		{
			m_pGame->PopAllStage();

			CUR_PLAYER_OBJ->ChangeState(STATE_REBORN);
			m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
		}
	}

	CUR_STAGE->ProcessEffects();
}

void GameStateDie::Render()
{
	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth - 25, m_pGame->m_nHeight - 25, 0.5f);
	}

	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();
}

void GameStateDie::UpdateDeathPositions(void * pParam)
{
	GameStateDie *pThis = (GameStateDie *)pParam;

	auto strLastStage = pThis->m_pGame->m_strLastStage;
	auto strPositions = pThis->m_pGame->m_LastStageRecord.m_strDeathPositions;

	pThis->m_Transaction.BeginJob(JOB_UPDATE_DEATHPOSITION);

	ServerRequest req;
	try
	{
		NaString strAdd;

		strAdd.Format(L"%d:%d:%d",
			pThis->m_nCurSubStageIndex,
			(int)pThis->m_pGame->m_pCurPlayer->m_pControlObj->m_fX,
			(int)pThis->m_pGame->m_pCurPlayer->m_pControlObj->m_fY);

		if (strPositions.GetLength() > 0)
			strPositions += L",";

		strPositions += strAdd;
		pThis->m_pGame->m_LastStageRecord.m_strDeathPositions = strPositions;

		req.UpdateStageRecordInfo(strLastStage, strPositions);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
	}
}
