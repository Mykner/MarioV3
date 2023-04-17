#include "Bobomb.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(Bobomb)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Bobomb"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Bobomb")
END_IMPL_PROPERTYMAP()

Bobomb::Bobomb(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_BOBOMB;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nBombTimer = -1;
}

Bobomb::~Bobomb()
{
}

void Bobomb::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
	case STATE_FALL:
	case STATE_JUMP:
		if (m_bWinged)
		{
			nState = STATE_FLY;
		}
		break;
	}

	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_KICKED:
		break;
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

			switch (nPower)
			{
			case POWER_PRESS_YOSHI:
				{
					if (pInfo->pFromObject && pInfo->pFromObject->m_nType == VEHICLE_YOSHI)
					{
						OnStomped();
						nState = STATE_DEAD;

						SOUND_MANAGER->PlaySoundEffect(L"YoshiStomp");
					}
					else
					{
						OnStomped(false, pInfo->pFromObject);

						if (m_nBombTimer == -1)
							m_nBombTimer = 256;

						// #TODO Change 10 to FromObject Size.cx/2
						if (pInfo->pFromObject->m_fX > m_fX)
						{
							m_fX = pInfo->pFromObject->m_fX;
							POINT ptOffset = FindCollidableOffset(COLLISION_LEFT, 32);
							m_fX += ptOffset.x;

							m_fXS = -3.0f;
							m_bFlip = true;
						}
						else
						{
							m_fX = pInfo->pFromObject->m_fX;
							POINT ptOffset = FindCollidableOffset(COLLISION_RIGHT, 32);
							m_fX += ptOffset.x;

							m_fXS = 3.0f;
							m_bFlip = false;
						}
						m_fYS = -2.0f;

						m_pKickedBy = pInfo->pFromObject;
						m_nKickCooltime = 16;
						m_nInvinsibleTime = 10;

						SOUND_MANAGER->PlaySoundEffect(L"Kicked");
					}
				}
				break;
			case POWER_PRESS_GOOMBASHOES:
				{
					m_nNextState = nState = STATE_BURNT;
					SOUND_MANAGER->PlaySoundEffect(L"Kicked");
				}
				break;
			default:
				SOUND_MANAGER->PlaySoundEffect(L"Pressed");

				if (m_bWinged)
				{
					m_bWinged = false;
					OnLostWing();

					nState = STATE_WALK;
				}
				break;
			}
		}
		break;
	case STATE_BURNT:
		if (m_bWinged)
		{
			m_bWinged = false;
			OnLostWing();
		}

		if (pInfo && pInfo->nPower == BURN_STARMAN)
		{
			EnemyObjectBase::ChangeState(nState, pInfo);
			return;
		}
		
		if (m_nBombTimer == -1 &&
			pInfo && pInfo->nPower >= BURN_BOBOMB)
		{
			ReleaseEnemyStack();

			nState = STATE_STAND;
			m_nBombTimer = 8;
		}
		else if (m_nBombTimer == -1)
		{
			ReleaseEnemyStack();

			m_nBombTimer = 256;
			nState = STATE_STAND;
		}
		else
			return;
		break;
	case STATE_PUSHEDUP:
		ReleaseEnemyStack();
			
		if (m_bWinged)
		{
			m_bWinged = false;
			OnLostWing();
		}

		m_fXS = 0;
		m_fYS = -3.0f;

		if (m_nBombTimer == -1)
		{
			m_nBombTimer = 256;
		}
		nState = STATE_STAND;
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;

	if (pInfo)
	{
		m_ChangeStateInfo = *pInfo;
		m_ChangeStateInfo.nNextState = nState;
	}
	else
	{
		m_ChangeStateInfo.nNextState = -1;
	}
}

