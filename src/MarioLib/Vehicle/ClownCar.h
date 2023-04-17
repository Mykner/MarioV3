#pragma once

#include "Common.h"

#include "VehicleObjectBase.h"

class ClownCar : public VehicleObjectBase
{
public:
	ClownCar(Game *pGame, Stage *pStage);
	virtual ~ClownCar();

	virtual void OnAfterProcess();

	virtual void OnFly();

	virtual void PositionPassenger();
	virtual void OnRidePassenger(GameObjectBase * pPassenger);
	virtual void OnGetOffPassenger();
	virtual void OnDamagedPassenger();

	void OnBounce();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnTrampolinJump();

	virtual void OnControlBy(GameObjectBase *pRider);

	virtual bool CanPressEnemy();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();
	virtual int GetBodySpriteIndex();
	virtual int GetWingSpriteIndex();
	POINT GetWingSpriteOffset();

	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual NaRect GetRect();
	virtual SIZE GetSize();

	int m_nBounceCooltime;
};