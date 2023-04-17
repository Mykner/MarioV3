#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class StaticCoin : public ItemObjectBase
{
public:
	StaticCoin(Game *pGame, Stage *pStage);
	virtual ~StaticCoin();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void OnPushedUp();

	virtual bool CanApplied();

	virtual int GetSpriteIndex();
};

