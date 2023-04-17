#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Bloober : public EnemyObjectBase
{
public:
	Bloober(Game *pGame, Stage *pStage);
	virtual ~Bloober();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnSwim();
	virtual void OnSwimJump();
	virtual void OnPressed();
	virtual bool IsAlive();

	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBounceTrampolinMiniHorz();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex(); 
	virtual int GetSpriteIndex();

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

