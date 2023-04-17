#pragma once

#include "Common.h"

#include "ControllableObject.h"

class Sprite;
class VehicleObjectBase : public ControllableObject
{
public:
	VehicleObjectBase(Game *pGame, Stage *pStage);
	virtual ~VehicleObjectBase();

	virtual void Process();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual bool CheckRideVehicle();

	virtual void OnIdle();
	virtual void OnStand();
	virtual void OnPipeUp();

	virtual void OnControlBy(GameObjectBase *pRider) {};

	virtual void PositionPassenger();
	virtual void OnRidePassenger(GameObjectBase *pPassenger);
	virtual void OnGetOffPassenger();
	virtual void OnDamagedPassenger();

	virtual bool IsOpponent(GameObjectBase *pObj);
	virtual bool HasMushroom();
	virtual bool CanRided(GameObjectBase *pByWho);
	virtual bool CanBornFromBlock();
	virtual bool CanCarry();

	virtual void SetPosition(float fX, float fY);

	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual NaRect GetRect();
	virtual int GetMaskedType();

	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	int m_nRideType;
	GameObjectBase* m_pPassenger;
};