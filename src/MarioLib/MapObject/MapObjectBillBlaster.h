#pragma once

#include "MapObjectBase.h"

#include "PropertyMap.h"

class Game;
class Sprite;
class MapObjectBillBlaster : public MapObjectBase
{
public:
	MapObjectBillBlaster(Game *pGame, Stage *pStage);
	virtual ~MapObjectBillBlaster();

	virtual bool Tileize(int nPhase = 0);
	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	int GetFireType();

	int m_nFireType;
	int m_nFireTypeIndex;
	int m_nInterval;
	int m_nChance;
	int m_nMaxCount;
	int m_nHeight;

	SIZE m_sizeHead;
	SIZE m_sizeBody;
	Sprite* m_pSpriteDesign;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"FireType", &m_nFireTypeIndex },
		{ L"Interval", &m_nInterval },
		{ L"Chance", &m_nChance },
		{ L"MaxCount", &m_nMaxCount },
		{ L"Height", &m_nHeight },
	END_DECLARE_PROPERTYMAP()
};

