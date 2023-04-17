#pragma once

#include "Game.h"
#include "NaPlatformWindow.h"

class Sprite;
class GameObjectBase;
class FakeGame : public Game
{
public:
	FakeGame();
	virtual ~FakeGame();

	virtual void Init(NaPlatformWindowHandle hWindow, const wchar_t* szCurrentDir);
	virtual void ProcessGameLoop();
	virtual void Render();

	int m_nCurEdit;
	int m_nBody;
	int m_nHead;
	POINT m_ptOffset;
	Sprite *m_pSprite;
};

