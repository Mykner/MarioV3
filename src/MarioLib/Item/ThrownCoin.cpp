#include "ThrownCoin.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

ThrownCoin::ThrownCoin(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_THROWNCOIN;
	m_nState = STATE_JUMP;

	RequestSprites(pStage);

	m_nLifeFrame = 600;
}

ThrownCoin::~ThrownCoin()
{
}

void ThrownCoin::RequestSprites(Stage * pStage)
{
	if (pStage)
		m_ppSprite = &pStage->m_pSpriteEffect;
	else
		m_ppSprite = &CUR_STAGE->m_pSpriteEffect;
}

void ThrownCoin::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch(nState)
	{
	case STATE_PUSHEDUP:
		if (pInfo && pInfo->pFromObject)
		{
			if (pInfo->pFromObject->m_fXS != 0)
				m_fXS = pInfo->pFromObject->m_fXS / abs(pInfo->pFromObject->m_fXS);
			m_fYS = -3.0f;

			if (pInfo->pFromObject->m_pController)
			{
				CreateParameter param;
				param.pFromPlayer = pInfo->pFromObject->m_pController;
				CUR_STAGE->ReserveCreateGameObject(m_fX, m_fY - 16, EFFECT_COIN, &param);
			}
		}
		break;
	default:
		ItemObjectBase::ChangeState(nState, pInfo);
		return;
	}

	m_nNextState = nState;
}

void ThrownCoin::Process()
{
	ProcessBase();

	if (CanApplied() && m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyItem(this);
		SetState(STATE_DEAD);
	}

	if (m_nLifeFrame > 0)
	{
		m_nLifeFrame--;
		if (m_nLifeFrame == 0)
		{
			SetState(STATE_DEAD);
		}
	}
}

void ThrownCoin::OnBeforeProcess()
{
	CheckCollision();
	CheckCollisionPlayer();
}

void ThrownCoin::OnJump()
{
	m_fXS = ((rand() % 200) - 100) / 100.0f;
	m_fYS = -2.0f;
	
	m_fYS -= ((rand() % 200) / 100.0f);

	ChangeState(STATE_WALK);
}

void ThrownCoin::OnThrown()
{
	if (m_nStateFrame == 0)
	{
		// Thrown by BillBlaster
		if (m_fXS != 0 && m_fYS == 0)
		{
			float fXSAdd = ((rand()) % 100) / 100.0f;
			if (m_fXS < 0)
				m_fXS -= fXSAdd;
			else
				m_fXS += fXSAdd;

			m_fYS = ((rand() % 400) - 200) / 100.0f;
		}
	}
	else if (m_nStateFrame == 1)
	{
		// Thrown by Lakitu (from Reserved ... first frame is 1. Bug?)
		if (m_fXS == 0 && m_fYS < 0)
		{
			float fYSAdd = ((rand()) % 100) / 100.0f;
			if (m_fYS < 0)
				m_fYS -= fYSAdd;
			else
				m_fYS += fYSAdd;

			m_fXS = ((rand() % 400) - 200) / 100.0f;
		}
	}

	GameObjectBase::OnThrown();
}

void ThrownCoin::OnFall()
{
	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(0.33f, 5.0f);
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			
			OnLanding();
		}
	}

	CheckReachWall();
}

void ThrownCoin::OnThrownFly()
{
	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void ThrownCoin::OnPushedUp()
{
	CUR_PLAYER_OBJ->OnApplyItem(this);
	ChangeState(STATE_DEAD);

	SOUND_MANAGER->PlaySoundEffect(L"Coin");
}

bool ThrownCoin::CanApplied()
{
	if (m_nState == STATE_DEAD)
		return false;

	if (m_nState == STATE_HELDBYTONGUE)
		return false;

	if (m_nState == STATE_THROWN && m_nStateFrame < 8)
		return false;

	return true;
}

int ThrownCoin::GetSpriteIndex()
{
	if (m_nLifeFrame < 8 * 24)
	{
		if (m_pGame->m_nGameFrame % 2 < 1)
			return -1;
	}

	int nFrame = SPRITE_INDEX_CYCLE(0, 4, 4);

	return nFrame;
}
