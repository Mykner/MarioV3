#include "EffectDeathPosition.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectDeathPosition::EffectDeathPosition(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_DEATHPOSITION;
	m_nState = STATE_IDLE;
	RequestSprites(pStage);

	m_bIsLargeMark = true;

	m_fMarkScale = m_fScaleX = m_fScaleY = m_fLargeMarkScale;
}

EffectDeathPosition::~EffectDeathPosition()
{
}

void EffectDeathPosition::RequestSprites(Stage * pStage)
{
	m_pSprite = SPR_MANAGER->Find(L"Etc");
}

void EffectDeathPosition::ParseParameter(CreateParameter * pParam)
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

void EffectDeathPosition::Process()
{
	if (m_nStateFrame == 0)
	{
		auto rcViewport = CAMERA->GetViewport();
		if (m_fY > rcViewport.bottom)
			m_fY = rcViewport.bottom - ((rand() % 40) / 10.0f);

		if (m_fScaleX == 0.5f)
		{
			m_bIsLargeMark = true;
			m_fMarkScale = m_fLargeMarkScale;
		}
		else
		{
			m_bIsLargeMark = false;
			m_fMarkScale = m_fSmallMarkScale;
		}
	}

	if (m_nStateFrame < 6)
	{
		// Scale up (bounce)
		m_fScaleX = m_fScaleY = (m_fMarkScale * 0.22f * m_nStateFrame);
	}
	else if (m_nStateFrame < 24)
	{
		// Scale down
		if (m_fScaleX > m_fMarkScale)
		{
			m_fScaleX = m_fScaleY = m_fMarkScale + (m_fScaleX - m_fMarkScale) / 2.0f;
		}
	}

	if (m_nState != STATE_DEAD)
	{
		if (m_nStateFrame == m_nMaxLifeFrame)
			ChangeState(STATE_DEAD);
	}
}

void EffectDeathPosition::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (m_nStateFrame < 16)
	{
		nColor = nColor & (0x0affffff * m_nStateFrame);
	}
	else if (m_nStateFrame > m_nMaxLifeFrame - 16)
	{
		nColor = nColor & (0xffffffff - (0x0a000000 * (m_nStateFrame - (m_nMaxLifeFrame - 16))));
	}

	bool bShadow = false;
	if (nColor > 0xa0000000)
		bShadow = true;

	nZOrder = Z_EFFECT;
	if (m_bIsLargeMark)
		nZOrder += 1;

	m_pSprite->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT, bShadow);
}

int EffectDeathPosition::GetSpriteIndex()
{
	return SPRIDX_ETC_DEATH_POSITION;
}
