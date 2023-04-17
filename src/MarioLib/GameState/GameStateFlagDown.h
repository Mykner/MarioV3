#pragma once

#include "GameStateBase.h"

class GameStateFlagDown : public GameStateBase
{
public:
	GameStateFlagDown(Game *pGame);
	virtual ~GameStateFlagDown();

	virtual void Init();
	virtual void Process();
	virtual void Render();


};

