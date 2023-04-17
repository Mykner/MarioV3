#include "BowserJunior.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "Fireball.h"
#include "SoundManager.h"

#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(BowserJunior)
	PROP_BOOL("PowerUp",		VT_BOOL,	false,	nullptr, "BowserJunior"),
	PROP_BOOL("Winged",			VT_BOOL,	false,	nullptr, "BowserJunior"),
	PROP_BOOL("CanFire",		VT_BOOL,	true,	nullptr, "BowserJunior"),
	PROP_BOOL("CanThrowHammer", VT_BOOL,	false,	nullptr, "BowserJunior"),
END_IMPL_PROPERTYMAP()

BowserJunior::BowserJunior(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_BOWSERJUNIOR;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_bAimFire = false;

	m_nDamagedCooltime = 0;
	m_nDamagedCount = 0;
	m_nJumpCount = 0;
	m_nActionPhase = 0;

	m_nThrowFireCooltime = 0;

	m_bCanFire = true;
	m_bCanThrowHammer = false;
}

BowserJunior::~BowserJunior()
{
}

void BowserJunior::OnAfterProcess()
{
	EnemyObjectBase::OnAfterProcess();

	if (m_nDamagedCooltime > 0)
		m_nDamagedCooltime--;
}

void BowserJunior::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	if (m_nNextState != -1)
		return;

	switch (nState)
	{
	case STATE_PRESSED:
		{
			if (m_nState == STATE_SHELLRUN ||
				m_nState == STATE_SHELLAWAKE)
			{
				return;
			}

			if ((m_bPowerUp && m_nDamagedCount < 10) ||
				(m_nDamagedCount < 5))
			{
				OnDamaged();

				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
			}
			else
			{
				// Dead.
				ChangeState(STATE_BURNT, pInfo);
				return;
			}
		}
		break;
	case STATE_BURNT:
		{
			if ((m_bPowerUp && m_nDamagedCount < 10) ||
				(m_nDamagedCount < 5))
			{
				OnDamaged();

				SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
				return;
			}
			else 
			{
				if (IS_MODERN)
				{
					if (m_bFlip)
						m_fXS = 2.0f;
					else
						m_fXS = -2.0f;
				}
				else
				{
					m_fXS = 0;
					m_bFlipV = true;
				}
				m_fYS = -3.0f;

				if (pInfo && pInfo->pFromObject)
				{
					if (pInfo->pFromObject->m_pController)
						pInfo->pFromObject->m_pController->AddScoreWithEffect(5000, this);
				}

				SOUND_MANAGER->PlaySoundEffect(L"Kicked");
			}
		}
		break;
	default:
        EnemyObjectBase::ChangeState(nState);
		return;
		break;
	}

	m_nNextState = nState;
}

void BowserJunior::OnChangeState(int nState)
{
	switch (nState)
	{
	case STATE_WALK:
		m_fXS = 0;
		break;
	case STATE_JUMP:
		{
			switch (m_nJumpCount)
			{
			case 0:
				// Back jump
				m_fXS = -1.0f;
				if (m_bFlip)
					m_fXS *= -1.0f;
				m_fYS = -3.0f;
				break;
			case 1:			
				// Front jump
				m_fXS = 1.0f;
				if (m_bFlip)
					m_fXS *= -1.0f;
				m_fYS = -3.0f;
				break;
			case 2:
				// Random jump
				m_fXS = 1.0f;
				if (rand() % 2 == 0)
					m_fXS *= -1.0f;
				m_fYS = -3.0f;
				break;
			case 3:
				// High jump
				m_fXS = 0;
				m_fYS = -5.0f;
				break;
			}

			if (m_fYS == -3.0f && m_bWinged)
				m_fYS = -4.0f;
			
			m_nJumpCount++;
			if (m_nJumpCount == 4)
				m_nJumpCount = 0;
		}
		break;
	case STATE_FLY:
		m_fXS = 0;
		m_fYS = -0.1f;
		break;
	case STATE_THROWFIRE:
		m_fXS = 0;
		break;
	case STATE_PRESSED:
		m_fXS = 0;
		m_fYS = -3.0f;
		break;
	case STATE_SHELLRUN:
		if (m_fX < CUR_PLAYER_OBJ->m_fX)
			m_bFlip = false;
		else
			m_bFlip = true;
		break;
	case STATE_SHELLAWAKE:
		m_fXS = 0;
		m_fYS = -3.0f;
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void BowserJunior::OnIdle()
{
	ChangeState(STATE_WALK);
}

void BowserJunior::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);

	m_fXS = 0;
}

