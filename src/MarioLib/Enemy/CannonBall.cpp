#include "CannonBall.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

CannonBall::CannonBall(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_CANNONBALL;
	m_nState = STATE_IDLE;

	m_bFlip = true;
}

CannonBall::~CannonBall()
{
}

void CannonBall::OnBeforeProcess()
{
	CheckCollisionEnemy();
}

void CannonBall::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
		break;
	case STATE_PRESSED:
		EnemyObjectBase::ChangeState(nState, pInfo);
		m_bFlipV = false;
		return;
		break;
	case STATE_PUSHEDUP:		
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void CannonBall::OnChangeState(int nState)
{
}

void CannonBall::OnIdle()
{
	ChangeState(STATE_THROWN);
}

void CannonBall::OnThrown()
{
	if (m_nStateFrame == 8)
		ChangeState(STATE_WALK);
}

void CannonBall::OnThrownFly()
{
	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);
}

void CannonBall::OnWalk()
{
	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);
}

void CannonBall::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool CannonBall::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_FIREBALL:
	case BURN_BOWSERFIRE:
		return false;
		break;
	case BURN_TURTLESHELL:
	case BURN_YOSHIFIRE:
	case BURN_STARMAN:
		return true;
		break;
	}

	return false;
}

bool CannonBall::CanBounceTrampolinMiniHorz()
{
	return false;
}

int CannonBall::GetSpriteIndex()
{
	return SPRIDX_CANNONBALL;
}
