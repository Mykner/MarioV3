#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class SuperStar : public ItemObjectBase
{
public:
	SuperStar(Game *pGame, Stage *pStage);
	virtual ~SuperStar();

	virtual void OnWalk();
	virtual int GetSpriteIndex();
};

