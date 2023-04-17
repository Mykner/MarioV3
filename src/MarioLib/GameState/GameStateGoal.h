#pragma once

#include "GameStateBase.h"

class GameObjectBase;
class GameStateGoal : public GameStateBase
{
public:
	GameStateGoal(Game *pGame);
	virtual ~GameStateGoal();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	void FireBoom();

	int m_nTimeStamp;
	int m_nFireworksCount;
	int m_nFireworksFired;
	GameObjectBase* m_pCastle;
};

