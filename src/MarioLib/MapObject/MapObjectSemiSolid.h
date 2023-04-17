#pragma once

#include "Common.h"

#include "MapObjectBase.h"
#include "PropertyMap.h"

class MapObjectSemiSolid : public MapObjectBase
{
public:
	MapObjectSemiSolid(Game *pGame, Stage *pStage);
	virtual ~MapObjectSemiSolid();

	virtual bool Tileize(int nPhase = 0);
	virtual void Install();
	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	int GetSubTypedTileIndex(int nIndex);
	int GetCorrectTileIndex(int x, int y);

	int m_nWidth, m_nHeight;
	int m_nSubType;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Width", &m_nWidth)
		DECL_PROP("SubType", &m_nSubType)
	END_DECLARE_PROPERTYMAP()
};