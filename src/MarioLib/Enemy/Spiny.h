#pragma once

#include "Common.h"

#include "Turtle.h"

class Spiny : public Turtle
{
public:
	Spiny(Game *pGame, Stage *pStage);
	virtual ~Spiny();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnThrown(); 
	virtual void OnFly();
	virtual void OnLanding();

	void ThrowSpike();

	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);	

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);

	int m_nThrowSpikeCooltime;
	bool m_bThrowSpikeDirection;
};