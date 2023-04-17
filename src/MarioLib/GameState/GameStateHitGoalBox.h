#pragma once

#include "GameStateBase.h"

class GameStateHitGoalBox : public GameStateBase
{
public:
	GameStateHitGoalBox(Game *pGame);
	virtual ~GameStateHitGoalBox();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	int m_nTimeStamp;
};

