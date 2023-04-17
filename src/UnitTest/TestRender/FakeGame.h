#pragma once

#include "Game.h"
#include "NaPlatformWindow.h"

class Sprite;
class FakeGame : public Game
{
public:
	FakeGame();
	virtual ~FakeGame();

	virtual void Init(NaPlatformWindowHandle hWindow, const wchar_t* szCurrentDir);
	virtual void ProcessGameLoop();
	virtual void Render();

	int m_nFrame;
	Sprite *m_pSprite;
};

