#include "EffectSceneCircle.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ResourceHolder.h"

EffectSceneCircle::EffectSceneCircle(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_SCENE_CIRCLE;
	m_nState = STATE_IDLE;

	RequestSprites(pStage);

	m_nSubType = EFFECT_SCENE_CIRCLEOUT;

	m_nLength = 256;
	m_nStartDelay = 0;
	m_bFinished = false;
}

EffectSceneCircle::~EffectSceneCircle()
{
}

void EffectSceneCircle::RequestSprites(Stage * pStage)
{
	m_pSprite = SPR_MANAGER->Request(L"Etc");
	m_pSpriteCircle = SPR_MANAGER->Request(L"EtcBigCircle");
}

void EffectSceneCircle::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->nDuration != 0)
		m_nLength = pParam->nDuration;
	if (pParam->nStartDelay != 0)
		m_nStartDelay = pParam->nStartDelay;
}

void EffectSceneCircle::Process()
{
	if (m_nState != STATE_DEAD)
	{
		if (!m_bFinished && m_nStateFrame == m_nStartDelay + m_nLength)
			m_bFinished = true;

		// Stay 24 frame.
		if (m_nStateFrame == m_nStartDelay + m_nLength + 24)
			ChangeState(STATE_DEAD);
	}
}

void EffectSceneCircle::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	float fScale;

	int nStateFrame = m_nStateFrame - m_nStartDelay;
	if (nStateFrame < 0)
		nStateFrame = 0;
	if (nStateFrame > m_nLength)
		nStateFrame = m_nLength;

	bool bDetailEffect = false;
	if (m_pGame->m_Option.m_bDetailCircleEffect)
		bDetailEffect = true;

	// #TODO Get width from sprite info
	float fOriginalCircleSize = 256.0f;
	if (bDetailEffect)
		fOriginalCircleSize = 30.0f;

	switch (m_nType)
	{
	case EFFECT_SCENE_CIRCLEIN:
		fScale = ((m_pGame->m_nWidth * 2.5f) * ((float)nStateFrame / (float)m_nLength)) / fOriginalCircleSize;
		break;
	case EFFECT_SCENE_CIRCLEOUT:
		fScale = ((m_pGame->m_nWidth * 2.5f) * (1 - (float)nStateFrame / (float)m_nLength)) / fOriginalCircleSize;
		break;
	}

	nColor = 0xff000000;
	
	if (!bDetailEffect)
	{
		float fAngle = 0;
		m_pSpriteCircle->RenderToQueue(m_fX, m_fY, SPRIDX_ETC_BIGCIRCLE,
			false, false, nColor,
			fScale, fScale, Z_SCENE_EFFECT, false, fAngle);
	}
	else
	{
		int nSmoothCircleFactor = 90;
		if (m_pGame->m_pResourceHolder->m_bUseDirectX)
			nSmoothCircleFactor = 360;

		for (int i = 0; i < nSmoothCircleFactor; i++)
		{
			float fAngle = i;
			m_pSprite->RenderToQueue(m_fX, m_fY, SPRIDX_ETC_CIRCLE,
				false, false, nColor,
				fScale, fScale, Z_SCENE_EFFECT, false, fAngle);
		}
	}

	float fOriginalHalfCircleSize = fOriginalCircleSize / 2.0f - 1.0f;
	NaRectT<float> rc;
	rc.top = m_fY - (fOriginalHalfCircleSize * fScale);
	rc.left = m_fX - (fOriginalHalfCircleSize * fScale);
	rc.right = m_fX + (fOriginalHalfCircleSize * fScale);
	rc.bottom = m_fY + (fOriginalHalfCircleSize * fScale);

	// T
	if (rc.top > 0)
	{
		m_pSprite->RenderToQueue(0, 0, SPRIDX_ETC_DOT,
			false, false, nColor,
			m_pGame->m_nWidth, rc.top + 1, Z_SCENE_EFFECT);
	}

	// B
	if (rc.bottom < m_pGame->m_nHeight)
	{
		m_pSprite->RenderToQueue(0, rc.bottom - 1, SPRIDX_ETC_DOT,
			false, false, nColor,
			m_pGame->m_nWidth, m_pGame->m_nHeight - rc.bottom + 2, Z_SCENE_EFFECT);
	}

	// L
	if (rc.left > 0)
	{
		m_pSprite->RenderToQueue(0, rc.top, SPRIDX_ETC_DOT,
			false, false, nColor,
			rc.left + 1, rc.bottom - rc.top, Z_SCENE_EFFECT);
	}

	// R
	if (rc.right < m_pGame->m_nWidth)
	{
		m_pSprite->RenderToQueue(rc.right - 1, rc.top, SPRIDX_ETC_DOT,
			false, false, nColor,
			m_pGame->m_nWidth - rc.right + 2, rc.bottom - rc.top, Z_SCENE_EFFECT);
	}
}
