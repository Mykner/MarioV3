#pragma once

#include "Common.h"

#include "GameObjectBase.h"
#include "PropertyMap.h"

class EventObjectBase : public GameObjectBase
{
public:
	EventObjectBase(Game *pGame, Stage *pStage);
	virtual ~EventObjectBase();
	
	virtual bool CanLand();
	virtual bool CanCollide(int nDir);
	virtual bool CanApplied() { return false; };
	virtual bool CanBornFromBlock();
	virtual bool CanBurnt(int nType);

	virtual int GetSpriteIndex();
	virtual int GetMaskedType();

	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	bool m_bCanCollide;
	bool m_bCanLand;
	bool m_bEventDead;
};