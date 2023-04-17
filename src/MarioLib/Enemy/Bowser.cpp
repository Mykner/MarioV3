#include "Bowser.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "BowserFire.h"
#include "SoundManager.h"

#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Bowser)
	PROP_INT("RealType",		VT_I4,		-1,		g_szEnemyTypes, "Bowser"),
	PROP_BOOL("CanFire",		VT_BOOL,	true,	0, "Bowser"),
	PROP_BOOL("CanThrowHammer", VT_BOOL,	false,	0, "Bowser"),
	PROP_BOOL("PowerUp",		VT_BOOL,	false,	nullptr, "Bowser"),
	PROP_BOOL("Winged",			VT_BOOL,	false,	nullptr, "Bowser"),
END_IMPL_PROPERTYMAP()

Bowser::Bowser(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_BOWSER;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nInitialX = -1;
	m_bWalkFront = true;
	m_bAimFire = false;
	m_nRealType = ENEMY_BOWSER - ENEMY_BEGIN;

	m_bCanFire = true;
	m_bCanThrowHammer = false;
	m_pHammer = nullptr;
	m_nThrowHammerCombo = 0;

	m_nDamagedCount = 0;
	m_nDamagedCooltime = 0;
	m_nThrowFireCooltime = 0;
	m_nChargeFireFrame = 0;
}

Bowser::~Bowser()
{
}

void Bowser::OnAfterProcess()
{
	EnemyObjectBase::OnAfterProcess();

	if (m_pHammer)
	{
		POINT ptHand = GetHandPosition();
		m_pHammer->m_fX = ptHand.x;
		m_pHammer->m_fY = ptHand.y;
	}

	if (m_nDamagedCooltime > 0)
		m_nDamagedCooltime--;
}

void Bowser::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	if (m_nNextState != -1)
		return;

	switch (nState)
	{
	case STATE_WALK:
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_BURNT:
		if (OnDamaged((pInfo && pInfo->pFromObject) ? pInfo->pFromObject : nullptr) == false)
		{
			return;
		}
		break;
	case STATE_PRESSED:
		{
			int nPower = POWER_PRESS_NORMAL;
			if (pInfo && pInfo->nPower)
				nPower = pInfo->nPower;

			switch (nPower)
			{
			case POWER_PRESS_YOSHI:
				{
					OnStomped(false, pInfo->pFromObject);
					SOUND_MANAGER->PlaySoundEffect(L"YoshiStompImmune");
				}
				break;
			case POWER_PRESS_GOOMBASHOES:
				if (OnDamaged((pInfo && pInfo->pFromObject) ? pInfo->pFromObject : nullptr))
					m_nNextState = STATE_BURNT;
				break;
			default:
				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				break;
			}

			// Cannot press!
			return;
		}
		break;
	default:
        EnemyObjectBase::ChangeState(nState);
		return;
		break;
	}

	m_nNextState = nState;
}

void Bowser::OnChangeState(int nState)
{
	switch (nState)
	{
	case STATE_JUMP:
		if (IS_CLASSIC)
			m_fYS = -2.0f;
		else
			m_fYS = -4.0f;

		if (m_bPowerUp)
			m_fYS *= 1.5f;
		break;
	case STATE_FLY:
		m_fXS = 0;
		m_fYS = -0.1f;
		break;
	case STATE_THROWFIRE:
		m_nChargeFireFrame = 0;
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void Bowser::OnIdle()
{
	m_nInitialX = m_fX;
	ChangeState(STATE_WALK);
}

void Bowser::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);
}

void Bowser::OnWalk()
{
	CheckFalling();
		
	if (CUR_PLAYER_OBJ->m_fX < m_fX || !IS_CLASSIC)
	{
		if (IS_CLASSIC)
			m_bFlip = true;
		else if (CUR_PLAYER_OBJ->m_fX < m_fX)
			m_bFlip = true;
		else
			m_bFlip = false;

		if (m_nStateFrame % 10 == 0 && rand() % 8 == 0)
		{
			ChangeState(STATE_JUMP);
		}

		if (m_nStateFrame % 4 == 0)
		{
			if (m_nInitialX != -1)
			{
				if (m_fX < m_nInitialX - 16 * 4 || rand() % 10 == 0)
					m_bWalkFront = false;
				else if (m_fX > m_nInitialX + 16 || rand() % 10 == 0)
					m_bWalkFront = true;
			}

			if (m_bWalkFront)
			{
				m_fXS = -1 * 0.4f;
			}
			else
			{
				m_fXS = 0.4f;
			}
		}

		CheckThrowFire();
		CheckThrowHammer();
	}
	else 
	{
		m_bAimFire = false;
		m_bFlip = false;
		m_fXS = 0.5f;

		if (m_pHammer)
		{
			m_pHammer->m_pOwner = nullptr;
			m_pHammer->ChangeState(STATE_DEAD);
			m_pHammer = nullptr;
		}
	}

	CheckReachWall();
}

