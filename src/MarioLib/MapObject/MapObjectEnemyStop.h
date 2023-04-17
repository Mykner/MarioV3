#pragma once

#include "MapObjectBase.h"

class Game;
class MapObjectEnemyStop : public MapObjectBase
{
public:
	MapObjectEnemyStop(Game *pGame, Stage *pStage);
	virtual ~MapObjectEnemyStop();

	virtual void Install();
	virtual bool Tileize(int nPhase = 0);
	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	Sprite *m_pSpritePlayer;
};

