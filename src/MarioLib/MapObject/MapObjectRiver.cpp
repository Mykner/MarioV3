#include "MapObjectRiver.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectRiver)
	PROP_INT("CanSwim", VT_BOOL, false, 0, "River"),
END_IMPL_PROPERTYMAP()


MapObjectRiver::MapObjectRiver(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_RIVER;

	m_nWidth = 1;
	m_nHeight = 1;
	m_bCanSwim = false;
}

MapObjectRiver::~MapObjectRiver()
{
}

bool MapObjectRiver::Tileize(int nPhase)
{
	//static NaRect rc;
	if (nPhase == 0)
	{
		CalcSize();
		m_rc = GetRect();

		for (int i = m_nTX; i < m_nTX + m_nWidth; i++)
			m_pStage->SetTileData(i, m_nTY, TILE_RIVER_T);
		
		return false;
	}
	else if (nPhase == 2)
	{
		// Body
		for (int i = m_nTY + 1; i < m_nTY + m_nHeight; i++)
		{
			for (int j = m_nTX; j < m_nTX + m_nWidth; j++)
			{
				if (m_pStage->GetTileData(j, i) == TILE_EMPTY)
					m_pStage->SetTileData(j, i, TILE_RIVER_B);
			}
		}

		// #TODO Improvement Swimarea handling
		if (m_bCanSwim)
		{
			// Initilize SwimArea
			NaRect rc = m_rc;
			rc.top += 8;
			m_pStage->m_vecSwimArea.push_back(rc);
			
			//m_pStage->AddSwimZone();
		}

		return true;
	}
	return false;
}

void MapObjectRiver::CalcSize()
{
	m_nWidth = 1;
	for (int i = m_nTX + 1; i < m_nTX + GameDefaults::nPageTileWidth; i++)
	{
		if (m_pStage->GetTileData(i, m_nTY) != TILE_EMPTY &&
			m_pStage->GetTileData(i, m_nTY) != TILE_RIVER_T)
			break;
		m_nWidth++;
	}

	m_nHeight = 1;
	for (int j = m_nTY + 1; j < m_nTY + GameDefaults::nPageTileHeight; j++)
	{
		bool bStop = false;
		for (int i = m_nTX + 1; i < m_nTX + m_nWidth; i++)
		{
			if (m_pStage->GetTileData(i, j) != TILE_EMPTY &&
				m_pStage->GetTileData(i, j) != TILE_RIVER_T)
			{
				bStop = true;
				break;
			}
		}

		if (bStop)
			break;
		m_nHeight++;
	}
}

SIZE MapObjectRiver::GetSize()
{
	CalcSize();

	SIZE s;
	s.cx = m_nWidth;
	s.cy = m_nHeight;

	return s;
}

void MapObjectRiver::Render(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	// Head
	m_pStage->RenderTile(x + 0, y, TILE_RIVER_T, nColor, nZOrder);
	for (int i = 1; i < s.cx; i++)
	{
		m_pStage->RenderTile(x + (i * TILE_XS), y, TILE_RIVER_T, nColor, nZOrder);
	}

	// Body
	for (int i = 1; i < s.cy; i++)
	{
		for (int j = 0; j < s.cx; j++)
		{
			m_pStage->RenderTile(x + (j * TILE_XS), y + (i * TILE_YS), TILE_RIVER_B, nColor, nZOrder);
		}
	}
}
