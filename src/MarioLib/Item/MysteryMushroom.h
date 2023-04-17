#pragma once

#include "Common.h"

#include "GameObjectBase.h"

#include "Mushroom.h"

class MysteryMushroom : public Mushroom
{
public:
	MysteryMushroom(Game *pGame, Stage *pStage);
	virtual ~MysteryMushroom();

	virtual int GetSpriteIndex();
	virtual const wchar_t * GetTypeName();
};

