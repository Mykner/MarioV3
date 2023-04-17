#pragma once

#include "Common.h"

#include "BulletObjectBase.h"

class Wrench : public BulletObjectBase
{
public:
	Wrench(Game *pGame, Stage *pStage);
	virtual ~Wrench();
	
	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnWalk();

	void OnDamage();
	void OnDamagePlayer();

	virtual bool CanEat();

	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();
	virtual int GetSpriteIndex();

	GameObjectBase *m_pOwner;
	int m_nLifeFrame;
	int m_nTargetY;
};

