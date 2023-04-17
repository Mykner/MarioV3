#include "EffectSceneFade.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectSceneFade::EffectSceneFade(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_SCENE_FADE;
	m_nState = STATE_IDLE;

	RequestSprites(pStage);

	m_nSubType = EFFECT_SCENE_FADEOUT;
	
	m_nLength = 256;
	m_nStartDelay = 0;
	m_bFinished = false;
}

EffectSceneFade::~EffectSceneFade()
{
}

void EffectSceneFade::RequestSprites(Stage * pStage)
{
	m_pSprite = SPR_MANAGER->Find(L"Etc");
}

void EffectSceneFade::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->nDuration != 0)
		m_nLength = pParam->nDuration;
	if (pParam->nStartDelay != 0)
		m_nStartDelay = pParam->nStartDelay;
}

void EffectSceneFade::Process()
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

void EffectSceneFade::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	
	int nStateFrame = m_nStateFrame - m_nStartDelay;
	if (nStateFrame < 0)
		nStateFrame = 0;
	if (nStateFrame > m_nLength)
		nStateFrame = m_nLength;

	switch (m_nType)
	{
	case EFFECT_SCENE_FADEIN:
		nColor = (0xff * nStateFrame) / m_nLength;
		nColor *= 0x1000000;
		nColor = 0xff000000 - nColor;
		break;
	case EFFECT_SCENE_FADEOUT:
		nColor = (0xff * nStateFrame) / m_nLength;
		nColor *= 0x1000000;
		break;
	}

	m_pSprite->RenderToQueue(0, 0, SPRIDX_ETC_DOT,
		false, false, nColor,
		m_pGame->m_nWidth, m_pGame->m_nHeight, Z_SCENE_EFFECT);
}
