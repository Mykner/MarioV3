#include "EffectSplash.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectSplash::EffectSplash(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_SPLASH;
	m_nState = STATE_IDLE;

	m_nSubType = EFFECT_DUST;
}

EffectSplash::~EffectSplash()
{
}

void EffectSplash::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;

	if (pParam->fXS != 0)
		m_fXS = pParam->fXS;
	if (pParam->fYS != 0)
		m_fYS = pParam->fYS;

	m_nSubType = pParam->nType;
}

void EffectSplash::Process()
{
	if (m_nState != STATE_DEAD)
	{
		if (m_nStateFrame == 35)
			ChangeState(STATE_DEAD);
	}
}

void EffectSplash::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (m_nStateFrame < 8)
	{
		fScaleX *= ((m_nStateFrame / 16.0f) + 0.5f);
		fScaleY *= ((m_nStateFrame / 16.0f) + 0.5f);
	}

	if (m_nStateFrame > 30 - 8)
	{
		nColor -= ((0x10000000) * (m_nStateFrame - (30 - 8)));
	}

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		fScaleX, fScaleY, Z_EFFECT);
}

int EffectSplash::GetSpriteIndex()
{
	static int nFrameList[] = {
		0, 0, 0, 0, 1, 
		1, 1, 1, 1, 1, 
		1, 1, 2, 2, 2,
		2, 2, 2, 3, 3,
		4, 4, 3, 3, 4, 
		4, 3, 4, 3, 4,
		3, 4, 3, 4, 3,
		4,
	};

	int nFrame;
	nFrame = SPRIDX_EFFECT_SPLASH1;
	
 	if (m_nStateFrame < sizeof(nFrameList) / sizeof(nFrameList[0]))
 		nFrame = nFrame + nFrameList[m_nStateFrame];
 	else
 		nFrame = nFrame;

	return nFrame;
}
