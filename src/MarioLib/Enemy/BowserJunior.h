#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"
#include "ThrowingHammer.h"

class BowserJunior : public EnemyObjectBase
{
public:
	BowserJunior(Game *pGame, Stage *pStage);
	virtual ~BowserJunior();

	virtual void OnAfterProcess();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnWalk();
	virtual void OnShellRun();
	virtual void OnShellAwake();
	virtual void OnJump();
	virtual void OnFly();
	virtual void OnThrowFire();
	virtual void OnStacked();
	virtual void OnLanding();
	virtual void OnPressed();

	void OnDamaged();

	virtual bool IsAlive();

	void CheckThrowFire();

	virtual bool CanEat();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanDamageEnemy();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();

	void ThrowFire();
	void ThrowHammer();

	POINT GetMouthPosition();
	POINT GetHandPosition();

	int m_nThrowFireCooltime;

	bool m_bAimFire;
	int m_nDamagedCooltime;
	int m_nDamagedCount;
	int m_nJumpCount;
	int m_nActionPhase;

	bool m_bCanFire;
	bool m_bCanThrowHammer;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
		DECL_PROP("CanFire", &m_bCanFire)
		DECL_PROP("CanThrowHammer", &m_bCanThrowHammer)
	END_DECLARE_PROPERTYMAP()
};