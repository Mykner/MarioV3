#pragma once

#include "MapObjectBase.h"

#include "PropertyMap.h"

class Game;
class Sprite;
class MapObjectWall : public MapObjectBase
{
public:
	MapObjectWall(Game *pGame, Stage *pStage);
	virtual ~MapObjectWall();

	virtual bool Tileize(int nPhase = 0);

	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	int m_nWidth;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Width", &m_nWidth },
	END_DECLARE_PROPERTYMAP()
};

