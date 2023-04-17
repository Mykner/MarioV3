#pragma once

#include "Common.h"

#include "BulletObjectBase.h"

class SpinySpike : public BulletObjectBase
{
public:
	SpinySpike(Game *pGame, Stage *pStage);
	virtual ~SpinySpike();
	
	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnWalk();
	void OnDamage();
	void OnDamagePlayer();

	virtual bool CanEat();
	virtual bool CanSpit();

	virtual SIZE GetSize();
	virtual int GetSpriteIndex();

	GameObjectBase *m_pOwner;
	int m_nLifeFrame;
	int m_nTargetY;
};

