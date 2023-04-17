#pragma once

#include "Common.h"

#include "GameObjectBase.h"

#include "Mushroom.h"

class Mushroom1UP : public Mushroom
{
public:
	Mushroom1UP(Game *pGame, Stage *pStage);
	virtual ~Mushroom1UP();

	virtual int GetSpriteIndex();
};

