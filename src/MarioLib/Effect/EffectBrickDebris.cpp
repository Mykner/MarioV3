#include "EffectBrickDebris.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectBrickDebris::EffectBrickDebris(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_BRICKDEBRIS;
	m_nState = STATE_IDLE;

	m_nTileType = -1;
}

EffectBrickDebris::~EffectBrickDebris()
{
}

void EffectBrickDebris::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	m_fXS = pParam->fXS;
	m_fYS = pParam->fYS;

	if (pParam->nType > 0)
	{
		m_nTileType = pParam->nType;
		m_ppSpriteTile = &m_pStage->m_pSpritePiece;
	}
}

void EffectBrickDebris::Process()
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

void EffectBrickDebris::Render(int nColor, int nZOrder)
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
    
    if (m_pGame->m_bModernStyle || m_nTileType != -1)
    {
		fAngle = m_nStateFrame * (22.5f);
	}

	if (m_nTileType != -1)
	{
		(*m_ppSpriteTile)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
	}
	else
	{
		(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			m_fScaleX, m_fScaleY, Z_EFFECT, bShadow, fAngle);
	}
}

int EffectBrickDebris::GetSpriteIndex()
{
	int nFrame = SPRIDX_BRICKDEBRIS1;
	if (m_nTileType == -1)
	{
		if (m_pGame->m_bModernStyle)
			return nFrame;

		int nVal = m_nStateFrame % 16;
		if (nVal < 4)
			nFrame = SPRIDX_BRICKDEBRIS1;
		else if (nVal < 8)
			nFrame = SPRIDX_BRICKDEBRIS2;
		else if (nVal < 12)
			nFrame = SPRIDX_BRICKDEBRIS3;
		else if (nVal)
			nFrame = SPRIDX_BRICKDEBRIS4;
	}
	else
		return m_nTileType;

	return nFrame;
}
