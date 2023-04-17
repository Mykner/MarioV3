#pragma once

#include "MapObjectBase.h"

#include "PropertyMap.h"

class Game;
class Sprite;
class MapObjectMushroomIsland : public MapObjectBase
{
public:
	MapObjectMushroomIsland(Game *pGame, Stage *pStage);
	virtual ~MapObjectMushroomIsland();

	virtual bool Tileize(int nPhase = 0);
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	int GetSubTypedTileIndex(int nIndex);
	int GetCorrectHeadTileIndex(int x, int y);

	SIZE m_sizeHead;
	SIZE m_sizeBody;

	int m_nWidth;
	int m_nSubType;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Width", &m_nWidth)
		DECL_PROP("SubType", &m_nSubType)
	END_DECLARE_PROPERTYMAP()
};

