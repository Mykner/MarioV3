#include "MapObjectMushroomIsland.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectMushroomIsland)
	PROP_INT("Width",	VT_I4, 3, nullptr, "MushroomIsland"),
	PROP_INT("SubType", VT_I4, 0, nullptr, "MushroomIsland"),
END_IMPL_PROPERTYMAP()

MapObjectMushroomIsland::MapObjectMushroomIsland(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_MUSHROOMISLAND;

	m_nWidth = 3;
	m_nSubType = 0;
}

MapObjectMushroomIsland::~MapObjectMushroomIsland()
{
}

bool MapObjectMushroomIsland::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		/*
		m_pStage->SetTileData(m_nTX + 0, m_nTY, TILE_MUSHROOMHEAD_L);
		for (int i = m_nTX + 1; i < m_nTX + m_nWidth - 1; i++)
		{
			m_pStage->SetTileData(i, m_nTY, TILE_MUSHROOMHEAD_C);
		}
		m_pStage->SetTileData(m_nTX + m_nWidth - 1, m_nTY, TILE_MUSHROOMHEAD_R);
		*/

		for (int i = m_nTX; i < m_nTX + m_nWidth; i++)
		{
			int nIdx = GetCorrectHeadTileIndex(i - m_nTX, 0);
			m_pStage->SetTileData(i, m_nTY, nIdx);
		}
		
		return false;
	}
	else if (nPhase == 2)
	{
		// Body
		if (!m_pGame->m_bModernStyle)
		{
			// Old Style
			for (int i = m_nTY + 1; i < m_pStage->m_sizeTile.cy; i++)
			{
				bool bStop = false;
				int j = m_nTX + (m_nWidth / 2);

				if (m_pStage->GetTileData(j, i) != TILE_EMPTY)
					continue;

				if (i == m_nTY + 1)
					m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_T);
				else
					m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_B);
			}
		}
		else
		{
			if (m_nWidth % 2 == 1)
			{
				for (int i = m_nTY + 1; i < m_pStage->m_sizeTile.cy; i++)
				{
					bool bStop = false;
					int j = m_nTX + (m_nWidth / 2);

					if (m_pStage->GetTileData(j, i) != TILE_EMPTY)
						continue;

					if (i == m_nTY + 1)
						m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_T);
					else
						m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_B);
				}
			}
			else
			{
				for (int i = m_nTY + 1; i < m_pStage->m_sizeTile.cy; i++)
				{
					int nBodyLeft = m_nTX + (m_nWidth / 2) - 1;
					for (int j = nBodyLeft; j < nBodyLeft + 2; j++)
					{
						bool bStop = false;
						if (m_pStage->GetTileData(j, i) != TILE_EMPTY)
							continue;

						if (i == m_nTY + 1)
						{
							if (j == nBodyLeft)
								m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_LT);
							else
								m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_RT);
						}
						else
						{
							if (j == nBodyLeft)
								m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_LB);
							else
								m_pStage->SetTileData(j, i, TILE_MUSHROOMBODY_RB);
						}
					}
				}
			}
		}

		return true;
	}

	return false;
}

SIZE MapObjectMushroomIsland::GetSize()
{
	CalcSize();

	SIZE s;
	s.cx = m_nWidth;
	s.cy = 1;

	for (int i = m_nTY + 1; i < m_pStage->m_sizeTile.cy; i++)
	{
		int j = m_nTX + (m_nWidth / 2);

		if (m_pStage->GetTileData(j, i) != TILE_EMPTY)
		{
			s.cy = i - m_nTY;
			break;
		}
		else
			s.cy++;
	}

	return s;
}

void MapObjectMushroomIsland::Render(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	// Head
	/*
	m_pStage->RenderTile(x + 0, y, TILE_MUSHROOMHEAD_L, nColor, nZOrder);
	for (int i = 1; i < m_nWidth - 1; i++)
	{
		m_pStage->RenderTile(x + (i * TILE_XS), y, TILE_MUSHROOMHEAD_C, nColor, nZOrder);
	}

	m_pStage->RenderTile(x + (m_nWidth - 1) * TILE_XS, y, TILE_MUSHROOMHEAD_R, nColor, nZOrder);
	*/

	for (int i = 0; i < m_nWidth; i++)
	{
		int nIdx = GetCorrectHeadTileIndex(i, 0);
		m_pStage->RenderTile(x + (i * TILE_XS), y, nIdx, nColor, nZOrder);
	}

	if (nZOrder == Z_MAP)
		nZOrder = Z_BEHIND_MAP;

	// Body
	for (int i = 1; i < s.cy; i++)
	{
		ty = y + (i * TILE_YS);

		if (m_nWidth % 2 == 1)
		{
			if (i == 1)
				m_pStage->RenderTile(x + (m_nWidth / 2 * TILE_XS), y + (i * TILE_YS), TILE_MUSHROOMBODY_T, nColor, nZOrder);
			else
				m_pStage->RenderTile(x + (m_nWidth / 2 * TILE_XS), y + (i * TILE_YS), TILE_MUSHROOMBODY_B, nColor, nZOrder);
		}
		else
		{
			int nBodyLeft = tx + (m_nWidth / 2) - 1;
			for (int j = 0; j < 2; j++)
			{
				//if (m_pStage->GetTileData(j, ty) != TILE_EMPTY)
				//	continue;
				int nIdx;
				if (i == 1)
				{
					if (j == 0)
						nIdx = TILE_MUSHROOMBODY_LT;
					else
						nIdx = TILE_MUSHROOMBODY_RT;
				}
				else
				{
					if (j == 0)
						nIdx = TILE_MUSHROOMBODY_LB;
					else
						nIdx = TILE_MUSHROOMBODY_RB;
				}

				m_pStage->RenderTile(
					x + (m_nWidth / 2 * TILE_XS) - (TILE_XS) + (j * TILE_XS), 
					y + (i * TILE_YS), 
					nIdx, 
					nColor, 
					nZOrder);
			}

		}
	}
}

int MapObjectMushroomIsland::GetSubTypedTileIndex(int nIndex)
{
	int nSet[3] =
	{
		TILE_MUSHROOMHEAD_L,
		TILE_MUSHROOMHEAD_C,
		TILE_MUSHROOMHEAD_R,
	};

	int nSkinIndex = m_nSubType;
	if (nSkinIndex < 0)
		nSkinIndex = 0;
	if (nSkinIndex > 2)
		nSkinIndex = 2;

	return nSet[nIndex] + (nSkinIndex * 16);
}

int MapObjectMushroomIsland::GetCorrectHeadTileIndex(int x, int y)
{
	int nSkinIndex = m_nSubType;
	if (nSkinIndex < 0)
		nSkinIndex = 0;
	if (nSkinIndex > 2)
		nSkinIndex = 2;

	if (y == 0)
	{
		if (x == 0)
			return GetSubTypedTileIndex(0);
		else if (x == m_nWidth - 1)
			return GetSubTypedTileIndex(2);
		else
			return GetSubTypedTileIndex(1);
	}

	return 0;
}
