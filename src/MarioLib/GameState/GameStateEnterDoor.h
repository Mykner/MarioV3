#pragma once

#include "GameStateBase.h"

class PipeInfo;
class GameObjectBase;
class NaRect;
class GameStateEnterDoor : public GameStateBase
{
public:
	GameStateEnterDoor(Game *pGame);
	virtual ~GameStateEnterDoor();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	GameObjectBase *m_pDoorEntrance;
	GameObjectBase *m_pDoorExit;
};