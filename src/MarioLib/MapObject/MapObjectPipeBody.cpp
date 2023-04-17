#include "MapObjectPipeBody.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"

MapObjectPipeBody::MapObjectPipeBody(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_PIPEBODY;
}

MapObjectPipeBody::~MapObjectPipeBody()
{
}

bool MapObjectPipeBody::Tileize(int nPhase)
{
	for (int i = m_nTY; i < m_nTY + GameDefaults::nPageTileHeight; i++)
	{
		if (m_pStage->GetTileData(m_nTX, i) != TILE_EMPTY ||
			m_pStage->GetTileData(m_nTX + 1, i) != TILE_EMPTY)
			break;

		m_pStage->SetTileData(m_nTX, i, TILE_PIPEBODY_L);
		m_pStage->SetTileData(m_nTX + 1, i, TILE_PIPEBODY_R);
	}

	return true;
}

void MapObjectPipeBody::CalcSize()
{
	m_sizeBody.cx = 2;
	m_sizeBody.cy = 0;

	for (int i = m_nTY; i < m_nTY + GameDefaults::nPageTileHeight; i++)
	{
		if (m_pStage->GetTileData(m_nTX, i) != TILE_EMPTY ||
			m_pStage->GetTileData(m_nTX + 1, i) != TILE_EMPTY)
			break;
		m_sizeBody.cy++;
	}
}

SIZE MapObjectPipeBody::GetSize()
{
	CalcSize();

	return m_sizeBody;
}

void MapObjectPipeBody::Render(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	for (int i = 0; i < s.cy; i++)
	{
		ty = (y + i) / TILE_YS;
		if (m_pStage->GetTileData(tx, ty) != TILE_EMPTY ||
			m_pStage->GetTileData(tx + 1, ty) != TILE_EMPTY)
			break;

		m_pStage->RenderTile(x + (TILE_XS * 0), y + (i * TILE_YS), TILE_PIPEBODY_L, nColor, nZOrder);
		m_pStage->RenderTile(x + (TILE_XS * 1), y + (i * TILE_YS), TILE_PIPEBODY_R, nColor, nZOrder);
	}
}
