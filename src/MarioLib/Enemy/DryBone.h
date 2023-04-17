#pragma once

#include "Common.h"

#include "RedTurtle.h"

class DryBone : public RedTurtle
{
public:
	DryBone(Game *pGame, Stage *pStage);
	virtual ~DryBone();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnShellIdle();
	virtual void OnShellAwake();
	virtual void OnMove();

	void SplitParts();

	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanKicked();
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanCarried();

	virtual int GetSpriteIndex();

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

