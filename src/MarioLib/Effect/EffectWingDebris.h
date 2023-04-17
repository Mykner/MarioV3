#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectWingDebris : public EffectObjectBase
{
public:
	EffectWingDebris(Game *pGame, Stage *pStage);
	virtual ~EffectWingDebris();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	Sprite **m_ppSpriteTile;
};