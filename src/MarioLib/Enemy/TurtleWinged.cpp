//
// TurtleWinged is deprecated!
//
/*
#include "TurtleWinged.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

TurtleWinged::TurtleWinged(Game *pGame, Stage *pStage) :
	Turtle(pGame, pStage)
{
	m_nType = ENEMY_TURTLE_WINGED;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_bWinged = true;

	m_nSpriteIndex = 0;
}

TurtleWinged::~TurtleWinged()
{
}

void TurtleWinged::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_JUMP:
		m_fYS = -4.0f;
		break;
	default:
		Turtle::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void TurtleWinged::RenderDesign(int nColor, int nZOrder)
{
	Render(nColor);
	
// 	NaPointT<float> pt = CAMERA->GetPosition();
// 	int _x = x - pt.x;
// 	int _y = y - pt.y;
// 
// 	int nFrame = GetSpriteIndex();
// 	int nZOrder = GetZOrder();
// 	if (nFrame != -1)
// 	{
// 		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame, m_bFlip, m_bFlipV, nColor,
// 			m_fScaleX, m_fScaleY, nZOrder);
// 	}

}

void TurtleWinged::OnWalk()
{
	if (m_bWinged)
	{
		ChangeState(STATE_JUMP);
	}
	else
	{
		EnemyObjectBase::OnWalk();
	}
}

void TurtleWinged::OnJump()
{
	CheckHitBlock(0);

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();
		
		m_fYS += 0.15f;
		if (m_fYS > 4.0f)
			m_fYS = 4.0f;
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;

			if (m_fXS != 0)
				ChangeState(STATE_WALK);
			else
				ChangeState(STATE_STAND);
		}
	}
	
	CheckReachWall();

	if (m_bReachFriend)
		OnReachFriend();
}

int TurtleWinged::GetSpriteIndex()
{
	int nFrame = m_nSpriteIndex;

	switch (m_nState)
	{
	case STATE_IDLE:
	case STATE_JUMP:
	case STATE_FALL:
	case STATE_WALK:
		if (m_bWinged)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_TURTLE_WINGED1, 2, 8);
		else
			nFrame = Turtle::GetSpriteIndex();
		break;
	case STATE_HELDBYTONGUE:
		if (nFrame == 0)
			nFrame = SPRIDX_TURTLE_SHELL1;
		break;
	default:
		nFrame = Turtle::GetSpriteIndex();
		break;	
	}

	m_nSpriteIndex = nFrame;

	return nFrame;
}
*/