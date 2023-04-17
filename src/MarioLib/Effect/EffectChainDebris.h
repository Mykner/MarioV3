#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectChainDebris : public EffectObjectBase
{
public:
	EffectChainDebris(Game *pGame, Stage *pStage);
	virtual ~EffectChainDebris();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
};

