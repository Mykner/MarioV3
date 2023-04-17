#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Podoboo : public EnemyObjectBase
{
public:
	Podoboo(Game *pGame, Stage *pStage);
	virtual ~Podoboo();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderWing(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnMoveUp();
	virtual void OnMoveDown();
	virtual void OnFly();
	
	virtual void OnTrampolinJump();
	virtual void OnBounceTrampolinMiniHorz(GameObjectBase *pTrampolin);

	virtual bool IsAlive();

	virtual bool CanSpit();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();
	virtual int GetZOrder();
	virtual void GetScale(float & fScaleX, float & fScaleY);

	int m_nInitialY;
	float m_fAngle;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

