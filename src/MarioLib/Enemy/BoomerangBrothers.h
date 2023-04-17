#pragma once

#include "Common.h"

#include "HammerBrothers.h"
#include "Boomerang.h"

class BoomerangBrothers : public HammerBrothers
{
public:
	BoomerangBrothers(Game *pGame, Stage *pStage);
	virtual ~BoomerangBrothers();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnWalk();

	virtual bool CheckCreateWeapon();
	virtual void UnlinkWeapons();

	virtual int GetSpriteIndex();
	virtual POINT GetHandPosition();
	virtual SIZE GetSize();

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

