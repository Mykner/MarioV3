#include "SledgeBrothers.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(SledgeBrothers)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "SledgeBrothers"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "SledgeBrothers")
END_IMPL_PROPERTYMAP()

SledgeBrothers::SledgeBrothers(Game *pGame, Stage *pStage) :
	HammerBrothers(pGame, pStage)
{
	m_nType = ENEMY_SLEDGEBROTHERS;

	m_bPassGround = false;
}

SledgeBrothers::~SledgeBrothers()
{
}

void SledgeBrothers::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_STAND:
		m_fXS = 0;
		break;
	case STATE_MOVEUP:
		if (!m_bWinged)
			m_fYS = -6.5f;
		else
			m_fYS = -8.0f;
		break;
	case STATE_FLY:
		m_fYS = -0.1f;
		break;
	default:
		HammerBrothers::OnChangeState(nState);
		break;
	}
}

void SledgeBrothers::OnStand()
{
	if (m_nStateFrame == 24)
		ChangeState(STATE_WALK);
}

void SledgeBrothers::OnMoveUp()
{
	CheckThrowWeapon();

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();
		if (m_fYS < 1.0f && m_fYS > -1.0f)
		{
			m_fXS *= 0.75f;
			if (m_fYS < 0 && m_fYS + (fGravity * 0.5f) > 0)
				m_fXS = 0;

			if (m_bWinged && m_fYS > 0)
				ChangeState(STATE_FLY);

			OnApplyGravity(0.25f, 5.0f);
		}
		else
			OnApplyGravity(1, 5.0f);
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

void SledgeBrothers::OnMoveDown()
{
	CheckThrowWeapon();

	if (m_fYS < 4.0f || !m_bCollision[COLLISION_BOTTOM])
	{
		float fGravity = CUR_STAGE->GetGravity();
		if (m_fYS < 1.0f)
		{
			m_fXS *= 0.75f;
			if (m_fYS < 0 && m_fYS + (fGravity * 0.5f) > 0)
				m_fXS = 0;

			if (m_bWinged && m_fYS > 0)
			{
				m_bPassGround = true;
				ChangeState(STATE_FLY);
			}

			OnApplyGravity(0.25f, 5.0f);
		}
		else
			OnApplyGravity(1, 5.0f);
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

void SledgeBrothers::OnFall()
{
	CheckThrowWeapon();

	if ((m_bPassGround && m_fYS < 5.0f) || !m_bCollision[COLLISION_BOTTOM])
	{
		if (CheckRideVehicle())
		{
			ChangeState(STATE_RIDE);
			return;
		}

		OnApplyGravity(1, 5.0f);
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

void SledgeBrothers::OnFly()
{
	if (m_nStateFrame == 48)
		ChangeState(STATE_FALL);
}

void SledgeBrothers::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
		ChangeState(STATE_STAND);

	if (!IsOutOfCamera())
	{
		CUR_STAGE->MakeEarthquake(POWER_EARTHQUAKE_SLEDGEBROTHERS);

		SOUND_MANAGER->PlaySoundEffect(L"Cannon");
	}

	m_bPassGround = false;
}

std::vector<POINT> SledgeBrothers::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -10;
	pt.y = -18;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int SledgeBrothers::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
		nFrame = SPRIDX_SLEDGEBROTHERS1;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_SLEDGEBROTHERS1;
		break;
	case STATE_STAND:
		if (m_pWeapon != nullptr)
			nFrame = SPRIDX_SLEDGEBROTHERS3;
		else
			nFrame = SPRIDX_SLEDGEBROTHERS1;
		break;
	default:
		if (m_pWeapon != nullptr)
			nFrame = SPRIDX_SLEDGEBROTHERS3;
		else
		{
			if (m_nThrowWeaponCooltime >= 16 && m_nThrowWeaponCooltime < 32)
				nFrame = SPRIDX_SLEDGEBROTHERS4;
			else
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_SLEDGEBROTHERS1, 2, 8);
		}
		break;
	}

	return nFrame;
}

SIZE SledgeBrothers::GetSize()
{
	SIZE s;
	s.cx = 16 * m_fScaleX;
	s.cy = 32 * m_fScaleY;

	return s;
}

POINT SledgeBrothers::GetHandPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? 7 : -7));
	pt.y = m_fY - (m_fScaleY * 32);

	return pt;
}
