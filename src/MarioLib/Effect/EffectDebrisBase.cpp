#include "EffectDebrisBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectDebrisBase::EffectDebrisBase(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_DEBRISBASE;
	m_nState = STATE_IDLE;

	m_ppSprite = nullptr;
	m_nSpriteIndex = -1;

	m_fXS = 0;
	m_fYS = -1.0f;
	m_fZS = 0;
}

EffectDebrisBase::~EffectDebrisBase()
{
}

void EffectDebrisBase::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fXS != 0)
		m_fXS = pParam->fXS;
	if (pParam->fYS != 0)
		m_fYS = pParam->fYS;
	if (pParam->fZS != 0)
		m_fZS = pParam->fZS;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	if (pParam->pSprite != nullptr)
		m_pSprite = pParam->pSprite;

	if (pParam->nType != -1)
		m_nSpriteIndex = pParam->nType;
}

void EffectDebrisBase::Process()
{
	if (m_nState != STATE_DEAD && IsOutOfStage())
	{
		ChangeState(STATE_DEAD);
	}
	else
	{
		if (m_fYS < 0)
			m_fYS += 0.1f;
		else if (m_fYS < 3.0f)
			m_fYS += 0.2f;
		else
			m_fXS *= 0.95f;
	}
}

void EffectDebrisBase::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	if (m_pSprite == nullptr || m_nSpriteIndex == -1)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	bool bShadow = IS_MODERN;
	float fAngle = m_nStateFrame * m_fZS;
	if (m_fXS < 0)
		fAngle *= -1;
	if (!m_bFlip)
		fAngle *= -1;

	m_pSprite->RenderToQueue(x - pt.x, y - pt.y, m_nSpriteIndex, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
}
