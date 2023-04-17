#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectEggDebris : public EffectObjectBase
{
public:
	EffectEggDebris(Game *pGame, Stage *pStage);
	virtual ~EffectEggDebris();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	int m_nSubType;
	Sprite **m_ppSpriteTile;
};

