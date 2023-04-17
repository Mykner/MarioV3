#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"
#include "BulletObjectBase.h"

class HammerBrothers : public EnemyObjectBase
{
public:
	HammerBrothers(Game *pGame, Stage *pStage);
	virtual ~HammerBrothers();

	virtual void OnAfterProcess();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void SetState(int nState);

	virtual void OnIdle();
	virtual void OnWalk();
	virtual void OnMoveUp();
	virtual void OnMoveDown();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnStacked();
	virtual void OnItemPop();
	virtual void OnItemPopDown();
	virtual void OnRide();

	virtual void OnPressed();
	virtual bool IsAlive();

	virtual bool CheckCreateWeapon();
	virtual void CheckThrowWeapon();
	void CreateWeapon();
	virtual void UnlinkWeapons();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex(); 
	virtual int GetSpriteIndex();
	virtual POINT GetHandPosition();

	int m_nInitialX;
	int m_nThrowWeaponCooltime;
	BulletObjectBase *m_pWeapon;
	int m_nJump;
	int m_nWeaponType;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

