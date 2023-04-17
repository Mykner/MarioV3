#pragma once

#include "GameStateBase.h"
#include "Game.h"
#include "ServerTransaction.h"

class GameStateLogo : public GameStateBase
{
public:
	GameStateLogo(Game *pGame);
	virtual ~GameStateLogo();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	static void CheckPermissions(void *pParam);

	ServerTransaction m_Transaction;
	bool m_bHavePermission;
	NaString m_strLocalKey;
};

