#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class DoorKeyCoin : public ItemObjectBase
{
public:
	DoorKeyCoin(Game *pGame, Stage *pStage);
	virtual ~DoorKeyCoin();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);
	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void OnJump();
	virtual void OnPushedUp();

	virtual bool IsOutOfCamera();
	virtual bool CanApplied();

	virtual int GetSpriteIndex();

	bool m_bIsLastKey;
	POINT m_ptEffectBase;
};