#include "PiranhaPlant.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(PiranhaPlant)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "PiranhaPlant"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "PiranhaPlant")
END_IMPL_PROPERTYMAP()

PiranhaPlant::PiranhaPlant(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_PIRANHA_PLANT;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nBaseState = STATE_STAND;

	m_nPipeDepth = 0;
	m_bHideForExitPipe = false;

	// For FirePiranhaPlant only
	m_bLookAtDown = true;
}

PiranhaPlant::~PiranhaPlant()
{
}

void PiranhaPlant::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_MOVEUP:
	case STATE_MOVEDOWN:
		m_fYS = 0.5f;
		
		if (m_bPowerUp)
			m_fYS *= 2;

		if (nState == STATE_MOVEUP)
			m_fYS *= -1;
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
				{
					if (m_nType == ENEMY_MUNCHER)
						return;

					m_nNextState = STATE_BURNT;
					SOUND_MANAGER->PlaySoundEffect(L"Kicked");
				}
				break;
			default:
				if (m_nType == ENEMY_MUNCHER)
					return;

				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				break;
			}

			// Cannot press!
			return;
		}
		break;
	case STATE_PUSHEDUP:
		ReleaseEnemyStack();

		m_fYS = -3.0f;
		nState = STATE_FALL;
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void PiranhaPlant::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_JUMP:
		m_fYS = -7.0f;
		break;
	case STATE_FLY:
		m_fYS = 0;
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void PiranhaPlant::OnIdle()
{
	if (m_nStateFrame == 0)
	{
		m_fInitialY = m_fY;
		ChangeState(STATE_STAND);
	}
}

void PiranhaPlant::OnStand()
{
	if (m_nType == ENEMY_FIREPIRANHAPLANT)
		CheckLookAtPlayer();

	if (m_bCollision[COLLISION_BOTTOM])
	{
		if (m_bWinged && m_nStateFrame == 48)
		{
			ChangeState(STATE_JUMP);
		}

		switch (m_nType)
		{
		case ENEMY_FIREPIRANHAPLANT:
			if (m_nStateFrame == 91)
			{
				ThrowFireBall();
			}
			if (m_nStateFrame == 180)
			{
				bool bPipeExist = CheckPipeExist();
				if (bPipeExist)
					ChangeState(STATE_MOVEDOWN);
				else
					ChangeState(STATE_STAND);
			}
			break;
		default:
			if (m_nStateFrame == 60)
			{
				bool bPipeExist = CheckPipeExist();
				if (bPipeExist)
					ChangeState(STATE_MOVEDOWN);
			}
			break;
		}
		
	}
	else
	{
		ChangeState(STATE_FALL);
	}
}

void PiranhaPlant::OnJump()
{
	if (m_bCollision[COLLISION_TOP])
	{
		SIZE size = GetSize();
		m_fYS = 0;
		m_fY = ((m_ptCollision[COLLISION_TOP].y + 1)) + size.cy;

		ChangeState(STATE_FALL);
	}

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		if (m_nType == ENEMY_FIREPIRANHAPLANT ||
			m_nType == ENEMY_MUNCHER)
		{
			if (m_fYS > -1.0f)
				ChangeState(STATE_FLY);
		}

		OnApplyGravity(1, 4.0f);
	}
	else
	{
		if ((int)m_fY - 4 <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;

			OnLanding();
		}
	}
}

void PiranhaPlant::OnFly()
{
	int nPhaseFrame = 24;

	int nVal = m_nStateFrame % (nPhaseFrame * 4);
	if (nVal < nPhaseFrame || (nVal >= nPhaseFrame * 3))
		m_fYS += 0.05f;
	else if (nVal >= nPhaseFrame && nVal < nPhaseFrame * 3)
		m_fYS -= 0.05f;

	CheckLookAtPlayer();

	if (m_nType == ENEMY_FIREPIRANHAPLANT)
	{
		if (m_nStateFrame == nPhaseFrame * 6)
			ThrowFireBall();

		if (m_nStateFrame == nPhaseFrame * 12)
			ChangeState(STATE_FALL);
	}
	else if (m_nType == ENEMY_MUNCHER)
	{
		if (m_nStateFrame == nPhaseFrame * 6)
			ChangeState(STATE_FALL);
	}
}

