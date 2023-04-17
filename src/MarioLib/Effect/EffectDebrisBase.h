#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectDebrisBase : public EffectObjectBase
{
public:
	EffectDebrisBase(Game *pGame, Stage *pStage);
	virtual ~EffectDebrisBase();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	float m_fZS;
};
