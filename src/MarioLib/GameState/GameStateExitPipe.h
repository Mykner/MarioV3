#pragma once

#include "GameStateBase.h"

class GameObjectBase;
class GameStateExitPipe : public GameStateBase
{
public:
	GameStateExitPipe(Game *pGame);
	virtual ~GameStateExitPipe();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	GameObjectBase* FindPiranhaPlant();

	std::vector<GameObjectBase*> m_vecPiranhaPlant;
};