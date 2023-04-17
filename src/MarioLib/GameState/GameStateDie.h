#pragma once

#include "GameStateBase.h"
#include "Game.h"
#include "ServerTransaction.h"

class GameStateDie : public GameStateBase
{
public:
	GameStateDie(Game *pGame);
	virtual ~GameStateDie();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	static void UpdateDeathPositions(void* pParam);

	ServerTransaction m_Transaction;
	NaStrArray m_arDeathPosition;
	int m_nCurSubStageIndex;
};

