#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class CheepCheepFly : public EnemyObjectBase
{
public:
	CheepCheepFly(Game *pGame, Stage *pStage);
	virtual ~CheepCheepFly();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnIdle();
	virtual void OnStand();
	virtual void OnJump();
	virtual void OnPressed();
	virtual bool IsAlive();

	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);

	virtual int GetSpriteIndex();
};

