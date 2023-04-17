#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

#include <vector>

class FireBall;
class FireBar : public EnemyObjectBase
{
public:
	FireBar(Game *pGame, Stage *pStage);
	virtual ~FireBar();

	virtual bool IsOverlapped(GameObjectBase *pOther);
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnIdle();
	virtual void OnStacked();

	virtual bool IsAlive();

	virtual bool IsInvinsible();
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBornFromBlock();

	virtual bool CanEat();

	virtual int GetZOrder();
	virtual std::vector<NaRect> GetRectList();

	void CreateFireBalls(Game *pGame, Stage *pStage);
	void PositionFireBalls();

	float m_fAngle;
	float m_fSpeed;
	int m_nLength;
	std::vector<FireBall*> m_vecFireBall;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Length", &m_nLength },
		{ L"Speed", &m_fSpeed },
		{ L"InitialAngle", &m_fAngle },
	END_DECLARE_PROPERTYMAP()
};

