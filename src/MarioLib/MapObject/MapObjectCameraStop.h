#pragma once

#include "MapObjectBase.h"

class Game;
class MapObjectCameraStop : public MapObjectBase
{
public:
	MapObjectCameraStop(Game *pGame, Stage *pStage);
	virtual ~MapObjectCameraStop();

	virtual void Install();
	virtual bool Tileize(int nPhase = 0);
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	Sprite *m_pSpritePlayer;
};

