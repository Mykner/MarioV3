#pragma once

#include "Common.h"

#include "EffectObjectBase.h"

class EffectBrickDebris : public EffectObjectBase
{
public:
	EffectBrickDebris(Game *pGame, Stage *pStage);
	virtual ~EffectBrickDebris();

	virtual void ParseParameter(CreateParameter *pParam);

	virtual void Process();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();

	Player* m_pPlayer;
	int m_nTileType;
	Sprite **m_ppSpriteTile;
};

