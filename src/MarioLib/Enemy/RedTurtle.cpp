#include "RedTurtle.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(RedTurtle)
	PROP_INT("MoveDistance", VT_I4, 104, nullptr, "RedTurtleWingedPatrol"),
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "RedTurtle"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "RedTurtle"),
END_IMPL_PROPERTYMAP()

RedTurtle::RedTurtle(Game *pGame, Stage *pStage) :
	Turtle(pGame, pStage)
{
	m_nType = ENEMY_REDTURTLE;
	m_nState = STATE_IDLE;

	m_nSpriteIndex = 0;

	m_bFlip = true;
	m_nDistance = 104;
}

RedTurtle::~RedTurtle()
{
}

void RedTurtle::OnIdle()
{
	if (m_bWinged)
		m_nBaseState = STATE_MOVE;
	else
		m_nBaseState = STATE_WALK;

	ChangeState(m_nBaseState);
}

void RedTurtle::OnWalk()
{
	if (CheckFalling())
	{
		if (m_nStateFrame != 0)
		{
			m_bFlip = !m_bFlip;
			m_fXS *= -1;
			return;
		}
	}

	SIZE s = GetSize();
	float fFront = m_fX + m_fXS;
	if (m_bFlip)
		fFront -= (s.cx / 4);
	else
		fFront += (s.cx / 4);

	POINT ptNextGround = {
		(long)((fFront) / TILE_XS),
		(long)((m_fY + m_fYS) / TILE_YS),
	};		

	int nNextGroundData = m_pStage->GetTileData(ptNextGround.x, ptNextGround.y);
	if (m_pStage->GetDataType(nNextGroundData) == TILETYPE_EMPTY)
	{
		int nCurGroundData = m_pStage->GetTileData(m_fX / TILE_XS, (m_fY + m_fYS) / TILE_YS);
		if (m_pStage->GetDataType(nCurGroundData) != TILETYPE_EMPTY)
		{
			m_bFlip = !m_bFlip;
			m_fXS *= -1;
		}
		else
		{
			// Where am I?!
		}
	}

	if (m_bReachFriend)
	{
		OnReachFriend();
		return;
	}

	if (CheckReachWall())
		return;
}

void RedTurtle::OnMove()
{
	float fCur = (m_nDistance / 2.0f) * sin((-90 + m_nStateFrame) * fToRadian);
	float fNew = (m_nDistance / 2.0f) * sin((-90 + (m_nStateFrame + 1)) * fToRadian);
	m_fYS = fNew - fCur;
}

void RedTurtle::OnAfterItemPop(int nNextStatePopUp, int nNextStatePopDown)
{
	if (m_bWinged)
		m_nBaseState = STATE_MOVE;
	else
		m_nBaseState = STATE_WALK;

	if (m_nState == STATE_ITEMPOP)
		ChangeState(m_nBaseState);
	else if (m_nState == STATE_ITEMPOPDOWN)
		ChangeState(nNextStatePopDown);
}

void RedTurtle::RenderDesign(int nColor, int nZOrder)
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

		if (m_bWinged)
		{
			(*m_ppSprite)->RenderToQueue(_x, _y + m_nDistance, nFrame, m_bFlip, m_bFlipV, COLOR_ARGB(150, 255, 255, 255),
				m_fScaleX, m_fScaleY, nZOrder);

			Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
			SIZE s = GetSize();

			pSpriteDesign->RenderToQueue(_x, _y + 4, SPRIDX_DESIGN_MOVE_DOWN,
				false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

			pSpriteDesign->RenderToQueue(_x, _y + m_nDistance - s.cy - 4, SPRIDX_DESIGN_MOVE_UP,
				false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

			RenderWing(nColor, nZOrder);
		}
	}
}

int RedTurtle::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_SHELLAWAKE:
		if (IS_CLASSIC)
			nFrame = SPRIDX_REDTURTLE_SHELL1 + (m_nStateFrame / 8) % 2;
		else
		{
			nFrame = SPRIDX_REDTURTLE_SPIN1;
		}
		break;
	case STATE_PRESSED:
	case STATE_SHELLIDLE:
	case STATE_CARRYBYHAND:
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		if (IS_MODERN)
		{
			if (m_bDeadInside)
				nFrame = SPRIDX_REDTURTLE_SHELL1;
			else
				nFrame = SPRIDX_REDTURTLE_SPIN1;
		}
		else
			nFrame = SPRIDX_REDTURTLE_SHELL1;
		break;
	case STATE_SHELLRUN:
		if (m_pGame->m_bModernStyle)
		{
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_REDTURTLE_SPIN1, 4, 3);
		}
		else
			nFrame = SPRIDX_REDTURTLE_SHELL1;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_REDTURTLE_SHELL1;
		break;
	case STATE_HELDBYTONGUE:
		if (nFrame == 0)
			nFrame = SPRIDX_REDTURTLE_SHELL1;
		break;
	default:
		nFrame = SPRIDX_REDTURTLE1 + (m_nStateFrame / 8) % 2;
		if (m_nShape < TURTLESHAPE_NORMAL || m_bDeadInside)
			nFrame = SPRIDX_REDTURTLE_SHELL1;
		break;
	}

	m_nSpriteIndex = nFrame;

	return nFrame;
}
