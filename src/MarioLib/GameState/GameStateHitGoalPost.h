#pragma once

#include "GameStateBase.h"

class GameStateHitGoalPost : public GameStateBase
{
public:
	GameStateHitGoalPost(Game *pGame);
	virtual ~GameStateHitGoalPost();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	int m_nTimeStamp;
};

