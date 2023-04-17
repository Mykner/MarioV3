#pragma once

#include "GameStateBase.h"

class GameStateShapeChange : public GameStateBase
{
public:
	GameStateShapeChange(Game *pGame);
	virtual ~GameStateShapeChange();

	virtual void Process();
	virtual void Render();

	bool m_bDebugShapeChangeFrame;
};

