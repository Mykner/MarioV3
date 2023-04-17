#include "StaticCoin.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"

StaticCoin::StaticCoin(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_COIN;
	m_nState = STATE_IDLE;
}

StaticCoin::~StaticCoin()
{
}

void StaticCoin::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch(nState)
	{
	case STATE_PUSHEDUP:
		if (pInfo && pInfo->pFromObject)
		{
			if (pInfo->pFromObject->m_fXS != 0)
				m_fXS = pInfo->pFromObject->m_fXS / abs(pInfo->pFromObject->m_fXS);
			m_fYS = -3.0f;

			if (pInfo->pFromObject->m_pController)
			{
				CreateParameter param;
				param.pFromPlayer = pInfo->pFromObject->m_pController;
				CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY - 16, EFFECT_COIN, &param);
			}
		}
		break;
	default:
		ItemObjectBase::ChangeState(nState, pInfo);
		return;
	}

	m_nNextState = nState;
}

void StaticCoin::Process()
{
	switch (m_nState)
	{
	case STATE_PUSHEDUP:
		OnPushedUp();
		break;
	}

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
		SetState(STATE_DEAD);
	}
}

void StaticCoin::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void StaticCoin::OnPushedUp()
{
	CUR_PLAYER_OBJ->OnApplyItem(this);
	SetState(STATE_DEAD);
}

bool StaticCoin::CanApplied()
{
	if (m_nState == STATE_DEAD)
		return false;

	if (m_nState == STATE_HELDBYTONGUE)
		return false;

	return true;
}

int StaticCoin::GetSpriteIndex()
{
	// 24 8 8 -- 8

	int nVal = (m_pGame->m_nGameFrame % 48);
	if (nVal < 24)
		return SPRIDX_COIN;
	else if (nVal < 32)
		return SPRIDX_COIN2;
	else if (nVal < 40)
		return SPRIDX_COIN3;
	else if (nVal < 48)
		return SPRIDX_COIN2;

	return SPRIDX_COIN;
}
