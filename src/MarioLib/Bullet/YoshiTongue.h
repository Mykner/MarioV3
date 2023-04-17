#pragma once

#include "Common.h"

#include "BulletObjectBase.h"

class YoshiTongue : public BulletObjectBase
{
public:
	YoshiTongue(Game *pGame, Stage *pStage);
	virtual ~YoshiTongue();
	
	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void CheckCollisionEnemy();

	virtual void OnWalk();

	void OnDamage();

	virtual NaRect GetRect();
	virtual SIZE GetSize();

	bool m_bUsed;
	int m_nLength;
	GameObjectBase *m_pOwner;
	GameObjectBase *m_pHold;
};