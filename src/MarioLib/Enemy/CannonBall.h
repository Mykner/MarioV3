#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class CannonBall : public EnemyObjectBase
{
public:
	CannonBall(Game *pGame, Stage *pStage);
	virtual ~CannonBall();

	virtual void OnBeforeProcess();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnThrown();
	virtual void OnThrownFly();
	virtual void OnWalk();
	virtual void OnPressed();

	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanBounceTrampolinMiniHorz();

	virtual int GetSpriteIndex();
};

