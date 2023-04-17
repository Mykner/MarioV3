#include "Thwomp.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(Thwomp)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Thwomp"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Thwomp"),
END_IMPL_PROPERTYMAP()

Thwomp::Thwomp(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_THWOMP;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nInitialY = -1;

	m_nBaseState = STATE_IDLE;
}

Thwomp::~Thwomp()
{
}

void Thwomp::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_TRAMPOLINPRESS:
	case STATE_TRAMPOLINJUMP:
		nState = STATE_MOVEUP;
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
			default:
				SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				break;
			}

			// Cannot press!
			return;
		}
		break;
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void Thwomp::OnChangeState(int nState)
{
	switch (nState)
	{
	case STATE_STAND:
		break;
	case STATE_FLY:
		if (m_bFlip)
			m_fXS = -0.5f;
		else
			m_fXS = 0.5f;
		break;
	case STATE_MOVEDOWN:
		m_fXS = 0;
		break;
	case STATE_MOVEUP:
		m_fXS = 0;
		m_fYS = -0.75f;
		break;
	}
}

void Thwomp::OnIdle()
{
	if (m_nInitialY == -1)
	{
		m_nInitialY = m_fY;
	}

	SIZE s = GetSize();
	if (m_nStateFrame > 48)
	{
		if (m_fY < CUR_PLAYER_OBJ->m_fY &&
			abs(m_fX - CUR_PLAYER_OBJ->m_fX) < 48)
		{
			ChangeState(STATE_MOVEDOWN);
		}
	}

	if (m_bWinged)
	{
		ChangeState(STATE_FLY);
	}
}

void Thwomp::OnFly()
{
	m_fYS = sin(m_nStateFrame * 2 * fToRadian) * 0.3f;

	if (m_nStateFrame > 48)
	{
		SIZE s = GetSize();
		if (m_fY < CUR_PLAYER_OBJ->m_fY &&
			abs(m_fX - CUR_PLAYER_OBJ->m_fX) < 48)
		{
			ChangeState(STATE_MOVEDOWN);
		}
	}

	if (CheckReachWall())
		return;

	if (m_bCollision[COLLISION_TOP])
	{
		SIZE s = GetSize();
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_TOP].y + s.cy + 1;
	}

	if (m_bCollision[COLLISION_BOTTOM])
	{
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	}
}

void Thwomp::OnMoveDown()
{
	if (m_bCollision[COLLISION_BOTTOM])
	{
		int nRet = SmashAll();
		CUR_STAGE->MakeEarthquake();

		m_fXS = 0;
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_BOTTOM].y;

		if (!m_bPowerUp)
		{
			if (nRet >= TILETYPE_SOFTHARD)
				ChangeState(STATE_STAND);
		}
		else
		{
			if (nRet >= TILETYPE_SUPERHARD)
				ChangeState(STATE_STAND);
		}

		// Collision with map object
		if (nRet == TILETYPE_EMPTY)
			ChangeState(STATE_STAND);
	}
	else
	{
		if (m_fYS < 5.0f)
			m_fYS += (CUR_STAGE->GetGravity() * 1.5f);
	}
}

void Thwomp::OnMoveUp()
{
	if (m_bCollision[COLLISION_TOP])
	{
		SIZE s = GetSize();
		m_fXS = 0;
		m_fYS = 0;
		m_fY = m_ptCollision[COLLISION_TOP].y + s.cy + 1;

		ChangeState(STATE_IDLE);
	}
	else if (m_fY <= m_nInitialY)
	{
		m_fXS = 0;
		m_fYS = 0;
		m_fY = m_nInitialY;

		ChangeState(STATE_IDLE);
	}
}

void Thwomp::OnStand()
{
	if (m_nStateFrame == 24)
		ChangeState(STATE_MOVEUP);
}

void Thwomp::OnStacked()
{
	ReleaseEnemyStack();
	ChangeState(STATE_IDLE);
}

