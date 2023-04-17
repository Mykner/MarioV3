#pragma once

#include "MapObjectBase.h"

class Game;
class Sprite;
class Stage;
class MapObjectPipe;
class MapObjectPipeExit : public MapObjectBase
{
public:
	MapObjectPipeExit(Game *pGame, Stage *pStage);
	virtual ~MapObjectPipeExit();

	virtual bool Tileize(int nPhase = 0);
	virtual void Install();

	virtual SIZE GetSize();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	MapObjectBase* FindPipe();
	int GetPipeDir(MapObjectPipe *pPipe);

	MapObjectBase *m_pPipe;
	int m_nExitType;
	Sprite *m_pSpriteDesign;
};

