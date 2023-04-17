#include "EffectStarDust.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectStarDust::EffectStarDust(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_STARDUST;
	m_nState = STATE_IDLE;
}

EffectStarDust::~EffectStarDust()
{
}

void EffectStarDust::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	m_fXS = pParam->fXS;
	m_fYS = pParam->fYS;
}

void EffectStarDust::Process()
{
	if (m_nState != STATE_DEAD)
	{
		if (m_nStateFrame == 22)
			ChangeState(STATE_DEAD);
	}
}

void EffectStarDust::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (m_nStateFrame > 18)
	{
		nColor -= ((0x30000000) * (m_nStateFrame - 18));
	}

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT);
}

int EffectStarDust::GetSpriteIndex()
{
	return SPRITE_INDEX_CYCLE(SPRIDX_EFFECT_STARDUST1, 2, 1);
}
