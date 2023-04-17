#pragma once

#include "Common.h"

#include "HammerBrothers.h"
#include "Boomerang.h"

class FireBrothers : public HammerBrothers
{
public:
	FireBrothers(Game *pGame, Stage *pStage);
	virtual ~FireBrothers();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnWalk();

	virtual bool CheckCreateWeapon();
	virtual void CheckThrowWeapon();

	void ThrowFireball();

	virtual int GetSpriteIndex();
	virtual POINT GetMouthPosition();
	virtual SIZE GetSize();

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

