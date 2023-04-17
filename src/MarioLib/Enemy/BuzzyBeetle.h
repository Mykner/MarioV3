#pragma once

#include "Common.h"

#include "Turtle.h"

class BuzzyBeetle : public Turtle
{
public:
	BuzzyBeetle(Game *pGame, Stage *pStage);
	virtual ~BuzzyBeetle();

	virtual void OnAfterProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnFly();

	virtual bool CanLand();
	virtual bool CanPressed(int nPower /* = POWER_PRESS_NORMAL */);
	virtual bool CanDamageEnemy();
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);

	virtual void AddPassenger(GameObjectBase *pObj);

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetSpriteIndex();

	std::vector<GameObjectBase*> m_vecPassenger;
};

