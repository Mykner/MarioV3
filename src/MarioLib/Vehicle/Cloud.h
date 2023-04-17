#pragma once

#include "Common.h"

#include "VehicleObjectBase.h"

class Cloud : public VehicleObjectBase
{
public:
	Cloud(Game *pGame, Stage *pStage);
	virtual ~Cloud();

	virtual void RequestSprites(Stage *pStage = nullptr);
	virtual void OnAfterProcess();

	virtual void OnFly();
	virtual void OnFall();

	virtual void OnItemPop();
	virtual void OnItemPopDown();

	virtual void PositionPassenger();
	virtual void OnRidePassenger(GameObjectBase * pPassenger);
	virtual void OnGetOffPassenger();

	virtual bool CanPressEnemy();

	virtual int GetSpriteIndex();

	virtual NaRect GetRect();

	int m_nLifeFrame;
};