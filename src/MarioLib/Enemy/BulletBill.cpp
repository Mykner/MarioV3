#include "BulletBill.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

BulletBill::BulletBill(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_BULLETBILL;
	m_nState = STATE_IDLE;

	m_bFlip = true;
}

BulletBill::~BulletBill()
{
}

void BulletBill::OnBeforeProcess()
{
	CheckCollisionEnemy();
}

void BulletBill::ChangeState(int nState, ChangeStateInfo *pInfo)
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

void BulletBill::OnChangeState(int nState)
{
	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -1.5f;
		else
			m_fXS = 1.5f;
		break;
	}
}

void BulletBill::OnIdle()
{
	ChangeState(STATE_THROWN);
}

void BulletBill::OnThrown()
{
	if (m_nStateFrame == 8)
		ChangeState(STATE_WALK);
}

void BulletBill::OnWalk()
{
	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);
}

void BulletBill::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool BulletBill::CanBurnt(int nType)
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

bool BulletBill::CanBounceTrampolinMiniHorz()
{
	return false;
}

bool BulletBill::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

int BulletBill::GetSpriteIndex()
{
	return SPRIDX_BULLETBILL;
}

int BulletBill::GetZOrder()
{
	if (m_nState == STATE_WALK && m_nStateFrame < 16)
		return Z_BEHIND_MAP;

	if (m_nState == STATE_THROWN && m_nStateFrame < 16)
		return Z_BEHIND_MAP;

	return Z_BULLET;
}
