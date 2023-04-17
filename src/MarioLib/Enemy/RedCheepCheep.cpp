#include "RedCheepCheep.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

RedCheepCheep::RedCheepCheep(Game *pGame, Stage *pStage) :
	CheepCheep(pGame, pStage)
{
	m_nType = ENEMY_REDCHEEPCHEEP;
	m_nState = STATE_IDLE;

	m_bFlip = true;
}

RedCheepCheep::~RedCheepCheep()
{
}

void RedCheepCheep::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_SWIM:
		if (m_bFlip)
			m_fXS = -0.25f;
		else
			m_fXS = 0.25f;
		m_fXS *= 2.0f;
		break;
	case STATE_JUMP:
		if (m_nJump == 1)
			m_nJump = 2;
		CheepCheep::ChangeState(nState, pInfo);
		return;
		break;
	case STATE_PUSHEDUP:
	case STATE_PRESSED:		
	case STATE_BURNT:
	default:
		CheepCheep::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

int RedCheepCheep::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_REDCHEEPCHEEP1;
		break;
	default:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_REDCHEEPCHEEP1, 2, 8);
		break;
	}

	return nFrame;
}
