#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectCoin : public EffectObjectBase
{
public:
	EffectCoin(Game *pGame, Stage *pStage);
	virtual ~EffectCoin();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual int GetSpriteIndex();

	ControllerBase * m_pPlayer;
};

