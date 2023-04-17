#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectSceneCircle : public EffectObjectBase
{
public:
	EffectSceneCircle(Game *pGame, Stage *pStage);
	virtual ~EffectSceneCircle();

	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	bool m_bFinished;
	int m_nSubType;
	int m_nLength;
	int m_nStartDelay;

	Sprite *m_pSpriteCircle;
};

