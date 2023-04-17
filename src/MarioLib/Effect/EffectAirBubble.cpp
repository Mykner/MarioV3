#include "EffectAirBubble.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectAirBubble::EffectAirBubble(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_AIRBUBBLE;
	m_nState = STATE_IDLE; 

	m_nDuration = -1;
}

EffectAirBubble::~EffectAirBubble()
{
}

void EffectAirBubble::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->nDuration > 0)
		m_nDuration = pParam->nDuration;
}

void EffectAirBubble::Process()
{
	if (m_nState == STATE_IDLE && m_nStateFrame == 0)
	{
		m_fYS = -1.0f;
	}

	m_fYS = -(rand() % 2);

	if (m_pGame->m_bModernStyle)
	{
		float fLimit = 0.5f;
		if (m_nDuration != -1 && m_nStateFrame > m_nDuration)
		{
			m_fScaleX -= 0.1f;
			m_fScaleY -= 0.1f;

			fLimit = 0.1f;
		}
		else if (m_nStateFrame % 4 == 0)
		{
			float fScale = (float)(rand() % 300) / 100.0f;
			if (m_fScaleX < fScale)
			{
				m_fScaleX += 0.1f;
				m_fScaleY += 0.1f;
			}
			else
			{
				m_fScaleX -= 0.1f;
				m_fScaleY -= 0.1f;
			}
		}		

		if (m_fScaleX < fLimit)
			m_fScaleX = m_fScaleY = fLimit;

	}

	if (m_nState != STATE_DEAD)
	{
		if (!CanSwim() || (m_nDuration != -1 && m_fScaleX <= 0.1f))
		{
			ChangeState(STATE_DEAD);
		}
	}
}

void EffectAirBubble::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (IS_MODERN)
		nColor -= 0x90000000;

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT);
}

int EffectAirBubble::GetSpriteIndex()
{
	return SPRIDX_EFFECT_AIRBUBBLE;
}
