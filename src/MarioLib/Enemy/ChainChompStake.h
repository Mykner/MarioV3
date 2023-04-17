#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class ChainChomp;
class ChainChompStake : public EnemyObjectBase
{
public:
	ChainChompStake(Game *pGame, Stage *pStage);
	virtual ~ChainChompStake();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnLanding();

	virtual bool IsAlive();

	virtual bool CanLand();
	virtual bool CanCollide(int nDir);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanEat();
	virtual bool CanDamageEnemy();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual NaRect GetRect();
	virtual SIZE GetSize();

	ChainChomp* m_pChainChomp;
};