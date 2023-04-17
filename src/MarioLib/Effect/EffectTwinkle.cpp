#include "EffectTwinkle.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectTwinkle::EffectTwinkle(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_TWINKLE;
	m_nState = STATE_IDLE;

	m_nSubType = EFFECT_TWINKLE;
}

EffectTwinkle::~EffectTwinkle()
{
}

void EffectTwinkle::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	m_fXS = pParam->fXS;
	m_fYS = pParam->fYS;

	if (pParam->nType != -1)
		m_nSubType = pParam->nType;
}

void EffectTwinkle::Process()
{
	if (m_nState != STATE_DEAD)
	{
		if (GetSpriteIndex() == -1)
			ChangeState(STATE_DEAD);
	}
}

void EffectTwinkle::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();
	
	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, 
		m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, IS_MODERN);
}

int EffectTwinkle::GetSpriteIndex()
{
	switch (m_nSubType)
	{
	default:
	case EFFECT_TWINKLE:
		{
			int nBase = SPRIDX_EFFECT_TWINKLE_PINK1;
			int nFrames[] = {
				//0, 1, 3, 2, 2, 4, 4, 0,
				0, 0, 1, 1, 3, 3, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 0, 0,
			};
			int nLen = sizeof(nFrames) / sizeof(nFrames[0]);
			if (m_nStateFrame >= nLen)
				return -1;

			return nBase + nFrames[m_nStateFrame];
		}
		break;
	case EFFECT_GOLDTWINKLE:
		{
			int nBase = SPRIDX_EFFECT_TWINKLE_GOLD1;
			int nFrames[] = {
				0, 3, 3, 3, 2, 2, 1, 1, 1, 0, 0,
			};

			//-> Last 0 is Half sized

			int nLen = sizeof(nFrames) / sizeof(nFrames[0]);
			if (m_nStateFrame >= nLen)
				return -1;

			return nBase + nFrames[m_nStateFrame];
		}
		break;
	}
	return SPRITE_INDEX_CYCLE(SPRIDX_EFFECT_STARDUST1, 2, 1);
}
