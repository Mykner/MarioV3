#include "EffectFireBoom.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

EffectFireBoom::EffectFireBoom(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_EXPLOSION;
	m_nState = STATE_IDLE;

	m_nSubType = m_nType;
}

EffectFireBoom::~EffectFireBoom()
{
}

void EffectFireBoom::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
	
	if (pParam->nType > 0)
		m_nSubType = pParam->nType;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;
	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
}

void EffectFireBoom::Process()
{
	if (m_nState == STATE_IDLE && m_nStateFrame == 0)
	{
		if (m_nSubType == EFFECT_FIREWORKS)
			SOUND_MANAGER->PlaySoundEffect(L"Cannon");
	}

	if (m_nState != STATE_DEAD)
	{
		bool bDead = false;
		switch (m_nSubType)
		{
		case EFFECT_EXPLOSION:
			if (m_nStateFrame == 7)
				bDead = true;
			break;
		case EFFECT_FIREWORKS:
			if (m_nStateFrame == 25)
				bDead = true;
			break;
		case EFFECT_BOMBEXPLOSION:
			if (m_nStateFrame == 9)
				bDead = true;
			break;
		}
		
		if (bDead)
		{
			ChangeState(STATE_DEAD);
		}
	}
}

int EffectFireBoom::GetSpriteIndex()
{
	int nFrame = SPRIDX_EFFECT_EXPLOSION1;

	switch (m_nSubType)
	{
	case EFFECT_EXPLOSION:
		nFrame += (m_nStateFrame / 2);
		nFrame = MIN(nFrame, SPRIDX_EFFECT_EXPLOSION3);
		break;
	case EFFECT_FIREWORKS:
		nFrame += (m_nStateFrame / 8);
		nFrame = MIN(nFrame, SPRIDX_EFFECT_EXPLOSION3);
		break;
	case EFFECT_BOMBEXPLOSION:
		nFrame += (m_nStateFrame / 2);
		nFrame = MIN(nFrame, SPRIDX_EFFECT_EXPLOSION4);
		break;
	}
	
	return nFrame;
}