void PiranhaPlant::OnMoveUp()
{
	m_nPipeDepth = m_fY - m_fInitialY;

	if (m_nType == ENEMY_FIREPIRANHAPLANT)
		CheckLookAtPlayer();

	if (m_fY == m_fInitialY)
	{
		m_fYS = 0;

		if (m_bWinged)
			ChangeState(STATE_STAND);
	}

	if (m_nStateFrame == 120)
	{
		switch (m_nType)
		{
		case ENEMY_FIREPIRANHAPLANT:
			ChangeState(STATE_STAND);
			break;
		default:
			ChangeState(STATE_MOVEDOWN);
			break;
		}
	}

	if (!CheckPipeExist())
		ChangeState(STATE_STAND);
}

void PiranhaPlant::OnMoveDown()
{
	m_nPipeDepth = m_fY - m_fInitialY;
	
	if (m_nType == ENEMY_FIREPIRANHAPLANT)
		CheckLookAtPlayer();

	if (m_fY >= m_fInitialY + (30 * m_fScaleY))
	{
		m_fYS = 0;
		m_bHideForExitPipe = false;
	}

	if (m_nStateFrame >= 120)
	{
		// Prevent player killed too often
		int nDistance = abs(m_fX - CUR_PLAYER_OBJ->m_fX);
		if (nDistance > 25)
			ChangeState(STATE_MOVEUP);
	}

	if (!CheckPipeExist())
		ChangeState(STATE_STAND);
}

void PiranhaPlant::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
	{
		if (!CheckPipeExist())
			ChangeState(STATE_STAND);
		else
			ChangeState(STATE_MOVEDOWN);
	}
}

void PiranhaPlant::OnStacked()
{
	EnemyObjectBase::OnStacked();
	if (m_pUnderMyFoot == nullptr)
		return;

	switch (m_nType)
	{
	case ENEMY_FIREPIRANHAPLANT:
		CheckLookAtPlayer();

		if (m_nStateFrame == 91)
			ThrowFireBall();

		if (m_nStateFrame == 180)
			m_nStateFrame = 0;
		break;
	}
}

void PiranhaPlant::OnRide()
{
	EnemyObjectBase::OnRide();

	switch (m_nType)
	{
	case ENEMY_FIREPIRANHAPLANT:
		CheckLookAtPlayer();

		if (m_nStateFrame == 91)
			ThrowFireBall();

		if (m_nStateFrame == 180)
			m_nStateFrame = 0;
		break;
	}
}

bool PiranhaPlant::CheckPipeExist()
{
	int nData = CUR_STAGE->GetTileData(
		(m_fX - 8) / TILE_XS, 
		(m_fInitialY + 1) / TILE_YS);

	if (nData == TILE_PIPEHEAD_L)
		return true;

	return false;
}

void PiranhaPlant::HideForExitPipe()
{
	if (m_nState == STATE_IDLE && m_nStateFrame == 0)
		m_fInitialY = m_fY;

	ChangeState(STATE_MOVEDOWN);

	m_bHideForExitPipe = true;
}

void PiranhaPlant::CheckLookAtPlayer()
{
	if (m_fY - 16 < CUR_PLAYER_OBJ->m_fY)
		m_bLookAtDown = true;
	else
		m_bLookAtDown = false;

	if (m_fX > CUR_PLAYER_OBJ->m_fX)
		m_bFlip = true;
	else
		m_bFlip = false;
}

