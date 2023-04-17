#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectScore : public EffectObjectBase
{
public:
	EffectScore(Game *pGame, Stage *pStage);
	virtual ~EffectScore();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	GameObjectBase *m_pFlagPole;
	int m_nScore;
	ControllerBase *m_pPlayer;
};