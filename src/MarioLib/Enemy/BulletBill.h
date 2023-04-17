#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class BulletBill : public EnemyObjectBase
{
public:
	BulletBill(Game *pGame, Stage *pStage);
	virtual ~BulletBill();

	virtual void OnBeforeProcess();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnThrown();
	virtual void OnWalk();
	virtual void OnPressed();

	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanBounceTrampolinMiniHorz();

	virtual bool IsAlive();

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
};

