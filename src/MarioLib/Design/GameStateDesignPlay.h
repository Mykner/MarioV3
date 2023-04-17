#pragma once

#include "GameStateBase.h"

class GameStateDesignPlay : public GameStateBase
{
public:
	GameStateDesignPlay(Game *pGame);
	virtual ~GameStateDesignPlay();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	int m_nStatusMode;
	bool m_bFrameCheck;
	int m_nObserveIdx;
};

