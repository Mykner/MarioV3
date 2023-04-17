#include "MapObjectIsland.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectIsland)
	PROP_INT("Width", VT_I4, 3, nullptr, ""),
END_IMPL_PROPERTYMAP()

MapObjectIsland::MapObjectIsland(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_ISLAND;

	m_nWidth = 3;
}

MapObjectIsland::~MapObjectIsland()
{
}

bool MapObjectIsland::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		m_pStage->SetTileData(m_nTX + 0, m_nTY, TILE_ISLANDHEAD_L);
		for (int i = m_nTX + 1; i < m_nTX + m_nWidth - 1; i++)
		{
			m_pStage->SetTileData(i, m_nTY, TILE_ISLANDHEAD_C);
		}

		m_pStage->SetTileData(m_nTX + m_nWidth - 1, m_nTY, TILE_ISLANDHEAD_R);
		
		return false;
	}
	else if (nPhase == 2)
	{
		// Body
		for (int i = m_nTY + 1; i < m_pStage->m_sizeTile.cy; i++)
		{
			bool bStop = false;
			for (int j = m_nTX + 1; j < m_nTX + m_nWidth - 1; j++)
			{
				if (m_pStage->GetTileData(j, i) != TILE_EMPTY)
				{
					bStop = true;
					break;
				}
			}

			if (bStop)
				break;

			for (int j = m_nTX; j < m_nTX + m_nWidth; j++)
			{
				if (IS_CLASSIC && (j == m_nTX || j == m_nTX + m_nWidth - 1))
					continue;

				if (j == m_nTX)
					m_pStage->SetTileData(j, i, TILE_ISLANDBODY_L);
				else if (j == m_nTX + m_nWidth - 1)
					m_pStage->SetTileData(j, i, TILE_ISLANDBODY_R);
				else
					m_pStage->SetTileData(j, i, TILE_ISLANDBODY_C);
			}
		}

		return true;
	}
	return false;
}

SIZE MapObjectIsland::GetSize()
{
	CalcSize();

	SIZE s;
	s.cx = m_nWidth;
	s.cy = 1;

	for (int i = m_nTY + 1; i < m_pStage->m_sizeTile.cy; i++)
	{
		bool bStop = false;
		for (int j = m_nTX + 1; j < m_nTX + m_nWidth - 1; j++)
		{
			if (m_pStage->GetTileData(j, i) != TILE_EMPTY)
			{
				s.cy = i - m_nTY;
				bStop = true;
				break;
			}
		}

		if (bStop)
			break;
		else
			s.cy++;
	}

	return s;
}

void MapObjectIsland::Render(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	// Head
	m_pStage->RenderTile(x + 0, y, TILE_ISLANDHEAD_L, nColor, nZOrder);
	for (int i = 1; i < m_nWidth - 1; i++)
	{
		m_pStage->RenderTile(x + (i * TILE_XS), y, TILE_ISLANDHEAD_C, nColor, nZOrder);
	}

	m_pStage->RenderTile(x + (m_nWidth - 1) * TILE_XS, y, TILE_ISLANDHEAD_R, nColor, nZOrder);

	if (nZOrder == Z_MAP)
		nZOrder = Z_BEHIND_MAP;

	// Body
	for (int i = 1; i < s.cy; i++)
	{
		ty = y + (i * TILE_YS);
// 		for (int j = 1; j < m_nWidth - 1; j++)
// 		{
// 			tx = x + (j * TILE_XS);
// 			if (m_pStage->GetTileData(tx, ty) != TILE_EMPTY)
// 				break;
// 		}

		for (int j = 0; j < m_nWidth; j++)
		{
			if (IS_CLASSIC && (j == 0 || j == m_nWidth - 1))
				continue;

			int nTileIndex = TILE_ISLANDBODY_C;
			if (j == 0)
				nTileIndex = TILE_ISLANDBODY_L;
			else if (j == m_nWidth - 1)
				nTileIndex = TILE_ISLANDBODY_R;

			m_pStage->RenderTile(x + (j * TILE_XS), y + (i * TILE_YS), nTileIndex, nColor, nZOrder);
		}
	}
}
