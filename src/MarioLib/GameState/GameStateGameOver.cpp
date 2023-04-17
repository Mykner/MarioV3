#include "GameStateGameOver.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "Sprite.h"
#include "UserProfile.h"

GameStateGameOver::GameStateGameOver(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_GAMEOVER;
}

GameStateGameOver::~GameStateGameOver()
{
}

long GameStateGameOver::GetBackgroundColor()
{
	return 0;
}

void GameStateGameOver::Init()
{
	DeleteOldState();

	CUR_PLAYER->LockInput();
	
	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlayBGM(L"GameOver", false);
}

void GameStateGameOver::Process()
{
	CUR_STAGE->ProcessEffects();

	if (KE(VK_SPACE))
		m_nStateFrame = 360;

	if (m_nStateFrame == 360)
	{
		CUR_PLAYER->m_nTime = -1;
		CUR_PLAYER->Reset();

		m_pGame->m_pCurStage->Clear();
		m_pGame->m_pCurStage->Load(m_pGame->m_strInitialStage);

		CUR_PLAYER_OBJ->SetState(STATE_STAND);
		CUR_PLAYER_OBJ->m_nSpriteIndex = CUR_PLAYER_OBJ->GetSpriteIndex();
		
		if (CUR_PLAYER->m_pProfile)
		{
			if (CUR_PLAYER->m_pControlObj)
				CUR_PLAYER->m_pControlObj->OnChangeShape(SHAPE_MINI);
			CUR_PLAYER->m_pProfile->Update(CUR_PLAYER);
			CUR_PLAYER->SaveUserProfile();
		}

		m_pGame->InitPlayerObject();
		m_pGame->InitCamera();
		m_pGame->ChangeState(GAMESTATE_TITLE);

		SOUND_MANAGER->StopBGM();

		DeleteOldState();
	}
}

void GameStateGameOver::Render()
{
	const int nWidth = m_pGame->Width();
	DRAWTEXT((nWidth / 2) - 40, 128, L"GAME OVER");
}
