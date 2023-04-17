#include "EffectObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"

#include "EffectScore.h"
#include "EffectCoin.h"
#include "EffectFireBoom.h"
#include "EffectBrickDebris.h"
#include "EffectAirBubble.h"
#include "EffectEggDebris.h"
#include "EffectPop.h"
#include "EffectDust.h"
#include "EffectHit.h"
#include "EffectStarDust.h"
#include "EffectDryBoneDebris.h"
#include "EffectTwinkle.h"
#include "EffectSceneFade.h"
#include "EffectSceneCircle.h"
#include "EffectWingDebris.h"
#include "EffectParalyze.h"
#include "EffectChainDebris.h"
#include "EffectDebrisBase.h"
#include "EffectSpin.h"
#include "EffectSplash.h"
#include "EffectDeathPosition.h"

EffectObjectBase::EffectObjectBase(Game *pGame, Stage *pStage) :
	GameObjectBase(pGame, pStage)
{
	m_nType = EFFECT_NONE;
	m_nState = STATE_IDLE;

	RequestSprites(pStage);

	m_pPlayer = nullptr;
}

EffectObjectBase::~EffectObjectBase()
{
}

void EffectObjectBase::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
}

void EffectObjectBase::OnBeforeProcess()
{
	// Do not need collision check.
}

void EffectObjectBase::Process()
{
	ChangeState(STATE_DEAD);
}

void EffectObjectBase::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
		m_fScaleX, m_fScaleY, Z_EFFECT);
}

int EffectObjectBase::GetSpriteIndex()
{
	return -1;
}

GameObjectBase * EffectObjectBase::CreateInstance(Game * pGame, Stage * pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	switch (nType)
	{
	case EFFECT_SCORE:					pObj = new EffectScore(pGame, pStage);			break;
	case EFFECT_COIN:					pObj = new EffectCoin(pGame, pStage);			break;
	case EFFECT_EXPLOSION:				pObj = new EffectFireBoom(pGame, pStage);		break;
	case EFFECT_BRICKDEBRIS:			pObj = new EffectBrickDebris(pGame, pStage);	break;
	case EFFECT_AIRBUBBLE:				pObj = new EffectAirBubble(pGame, pStage);		break;
	case EFFECT_EGGDEBRIS:				pObj = new EffectEggDebris(pGame, pStage);		break;
	case EFFECT_POP:					pObj = new EffectPop(pGame, pStage);			break;
	case EFFECT_DUST:					pObj = new EffectDust(pGame, pStage);			break;
	case EFFECT_HIT:					pObj = new EffectHit(pGame, pStage);			break;
	case EFFECT_STARDUST:				pObj = new EffectStarDust(pGame, pStage);		break;
	case EFFECT_DRYBONEDEBRIS:			pObj = new EffectDryBoneDebris(pGame, pStage);	break;
	case EFFECT_TWINKLE:				pObj = new EffectTwinkle(pGame, pStage);		break;

	case EFFECT_SCENE_FADEIN:
	case EFFECT_SCENE_FADEOUT:
		pObj = new EffectSceneFade(pGame, pStage);		
		pObj->m_nType = nType;
		break;
	case EFFECT_SCENE_CIRCLEIN:
	case EFFECT_SCENE_CIRCLEOUT:
		pObj = new EffectSceneCircle(pGame, pStage);
		pObj->m_nType = nType;
		break;

	case EFFECT_WINGDEBRIS:			pObj = new EffectWingDebris(pGame, pStage);			break;
	case EFFECT_PARALYZE:			pObj = new EffectParalyze(pGame, pStage);			break;
	case EFFECT_CHAINDEBRIS:		pObj = new EffectChainDebris(pGame, pStage);		break;
	case EFFECT_DEBRISBASE:			pObj = new EffectDebrisBase(pGame, pStage);			break;
	case EFFECT_SPIN:				pObj = new EffectSpin(pGame, pStage);				break;
	case EFFECT_SPLASH:				pObj = new EffectSplash(pGame, pStage);				break;
	case EFFECT_DEATHPOSITION:		pObj = new EffectDeathPosition(pGame, pStage);		break;
	}

	return pObj;
}
