#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectStarDust : public EffectObjectBase
{
public:
	EffectStarDust(Game *pGame, Stage *pStage);
	virtual ~EffectStarDust();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();
};

