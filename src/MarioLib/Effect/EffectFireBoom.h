#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectFireBoom : public EffectObjectBase
{
public:
	EffectFireBoom(Game *pGame, Stage *pStage);
	virtual ~EffectFireBoom();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual int GetSpriteIndex();

	Player* m_pPlayer;
	int m_nSubType;
};