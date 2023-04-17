#include "CheepCheepFly.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

CheepCheepFly::CheepCheepFly(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_CHEEPCHEEP_FLY;
	m_nState = STATE_IDLE;

	m_bFlip = true;
}

CheepCheepFly::~CheepCheepFly()
{
}

void CheepCheepFly::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_JUMP:
		m_fXS = (rand() % 20) / 10.0f;
		if (m_bFlip)
			m_fXS *= -1;
		m_fYS = -4.2f;
		break;
	case STATE_PRESSED:
	case STATE_BURNT:
		{
			m_bFlipV = true;
			m_fXS = 0;
			m_fYS = -1.0f;
			if (pInfo->pFromObject)
				GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);

			nState = STATE_BURNT;

			SOUND_MANAGER->PlaySoundEffect(L"Pressed");
		}
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void CheepCheepFly::OnIdle()
{
	OnStand();
}

void CheepCheepFly::OnStand()
{
	if (rand() % 16 == 0)
	{
		NaRect rcView = CAMERA->GetViewport();
		m_fX += ((rand() % 160) - 80);
		if (m_fX < rcView.Center())
			m_bFlip = false;
		else
			m_bFlip = true;
		
		ChangeState(STATE_JUMP);
	}
}

void CheepCheepFly::OnJump()
{
	OnApplyGravity(0.125f, 4.0f);

	if (m_fYS > 0 && m_fY > GameDefaults::nPageHeight)
	{
		ChangeState(STATE_DEAD);
	}
}

void CheepCheepFly::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool CheepCheepFly::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool CheepCheepFly::CanPressed(int nPower)
{
	return true;
}

int CheepCheepFly::GetSpriteIndex()
{
	int nFrame;
	switch (m_nState)
	{
	case STATE_JUMP:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_CHEEPCHEEP1, 2, 8);
		break;
	case STATE_PRESSED:
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_CHEEPCHEEP1;
		break;
	default:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_CHEEPCHEEP1, 2, 8);
		break;
	}

	return nFrame;
}
