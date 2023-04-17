#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"
#include "ThrowingHammer.h"

class Bowser : public EnemyObjectBase
{
public:
	Bowser(Game *pGame, Stage *pStage);
	virtual ~Bowser();

	virtual void OnAfterProcess();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnWalk();
	virtual void OnJump();
	virtual void OnFly();
	virtual void OnThrowFire();
	virtual void OnStacked();
	virtual void OnItemPop();
	virtual void OnItemPopDown();
	virtual void OnRide();
	virtual bool OnDamaged(GameObjectBase *pAttacker);

	virtual bool IsAlive();

	void CheckThrowFire();
	void CheckThrowHammer();

	virtual bool CanEat();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();

	void ThrowFire();
	POINT GetMouthPosition();
	POINT GetHandPosition();

	int m_nThrowFireCooltime;

	int m_nInitialX;
	bool m_bWalkFront;
	bool m_bAimFire;
	int m_nChargeFireFrame;
	int m_nDamagedCooltime;
	int m_nDamagedCount;
	int m_nThrowHammerCooltime;
	int m_nThrowHammerCombo;
	ThrowingHammer *m_pHammer;

	int m_nRealType;
	bool m_bCanFire;
	bool m_bCanThrowHammer;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"RealType", &m_nRealType },
		{ L"CanFire", &m_bCanFire },
		{ L"CanThrowHammer", &m_bCanThrowHammer },
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};