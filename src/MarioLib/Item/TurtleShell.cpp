#include "TurtleShell.h"

#include "Game.h"
#include "Stage.h"

TurtleShell::TurtleShell(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_TURTLESHELL;
	m_nState = STATE_ITEMPOP;

	m_nBaseState = STATE_STAND;
}

TurtleShell::~TurtleShell()
{
}

void TurtleShell::OnThrown()
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

			ChangeState(STATE_STAND);
		}
	}

	CheckReachWall();

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

int TurtleShell::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return SpriteIndexItem::SPRIDX_BLUETURTLE_SHELL;
}
