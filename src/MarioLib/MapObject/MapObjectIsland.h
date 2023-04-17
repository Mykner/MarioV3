#pragma once

#include "MapObjectBase.h"

#include "PropertyMap.h"

class Game;
class Sprite;
class MapObjectIsland : public MapObjectBase
{
public:
	MapObjectIsland(Game *pGame, Stage *pStage);
	virtual ~MapObjectIsland();

	virtual bool Tileize(int nPhase = 0);
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	SIZE m_sizeHead;
	SIZE m_sizeBody;

	int m_nWidth;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Width", &m_nWidth },
	END_DECLARE_PROPERTYMAP()
};

