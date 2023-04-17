#include "MapObjectWall.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectWall)
	PROP_INT("Width", VT_I4, 1, 0, "MapObjectWall"),
END_IMPL_PROPERTYMAP()


MapObjectWall::MapObjectWall(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_WALL;

	m_nWidth = 1;
}

MapObjectWall::~MapObjectWall()
{
}

bool MapObjectWall::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		for (int i = m_nTX; i < m_nTX + m_nWidth; i++)
			m_pStage->SetTileData(i, m_nTY, TILE_CASTLE_HEAD1);
		
		return false;
	}
	else if (nPhase == 2)
	{
		// Body
		for (int i = m_nTY + 1; i < GameDefaults::nPageTileHeight - 3; i++)
		{
			for (int j = m_nTX; j < m_nTX + m_nWidth; j++)
			{
				if (m_pStage->GetTileData(j, i) == TILE_EMPTY)
					m_pStage->SetTileData(j, i, TILE_CASTLE_BRICK);
			}
		}

		return true;
	}
	return false;
}

void MapObjectWall::CalcSize()
{
	// #TODO Wall autosize?
}

SIZE MapObjectWall::GetSize()
{
	CalcSize();

	SIZE s;
	s.cx = m_nWidth;
	s.cy = 1;

	return s;
}

void MapObjectWall::Render(int nColor, int nZOrder)
{
	CalcSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	// Head
	m_pStage->RenderTile(x + 0, y, TILE_CASTLE_HEAD1, nColor, nZOrder);
	for (int i = 1; i < m_nWidth; i++)
	{
		m_pStage->RenderTile(x + (i * TILE_XS), y, TILE_CASTLE_HEAD1, nColor, nZOrder);
	}

	// Body
	for (int i = 1; i < GameDefaults::nPageTileHeight - 3; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			int tx = (x / TILE_XS) + j;
			int ty = (y / TILE_XS) + i;

			if (m_pStage->GetTileData(tx, ty) == TILE_EMPTY)
				m_pStage->RenderTile(x + (j * TILE_XS), y + (i * TILE_YS), TILE_CASTLE_BRICK, nColor, nZOrder);
		}
	}
}
