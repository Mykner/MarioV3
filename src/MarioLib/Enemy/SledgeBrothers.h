#pragma once

#include "Common.h"

#include "HammerBrothers.h"

class SledgeBrothers : public HammerBrothers
{
public:
	SledgeBrothers(Game *pGame, Stage *pStage);
	virtual ~SledgeBrothers();

	virtual void OnChangeState(int nState);

	virtual void OnStand();
	virtual void OnMoveUp();
	virtual void OnMoveDown();
	virtual void OnFall();
	virtual void OnFly();
	virtual void OnLanding();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetSpriteIndex();
	virtual SIZE GetSize();

	virtual POINT GetHandPosition();
	bool m_bPassGround;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

