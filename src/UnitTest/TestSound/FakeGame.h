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

	void InitUI();

	enum TestSoundUI
	{
		UI_UNDEFINED = 0,
		UI_FREQUENCY,
		UI_PLAY_BGM,
	};

	Sprite *m_pSprite;
};

