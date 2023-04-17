#pragma once

#include "Common.h"

#include "ItemObjectBase.h"

class ThrownCoin : public ItemObjectBase
{
public:
	ThrownCoin(Game *pGame, Stage *pStage);
	virtual ~ThrownCoin();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void OnJump();
	virtual void OnThrown();
	virtual void OnFall();
	virtual void OnThrownFly();
	virtual void OnPushedUp();

	virtual bool CanApplied();

	virtual int GetSpriteIndex();

	int m_nLifeFrame;
};

