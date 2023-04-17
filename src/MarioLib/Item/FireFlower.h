#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class FireFlower : public ItemObjectBase
{
public:
	FireFlower(Game *pGame, Stage *pStage);
	virtual ~FireFlower();

	virtual void OnChangeState(int nState);
	virtual void OnThrown();

	virtual int GetSpriteIndex();
};

