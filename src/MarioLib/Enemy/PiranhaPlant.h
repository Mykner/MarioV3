#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class PiranhaPlant : public EnemyObjectBase
{
public:
	PiranhaPlant(Game *pGame, Stage *pStage);
	virtual ~PiranhaPlant();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnStand();
	virtual void OnJump();
	virtual void OnFly();
	virtual void OnMoveUp();
	virtual void OnMoveDown();
	virtual void OnLanding();
	virtual void OnStacked();
	virtual void OnRide();

	bool CheckPipeExist();
	void HideForExitPipe();
	void CheckLookAtPlayer();
	void ThrowFireBall();

	virtual bool IsAlive();

	virtual bool CanLand();
	virtual bool CanCollide(int nDir);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanEat();
	virtual bool CanDamageEnemy();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual NaRect GetRect();
	virtual SIZE GetSize();

	int m_nPipeDepth;
	float m_fInitialY;
	bool m_bHideForExitPipe;
	bool m_bLookAtDown;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

