#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectParalyze : public EffectObjectBase
{
public:
	EffectParalyze(Game *pGame, Stage *pStage);
	virtual ~EffectParalyze();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	Sprite **m_ppSpriteTile;
};