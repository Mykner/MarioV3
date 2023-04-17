#include "EffectParalyze.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectParalyze::EffectParalyze(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_PARALYZE;
	m_nState = STATE_IDLE;
}

EffectParalyze::~EffectParalyze()
{
}

void EffectParalyze::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	if (pParam->pFromObject)
	{
		if (pParam->pFromObject->m_fX > m_fX)
			m_bFlip = true;
	}
}

void EffectParalyze::Process()
{
	if (m_nStateFrame == 24)
		ChangeState(STATE_DEAD);

	if (m_nState != STATE_DEAD && IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void EffectParalyze::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();
	bool bShadow = IS_MODERN;

	if (m_nStateFrame % 2 == 0)
	{
		if (m_bFlip)
			pt.x += 1;
		else
			pt.x -= 1;
	}

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow);
}

int EffectParalyze::GetSpriteIndex()
{
	int nFrame = SPRIDX_EFFECT_PARALYZE1;
	if (m_nStateFrame % 2 < 1)
		nFrame = SPRIDX_EFFECT_PARALYZE1;
	else
		nFrame = SPRIDX_EFFECT_PARALYZE2;

	return nFrame;
}
