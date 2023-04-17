#include "FireFlower.h"

#include "Game.h"
#include "Stage.h"

FireFlower::FireFlower(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_FIREFLOWER;
	m_nState = STATE_ITEMPOP;

	m_nBaseState = STATE_STAND;
}

FireFlower::~FireFlower()
{
}

void FireFlower::OnChangeState(int nState)
{
	switch(nState)
	{
	case STATE_STAND:
		m_fXS = 0;
		break;
	default:
		ItemObjectBase::OnChangeState(nState);
		break;
	}
}

void FireFlower::OnThrown()
{
	if (m_nStateFrame < 8)
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;

			if (m_bStandOnJumpBlock)
				ChangeState(STATE_BLOCKJUMP);
			else if (abs(m_fXS) <= 0.1f)
			{
				m_fXS = 0;
				ChangeState(STATE_STAND);
			}
			else if (m_fXS > 0)
				m_fXS -= 0.1f;
			else if (m_fXS < 0)
				m_fXS += 0.1f;
		}
	}

	CheckReachWall();

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

int FireFlower::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return 4 + (m_nStateFrame % 8 / 2);
}
