#include "MapObjectRiverBridge.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectRiverBridge)
	PROP_INT("Width", VT_I4, -1, 0, "RiverBridge"),
END_IMPL_PROPERTYMAP()

MapObjectRiverBridge::MapObjectRiverBridge(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_RIVERBRIDGE;
	m_pSprite = SPR_MANAGER->Find(L"MapObject");

	m_nWidth = -1;
	m_nRealWidth = 1;
}

MapObjectRiverBridge::~MapObjectRiverBridge()
{
}

bool MapObjectRiverBridge::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		CalcSize();

		int nLen = m_nWidth;
		if (nLen == -1)
			nLen = m_nRealWidth;

		for (int i = m_nTX; i < m_nTX + nLen; i++)
		{
			m_pStage->SetTileData(i, m_nTY - 1, TILE_BRIDGE_T);
			m_pStage->SetTileData(i, m_nTY - 0, TILE_BRIDGE_B);
		}
	}

	return true;
}

void MapObjectRiverBridge::Install()
{
	CalcSize();
}

void MapObjectRiverBridge::CalcSize()
{
	if (m_nWidth != -1)
		return;

	m_nRealWidth = 0;
	for (int i = m_nTX; i < m_nTX + GameDefaults::nPageTileWidth; i++)
	{
		int nData = m_pStage->GetTileData(i, m_nTY);
		if (nData != TILE_EMPTY)
			break;

		m_nRealWidth++;
	}
}

SIZE MapObjectRiverBridge::GetSize()
{
	CalcSize();

	SIZE s = { m_nWidth, 1 };
	if (m_nWidth == -1)
		s = { m_nRealWidth, 1 };

	return s;
}

void MapObjectRiverBridge::Render(int nColor, int nZOrder)
{
	int nLen = m_nWidth;
	if (nLen == -1)
		nLen = m_nRealWidth;

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	for (int i = 0; i < nLen; i++)
	{
		m_pStage->RenderTile(x + (i * TILE_XS), y - TILE_YS, TILE_BRIDGE_T, nColor, nZOrder);
		m_pStage->RenderTile(x + (i * TILE_XS), y, TILE_BRIDGE_B, nColor, nZOrder);
	}
}
