#pragma once

#include "MapObjectBase.h"

class Game;
class MapObjectPipeBody : public MapObjectBase
{
public:
	MapObjectPipeBody(Game *pGame, Stage *pStage);
	virtual ~MapObjectPipeBody();

	virtual bool Tileize(int nPhase = 0);
	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	SIZE m_sizeBody;
};

