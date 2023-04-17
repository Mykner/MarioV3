#pragma once

#include "MapObjectBase.h"

class Game;
class MapObjectCheckPoint : public MapObjectBase
{
public:
	MapObjectCheckPoint(Game *pGame, Stage *pStage);
	virtual ~MapObjectCheckPoint();

	virtual void Install();
	virtual bool Tileize(int nPhase = 0);
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	Sprite *m_pSpritePlayer;
};