void PiranhaPlant::ThrowFireBall()
{
	if (IsOutOfCamera())
		return;

	int nDistX = abs(m_fX - CUR_PLAYER_OBJ->m_fX);
	int nDistY = abs(m_fY - CUR_PLAYER_OBJ->m_fY);

	CreateParameter param;
	param.pFromPlayer = nullptr;
	param.pFromObject = this;
	param.nType = -1;

	if (nDistX > 48)
	{
		param.fXS = 1.25f;
		param.fYS = 0.75f;
	}
	else
	{
		param.fXS = 1.0f;
		param.fYS = 1.0f;
	}
	
	if (!m_bLookAtDown)
		param.fYS *= -1.0f;

	if (m_bPowerUp)
	{
		param.fScaleX = 2.0f;
		param.fScaleY = 2.0f;
	}

	CUR_STAGE->CreateBullet(m_fX, m_fY - (12.0f * m_fScaleY), BULLET_FIREBALL, &param);
	SOUND_MANAGER->PlaySoundEffect(L"ThrowFireball");
}

bool PiranhaPlant::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool PiranhaPlant::CanLand()
{
	if (m_nType == ENEMY_MUNCHER)
		return true;

	return false;
}

bool PiranhaPlant::CanCollide(int nDir)
{
	if (m_nType == ENEMY_MUNCHER)
		return true;

	return false;
}

bool PiranhaPlant::CanBurnt(int nType)
{
	if (m_nInvinsibleTime > 0)
		return false;
	if (m_nDamageCooltime > 0)
		return false;
	if (m_nType == ENEMY_MUNCHER)
	{
		switch (nType)
		{
		case BURN_POWBLOCK:
			return true;
		}

		return false;
	}

	return true;
}

bool PiranhaPlant::CanEat()
{
	if (m_nType == ENEMY_MUNCHER)
		return false;

	return EnemyObjectBase::CanEat();
}

bool PiranhaPlant::CanDamageEnemy()
{
	switch (m_nState)
	{
	case STATE_MOVEDOWN:
	case STATE_MOVEUP:
		if (m_fY >= m_fInitialY + 30)
			return false;
		break;
	}

	if (m_bHideForExitPipe)
		return false;

	return EnemyObjectBase::CanDamageEnemy();
}

bool PiranhaPlant::CanPressed(int nPower)
{
	switch (nPower)
	{
	case POWER_PRESS_YOSHI:
		return true;
		break;
	case POWER_PRESS_GOOMBASHOES:
		if (m_nType == ENEMY_MUNCHER)
			return false;
		return true;
		break;
	default:
		return false;
		break;
	}
	return false;
}

std::vector<POINT> PiranhaPlant::GetWingPosition()
{
	int nWidth = 9;
	if (m_nType == ENEMY_MUNCHER)
		nWidth = 6;

	if (m_nState == STATE_MOVEDOWN || m_nState == STATE_MOVEUP)
	{
		if (m_fY - m_fInitialY > 16)
			nWidth -= 5;
		else if (m_fY - m_fInitialY > 12)
			nWidth -= 4;
		else if (m_fY - m_fInitialY > 8)
			nWidth -= 3;
		else if (m_fY - m_fInitialY > 4)
			nWidth -= 2;
		else if (m_fY - m_fInitialY > 0)
			nWidth -= 1;
	}

	POINT pt;
	SIZE s = GetSize();
	pt.x = -nWidth;
	pt.y = -2;

	std::vector<POINT> vec;
	vec.push_back(pt);

	pt.x = +nWidth;
	vec.push_back(pt);

	return vec;
}

int PiranhaPlant::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_nStateFrame % 16 >= 8)
		nFrame = SPRIDX_WING2;

	return nFrame;
}

