#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Lakitu : public EnemyObjectBase
{
public:
	Lakitu(Game *pGame, Stage *pStage);
	virtual ~Lakitu();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnIdle();
	virtual void OnMove();
	void OnMoveInPage();
	void OnMoveToTarget();
	virtual void OnTrampolinJump();

	virtual void OnPressed();
	virtual bool IsAlive();
	virtual bool CanBounceTrampolinMiniHorz();

	void CheckThrowSpiny();
	void CheckAnotherLakitu();

	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanDamageEnemy();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();

	int m_nMoveMode;
	int m_nMaxCount;
	int m_nThrowCooltime;
	int m_nLastCameraX;

	int m_nThrowType;
	int m_nThrowTypeIndex;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"ThrowType", &m_nThrowTypeIndex },
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};
