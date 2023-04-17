#include "GoalPoint.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "ThemeManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(GoalPoint)
	PROP_INT("IsBigCastle", VT_BOOL, false, nullptr, "GoalPoint"),
END_IMPL_PROPERTYMAP()

GoalPoint::GoalPoint(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_GOALPOINT;
	m_nState = STATE_IDLE;
	RequestSprites(pStage);
	
	m_nFlagHeight = 0;
	m_bBigCastle = false;
}

GoalPoint::~GoalPoint()
{
}

void GoalPoint::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSpriteMapObject;
	m_ppSpriteTile = &pStage->m_pSprite;
}

void GoalPoint::CheckCollisionPlayer()
{
	m_bCollisionPlayer = false;
	if ((GameObjectBase*)CUR_PLAYER_OBJ == this)
		return;

	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rcPlayer.ptInRect(m_fX + (TILE_XS / 2) - 4, m_fY))
	{
		m_bCollisionPlayer = true;
	}
}

void GoalPoint::Process()
{
	if (m_nState == STATE_FLAGUP)
	{
		m_nFlagHeight++;
		if (m_nFlagHeight == 20)
		{
			ChangeState(STATE_EVENTDEAD);
		}
	}
	
	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
		ChangeState(STATE_EVENTDEAD);
	}
}

void GoalPoint::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void GoalPoint::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	
	bool bShadow = false;
	if (m_pGame->m_bModernStyle)
		bShadow = true;

	bool bGoalPoint = true;
	if (m_fX < GameDefaults::nPageWidth / 2)
		bGoalPoint = false;

	int nMasterZOrder = nZOrder;
	if (!m_bBigCastle)
	{
		(*m_ppSprite)->RenderToQueue(_x, _y - 60 - m_nFlagHeight, SPRIDX_MAPOBJ_CASTLE_FLAG,
			false, false, nColor, 1.0f, 1.0f, Z_CASTLE_FLAG, bShadow);

		int nTileSet[] = {
			-1, 0, 0, 0, -1,
			-1, 2, 3, 4, -1,
			0, 1, 1, 1, 0,
			3, 3, 5, 3, 3,
			3, 3, 6, 3, 3,
		};

		int nZorderSet[] = {
			0, 1, 1, 1, 0,
			0, 0, 1, 1, 0,
			0, 0, 1, 1, 1,
			0, 0, 0, 1, 1,
			0, 0, 0, 1, 1,
		};

		int _w = 5;
		int _h = 5;
		for (int i = 0; i < _w; i++)
		{
			for (int j = 0; j < _h; j++)
			{
				float tmpx = _x - (TILE_XS * (_w - 1) / 2) + (i * TILE_XS) - (TILE_XS / 2) + 8;
				float tmpy = _y + ((j - _h) * TILE_YS) + 16;
				int nTileSetIdx = (j * _w) + i;
				if (nTileSet[nTileSetIdx] == -1)
					continue;

				int tmpidx = GetTileData(nTileSet[(j * _w) + i]);

				if (nMasterZOrder == -1)
				{
					nZOrder = Z_CASTLE;
					if (bGoalPoint && nZorderSet[nTileSetIdx] == 1 && m_nState == STATE_EVENTDEAD)
						nZOrder = Z_MAPOBJECT_TOP;
					else if (bGoalPoint && nZorderSet[nTileSetIdx] == 2)
						nZOrder = Z_MAPOBJECT_TOP;
				}

				(*m_ppSpriteTile)->RenderToQueue(tmpx, tmpy, tmpidx, false, false, nColor, 1.0f, 1.0f, nZOrder, bShadow);
			}
		}
	}
	else
	{
		(*m_ppSprite)->RenderToQueue(_x, _y - 60 - (TILE_YS * 6) - m_nFlagHeight, SPRIDX_MAPOBJ_CASTLE_FLAG,
			false, false, nColor, 1.0f, 1.0f, Z_CASTLE_FLAG, bShadow);

		int nTileSet[] = {
			-1, -1, -1, 0, 0, 0, -1, -1, -1,
			-1, -1, -1, 2, 3, 4, -1, -1, -1,
			-1, -1,  0, 1, 1, 1,  0, -1, -1,
			-1, -1,  3, 3, 5, 3,  3, -1, -1,
			-1, -1,  3, 3, 6, 3,  3, -1, -1,
			 0,  0,  1, 1, 1, 1,  1,  0,  0,
			 3,  3,  3, 5, 3, 5,  3,  3,  3,
			 3,  3,  3, 6, 3, 6,  3,  3,  3,
			 3,  3,  3, 3, 3, 3,  3,  3,  3,
			 3,  3,  5, 3, 5, 3,  5,  3,  3,
			 3,  3,  6, 3, 6, 3,  6,  3,  3,
		};

		int nZorderSet[] = {
			0, 0, 0, 1, 1, 1, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 1, 0, 0, 0, 1, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0,
			1, 1, 0, 0, 0, 0, 0, 1, 1,
			0, 0, 0, 0, 1, 1, 1, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1,
			0, 0, 0, 0, 1, 1, 1, 1, 1,
			0, 0, 0, 0, 0, 1, 1, 1, 1,
			0, 0, 0, 0, 0, 1, 1, 1, 1,
		};

		int _w = 9;
		int _h = 11;
		for (int i = 0; i < _w; i++)
		{
			for (int j = 0; j < _h; j++)
			{
				float tmpx = _x - (TILE_XS * (_w - 1) / 2) + (i * TILE_XS) - (TILE_XS / 2) + 8;
				float tmpy = _y + ((j - _h) * TILE_YS) + 16;
				int nTileSetIdx = (j * _w) + i;
				if (nTileSet[nTileSetIdx] == -1)
					continue;

				int tmpidx = GetTileData(nTileSet[(j * _w) + i]);

				if (nMasterZOrder == -1)
				{
					nZOrder = Z_CASTLE;
					if (bGoalPoint && nZorderSet[nTileSetIdx] == 1 && m_nState == STATE_EVENTDEAD)
						nZOrder = Z_MAPOBJECT_TOP;
					else if (bGoalPoint && nZorderSet[nTileSetIdx] == 2)
						nZOrder = Z_MAPOBJECT_TOP;
				}

				(*m_ppSpriteTile)->RenderToQueue(tmpx, tmpy, tmpidx, false, false, nColor, 1.0f, 1.0f, nZOrder, bShadow);
			}
		}
	}
}

bool GoalPoint::CanApplied()
{
	if (m_nState != STATE_IDLE)
		return false;

	// This is start point
	if (m_fX < GameDefaults::nPageWidth / 2)
		return false;

	return true;
}

bool GoalPoint::CanPushedUp()
{
	return false;
}

unsigned char GoalPoint::GetTileData(int nIdx)
{
	unsigned char r = 0;
	switch (nIdx)
	{
	case 0: return TILE_CASTLE_HEAD1;
	case 1: return TILE_CASTLE_HEAD2;
	case 2: return TILE_CASTLE_WINDOW1;
	case 3: return TILE_CASTLE_BRICK;
	case 4: return TILE_CASTLE_WINDOW2;
	case 5: return TILE_CASTLE_DOOR_T;
	case 6: return TILE_CASTLE_DOOR_B;
	default: return TILE_EMPTY;
	}

	return TILE_EMPTY;
}

SIZE GoalPoint::GetSize()
{
	SIZE s;
	if (!m_bBigCastle)
	{
		s = {
		   TILE_XS * 5,
		   TILE_YS * 5
		};
	}
	else
	{
		s = {
			TILE_XS * 9,
			TILE_YS * 11
		};
	}
	return s;
}
