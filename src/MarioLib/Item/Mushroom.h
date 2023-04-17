#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class Mushroom : public ItemObjectBase
{
public:
	Mushroom(Game *pGame, Stage *pStage);
	virtual ~Mushroom();

	virtual int GetSpriteIndex();
};

