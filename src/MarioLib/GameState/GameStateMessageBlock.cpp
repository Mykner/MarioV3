#include "GameStateMessageBlock.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "Camera.h"
#include "Sprite.h"

#include "InputManager.h"
#include "SpriteManager.h"
#include "RenderManager.h"
#include "FModManager.h"

GameStateMessageBlock::GameStateMessageBlock(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_MESSAGEBLOCK;

	m_strMessage = L"";
	m_strMessageDisplay = L"";
	m_nCurrentIndex = 0;
	m_nColumn = 0;
	m_nRow = 0;
	m_bWaitNextPage = false;

	m_pSprite = nullptr;
}

GameStateMessageBlock::~GameStateMessageBlock()
{
}

void GameStateMessageBlock::Init()
{
	m_strMessage = m_pGame->m_paramNextState.m_strOption;
	m_strMessage.ReplaceAll(L"\\n", L"\n");

	m_pSprite = SPR_MANAGER->Find(L"Etc");

	PLAYSOUND(L"MessageBlock");
}

void GameStateMessageBlock::Process()
{
	if (m_nStateFrame < 2)
	{
		CUR_STAGE->OnBeforeProcess();
		CUR_PLAYER->OnBeforeProcess();

		CUR_STAGE->Process();
		CUR_PLAYER->Process();

		CUR_STAGE->OnAfterProcess();
		CUR_PLAYER->OnAfterProcess();

	}

	bool bIncIdx = false;
	if (m_bWaitNextPage)
	{
		if (CUR_PLAYER->IsInputPressed(INPUT_F, false) || CUR_PLAYER->IsInputPressed(INPUT_J, false))
		{
			m_strMessageDisplay = L"";
			m_nRow = 0;
			m_nColumn = 0;
			m_bWaitNextPage = false;
		}
	}
	else if (m_nCurrentIndex == m_strMessage.GetLength())
	{
		if (CUR_PLAYER->IsInputPressed(INPUT_F, false) || CUR_PLAYER->IsInputPressed(INPUT_J, false))
		{
			PopState();
			delete this;

			return;
		}
	}
	else
	{
		if (CUR_PLAYER->IsInputHold(INPUT_F, false) || CUR_PLAYER->IsInputHold(INPUT_J, false))
			bIncIdx = true;
	}

	int nDelay = 5;
	if (m_strMessage[m_nCurrentIndex] == L'\n' ||
		m_strMessage[m_nCurrentIndex] == L'.')
		nDelay = 25;
	
	if (m_pGame->m_nGameFrame % nDelay == 0)
	{
		if (m_nCurrentIndex < m_strMessage.GetLength())
			bIncIdx = true;
	}

	if (bIncIdx && !m_bWaitNextPage)
	{
		m_strMessageDisplay.AppendFormat(L"%c", m_strMessage[m_nCurrentIndex]);

		if (m_strMessage[m_nCurrentIndex] == L'\n')
		{
			m_nColumn = 0;
			m_nRow++;
		}
		else if (m_nCurrentIndex != 0 && m_nColumn == 25)
		{
			m_strMessageDisplay += L"\n";
			m_nColumn = 0;
			m_nRow++;
		}
		else
			m_nColumn++;

		if (m_nRow == 8)
			m_bWaitNextPage = true;

		m_nCurrentIndex++;

		PLAYSOUND(L"Text");
	}

	CUR_STAGE->ProcessEffects();

	CAMERA->Process();
	CAMERA->ProcessPlayerBound();
}

void GameStateMessageBlock::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();
	CUR_PLAYER->RenderStatus();

	// 216, 70
	NaRect rc = { 0, 0, m_pGame->m_nWidth, m_pGame->m_nHeight };
	rc.left = (m_pGame->m_nWidth / 2) - (216 / 2);
	rc.top = 50;
	rc.right = rc.left + 216;
	rc.bottom = rc.top + 70;

	m_pSprite->RenderToQueue(rc.left, rc.top, SPRIDX_ETC_DOT, false, false,
		0xc0000000, rc.Width(), rc.Height(), Z_UI_BOX);

	RENDER_MANAGER->Text(rc.left + 4, rc.top + 4, m_strMessageDisplay);
}
