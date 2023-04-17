#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class ChainChomp : public EnemyObjectBase
{
public:
	ChainChomp(Game *pGame, Stage *pStage);
	virtual ~ChainChomp();

	virtual bool CanCollisionWith(GameObjectBase *pOther);

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderChain(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);
	
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnIdle();
	virtual void OnWalk();
	virtual void OnFly();
	virtual void OnStand();
	virtual void OnJump();
	virtual void OnRetreat();
	virtual void OnTrampolinJump();
	virtual void OnFall();

	bool CheckChainLimit();
	void MakeChainDebris();
	void CheckStakeAvailable();
	void PositionChain();
	void UnlinkStake();
	
	virtual bool IsAlive();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetSpriteIndex();

	GameObjectBase* m_pStake;
	bool m_bStake;
	float m_fAngle;
	NaPointT<float> m_ptTarget;
	NaPointT<float> m_ptChain[4];

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("ChainedToStake", &m_bStake)
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};