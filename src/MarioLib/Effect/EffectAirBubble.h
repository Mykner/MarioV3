#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectAirBubble : public EffectObjectBase
{
public:
	EffectAirBubble(Game *pGame, Stage *pStage);
	virtual ~EffectAirBubble();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	Player* m_pPlayer;
	int m_nDuration;
};