void Bowser::OnJump()
{
	// Jump Frames
	// 1 1 1 2 2 1 2 2 1 2 1 2 1 1 2 1 1 1 1 1 1 1 0 1 1 0 1 0 1 0 1 0 0 0 0 0 0 0 0 0 -1
	if (m_bCollision[COLLISION_TOP])
	{
		// Something on my head
		float fRangeX = (m_ptCollision[COLLISION_TOP].x + (TILE_XS / 2)) - m_fX;
		if (abs(fRangeX) >= 12.0f)
		{
			if (abs(fRangeX) < 16.0f)
			{
				// Slide
				if (fRangeX > 0)
					m_fX--;
				else
					m_fX++;
			}
		}
		else
		{
			// Hit the block
			SIZE size = GetSize();
			m_fYS = 0;
			m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

			ChangeState(STATE_FALL);

			int nPower = 2;

			CUR_STAGE->HitBlock(m_ptCollision[COLLISION_TOP].x / TILE_XS, (m_ptCollision[COLLISION_TOP].y) / TILE_YS, nPower, this, this);
			return;
		}
	}

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();

		if (m_fYS < 1.0f && m_fYS > -1.0f)
		{
			if (m_bWinged && m_fYS > 0)
				ChangeState(STATE_FLY);
		}

		if (IS_CLASSIC)
			m_fYS += 0.08f;
		else
			m_fYS += fGravity / 2.0f;

		if (m_fYS > 4.0f)
			m_fYS = 4.0f;

		if (CheckRideVehicle())
		{
			ChangeState(STATE_RIDE);
			return;
		}
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			
			OnLanding();
		}
	}

	CheckThrowFire();
	CheckThrowHammer();

	CheckReachWall();
}

void Bowser::OnFly()
{
	if (m_nStateFrame == 48)
		ChangeState(STATE_FALL);

	CheckThrowFire();
	CheckThrowHammer();
}

void Bowser::OnThrowFire()
{
	if (CheckFalling())
		return;

	if (m_nChargeFireFrame == 60)
		ThrowFire();

	if (m_bPowerUp && m_nStateFrame == 80)
		ChangeState(STATE_WALK);
	else if (m_nStateFrame == 120)
		ChangeState(STATE_WALK);

	CheckReachWall();

	m_nChargeFireFrame++;
}

void Bowser::OnStacked()
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
	CheckThrowHammer();

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

void Bowser::OnItemPop()
{
	m_nInitialX = m_fX;

	EnemyObjectBase::OnItemPop();
}

void Bowser::OnItemPopDown()
{
	m_nInitialX = m_fX;

	EnemyObjectBase::OnItemPopDown();
}

void Bowser::OnRide()
{
	EnemyObjectBase::OnRide();

	CheckThrowFire();
	CheckThrowHammer();

	if (m_bAimFire)
	{
		if (m_nChargeFireFrame == 60)
		{
			ThrowFire();
		}

		if (m_nChargeFireFrame == 120)
		{
			m_bAimFire = false;
			m_nChargeFireFrame = 0;
		}

		m_nChargeFireFrame++;
	}
}

bool Bowser::OnDamaged(GameObjectBase *pAttacker)
{
	if ((m_bPowerUp && m_nDamagedCount < 12) ||
		(m_nDamagedCount < 6))
	{
		m_nDamagedCount++;
		m_nDamagedCooltime = 16;

		SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
		return false;
	}
	else
	{
		if (m_pHammer)
		{
			m_pHammer->m_pOwner = nullptr;
			m_pHammer->ChangeState(STATE_DEAD);
			m_pHammer = nullptr;
		}

		if (IS_MODERN)
		{
			if (m_bFlip)
				m_fXS = 2.0f;
			else
				m_fXS = -2.0f;
		}
		else
		{
			m_bFlipV = true;
			m_fXS = 0;
		}

		m_fYS = -3.0f;

		if (pAttacker)
		{
			if (pAttacker->m_pController)
				pAttacker->m_pController->AddScoreWithEffect(5000, this);
		}

		ReleaseVehicle();

		SOUND_MANAGER->PlaySoundEffect(L"Kicked");
	}
	return true;
}

bool Bowser::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

void Bowser::CheckThrowFire()
{
	if (!m_bCanFire)
		return;
	if (m_nThrowFireCooltime > 0)
		return;

	if ((IS_CLASSIC && rand() % 160 == 0) ||
		(!IS_CLASSIC && rand() % 60 == 0))
	{
		m_bAimFire = true;
	}

	if (m_bAimFire && m_nState == STATE_WALK)
	{
		ChangeState(STATE_THROWFIRE);
	}
}

