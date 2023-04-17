#include "MapObjectMessageBlock.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectMessageBlock)
	PROP_STR("Message", VT_ETC_NASTR, L"", 0, "MapObjectMessageBlock"),
END_IMPL_PROPERTYMAP()

MapObjectMessageBlock::MapObjectMessageBlock(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_MESSAGEBLOCK;
	m_pSprite = pStage->m_pSprite;

	m_strMessage = L"";
}

MapObjectMessageBlock::~MapObjectMessageBlock()
{
}

bool MapObjectMessageBlock::Tileize(int nPhase)
{
	m_pStage->SetTileData(m_nTX, m_nTY, TILE_MESSAGE);

	int nIdx = m_pStage->GetTileIndex(m_nTX, m_nTY);
	m_pStage->m_mapMessage.insert(
		std::pair<int, NaString>(nIdx, m_strMessage)
	);

	return true;
}

void MapObjectMessageBlock::Install()
{
	CalcSize();
}

void MapObjectMessageBlock::CalcSize()
{
}

SIZE MapObjectMessageBlock::GetSize()
{
	SIZE s = { 1, 1 };

	return s;
}

void MapObjectMessageBlock::Render(int nColor, int nZOrder)
{
	bool bNotShadow = false;
	if (nZOrder == -1)
	{
		bNotShadow = true;
		nZOrder = Z_MAP;
	}

	m_pStage->RenderTile(x, y, TILE_MESSAGE, nColor, nZOrder);
}
