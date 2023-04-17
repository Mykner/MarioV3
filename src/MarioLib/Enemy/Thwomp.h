#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Thwomp : public EnemyObjectBase
{
public:
	Thwomp(Game *pGame, Stage *pStage);
	virtual ~Thwomp();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnFly();
	virtual void OnMoveDown();
	virtual void OnMoveUp();
	virtual void OnStand();
	virtual void OnStacked();

	void SmashAll(bool& bHitHard, bool& bHitSuperHard, bool& bHitUnbreakable);

	virtual bool IsAlive();

	virtual bool CanCollide(int nDir);
	virtual bool CanEat();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanPushedUp();
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual SIZE GetSize();

	int m_nInitialY;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

