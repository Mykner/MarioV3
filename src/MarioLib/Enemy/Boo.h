#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Boo : public EnemyObjectBase
{
public:
	Boo(Game *pGame, Stage *pStage);
	virtual ~Boo();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnBeforeProcess();
	virtual void OnFly();
	virtual void OnMove();

	virtual bool IsAlive();

	virtual bool CanEat();
	virtual bool CanSpit();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanBornFromBlock();

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual SIZE GetSize();

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

