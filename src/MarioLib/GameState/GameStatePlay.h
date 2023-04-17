#pragma once

#include "GameStateBase.h"
#include "Game.h"

class GameStatePlay : public GameStateBase
{
public:
	GameStatePlay(Game *pGame);
	virtual ~GameStatePlay();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	void InitPauseMenuUI();

	enum GameStateUI
	{
		UI_UNDEFINED = 0,
		UI_GOTO_TITLE,
		UI_GIVEUP,
		UI_GIVEUP_ALL,
		UI_CONTINUE,
	};
};

