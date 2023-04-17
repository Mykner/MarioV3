//
// RedTurtleWingedPatrol is deprecated!
//
/*
#include "RedTurtleWingedPatrol.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(RedTurtleWingedPatrol)
	PROP_INT("MoveDistance", VT_I4, 104, nullptr, "RedTurtleWingedPatrol"),
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Turtle"),
END_IMPL_PROPERTYMAP()

RedTurtleWingedPatrol::RedTurtleWingedPatrol(Game *pGame, Stage *pStage) :
	RedTurtle(pGame, pStage)
{
	m_nType = ENEMY_REDTURTLE_WINGED_PATROL;
	m_nState = STATE_IDLE;

	m_nSpriteIndex = 0;

	m_bFlip = true;
	m_bWinged = true;
	m_nDistance = 104;
}

RedTurtleWingedPatrol::~RedTurtleWingedPatrol()
{
}

void RedTurtleWingedPatrol::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	RedTurtle::ChangeState(nState, pInfo);
}

void RedTurtleWingedPatrol::RenderDesign(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	int nFrame = GetSpriteIndex();
	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = (m_pGame->m_bModernStyle);

		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame, m_bFlip, m_bFlipV, nColor,
			m_fScaleX, m_fScaleY, nZOrder, bShadow);

		(*m_ppSprite)->RenderToQueue(_x, _y + m_nDistance, nFrame, m_bFlip, m_bFlipV, COLOR_ARGB(150, 255, 255, 255),
			m_fScaleX, m_fScaleY, nZOrder);

		Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
		SIZE s = GetSize();

		pSpriteDesign->RenderToQueue(_x, _y + 4, SPRIDX_DESIGN_MOVE_DOWN,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		pSpriteDesign->RenderToQueue(_x, _y + m_nDistance - s.cy - 4, SPRIDX_DESIGN_MOVE_UP,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);
	}
}

void RedTurtleWingedPatrol::OnIdle()
{
	ChangeState(STATE_MOVE);
}

void RedTurtleWingedPatrol::OnMove()
{
	float fCur = (m_nDistance / 2.0f) * sin((-90 + m_nStateFrame) * fToRadian);
	float fNew = (m_nDistance / 2.0f) * sin((-90 + (m_nStateFrame + 1)) * fToRadian);
	m_fYS = fNew - fCur;
}

void RedTurtleWingedPatrol::OnWalk()
{
	CheckFalling();

	CheckReachWall();

	if (m_bReachFriend)
		OnReachFriend();

	POINT ptNextGround = {
		(int)((m_fX + m_fXS) / TILE_XS),
		(int)((m_fY + m_fYS) / TILE_YS),
	};

	int nNextGroundData = m_pStage->GetTileData(ptNextGround.x, ptNextGround.y);
	if (m_pStage->GetDataType(nNextGroundData) == TILETYPE_EMPTY)
	{
		m_bFlip = !m_bFlip;
		m_fXS *= -1;
	}
}

int RedTurtleWingedPatrol::GetSpriteIndex()
{
	int nFrame = m_nSpriteIndex;

	switch (m_nState)
	{
	case STATE_IDLE:
	case STATE_MOVE:
	case STATE_MOVEDOWN:
	case STATE_MOVEUP:
		nFrame = SPRIDX_REDTURTLE_WINGED1 + (m_nStateFrame / 8) % 2;
		break;
	case STATE_HELDBYTONGUE:
		if (nFrame == 0)
			nFrame = SPRIDX_REDTURTLE_SHELL1;
		break;
	default:
		nFrame = RedTurtle::GetSpriteIndex();
		break;
	}

	m_nSpriteIndex = nFrame;

	return nFrame;
}
*/