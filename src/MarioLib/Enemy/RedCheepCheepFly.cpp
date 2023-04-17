#include "RedCheepCheepFly.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

RedCheepCheepFly::RedCheepCheepFly(Game *pGame, Stage *pStage) :
	CheepCheepFly(pGame, pStage)
{
	m_nType = ENEMY_REDCHEEPCHEEP_FLY;
}

RedCheepCheepFly::~RedCheepCheepFly()
{
}

void RedCheepCheepFly::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_JUMP:
	default:
		CheepCheepFly::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

int RedCheepCheepFly::GetSpriteIndex()
{
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
