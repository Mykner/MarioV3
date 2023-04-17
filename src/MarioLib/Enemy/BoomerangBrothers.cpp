#include "BoomerangBrothers.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(BoomerangBrothers)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "BoomerangBrothers"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "BoomerangBrothers")
END_IMPL_PROPERTYMAP()

BoomerangBrothers::BoomerangBrothers(Game *pGame, Stage *pStage) :
	HammerBrothers(pGame, pStage)
{
	m_nType = ENEMY_BOOMERANGBROTHERS;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nThrowWeaponCooltime = 0;
	m_pWeapon = nullptr;
	m_nJump = 0;
	m_nWeaponType = BULLET_BOOMERANG;
}

BoomerangBrothers::~BoomerangBrothers()
{
}

void BoomerangBrothers::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_WALK:
		if (rand() % 2 == 0)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_MOVEUP:
		if (!m_bWinged)
			m_fYS = -4.5f;
		else
			m_fYS = -6.0f;
		break;
	case STATE_MOVEDOWN:
		m_fYS = -2.0f;
		break;
	case STATE_PRESSED:
		if (m_bWinged)
		{
			m_fXS = 0;
			m_fYS = 0;

			ReleaseWing();

			SOUND_MANAGER->PlaySoundEffect(L"Pressed");
			return;
		}

		if (m_pWeapon)
		{
			m_pWeapon->m_pOwner = nullptr;
			m_pWeapon->ChangeState(STATE_DEAD);
			m_pWeapon = nullptr;
		}
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
	case STATE_DEAD:
		if (m_pWeapon)
		{
			m_pWeapon->m_pOwner = nullptr;
			m_pWeapon->ChangeState(STATE_DEAD);
			m_pWeapon = nullptr;
		}

		UnlinkWeapons();
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void BoomerangBrothers::OnWalk()
{
	CheckFalling();

	CheckReachWall();

	int nRange = 8;
	if (m_bWinged)
		nRange = 16;

	if (m_fX < m_nInitialX - nRange)
		m_fXS = 0.5f;
	else if (m_fX > m_nInitialX + nRange)
		m_fXS = -0.5f;

	if (CUR_PLAYER_OBJ->m_fX < m_fX)
		m_bFlip = true;
	else
		m_bFlip = false;

	CheckThrowWeapon();

	if (m_bWinged && 
		m_nType == ENEMY_BOOMERANGBROTHERS &&
		m_nStateFrame == 48 && 
		m_nJump < 3)
	{
		m_fYS = -3.0f;
		m_nJump++;
	}

	if (m_nStateFrame == 24 * 4)
	{
		/*
		// Decide Jump Up or Down
		if (rand() % 2 == 0)
			ChangeState(STATE_MOVEUP);
		else
		{
			int nData = CUR_STAGE->GetTileData(
				m_ptCollision->x / TILE_XS,
				(m_ptCollision->y / TILE_YS) + 1);
			int nType = CUR_STAGE->GetDataType(nData);
			if (m_fY < 200 && nType == TILETYPE_EMPTY)
				ChangeState(STATE_MOVEDOWN);
			else
				ChangeState(STATE_MOVEUP);
		}
		*/

		m_nJump = 0;
	}
}

bool BoomerangBrothers::CheckCreateWeapon()
{
	return (rand() % 120 == 0);
}

void BoomerangBrothers::UnlinkWeapons()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecBullet) {
		if (pObj->m_nType != m_nWeaponType)
			continue;

		if (((Boomerang*)pObj)->m_pOwner == this)
			((Boomerang*)pObj)->m_pOwner = nullptr;
	} END_VECTOR_LOOP();
}

int BoomerangBrothers::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
		nFrame = SPRIDX_BOOMERANGBROTHERS1;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_BOOMERANGBROTHERS1;
		break;
	default:
		if (m_pWeapon != nullptr)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BOOMERANGBROTHERS_CHARGE1, 2, 6);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BOOMERANGBROTHERS1, 2, 6);
		break;
	}

	return nFrame;
}

POINT BoomerangBrothers::GetHandPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? 8 : -8));
	pt.y = m_fY - (m_fScaleY * 20);

	return pt;
}

SIZE BoomerangBrothers::GetSize()
{
	SIZE s;
	s.cx = 13 * m_fScaleX;
	s.cy = 24 * m_fScaleY;
	return s;
}
