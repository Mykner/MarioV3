#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectSceneFade : public EffectObjectBase
{
public:
	EffectSceneFade(Game *pGame, Stage *pStage);
	virtual ~EffectSceneFade();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	bool m_bFinished;
	int m_nSubType;
	int m_nLength;
	int m_nStartDelay;
};

