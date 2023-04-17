#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Wrench;
class RockyWrench : public EnemyObjectBase
{
public:
	RockyWrench(Game *pGame, Stage *pStage);
	virtual ~RockyWrench();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnAfterProcess();

	virtual void OnStand();
	virtual void OnMoveDown();
	virtual void OnMoveUp();
	virtual void OnLanding();
	virtual void OnJumpInto();
	virtual void OnStacked();

	void CreateWrench();
	void ThrowWrench();

	virtual bool IsAlive();

	virtual bool CanSpit();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanBornFromBlock();
	virtual bool CanDamageEnemy();
	virtual bool CanCollide(int nDir);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual SIZE GetSize();

	POINT GetHandPosition();

	int m_nBlockBottom;
	int m_nThrowCooltime;
	Wrench *m_pWrench;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

