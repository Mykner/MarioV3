#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectPop : public EffectObjectBase
{
public:
	EffectPop(Game *pGame, Stage *pStage);
	virtual ~EffectPop();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual int GetSpriteIndex();
};

