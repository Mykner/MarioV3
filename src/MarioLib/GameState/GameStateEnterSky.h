#pragma once

#include "GameStateBase.h"

class PipeInfo;
class GameObjectBase;
class NaRect;
class GameStateEnterSky : public GameStateBase
{
public:
	GameStateEnterSky(Game *pGame);
	virtual ~GameStateEnterSky();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	GameObjectBase* FindVine();
};

