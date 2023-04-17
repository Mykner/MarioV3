#pragma once

#include "MapObjectBase.h"

class Game;
class Sprite;
class Stage;
class MapObjectSkyExit : public MapObjectBase
{
public:
	MapObjectSkyExit(Game *pGame, Stage *pStage);
	virtual ~MapObjectSkyExit();

	virtual bool Tileize(int nPhase = 0);

	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	Sprite *m_pSpriteDesign;
};

