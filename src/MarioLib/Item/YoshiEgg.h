#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class YoshiEgg : public ItemObjectBase
{
public:
	YoshiEgg(Game *pGame, Stage *pStage);
	virtual ~YoshiEgg();

	virtual void OnItemPop();
	virtual void OnStand();

	virtual bool CanApplied();

	virtual int GetSpriteIndex();
};

