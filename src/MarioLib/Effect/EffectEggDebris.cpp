#include "EffectEggDebris.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectEggDebris::EffectEggDebris(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_EGGDEBRIS;
	m_nState = STATE_IDLE;

	m_nSubType = 0;
}

EffectEggDebris::~EffectEggDebris()
{
}

void EffectEggDebris::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	m_fXS = pParam->fXS;
	m_fYS = pParam->fYS;

	m_nSubType = pParam->nType;
}

void EffectEggDebris::Process()
{
	if (m_nState != STATE_DEAD && IsOutOfStage())
	{
		ChangeState(STATE_DEAD);
	}
	else
	{
		m_fYS += 0.25f;
	}
}

void EffectEggDebris::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	bool bShadow = false;
	float fAngle = 0;
	if (m_pGame->m_bModernStyle)
	{
		bShadow = true;
    }
    
    if (m_pGame->m_bModernStyle)
    {
		fAngle = m_nStateFrame * (0.1f);
	}

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
}

int EffectEggDebris::GetSpriteIndex()
{
	int nFrame = SPRIDX_EFFECT_YOSHI_EGGDEBRIS1_LT + m_nSubType;
	
	if (m_nStateFrame > 16)
		nFrame += 4;

	return nFrame;
}
