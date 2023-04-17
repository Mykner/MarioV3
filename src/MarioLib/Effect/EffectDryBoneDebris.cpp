#include "EffectDryBoneDebris.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectDryBoneDebris::EffectDryBoneDebris(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_DRYBONEDEBRIS;
	m_nState = STATE_IDLE;

	m_nSubType = 0;
}

EffectDryBoneDebris::~EffectDryBoneDebris()
{
}

void EffectDryBoneDebris::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	m_fXS = pParam->fXS;
	m_fYS = pParam->fYS;
	m_bFlip = pParam->pFromObject->m_bFlip;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	m_nSubType = pParam->nType;
}

void EffectDryBoneDebris::Process()
{
	if (m_nState != STATE_DEAD && IsOutOfStage())
	{
		ChangeState(STATE_DEAD);
	}
	else
	{
		if (m_fYS < 4.0f)
			m_fYS += 0.125f;
	}
}

void EffectDryBoneDebris::Render(int nColor, int nZOrder)
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
		fAngle = m_nStateFrame * abs(m_fXS * 12.0f);
	}

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
}

int EffectDryBoneDebris::GetSpriteIndex()
{
	int nFrame = SPRIDX_EFFECT_DRYBONE_HEAD + m_nSubType;

	return nFrame;
}
