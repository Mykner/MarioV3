#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectDeathPosition : public EffectObjectBase
{
public:
	EffectDeathPosition(Game *pGame, Stage *pStage);
	virtual ~EffectDeathPosition();

	virtual void RequestSprites(Stage *pStage = nullptr);
	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual int GetSpriteIndex();

	bool m_bIsLargeMark;
	float m_fMarkScale;

	const float m_fLargeMarkScale = 0.5f;
	const float m_fSmallMarkScale = 0.3f;
	const int m_nMaxLifeFrame = 12800;
};

