#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class BossBridgeAxe : public EventObjectBase
{
public:
	BossBridgeAxe(Game *pGame, Stage *pStage);
	virtual ~BossBridgeAxe();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnEventDead();

	virtual int GetSpriteIndex();

	virtual bool CanApplied();

	virtual SIZE GetSize();
};

