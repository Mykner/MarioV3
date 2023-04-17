#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class TurtleShell : public ItemObjectBase
{
public:
	TurtleShell(Game *pGame, Stage *pStage);
	virtual ~TurtleShell();

	virtual void OnThrown();

	virtual int GetSpriteIndex();
};

