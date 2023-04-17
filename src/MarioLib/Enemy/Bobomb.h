#pragma once

#include "Common.h"

#include "EnemyObjectBase.h"

class Bobomb : public EnemyObjectBase
{
public:
	Bobomb(Game *pGame, Stage *pStage);
	virtual ~Bobomb();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);

	virtual void OnAfterProcess();

	virtual void OnStand();
	virtual void OnFall();
	virtual void OnFly();
	virtual void OnPressed();
	virtual void OnLanding();
	virtual void OnKicked();

	void ExplodeBomb();

	virtual bool IsAlive();

	virtual bool CanDamageEnemy();
	virtual bool CanBurnt(int nType);
	virtual bool CanKicked();
	virtual bool CanCarried();

	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);

	int m_nBombTimer;
	int m_nKickedCooltime;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("PowerUp", &m_bPowerUp)
		DECL_PROP("Winged", &m_bWinged)
	END_DECLARE_PROPERTYMAP()
};

