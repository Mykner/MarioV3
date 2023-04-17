#pragma once

#include "Common.h"

#include "BulletObjectBase.h"

class ThrowingHammer : public BulletObjectBase
{
public:
	ThrowingHammer(Game *pGame, Stage *pStage);
	virtual ~ThrowingHammer();
	
	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnIdle();
	virtual void OnWalk();
    virtual void OnBurnt();
	
	virtual void OnDamageEnemy();
	virtual void OnDamagePlayer();

	virtual bool CanEat();
	virtual bool CanSpit();

	virtual SIZE GetSize();
	virtual int GetSpriteIndex();
};

