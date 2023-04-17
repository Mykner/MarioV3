#include "EffectCoin.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

EffectCoin::EffectCoin(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_COIN;
	m_nState = STATE_IDLE;

	m_pPlayer = nullptr;
}

EffectCoin::~EffectCoin()
{
}

void EffectCoin::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	m_pPlayer = pParam->pFromPlayer;
}

void EffectCoin::Process()
{
	if (m_nState == STATE_IDLE && m_nStateFrame == 0)
	{
		SOUND_MANAGER->PlaySoundEffect(L"Coin");
	}

	if (m_nState == STATE_DEAD)
		return;

	int nCoinAnim[30] = {
		-5, -5, -5, -4, -4, -4, -4, -3, -2, -2,
		-2, -1, -1, -1,  0,  0,  0,  1,  1,  1, 
		 2,  2,  2,  2,  3,  3,  4,  4,  4,  5 
	};

	if (m_nStateFrame == 30)
	{
		if (m_pPlayer != nullptr)
		{
			CreateParameter param;
			param.nType = EFFECT_SCORE;
			param.nScore = 200;
			param.pFromPlayer = m_pPlayer;
			param.fYS = -0.5f;
			CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY, EFFECT_SCORE, &param);
		}
		ChangeState(STATE_DEAD);
	}
	else if (m_nStateFrame < 30)
	{
		m_fY += nCoinAnim[m_nStateFrame];
	}
}

int EffectCoin::GetSpriteIndex()
{
	// #TODO improvement
	int nFrame = ((m_nStateFrame + 1) % 8) / 2;

	return nFrame;
}
