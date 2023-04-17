#pragma once

#include "Game.h"
#include "NaPlatformWindow.h"

class DesignGame : public Game
{
public:
	DesignGame();
	virtual ~DesignGame();

	virtual void Init(NaPlatformWindowHandle hWindow, const wchar_t* szCurrentDir);

	virtual void InitState(int nState);

	NaStrArray m_arGameTheme;
};
