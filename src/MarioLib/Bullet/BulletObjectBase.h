#pragma once

#include "Common.h"

#include "GameObjectBase.h"

class BulletObjectBase : public GameObjectBase
{
public:
	BulletObjectBase(Game *pGame, Stage *pStage);
	virtual ~BulletObjectBase();
	
	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void OnBeforeProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnDamageEnemy();
	virtual void OnDamagePlayer();

	virtual bool CanEat();
	virtual bool CanSpit();
	virtual int GetSpriteIndex();

	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	GameObjectBase *m_pOwner;
	int m_nLifeFrame;
};