void Bobomb::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_STAND:
		m_fXS = 0;
		m_fYS = 0;
		break;
	case STATE_FLY:
		if (m_bFlip)
			m_fXS = -1.0f;
		else
			m_fXS = 1.0f;
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void Bobomb::OnAfterProcess()
{
	EnemyObjectBase::OnAfterProcess();

	if (m_nBombTimer > 0)
	{
		m_nBombTimer--;
		if (m_nBombTimer == 0)
		{
			ExplodeBomb();
			SetState(STATE_DEAD);
		}
	}
}

void Bobomb::OnStand()
{
	if (m_nBombTimer == -1)
		ChangeState(STATE_WALK);

	CheckFalling();
}

void Bobomb::OnFall()
{
	m_bReachFriend = false;

	GameObjectBase::OnFall();

	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

void Bobomb::OnFly()
{
	int nPhaseFrame = 20;
	int nVal = m_nStateFrame % (nPhaseFrame * 4);
	if (nVal < nPhaseFrame || (nVal >= nPhaseFrame * 3))
		m_fYS -= 0.05f;
	else if (nVal >= nPhaseFrame && nVal < nPhaseFrame * 3)
		m_fYS += 0.05f;
}

void Bobomb::OnPressed()
{
	if (m_nBombTimer == -1 && m_nStateFrame == 0)
		m_nBombTimer = 256;

	if (CheckFalling() == false)
	{
		m_fXS *= 0.7f;
		if (abs(m_fYS) > 1.0f)
			m_fYS = -m_fYS;
	}
}

void Bobomb::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
	{		
		if (m_nBombTimer == -1)
			ChangeState(STATE_WALK);
		else if (abs(m_fXS) > 2.2f)
		{
			m_fXS *= 0.3f;
			m_fYS = -0.5f;
			ChangeState(STATE_FALL);
		}
		else
			ChangeState(STATE_STAND);
	}
}

void Bobomb::OnKicked()
{
	if (CheckHitBlock(POWER_HITBLOCK_NORMAL))
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			OnLanding();
		}
	}

	if (CheckReachWall())
	{
		m_fXS = 0;
		return;
	}
}

void Bobomb::ExplodeBomb()
{
	int nTX = m_fX / TILE_XS;
	int nTY = (m_fY - 8) / TILE_YS;
	int nRadius = m_bPowerUp ? 2 : 1;

	// Destroy Blocks
	for (int _x = nTX - nRadius; _x <= nTX + nRadius; _x++)
	{
		for (int _y = nTY - nRadius; _y <= nTY + nRadius; _y++)
		{
			if (!CUR_STAGE->IsHardTile(_x, _y))
				continue;

			int nData = CUR_STAGE->GetTileData(_x, _y);
			int nType = CUR_STAGE->GetDataType(nData);
			if (nType != TILETYPE_SUPERHARD && 
				CUR_STAGE->CanCrush(_x, _y, POWER_HITBLOCK_GIANTSHELL))
			{
				CUR_STAGE->CrushBlock(_x, _y, this);
			}
		}
	}
	
	NaRect rc = GetRect();
	rc.left -= 16;
	rc.top -= 16;
	rc.right += 16;
	rc.bottom += 16;

	// Destroy Enemies
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEnemy) {
		NaRect rcObj = pObj->GetRect();
		if (!rc.IsOverlapped(rcObj))
			continue;

		if (pObj->IsAlive() && pObj->CanBurnt(BURN_BOBOMB))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			info.nPower = BURN_STARMAN;
			pObj->ChangeState(STATE_BURNT, &info);
		}
	} END_VECTOR_LOOP();

	// Destroy Player
	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rc.IsOverlapped(rcPlayer))
	{
		GameObjectBase *pObj = CUR_PLAYER_OBJ;
		if (pObj->IsAlive() && pObj->CanBurnt(BURN_BOBOMB))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			info.nPower = BURN_STARMAN;
			pObj->ChangeState(STATE_BURNT, &info);
		}
	}

	// Effects
	{
		CreateParameter param;
		param.nType = EFFECT_BOMBEXPLOSION;
		param.pFromObject = this;
		param.fScaleX = 1.5f * nRadius;
		param.fScaleY = 1.5f * nRadius;
		CUR_STAGE->CreateEffect(m_fX, m_fY - (8 * nRadius), EFFECT_EXPLOSION, &param);

		param.nType = EFFECT_DUST;
		param.fXS = -0.5f;
		param.fYS = -0.5f;
		param.fScaleX = 1.0f * nRadius;
		param.fScaleY = 1.0f * nRadius;
		CUR_STAGE->CreateEffect(m_fX - (8 * nRadius), m_fY - (16 * nRadius), EFFECT_DUST, &param);

		param.fXS = -0.5f;
		param.fYS = 0.5f;
		CUR_STAGE->CreateEffect(m_fX - (8 * nRadius), m_fY, EFFECT_DUST, &param);

		param.fXS = 0.5f;
		param.fYS = -0.5f;
		CUR_STAGE->CreateEffect(m_fX + (8 * nRadius), m_fY - (16 * nRadius), EFFECT_DUST, &param);

		param.fXS = 0.5f;
		param.fYS = 0.5f;
		CUR_STAGE->CreateEffect(m_fX + (8 * nRadius), m_fY, EFFECT_DUST, &param);

		CUR_STAGE->MakeEarthquake();
	}

	SOUND_MANAGER->PlaySoundEffect(L"Cannon");
}