void BowserJunior::OnWalk()
{
	if (CheckFalling())
		return;

	if (m_fX < CUR_PLAYER_OBJ->m_fX)
		m_bFlip = false;
	else
		m_bFlip = true;

	//
	// 0 s jump
	// 1 hammer
	// 2 s jump
	// 3 hammer
	// 4 fire
	// 5 fire
	// 6 fire
	// 7 s jump
	// 8 h jump
	//
	switch (m_nActionPhase)
	{
	case 0: // Short jump
	case 1:	// Short jump
	case 5:	// Random jump
	case 6:	// High jump
		if (m_bCanThrowHammer == false && m_nStateFrame == 4)
		{
			ChangeState(STATE_JUMP);
			m_nActionPhase++;
		}
		else if (m_bCanThrowHammer)
		{
			if (m_nStateFrame > 20 && m_nStateFrame < 120 && m_nStateFrame % 20 == 0)
			{
				ThrowHammer();
			}

			if (m_nStateFrame == 160)
			{
				ChangeState(STATE_JUMP);
				m_nActionPhase++;
			}
		}
		break;
	case 2:
	case 3:
	case 4: // Throw Fireball
		if (m_nStateFrame == 4)
		{
			if (m_bCanFire)
			{
				ChangeState(STATE_THROWFIRE);
				m_nActionPhase++;
			}
			else
			{
				ChangeState(STATE_JUMP);
				m_nActionPhase = 6;
			}
		}
		break;
	case 7:
		if (m_nStateFrame == 100)
		{
			// Same as phase 0
			ChangeState(STATE_JUMP);
			m_nActionPhase = 1;
		}
		break;
	}

	CheckReachWall();
}

void BowserJunior::OnShellRun()
{
	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}

		if (m_nStateFrame % 10 == 0)
		{
			SOUND_MANAGER->PlaySoundEffect(L"BoomerangSpin");
		}
	}

	if (m_nStateFrame < 260)
	{
		if (m_fX < CUR_PLAYER_OBJ->m_fX)
		{
			m_bFlip = false;
			if (m_fXS < 1.8f)
				m_fXS += 0.1f;
		}
		else
		{
			m_bFlip = true;
			if (m_fXS > -1.8f)
				m_fXS -= 0.1f;
		}
	}

	if (m_nStateFrame >= 260)
	{
		if (m_fXS > 0)
			m_fXS -= 0.05f;
		else if (m_fXS < 0)
			m_fXS += 0.05f;

		if (abs(m_fXS) < 0.1f)
			ChangeState(STATE_SHELLAWAKE);
	}

	CheckReachWall();
}

void BowserJunior::OnShellAwake()
{
	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(0.5f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}
	}

	if (m_nStateFrame == 100)
		ChangeState(STATE_WALK);
}

void BowserJunior::OnJump()
{
	if (m_bCollision[COLLISION_TOP])
	{
		// Hit the block
		SIZE size = GetSize();
		m_fYS = 0;
		m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

		ChangeState(STATE_FALL);

		int nPower = 2;
		CUR_STAGE->HitBlock(
			m_ptCollision[COLLISION_TOP].x / TILE_XS, 
			(m_ptCollision[COLLISION_TOP].y) / TILE_YS, 
			nPower, 
			this, 
			this);
		return;
	}

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();

		if (m_nJumpCount != 0)
		{
			OnApplyGravity(0.5f, 4.0f);
		}
		else 
		{
			if (m_fYS < 1.0f && m_fYS > -1.0f)
			{
				if (m_bWinged && m_fYS > 0)
					ChangeState(STATE_FLY);
			}

			if (m_fYS < -1.0f)
				OnApplyGravity(0.5f, 4.0f);
			else if (m_fYS < 1.0f)
				OnApplyGravity(0.25f, 4.0f);
			else
				OnApplyGravity(2.0f, 5.0f);
		}

	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fXS = 0;
			m_fYS = 0;
			
			OnLanding();
		}
	}

	CheckReachWall();
}

void BowserJunior::OnFly()
{
	if (m_nStateFrame == 48)
		ChangeState(STATE_FALL);
}

void BowserJunior::OnThrowFire()
{
	if (CheckFalling())
		return;

	if (m_nStateFrame == 4)
		m_bAimFire = true;

	if (m_nStateFrame == 60)
		ThrowFire();

	if (m_nStateFrame == 62)
		ChangeState(STATE_WALK);

	CheckReachWall();
}

void BowserJunior::OnStacked()
{
	EnemyObjectBase::OnStacked();

	if (CUR_PLAYER_OBJ->m_fX < m_fX)
	{
		m_bFlip = true;
	}
	else
	{
		m_bFlip = false;
	}

	CheckThrowFire();

	if (m_bAimFire && m_nThrowFireCooltime == 0)
	{
		if (m_bPowerUp)
			m_nThrowFireCooltime = 80;
		else
			m_nThrowFireCooltime = 120;
	}

	if (m_nThrowFireCooltime > 0)
		m_nThrowFireCooltime--;

	if (m_nThrowFireCooltime == 60)
	{
		ThrowFire();
	}
}

void BowserJunior::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
		ChangeState(STATE_STAND);

	if (!IsOutOfCamera() && m_nJumpCount == 0)
	{
		CUR_STAGE->MakeEarthquake(POWER_EARTHQUAKE_SLEDGEBROTHERS);

		SOUND_MANAGER->PlaySoundEffect(L"Cannon");
	}
}

void BowserJunior::OnPressed()
{
	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1.0f, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}
	}

	if (m_nStateFrame == 120)
	{
		ChangeState(STATE_SHELLRUN);
	}
}

