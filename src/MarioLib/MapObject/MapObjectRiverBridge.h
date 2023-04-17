#pragma once

#include "Common.h"

#include "MapObjectBase.h"
#include "PropertyMap.h"

class MapObjectRiverBridge : public MapObjectBase
{
public:
	MapObjectRiverBridge(Game *pGame, Stage *pStage);
	virtual ~MapObjectRiverBridge();

	virtual bool Tileize(int nPhase = 0);
	virtual void Install();
	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	int m_nWidth;
	int m_nRealWidth;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Width", &m_nWidth },
	END_DECLARE_PROPERTYMAP()
};