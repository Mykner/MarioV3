#pragma once

#include "Common.h"

#include "VehicleObjectBase.h"

class Yoshi : public VehicleObjectBase
{
public:
	Yoshi(Game *pGame, Stage *pStage);
	virtual ~Yoshi();

	virtual void CheckCollisionEnemy();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnAfterProcess();

	void ProcessHatch();

	virtual void OnHatch();
	virtual void OnIdle();

	virtual void PositionPassenger();
	virtual void OnRidePassenger(GameObjectBase * pPassenger);
	virtual void OnGetOffPassenger();
	virtual void OnDamagedPassenger();

	virtual void OnStand();
	virtual void OnWalk();
	virtual void OnFall();
	virtual void OnReachWall(bool bIsLeftWall);

	virtual void OnItemPop();
	virtual void OnItemPopDown();

	void OnKeepInMouth(GameObjectBase *pObj);
	void OnSwallow(GameObjectBase *pObj = nullptr);

	virtual void OnDamaged();

	void ConvertSpitType();

	virtual bool CanFire();
	virtual void ThrowFireball();

	virtual bool CanPressEnemy();
	virtual bool CanRided(GameObjectBase *pByWho);
	virtual bool CanStandOnSpike();

	virtual int GetPower(int nPowerForWhat);

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();
	
	int GetBodySpriteIndex();
	int GetHeadSpriteIndex();
	POINT GetHeadSpriteOffset();

	virtual int GetZOrder();
	virtual NaRect GetRect();
	virtual SIZE GetSize();
	virtual void GetScale(float &fScaleX, float &fScaleY);

	enum YoshiShapes
	{
		YOSHISHAPE_EGG = 0,
		YOSHISHAPE_BEGINHATCH,
		YOSHISHAPE_HATCHED,
	};

	int m_nKeepInMouthCooltime;
	int m_nKeepInMouthType;
	int m_nGulpCooltime;
	int m_nBodyFrame;
	int m_nHeadFrame;
	bool m_bHatched;
	int m_nHatchFrame;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Hatched", &m_bHatched)
	END_DECLARE_PROPERTYMAP()
};