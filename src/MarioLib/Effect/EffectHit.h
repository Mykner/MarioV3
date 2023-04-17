#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectHit : public EffectObjectBase
{
public:
	EffectHit(Game *pGame, Stage *pStage);
	virtual ~EffectHit();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual int GetSpriteIndex();
};

