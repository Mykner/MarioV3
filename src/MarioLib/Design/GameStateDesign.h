#pragma once

#include "GameStateBase.h"

class GameStateDesign : public GameStateBase
{
public:
	GameStateDesign(Game *pGame);
	virtual ~GameStateDesign();

	virtual void Init();
	virtual void Process();
	virtual void Render();
};

