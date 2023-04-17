#pragma once

#include "Game.h"
#include "NaPlatformWindow.h"

class DesignPlayGame : public Game
{
public:
	DesignPlayGame();
	virtual ~DesignPlayGame();

	virtual void Init(NaPlatformWindowHandle hWindow, const wchar_t* szCurrentDir);
	virtual void OnChangeState();

	void InitFromGame(Game* pGame);
	void RestoreGame();
	Game *m_pGame;

	bool m_bExitFlag;
};

