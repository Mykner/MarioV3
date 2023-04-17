#pragma once

#include "GameStateBase.h"

class GameStateGameOver : public GameStateBase
{
public:
	GameStateGameOver(Game *pGame);
	virtual ~GameStateGameOver();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();
};

