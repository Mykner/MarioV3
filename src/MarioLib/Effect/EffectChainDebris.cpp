#include "EffectChainDebris.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectChainDebris::EffectChainDebris(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_CHAINDEBRIS;
	m_nState = STATE_IDLE;

	m_fXS = ((rand() % 40) - 20) / 10.0f;
	m_fYS = -1.0f;

	RequestSprites(pStage);
}

EffectChainDebris::~EffectChainDebris()
{
}

void EffectChainDebris::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSpriteEnemy;
}

void EffectChainDebris::ParseParameter(CreateParameter * pParam)
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

void EffectChainDebris::Process()
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

void EffectChainDebris::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = SPRIDX_CHAINCHOMP_CHAIN;
	bool bShadow = IS_MODERN;
	float fAngle = 0;
	
	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
}
