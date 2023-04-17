#include "ChainChompStake.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ChainChomp.h"

ChainChompStake::ChainChompStake(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_CHAINCHOMPSTAKE;
	m_nState = STATE_IDLE;

	m_bFlip = false;
	m_nBaseState = STATE_STAND;

	m_pChainChomp = nullptr;
}

ChainChompStake::~ChainChompStake()
{
}

void ChainChompStake::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_PUSHEDUP:
		{
			m_fYS = -3.0f;
			nState = STATE_IDLE;
		}
		break;
	case STATE_IDLE:
	case STATE_FALL:
	case STATE_BURNT:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	case STATE_TRAMPOLINJUMP:
		EnemyObjectBase::ChangeState(STATE_BURNT, pInfo);
		return;
		break;
	case STATE_DEAD:
		if (m_pChainChomp)
		{
			m_pChainChomp->m_pStake = nullptr;
			m_pChainChomp = nullptr;
		}
		break;
	}

	m_nNextState = nState;
}

void ChainChompStake::OnChangeState(int nState)
{
	
}

void ChainChompStake::OnIdle()
{
	CheckFalling();
}

void ChainChompStake::OnLanding()
{
	m_fXS = 0;
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
		ChangeState(STATE_STAND);
}

bool ChainChompStake::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool ChainChompStake::CanLand()
{
	return true;
}

bool ChainChompStake::CanCollide(int nDir)
{
	return true;
}

bool ChainChompStake::CanBurnt(int nType)
{
	if (nType == BURN_THWOMPSMASH)
		return true;

	return false;
}

bool ChainChompStake::CanEat()
{
	return false;
}

bool ChainChompStake::CanDamageEnemy()
{
	return false;
}

bool ChainChompStake::CanPressed(int nPower)
{
	return false;
}

int ChainChompStake::GetSpriteIndex()
{
	return SPRIDX_CHAINCHOMP_STAKE;
}

int ChainChompStake::GetZOrder()
{
	switch (m_nState)
	{
	case STATE_BURNT:
		return Z_ENEMY;
	}

	return Z_MAP;
}

void ChainChompStake::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;
}

NaRect ChainChompStake::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();

	rc.left = m_fX - (s.cx / 2);
	rc.right = m_fX + (s.cx / 2) - 1;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	return rc;
}

SIZE ChainChompStake::GetSize()
{
	float fScaleX = m_fScaleX;
	float fScaleY = m_fScaleY;
	GetScale(fScaleX, fScaleY);

	SIZE s = 
	{ 
		16 * fScaleX, 
		16 * fScaleY 
	};
	return s;
}
