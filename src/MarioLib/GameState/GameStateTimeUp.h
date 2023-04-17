#pragma once

#include "GameStateBase.h"
#include "Game.h"

class GameStateTimeUp : public GameStateBase
{
public:
	GameStateTimeUp(Game *pGame);
	virtual ~GameStateTimeUp();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();
};