int Thwomp::SmashAll()
{
	int nTX = m_fX / TILE_XS;
	int nTY = (m_fY + m_fYS + 2) / TILE_YS;

	int nRet = TILETYPE_EMPTY;

	// Destroy Blocks
	SIZE s = GetSize();
	int nLeftTX = (m_fX - (s.cx / 2)) / TILE_XS;
	int nRightTX = (m_fX + (s.cx / 2)) / TILE_XS;

	for (int _x = nLeftTX; _x <= nRightTX; _x++)
	{
		int _y = nTY;
		if (!CUR_STAGE->IsHardTile(_x, _y))
			continue;

		int nData = CUR_STAGE->GetTileData(_x, _y);
		int nType = CUR_STAGE->GetDataType(nData);
		if (m_bPowerUp == false)
		{
			if (nType == TILETYPE_SOFTHARD &&
				CUR_STAGE->CanCrush(_x, _y, POWER_HITBLOCK_BIGMARIO))
			{
				CUR_STAGE->CrushBlock(_x, _y, this);
				nRet = TILETYPE_HARD;
			}
			else if (nType == TILETYPE_ITEM)
			{
				CUR_STAGE->HitBlock(_x, _y, POWER_HITBLOCK_NORMAL, this, this);
				nRet = TILETYPE_HARD;
			}
			else if ((nType == TILETYPE_HARD || nType == TILETYPE_SUPERHARD) && nRet < TILETYPE_HARD)
				nRet = TILETYPE_HARD;
		}
		else
		{
			if (nType != TILETYPE_SUPERHARD &&
				CUR_STAGE->CanCrush(_x, _y, POWER_HITBLOCK_GIANTMARIO))
			{
				CUR_STAGE->CrushBlock(_x, _y, this);
				if (nRet == TILETYPE_EMPTY)
					nRet = TILETYPE_HARD;
			}

			if (nType == TILETYPE_SUPERHARD && nRet < TILETYPE_SUPERHARD)
				nRet = TILETYPE_SUPERHARD;
		}

		if (nType >= TILETYPE_DAMAGE && nRet < TILETYPE_DAMAGE)
			nRet = nType;
		if (nType == TILETYPE_JUMP)
		{
			CUR_STAGE->PressJumpBlock(_x, _y);
		}
	}

	NaRect rc = GetRect();
	rc.top += rc.Height() / 2;
	rc.bottom += 2;

	// Destroy Enemies
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEnemy) {
		NaRect rcObj = pObj->GetRect();
		if (pObj == this)
			continue;
		if (!rc.IsOverlapped(rcObj))
			continue;

		if (pObj->IsAlive() && pObj->CanBurnt(BURN_THWOMPSMASH))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			info.nPower = BURN_THWOMPSMASH;
			pObj->ChangeState(STATE_BURNT, &info);
		}
	} END_VECTOR_LOOP();

	// Destroy Player
	NaRect rcPlayer = CUR_PLAYER_OBJ->GetRect();
	if (rc.IsOverlapped(rcPlayer))
	{
		GameObjectBase *pObj = CUR_PLAYER_OBJ;
		if (pObj->IsAlive() && pObj->CanBurnt(BURN_STARMAN))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			info.nPower = BURN_THWOMPSMASH;
			pObj->ChangeState(STATE_BURNT, &info);
		}
	}

	// Destroy EventObjects
	/*
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEventObject) {
		NaRect rcObj = pObj->GetRect();
		if (pObj == this)
			continue;
		if (!rc.IsOverlapped(rcObj))
			continue;

		if (pObj->IsAlive() && pObj->CanBurnt(BURN_THWOMPSMASH))
		{
			ChangeStateInfo info;
			info.nNextState = STATE_BURNT;
			info.pFromObject = this;
			info.nPower = BURN_THWOMPSMASH;
			pObj->ChangeState(STATE_BURNT, &info);
		}
	} END_VECTOR_LOOP();
	*/
	
	// Effects
	if (nRet != 0)
	{
		CreateParameter param;
		param.nType = EFFECT_DUST;
		param.fScaleX = 1.0f;
		param.fScaleY = 1.0f; 
		param.fYS = -0.5f;

		param.fXS = -0.5f;
		CUR_STAGE->CreateEffect(m_fX - 16, m_fY, EFFECT_DUST, &param);

		param.fXS = 0.5f;
		CUR_STAGE->CreateEffect(m_fX + 16, m_fY, EFFECT_DUST, &param);

		param.nType = EFFECT_SMALLDUST;
		param.fXS = -1.5f;
		CUR_STAGE->CreateEffect(m_fX - 24, m_fY, EFFECT_DUST, &param);

 		param.fXS = 1.5f;
 		CUR_STAGE->CreateEffect(m_fX + 24, m_fY, EFFECT_DUST, &param);
	}

	SOUND_MANAGER->PlaySoundEffect(L"Cannon");

	return nRet;
}

bool Thwomp::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool Thwomp::CanCollide(int nDir)
{
	return true;
}

bool Thwomp::CanEat()
{
	return false;
}

bool Thwomp::CanPressed(int nPower)
{
	switch (nPower)
	{
	case POWER_PRESS_YOSHI:
		return true;
	default:
		break;
	}

	return false;
}

bool Thwomp::CanPushedUp()
{
	return false;
}

bool Thwomp::CanBurnt(int nType)
{
	if (nType == BURN_STARMAN)
		return true;
	
	return false;
}

std::vector<POINT> Thwomp::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -13;
	pt.y = -14;

	std::vector<POINT> vec;
	vec.push_back(pt);

	pt.x *= -1;
	vec.push_back(pt);

	return vec;
}

int Thwomp::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_fYS < 0)
	{
		if (m_nStateFrame % 40 >= 20)
			nFrame = SPRIDX_WING2;
	}

	return nFrame;
}

int Thwomp::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	if (CUR_PLAYER && CUR_PLAYER_OBJ)
	{
		if (m_fY <= CUR_PLAYER_OBJ->m_fY)
		{
			if (abs(m_fX - CUR_PLAYER_OBJ->m_fX) < 48)
				return SPRIDX_THWOMP_ANGER;

			if (abs(m_fX - CUR_PLAYER_OBJ->m_fX) < 96)
				return SPRIDX_THWOMP_NEAR;
		}
	}

	return SPRIDX_THWOMP_IDLE;
}

int Thwomp::GetZOrder()
{
	return Z_AFTER_MAP;
}

SIZE Thwomp::GetSize()
{
	SIZE s;
	s.cx = 24 * m_fScaleX;
	s.cy = 32 * m_fScaleY;

	return s;
}
