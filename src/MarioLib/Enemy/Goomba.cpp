#include "Goomba.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"
#include "VehicleObjectBase.h"

BEGIN_IMPL_PROPERTYMAP(Goomba)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Goomba"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Goomba")
END_IMPL_PROPERTYMAP()

Goomba::Goomba(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_GOOMBA;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nJump = 0;
}

Goomba::~Goomba()
{
}

void Goomba::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:		
		{
			ReleaseEnemyStack();

			m_fXS = 0;
			m_fYS = 0;

			if (pInfo && pInfo->pFromObject)
				GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);			

			int nPower = POWER_PRESS_NORMAL;
			if (pInfo && pInfo->nPower)
				nPower = pInfo->nPower;

			if (m_pVehicle)
				m_pVehicle->OnDamagedPassenger();

			switch (nPower)
			{
			case POWER_PRESS_YOSHI:
				{
					OnStomped();
					nState = STATE_DEAD;

					SOUND_MANAGER->PlaySoundEffect(L"YoshiStomp");
				}
				break;
			case POWER_PRESS_GOOMBASHOES:
				{
					nState = STATE_BURNT;

					SOUND_MANAGER->PlaySoundEffect(L"Kicked");
				}
				break;
			default:
				if (m_bWinged)
				{
					m_fYS = 0;
					m_bWinged = false;
					nState = STATE_WALK;

					OnLostWing();
				}
				else
				{
					if (m_bPowerUp)
					{
						// Create normal size Goomba x2
						SplitToTwoGoombas();
						nState = STATE_DEAD;
					}
				}

				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				break;
			}
		}
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		// Use Default
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void Goomba::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_JUMP:
		if (m_nJump < 2)
		{
			m_fYS = -1.0f;
			m_nJump++;
		}
		else
		{
			m_fYS = -3.0f;
			m_nJump = 0;

			if (m_fX < CUR_PLAYER_OBJ->m_fX)
				m_bFlip = false;
			else
				m_bFlip = true;
		}
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void Goomba::OnWalk()
{
	if (m_bWinged)
	{
		ChangeState(STATE_JUMP);
	}
	else
	{
		EnemyObjectBase::OnWalk();
	}
}

void Goomba::OnJump()
{
	CheckHitBlock(0);

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();

		m_fYS += 0.15f;
		if (m_fYS > 4.0f)
			m_fYS = 4.0f;
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;

			if (m_fXS != 0)
				ChangeState(STATE_WALK);
			else
				ChangeState(STATE_STAND);
		}
	}

	CheckReachWall();

	if (m_bReachFriend)
		OnReachFriend();
}

void Goomba::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool Goomba::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

void Goomba::SplitToTwoGoombas()
{
	GameObjectBase *pObj;

	pObj = CUR_STAGE->ReserveCreateGameObject(m_fX + 8, m_fY, ENEMY_GOOMBA);
	if (pObj)
	{
		pObj->m_nState = STATE_THROWN;
		pObj->m_fXS = 1.7f;
		pObj->m_fYS = -1.0f;
		pObj->m_bFlip = false;

		pObj = nullptr;
	}

	pObj = CUR_STAGE->ReserveCreateGameObject(m_fX - 8, m_fY, ENEMY_GOOMBA);
	if (pObj)
	{
		pObj->m_nState = STATE_THROWN;
		pObj->m_fXS = -1.7f;
		pObj->m_fYS = -1.0f;
		pObj->m_bFlip = true;
	}

	CreateParameter param;
	param.fScaleX = 2.0f;
	param.fScaleY = 2.0f;
	param.nType = EFFECT_DUST;
	CUR_STAGE->CreateEffect(m_fX, m_fY - GetRect().Height() / 2, EFFECT_DUST, &param);

	param.fScaleX = 1.0f;
	param.fScaleX = 1.0f;
	CUR_STAGE->CreateEffect(m_fX - 12, m_fY - GetRect().Height() / 2, EFFECT_DUST, &param);
	CUR_STAGE->CreateEffect(m_fX + 12, m_fY - GetRect().Height() / 2, EFFECT_DUST, &param);
}

std::vector<POINT> Goomba::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -8;
	pt.y = -5;

	std::vector<POINT> vec;
	vec.push_back(pt);

	pt.x = +8;
	vec.push_back(pt);

	return vec;
}

int Goomba::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_fYS != 0)
	{
		if (m_nStateFrame % 16 >= 8)
			nFrame = SPRIDX_WING2;
	}
	else
	{
		if (m_nStateFrame % 32 >= 16)
			nFrame = SPRIDX_WING2;
	}

	return nFrame;
}

int Goomba::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
		nFrame = SPRIDX_GOOMBA_PRESSED;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_GOOMBA1;
		break;
	default:
		nFrame = SPRIDX_GOOMBA1 + (m_nStateFrame / 16) % 2;
		break;
	}

	return nFrame;
}