void Bowser::CheckThrowHammer()
{
	if (!m_bCanThrowHammer)
		return;
	if (m_bAimFire)
		return;

	if (m_nThrowHammerCooltime == 0 && rand() % 1 == 0)
	{
		POINT ptHand = GetHandPosition();

		CreateParameter param;
		param.pFromPlayer = nullptr;
		param.pFromObject = this;
		param.nType = BULLET_THROWINGHAMMER;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		m_pHammer = (ThrowingHammer*)CUR_STAGE->CreateBullet(
			ptHand.x,
			ptHand.y,
			BULLET_THROWINGHAMMER, &param);
	}

	switch (m_nThrowHammerCooltime)
	{
	default:
		if (m_pHammer)
		{
			POINT ptHand = GetHandPosition();

			m_pHammer->m_fX = ptHand.x;
			m_pHammer->m_fY = ptHand.y;
		}
		break;
	case 8:
		{
			if (m_pHammer)
			{
				m_pHammer->m_pOwner = nullptr;
				m_pHammer->ChangeState(STATE_WALK);
			}
			m_pHammer = nullptr;
		}
		break;
	case 9:
	case 32:
		if (rand() % 8 != 0 && m_nThrowHammerCombo < 8)
		{
			m_nThrowHammerCooltime = 0;
			m_nThrowHammerCombo++;
		}
		break;
	case 256:
		m_nThrowHammerCombo = 0;
		m_nThrowHammerCooltime = 0;
		break;
	}

	if (m_nThrowHammerCooltime > 0 || m_pHammer != nullptr)
		m_nThrowHammerCooltime++;
}

bool Bowser::CanEat()
{
	return false;
}

bool Bowser::CanPressed(int nPower)
{
	switch (nPower)
	{
	case POWER_PRESS_GOOMBASHOES:
		return true;
		break;
	}
	return false;
}

std::vector<POINT> Bowser::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -8;
	pt.y = -18;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int Bowser::GetWingSpriteIndex()
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

int Bowser::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_BURNT:
	case STATE_DEAD:
		{
			if (m_bWinged)
			{
				m_bWinged = false;
				OnLostWing();
			}

			switch (m_nRealType + ENEMY_BEGIN)
			{
			case ENEMY_GOOMBA: // World1
				nFrame = SPRIDX_GOOMBA1;
				break;
			case ENEMY_TURTLE: // World2
				nFrame = SPRIDX_TURTLE_SHELL1;
				break;
			case ENEMY_BUZZYBEETLE: // World3
				nFrame = SPRIDX_BUZZYBEETLE_SHELL;
				break;
			case ENEMY_SPINY: // World4
				nFrame = SPRIDX_SPINY1;
				break;
			case ENEMY_LAKITU: // World5
				nFrame = SPRIDX_LAKITU;
				break;
			case ENEMY_BLOOBER: // World6
				nFrame = SPRIDX_BLOOBER1;
				break;
			case ENEMY_HAMMERBROTHERS: // World7
				nFrame = SPRIDX_HAMMERBROTHERS1;
				break;
			default:
				nFrame = SPRIDX_BOWSER1;
				break;
			}
		}
		break;
	default:
		nFrame = SPRIDX_BOWSER1 + (m_nStateFrame / 16) % 2;
		if (m_bAimFire)
			nFrame += 2;
		break;
	}

	return nFrame;
}

void Bowser::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (IS_MODERN && m_nDamagedCooltime > 0)
	{
		fScaleX += (m_nDamagedCooltime / 2) / 15.0f;
		fScaleY += (m_nDamagedCooltime / 2) / 15.0f;
	}
}

SIZE Bowser::GetSize()
{
	SIZE s = { 24, 24 };
	s.cx *= m_fScaleX;
	s.cy *= m_fScaleY;

	return s;
}

void Bowser::ThrowFire()
{
	POINT ptMouth = GetMouthPosition();
	int nRandomY = m_fY - (((rand() % 4) * 16) - 12);

	CreateParameter param;
	param.pFromPlayer = nullptr;
	param.pFromObject = this;
	param.nType = BULLET_BOWSERFIRE;
	param.fScaleX = m_fScaleX;
	param.fScaleY = m_fScaleY;

	BowserFire *pFire = (BowserFire*)CUR_STAGE->CreateBullet(
		ptMouth.x,
		ptMouth.y,
		BULLET_BOWSERFIRE, &param);
	pFire->m_nTargetY = nRandomY;

	SOUND_MANAGER->PlaySoundEffect(L"BowserFire");

	m_bAimFire = false;
}

POINT Bowser::GetMouthPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? -16 : 16));
	pt.y = m_fY - (m_fScaleY * 16);

	return pt;
}

POINT Bowser::GetHandPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? +2 : -2));
	pt.y = m_fY - (m_fScaleY * 36);

	return pt;
}