bool Bobomb::IsAlive()
{
	if (m_nState == STATE_PRESSED)
		return true;

	return EnemyObjectBase::IsAlive();
}

bool Bobomb::CanDamageEnemy()
{
	if (m_nState == STATE_PRESSED || m_nBombTimer > 0)
		return false;

	if (IsAlive())
		return EnemyObjectBase::CanDamageEnemy();

	return false;
}

bool Bobomb::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_TURTLESHELL:
		if (m_nBombTimer == -1)
			return true;
		else
			return false;
		break;
	}

	return EnemyObjectBase::CanBurnt(nType);
}

bool Bobomb::CanKicked()
{
	if (m_nKickCooltime > 0)
		return false;

	if (m_nState == STATE_CARRYBYHAND)
		return false;

	if (m_nState == STATE_PRESSED || m_nBombTimer > 0)
		return true;

	return false;
}

bool Bobomb::CanCarried()
{
	if (m_nState == STATE_PRESSED || m_nBombTimer > 0)
		return true;

	return false;
}

std::vector<POINT> Bobomb::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -5;
	pt.y = -5;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int Bobomb::GetWingSpriteIndex()
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

int Bobomb::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_DEAD:
		return -1;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_BOBOMB_SIT;
		break;
	case STATE_FLY:
		nFrame = SPRIDX_BOBOMB_JUMP;
		break;
	default:
		if (m_nBombTimer >= 0)
		{
			if (m_nBombTimer > 256 - 6)
				nFrame = SPRIDX_BOBOMB_IGNITE1;
			//else if (m_nBombTimer > 256 - 12)
			//	nFrame = SPRIDX_BOBOMB_IGNITE2;
			else if (m_nBombTimer > 200)
				nFrame = SPRIDX_BOBOMB_SIT;
			else if (m_nBombTimer < 24)
				nFrame = SPRIDX_BOBOMB_IGNITE1;
			else
			{
				if (m_nBombTimer % 16 < 8)
					nFrame = SPRIDX_BOBOMB_IGNITE1;
				else
					nFrame = SPRIDX_BOBOMB_IGNITE2;
			}
		}
		else if (m_fYS != 0)
			nFrame = SPRIDX_BOBOMB_JUMP;
		else 
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BOBOMB1, 2, 10);
		break;
	}

	return nFrame;
}

void Bobomb::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nBombTimer > 0 && m_nBombTimer < 4)
	{
		fScaleX *= (1.0f + ((4 - m_nBombTimer) / 10.0f));
		fScaleY *= (1.0f + ((4 - m_nBombTimer) / 10.0f));
	}
}