void BowserJunior::OnDamaged()
{
	m_nDamagedCount++;
	m_nDamagedCooltime = 16;
}

bool BowserJunior::IsAlive()
{
	switch (m_nState)
	{
	case STATE_PRESSED:
		return true;
		break;
	}

	return EnemyObjectBase::IsAlive();
}

void BowserJunior::CheckThrowFire()
{
	if (!m_bCanFire)
		return;

	if (m_nThrowFireCooltime > 0)
		return;

	if (rand() % 60 == 0)
	{
		m_bAimFire = true;
	}

	if (m_bAimFire && m_nState == STATE_WALK)
	{
		ChangeState(STATE_THROWFIRE);
	}
}


bool BowserJunior::CanEat()
{
	return false;
}

bool BowserJunior::CanPressed(int nPower)
{
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_SHELLRUN:
	case STATE_SHELLAWAKE:
		return false;
		break;
	}

	return true;
}

bool BowserJunior::CanDamageEnemy()
{
	switch (m_nState)
	{
	case STATE_PRESSED:
		if (m_bCollision[COLLISION_BOTTOM])
			return true;
		break;
	case STATE_SHELLRUN:
	case STATE_SHELLAWAKE:
		return true;
		break;
	default:
		return EnemyObjectBase::CanDamageEnemy();
		break;
	}

	return false;
}

std::vector<POINT> BowserJunior::GetWingPosition()
{
	std::vector<POINT> vec;

	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_SHELLRUN:
	case STATE_SHELLAWAKE:
		return vec;
		break;
	}

	POINT pt;
	SIZE s = GetSize();
	pt.x = -6;
	pt.y = -10;

	vec.push_back(pt);

	return vec;
}

int BowserJunior::GetWingSpriteIndex()
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

int BowserJunior::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
		nFrame = SPRIDX_BOWSERJUNIOR_SHELL1;
		break;
	case STATE_SHELLRUN:
	case STATE_SHELLAWAKE:
		{
			int nDelay = 5;
			float fSpeed = abs(m_fXS);
			if (fSpeed > 0)
			{
				if (fSpeed < 1.0f)
					nDelay = 4;
				else if (fSpeed < 2.0f)
					nDelay = 3;
				else
					nDelay = 2;
			}

			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BOWSERJUNIOR_SHELL1, 4, nDelay);
		}
		break;
	case STATE_BURNT:
	case STATE_DEAD:
		{
			if (m_bWinged)
			{
				m_bWinged = false;
				OnLostWing();
			}

			nFrame = SPRIDX_BOWSERJUNIOR1;
		}
		break;
	default:
		nFrame = SPRIDX_BOWSERJUNIOR1 + (m_nStateFrame / 16) % 2;
		if (m_bAimFire)
			nFrame = SPRIDX_BOWSERJUNIOR3;
		break;
	}

	return nFrame;
}

void BowserJunior::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (IS_MODERN && m_nDamagedCooltime > 0)
	{
		fScaleX += (m_nDamagedCooltime / 2) / 10.0f;
		fScaleY += (m_nDamagedCooltime / 2) / 10.0f;
	}
}

SIZE BowserJunior::GetSize()
{
	SIZE s = { 16, 16 };
	s.cx *= m_fScaleX;
	s.cy *= m_fScaleY;

	return s;
}

void BowserJunior::ThrowFire()
{
	float fDistX = (m_fX - CUR_PLAYER_OBJ->m_fX);
	float fDistY = (CUR_PLAYER_OBJ->GetRect().Middle() - m_fY);

	POINT ptMouth = GetMouthPosition();
	int nRandomY = m_fY - (((rand() % 4) * 16) - 12);

	CreateParameter param;
	param.pFromPlayer = nullptr;
	param.pFromObject = this;
	param.nType = -1;
	param.fScaleX = 2.0f * m_fScaleX;
	param.fScaleY = 2.0f * m_fScaleY;

	float fDist = sqrt(fDistX * fDistX + fDistY * fDistY);

	param.fXS = abs(fDistX / fDist);
	param.fYS = (fDistY / fDist);	

	CUR_STAGE->CreateBullet(
		ptMouth.x,
		ptMouth.y,
		BULLET_FIREBALL, &param);
	
	SOUND_MANAGER->PlaySoundEffect(L"ThrowFireball");

	m_bAimFire = false;
}

void BowserJunior::ThrowHammer()
{
	POINT ptHand = GetHandPosition();

	CreateParameter param;
	param.pFromPlayer = nullptr;
	param.pFromObject = this;
	param.nType = BULLET_THROWINGHAMMER;
	param.fScaleX = m_fScaleX;
	param.fScaleY = m_fScaleY;

	GameObjectBase *pHammer = CUR_STAGE->CreateBullet(
		ptHand.x,
		ptHand.y,
		BULLET_THROWINGHAMMER, &param);
	pHammer->ChangeState(STATE_WALK);
}

POINT BowserJunior::GetMouthPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? -8 : 8));
	pt.y = m_fY - (m_fScaleY * 8);

	return pt;
}

POINT BowserJunior::GetHandPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? +2 : -2));
	pt.y = m_fY - (m_fScaleY * 16);

	return pt;
}
