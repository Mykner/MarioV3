#include "GameStateTimeUp.h"

#include "Player.h"
#include "Stage.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "RenderManager.h"

GameStateTimeUp::GameStateTimeUp(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_TIMEUP;
}

GameStateTimeUp::~GameStateTimeUp()
{
}

long GameStateTimeUp::GetBackgroundColor()
{
	return 0;
}

void GameStateTimeUp::Init()
{
	DeleteOldState();

	CUR_PLAYER->m_nTime = -1;
	SOUND_MANAGER->StopBGM();
}

void GameStateTimeUp::Process()
{
	CUR_PLAYER->OnBeforeProcess();
	CUR_PLAYER->Process();
	CUR_PLAYER->OnAfterProcess();

	if (KE(VK_SPACE))
		m_nStateFrame = 180;

	if (m_nStateFrame > 180)
	{
		if (CUR_PLAYER->m_nLife == 0)
		{
			if (m_pGame->m_nStagePackType == STAGEPACK_SINGLE)
				m_pGame->ChangeState(GAMESTATE_SINGLESTAGEGAMEOVER);
			else
				m_pGame->ChangeState(GAMESTATE_GAMEOVER);
		}
		else
		{
			CUR_PLAYER_OBJ->ChangeState(STATE_REBORN);
			m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
		}
	}
}

void GameStateTimeUp::Render()
{
	const int nWidth = m_pGame->Width();

	CUR_PLAYER->RenderStatus();

	DRAWTEXT((nWidth / 2) - 32, 128, L"TIME UP");
}
