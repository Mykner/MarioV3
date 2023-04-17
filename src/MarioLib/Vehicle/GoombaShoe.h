#pragma once

#include "Common.h"

#include "VehicleObjectBase.h"

class GoombaShoe : public VehicleObjectBase
{
public:
	GoombaShoe(Game *pGame, Stage *pStage);
	virtual ~GoombaShoe();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void OnStand();
	virtual void OnWalk();
	virtual void OnJump();
	virtual void OnFall();
	virtual void OnFly();
	virtual void OnGroundPound();

	int SmashAll();
	void SmashEnemies();

	virtual void OnItemPop();
	virtual void OnItemPopDown();
	virtual void OnPowerUp();

	virtual void OnBurnt();

	virtual void OnControlBy(GameObjectBase * pRider);

	virtual void PositionPassenger();
	virtual void OnRidePassenger(GameObjectBase * pPassenger);
	virtual void OnGetOffPassenger();

	virtual void OnDamagedPassenger();
	virtual void OnReachWall(bool bIsLeftWall);

	virtual bool IsAlive();
	virtual bool CanPressEnemy();
	virtual bool CheckFalling();
	
	virtual bool CanRided(GameObjectBase *pByWho);

	virtual int GetPower(int nPowerForWhat);
	virtual int GetSpriteIndex();
	virtual SIZE GetSpriteSize();
	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();

	virtual void GetScale(float & fScaleX, float & fScaleY);
	virtual NaRect GetRect();

	int m_nBounceCooltime;

	int m_nShoeType;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
		DECL_PROP("ShoeType", &m_nShoeType)
	END_DECLARE_PROPERTYMAP()
};