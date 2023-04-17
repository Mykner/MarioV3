#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectTwinkle : public EffectObjectBase
{
public:
	EffectTwinkle(Game *pGame, Stage *pStage);
	virtual ~EffectTwinkle();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();

	int m_nSubType;
};

