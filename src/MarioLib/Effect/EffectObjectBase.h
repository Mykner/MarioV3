#pragma once

#include "Common.h"

#include "GameObjectBase.h"

class Sprite;
class EffectObjectBase : public GameObjectBase
{
public:
	EffectObjectBase(Game *pGame, Stage *pStage);
	virtual ~EffectObjectBase();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void OnBeforeProcess();
	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();

	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	Player* m_pPlayer;
};

