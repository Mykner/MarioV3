#pragma once

#include "GameStateBase.h"

class GameObjectBase;
class GameStateExitDoor : public GameStateBase
{
public:
	GameStateExitDoor(Game *pGame);
	virtual ~GameStateExitDoor();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	GameObjectBase *m_pDoorEntrance;
	GameObjectBase *m_pDoorExit;
};