#include "ServerTransaction.h"

#include "Sprite.h"
#include "Game.h"
#include "SpriteManager.h"
#include "SpriteIndex.h"

ServerTransaction::ServerTransaction()
{
}

ServerTransaction::~ServerTransaction()
{
	if (m_bWorking == false)
	{
		DeleteThread();
	}
}

void ServerTransaction::Init(Game *pGame)
{
	m_pGame = pGame;

	m_bWorking = false;
	m_pJobThread = nullptr;
	m_nJobType = JOB_NONE;
	m_pSpriteEtc = SPR_MANAGER->Find(L"Etc");
}

bool ServerTransaction::IsWorking()
{
	return m_bWorking;
}

bool ServerTransaction::IsJobFinished()
{
	return (!m_bWorking && m_pJobThread != nullptr);
}

void ServerTransaction::DeleteThread()
{
	if (m_pJobThread != nullptr)
	{
		m_pJobThread->join();
		delete m_pJobThread;
		m_pJobThread = nullptr;
	}
}

void ServerTransaction::BeginJob(int nJobType)
{
	m_bWorking = true;
	m_nJobType = nJobType;
}

void ServerTransaction::ChangeJob(int nJobType)
{
	m_nJobType = nJobType;
}

void ServerTransaction::EndJob()
{
	m_bWorking = false;
}

void ServerTransaction::Render(int x, int y, float fScale)
{
	if (m_pSpriteEtc)
	{
		m_pSpriteEtc->RenderToQueue(
			x, y,
			SPRIDX_ETC_UI_LOADING_INDICATOR,
			false, false, -1L, fScale, fScale, Z_SCENE_EFFECT, false,
			(m_pGame->m_nGameFrame % 360) * 15
		);
	}
	else
	{
		NaDebugOut(L"Error: Sprite is not loaded: SpriteEtc\n");
	}
}
