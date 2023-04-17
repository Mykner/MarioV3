#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectSplash : public EffectObjectBase
{
public:
	EffectSplash(Game *pGame, Stage *pStage);
	virtual ~EffectSplash();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();

	int m_nSubType;
};

