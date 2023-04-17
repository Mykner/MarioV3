#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class BoomBoom : public EnemyObjectBase
{
public:
	BoomBoom(Game *pGame, Stage *pStage);
	virtual ~BoomBoom();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnWalk();
	virtual void OnJump();
	virtual void OnPressed();
	virtual bool IsAlive();

	void SplitToTwoBoomBooms();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();

	int m_nJump;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};