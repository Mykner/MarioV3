#include "EffectHit.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectHit::EffectHit(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_HIT;
	m_nState = STATE_IDLE;
}

EffectHit::~EffectHit()
{
}

void EffectHit::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fXS != 0)
		m_fXS = pParam->fXS;
	if (pParam->fYS != 0)
		m_fYS = pParam->fYS;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
}

void EffectHit::Process()
{
	if (m_nState != STATE_DEAD)
	{
		if (m_nStateFrame / 2 == 1)
			m_bFlip = true;
		else
			m_bFlip = false;

		if (m_nStateFrame == 7)
			ChangeState(STATE_DEAD);
	}
}

int EffectHit::GetSpriteIndex()
{
	return SPRIDX_EFFECT_HIT;
}
