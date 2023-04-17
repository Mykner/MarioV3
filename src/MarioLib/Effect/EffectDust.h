#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectDust : public EffectObjectBase
{
public:
	EffectDust(Game *pGame, Stage *pStage);
	virtual ~EffectDust();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();

	int m_nSubType;
};

