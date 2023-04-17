#include "EffectPop.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

EffectPop::EffectPop(Game *pGame, Stage *pStage) :
	EffectObjectBase(pGame, pStage)
{
	m_nType = EFFECT_POP;
	m_nState = STATE_IDLE;
}

EffectPop::~EffectPop()
{
}

void EffectPop::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
}

void EffectPop::Process()
{
	if (m_nState != STATE_DEAD)
	{
		if (m_nStateFrame == 8)
			ChangeState(STATE_DEAD);
	}
}

int EffectPop::GetSpriteIndex()
{
	static int nFrameList[] = {
		0, 0, 1, 1, 2, 2, 3, 3,
	};

	int nFrame;
	if (m_nStateFrame < sizeof(nFrameList) / sizeof(nFrameList[0]))
		nFrame = SPRIDX_EFFECT_POP1 + nFrameList[m_nStateFrame];
	else
		nFrame = SPRIDX_EFFECT_POP4;

	return nFrame;
}
