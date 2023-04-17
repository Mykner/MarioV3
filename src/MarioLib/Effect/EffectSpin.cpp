#include "EffectSpin.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectSpin::EffectSpin(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_SPIN;
	m_nState = STATE_IDLE;
}

EffectSpin::~EffectSpin()
{
}

void EffectSpin::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;

	if (pParam->fScaleX != 0)
		m_fScaleX = pParam->fScaleX;

	if (pParam->fScaleY != 0)
		m_fScaleY = pParam->fScaleY;
}

void EffectSpin::Process()
{
	if (m_nStateFrame > 12)
		ChangeState(STATE_DEAD);
}

void EffectSpin::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nColor == -1)
	{
		nColor -= ((0x70000000) + (0x05000000) * m_nStateFrame);
	}

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT);
}

int EffectSpin::GetSpriteIndex()
{
	return SPRITE_INDEX_CYCLE(SPRIDX_EFFECT_SPIN1, 4, 2);
}
