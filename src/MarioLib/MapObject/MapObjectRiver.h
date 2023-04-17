#pragma once

#include "MapObjectBase.h"

#include "PropertyMap.h"

class Game;
class Sprite;
class MapObjectRiver : public MapObjectBase
{
public:
	MapObjectRiver(Game *pGame, Stage *pStage);
	virtual ~MapObjectRiver();

	virtual bool Tileize(int nPhase = 0);

	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	NaRect m_rc;

	int m_nWidth;
	int m_nHeight;
	bool m_bCanSwim;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"CanSwim", &m_bCanSwim },
	END_DECLARE_PROPERTYMAP()
};

