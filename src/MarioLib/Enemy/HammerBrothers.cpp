#include "HammerBrothers.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"
#include "ThrowingHammer.h"

BEGIN_IMPL_PROPERTYMAP(HammerBrothers)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "HammerBrothers"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "HammerBrothers")
END_IMPL_PROPERTYMAP()

HammerBrothers::HammerBrothers(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_HAMMERBROTHERS;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nThrowWeaponCooltime = 0;
	m_pWeapon = nullptr;
	m_nJump = 0;
	m_nWeaponType = BULLET_THROWINGHAMMER;
}

HammerBrothers::~HammerBrothers()
{
}

void HammerBrothers::OnAfterProcess()
{
	EnemyObjectBase::OnAfterProcess();

	if (m_pWeapon)
	{
		m_pWeapon->m_fX += m_fXS;
		m_pWeapon->m_fY += m_fYS;
	}
}

void HammerBrothers::ChangeState(int nState, ChangeStateInfo *pInfo)
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

void HammerBrothers::SetState(int nState)
{
	switch (nState)
	{
	case STATE_DEAD:
		if (m_pWeapon)
		{
			m_pWeapon->m_pOwner = nullptr;
			m_pWeapon->ChangeState(STATE_DEAD);
			m_pWeapon = nullptr;
		}
		UnlinkWeapons();
		break;
	}

	EnemyObjectBase::SetState(nState);
}

void HammerBrothers::OnIdle()
{
	if (m_nStateFrame == 0)
	{
		m_nInitialX = m_fX;
	}

	if (!CheckFalling())
		ChangeState(STATE_WALK);
}

void HammerBrothers::OnWalk()
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
		m_nType == ENEMY_HAMMERBROTHERS &&
		m_nStateFrame == 48 && 
		m_nJump < 3)
	{
		m_fYS = -3.0f;
		m_nJump++;
	}

	if (m_nStateFrame == 24 * 4)
	{
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

		m_nJump = 0;
	}
}

void HammerBrothers::OnMoveUp()
{
	CheckThrowWeapon();

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(0.5f, 4.0f);
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

void HammerBrothers::OnMoveDown()
{
	CheckThrowWeapon();

	if (m_fYS < 4.0f || !m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(0.5f, 4.0f);
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

void HammerBrothers::OnReachWall(bool bIsLeftWall)
{
	GameObjectBase::OnReachWall(bIsLeftWall);
}

void HammerBrothers::OnStacked()
{
	EnemyObjectBase::OnStacked();

	if (CUR_PLAYER_OBJ->m_fX < m_fX)
		m_bFlip = true;
	else
		m_bFlip = false;

	CheckThrowWeapon();
}

void HammerBrothers::OnItemPop()
{
	m_nInitialX = m_fX;

	EnemyObjectBase::OnItemPop();
}

void HammerBrothers::OnItemPopDown()
{
	m_nInitialX = m_fX;

	EnemyObjectBase::OnItemPopDown();
}

void HammerBrothers::OnRide()
{
	EnemyObjectBase::OnRide();

	CheckThrowWeapon();
}

void HammerBrothers::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool HammerBrothers::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool HammerBrothers::CheckCreateWeapon()
{
	return (rand() % 60 == 0);
}

void HammerBrothers::CheckThrowWeapon()
{
	if (m_nThrowWeaponCooltime == 0 && CheckCreateWeapon())
	{
		CreateWeapon();
	}

	if (m_nThrowWeaponCooltime > 0 && m_nThrowWeaponCooltime < 16)
	{
		if (m_pWeapon)
		{
			POINT ptHand = GetHandPosition();

			m_pWeapon->m_fX = ptHand.x;
			m_pWeapon->m_fY = ptHand.y;
		}		
	}

	switch (m_nThrowWeaponCooltime)
	{
	case 16:
		{
			if (m_pWeapon)
				m_pWeapon->ChangeState(STATE_WALK);

			m_pWeapon = nullptr;
		}
		break;
	case 48:
		m_nThrowWeaponCooltime = 0;
		break;
	}

	if (m_nThrowWeaponCooltime > 0 || m_pWeapon != nullptr)
		m_nThrowWeaponCooltime++;
}

void HammerBrothers::CreateWeapon()
{
	POINT ptHand = GetHandPosition();

	CreateParameter param;
	param.pFromPlayer = nullptr;
	param.pFromObject = this;
	param.nType = m_nWeaponType;
	param.fScaleX = m_fScaleX;
	param.fScaleY = m_fScaleY;
	m_pWeapon = (BulletObjectBase*)CUR_STAGE->CreateBullet(
		ptHand.x,
		ptHand.y,
		m_nWeaponType, &param);
}

void HammerBrothers::UnlinkWeapons()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecBullet) {
		if (pObj->m_nType != m_nWeaponType)
			continue;

		if (((ThrowingHammer*)pObj)->m_pOwner == this)
			((ThrowingHammer*)pObj)->m_pOwner = nullptr;
	} END_VECTOR_LOOP();
}

std::vector<POINT> HammerBrothers::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -4;
	pt.y = -14;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int HammerBrothers::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_fYS != 0)
	{
		if (m_nStateFrame % 8 >= 4)
			nFrame = SPRIDX_WING2;
	}
	else
	{
		if (m_nStateFrame % 32 >= 16)
			nFrame = SPRIDX_WING2;
	}

	return nFrame;
}

int HammerBrothers::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
		nFrame = SPRIDX_HAMMERBROTHERS1;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_HAMMERBROTHERS1;
		break;
	default:
		if (m_pWeapon != nullptr)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_HAMMERBROTHERS_CHARGE1, 2, 6);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_HAMMERBROTHERS1, 2, 6);
		break;
	}

	return nFrame;
}

POINT HammerBrothers::GetHandPosition()
{
	POINT pt;
	pt.x = m_fX + (m_fScaleX * (m_bFlip ? 2 : -2));
	pt.y = m_fY - (m_fScaleY * 24);

	return pt;
}
