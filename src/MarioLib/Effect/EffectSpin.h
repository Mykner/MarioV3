#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectSpin : public EffectObjectBase
{
public:
	EffectSpin(Game *pGame, Stage *pStage);
	virtual ~EffectSpin();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	Player* m_pPlayer;
};

