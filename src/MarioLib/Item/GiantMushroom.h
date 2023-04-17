#pragma once

#include "Common.h"

#include "GameObjectBase.h"

#include "Mushroom.h"

class GiantMushroom : public Mushroom
{
public:
	GiantMushroom(Game *pGame, Stage *pStage);
	virtual ~GiantMushroom();

	virtual void Process();

	virtual void OnItemPop();

	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);
};

