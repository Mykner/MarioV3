#pragma once

#include "GameStateBase.h"

class GameObjectBase;
class GameStateSkyIntro : public GameStateBase
{
public:
	GameStateSkyIntro(Game *pGame);
	virtual ~GameStateSkyIntro();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	GameObjectBase* FindPiranhaPlant();
};

