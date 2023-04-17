#pragma once

#include "Common.h"

#include "BulletObjectBase.h"

class FireBall : public BulletObjectBase
{
public:
	FireBall(Game *pGame, Stage *pStage);
	virtual ~FireBall();
	
	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	void OnFireDown();
	void OnFireUp();
	virtual void OnFly();

	void OnDamage();
	void OnDamagePlayer();

	virtual bool CanEat();

	virtual void OnReachWall(bool bIsLeftWall);
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();
	virtual int GetSpriteIndex();

	GameObjectBase *m_pOwner;
	int m_nLifeFrame;
};