int PiranhaPlant::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		if (m_pGame->m_bModernStyle)
		{
			switch (m_nType)
			{
			case ENEMY_PIRANHA_PLANT:
				nFrame = SPRIDX_PIRANHAPLANT1;
				break;
			case ENEMY_MUNCHER:
				nFrame = SPRIDX_MUNCHER1;
				break;
			case ENEMY_FIREPIRANHAPLANT:
				nFrame = SPRIDX_FIREPIRANHAPLANT_UP1;
				break;
			}
		}
		else
			nFrame = -1;
		break;
	default:
		{
			if ((m_nState == STATE_MOVEDOWN || m_nState == STATE_MOVEUP) && 
				m_fY - m_fInitialY > 30)
				return -1;

			switch (m_nType)
			{
			case ENEMY_PIRANHA_PLANT:
				nFrame = SPRIDX_PIRANHAPLANT1 + (m_nStateFrame / 8) % 2;
				break;
			case ENEMY_MUNCHER:
				nFrame = SPRIDX_MUNCHER1 + (m_nStateFrame / 8) % 2;
				break;
			case ENEMY_FIREPIRANHAPLANT:
				{
					if (m_bLookAtDown)
						nFrame = SPRIDX_FIREPIRANHAPLANT_DOWN1;
					else
						nFrame = SPRIDX_FIREPIRANHAPLANT_UP1;

					if (m_nState == STATE_STAND)
					{
						if (m_nStateFrame < 60)
							nFrame += (m_nStateFrame / 8) % 2;
						else if (m_nStateFrame < 90)
							nFrame += 1;
						else if (m_nStateFrame < 120)
							;
						else
							nFrame += (m_nStateFrame / 8) % 2;
					}
					else
						nFrame += (m_nStateFrame / 8) % 2;
				}
				break;
			}
		}
		break;
	}

	return nFrame;
}

int PiranhaPlant::GetZOrder()
{
	int nBaseZOrder = EnemyObjectBase::GetZOrder();
	if (nBaseZOrder != Z_ENEMY)
		return nBaseZOrder;

	switch (m_nState)
	{
	case STATE_MOVEDOWN:
	case STATE_MOVEUP:
		return Z_BEHIND_MAP;
		break;
	}

	return Z_COMMON_OBJ;
}

void PiranhaPlant::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_bPowerUp && m_nPipeDepth > 16)
	{
		int nDepth = m_nPipeDepth - 16;
		if (nDepth > 16)
			nDepth = 16;
		
		fScaleX *= (1.0f - (nDepth / 32.0f));
		fScaleY *= (1.0f - (nDepth / 32.0f));
	}

	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 40)
	{
		float fFactorX = 1.0f, fFactorY = 1.0f;
		SIZE s = GetSpriteSize();
		if (s.cx > 16)
			fFactorX = (16.0f / (float)s.cx);

		if (s.cy > 16)
			fFactorY = (16.0f / (float)s.cy);

		float fFactor = fFactorX < fFactorY ? fFactorX : fFactorY;
		if (m_nStateFrame >= 36 && m_nStateFrame < 40)
		{
			// factor = 0.5

			// 0.5 === 1
			// 0.6 === f * 6/5
			// 0.7 === f * 7/5
			// ..
			// 1
			int nFrame = m_nStateFrame - 35;
			int nMaxFrame = 4;
			fFactor = fFactor * ((float)(nFrame + nMaxFrame) / (float)nMaxFrame);
		}

		fScaleX *= fFactor;
		fScaleY *= fFactor;
	}
}

NaRect PiranhaPlant::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();

	rc.left = m_fX - (s.cx / 2);
	rc.right = m_fX + (s.cx / 2) - 1;
	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	if (m_nState == STATE_MOVEUP || m_nState == STATE_MOVEDOWN)
	{
		if (m_fY - m_fInitialY > 30)
		{
			rc.top = m_fInitialY + 29;
			rc.bottom = m_fInitialY + 30;
		}
		else if (rc.top > m_fInitialY)
		{
			rc.bottom = rc.top + 1;
		}
		else
		{
			rc.bottom = m_fInitialY;

			if (rc.top > rc.bottom)
				rc.top = rc.bottom;
		}
	}

	return rc;
}

SIZE PiranhaPlant::GetSize()
{
	float fScaleX = m_fScaleX;
	float fScaleY = m_fScaleY;
	GetScale(fScaleX, fScaleY);

	SIZE s = 
	{ 
		14 * fScaleX, 
		16 * fScaleY 
	};
	return s;
}
