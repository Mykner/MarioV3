#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class FlagPole : public EventObjectBase
{
public:
	FlagPole(Game *pGame, Stage *pStage);
	virtual ~FlagPole();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void OnChangeState(int nState);

	void OnFlagDown();
	virtual bool CanApplied();

	virtual SIZE GetSize();

	int m_nFlagHeight;
};

