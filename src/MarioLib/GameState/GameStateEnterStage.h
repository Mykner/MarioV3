#pragma once

#include "GameStateBase.h"

class GameStateEnterStage : public GameStateBase
{
public:
	GameStateEnterStage(Game *pGame);
	virtual ~GameStateEnterStage();

	virtual void Init();
	virtual void Process();
	virtual void Render();
};

