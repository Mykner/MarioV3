#pragma once

#include "Common.h"

#include "MapObjectBase.h"
#include "PropertyMap.h"

class MapObjectMessageBlock : public MapObjectBase
{
public:
	MapObjectMessageBlock(Game *pGame, Stage *pStage);
	virtual ~MapObjectMessageBlock();

	virtual bool Tileize(int nPhase = 0);
	virtual void Install();
	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	NaString m_strMessage;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Message", &m_strMessage },
	END_DECLARE_PROPERTYMAP()
};