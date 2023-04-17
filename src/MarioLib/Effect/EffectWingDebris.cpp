#include "EffectWingDebris.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectWingDebris::EffectWingDebris(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_WINGDEBRIS;
	m_nState = STATE_FALL;

	RequestSprites(pStage);
}

EffectWingDebris::~EffectWingDebris()
{
}

void EffectWingDebris::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	m_ppSprite = &pStage->m_pSpriteEnemy;
}

void EffectWingDebris::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fXS != 0)
		m_fXS = pParam->fXS;
	if (pParam->fYS != 0)
		m_fYS = pParam->fYS;
	
	if (pParam->pFromObject)
		m_bFlip = pParam->pFromObject->m_bFlip;
	if (pParam->nFlip == 1)
		m_bFlip = !m_bFlip;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
}

void EffectWingDebris::Process()
{
	m_fYS = 1.0f;
	m_fXS = sin((m_nStateFrame * 10) * fToRadian);

	if (m_nState != STATE_DEAD && IsOutOfStage())
	{
		ChangeState(STATE_DEAD);
	}
}

void EffectWingDebris::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	bool bShadow = IS_MODERN;
	float fAngle = 0;
    
	int nVal = m_nStateFrame % 32;
	if (nVal < 8)
		fAngle = nVal * 5.0f; // 0~8
	else if (nVal < 24)
		fAngle = (16 - nVal) * 5.0f; // 8~-8
	else 
		fAngle = (nVal - 32) * 5.0f; // -8~0

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
}

int EffectWingDebris::GetSpriteIndex()
{
	int nFrame = SPRIDX_WING1;

	return nFrame;
}
