#include "Boo.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(Boo)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Boo")
END_IMPL_PROPERTYMAP()

Boo::Boo(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_BOO;
	m_nState = STATE_FLY;

	m_bFlip = true;

	m_nBaseState = STATE_FLY;
}

Boo::~Boo()
{
}

void Boo::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = m_pGame->m_bModernStyle && nZOrder != Z_MAP_SHADOW;
		float fAngle = 0;

		if (IS_SPINNING)
		{
			fAngle = m_nStateFrame * -14;
			if (m_fXS < 0)
				fAngle *= -1;
			if (!m_bFlip)
				fAngle *= -1;
		}

		float fScaleX, fScaleY;
		GetScale(fScaleX, fScaleY);

		(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);
	}
}

void Boo::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
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
					m_nNextState = STATE_BURNT;
					SOUND_MANAGER->PlaySoundEffect(L"Kicked");
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
	}

	EnemyObjectBase::ChangeState(nState, pInfo);
}

void Boo::OnBeforeProcess()
{
	CheckCollisionEnemy();
}

void Boo::OnFly()
{
	if (IsOutOfCamera())
		return;

	// Shy mode
	if (m_fX > CUR_PLAYER_OBJ->m_fX)
	{
		// I'm right
		if (m_bFlip && CUR_PLAYER_OBJ->m_bFlip)
		{
			ChangeState(STATE_MOVE);
			return;
		}

		if (!m_bFlip)
			m_bFlip = true;
	}
	else
	{
		if (!m_bFlip && !CUR_PLAYER_OBJ->m_bFlip)
		{
			ChangeState(STATE_MOVE);
			return;
		}

		if (m_bFlip)
			m_bFlip = false;
	}

	// Slowdown
	if (m_fXS < 0)
		m_fXS += 0.025f;
	if (m_fXS > 0)
		m_fXS -= 0.025f;
	if (m_fYS < 0)
		m_fYS += 0.025f;
	if (m_fYS > 0)
		m_fYS -= 0.025f;
	if (abs(m_fXS) < 0.1f)
		m_fXS = 0;
	if (abs(m_fYS) < 0.1f)
		m_fYS = 0;
}

void Boo::OnMove()
{
	float fMaxSpeed = 0.75f;

	if (m_fX > CUR_PLAYER_OBJ->m_fX)
	{
		// I'm right
		if (m_bFlip && !CUR_PLAYER_OBJ->m_bFlip)
		{
			ChangeState(STATE_FLY);
			return;
		}

		if (m_fXS > -fMaxSpeed)
			m_fXS -= 0.01f;

		if (!m_bFlip)
			m_bFlip = true;
	}
	else
	{
		if (!m_bFlip && CUR_PLAYER_OBJ->m_bFlip)
		{
			ChangeState(STATE_FLY);
			return;
		}

		if (m_fXS < fMaxSpeed)
			m_fXS += 0.01f;

		if (m_bFlip)
			m_bFlip = false;
	}

	if (m_fY < CUR_PLAYER_OBJ->m_fY)
	{
		if (m_fYS < fMaxSpeed)
			m_fYS += 0.01f;
	}
	else
	{
		if (m_fYS > -fMaxSpeed)
			m_fYS -= 0.01f;
	}
}

bool Boo::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool Boo::CanEat()
{
	return false;
}

bool Boo::CanSpit()
{
	return false;
}

bool Boo::CanPressed(int nPower)
{
	switch (nPower)
	{
	case POWER_PRESS_YOSHI:
	case POWER_PRESS_GOOMBASHOES:
		return true;
		break;
	}
	return false;
}

bool Boo::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_STARMAN:
		return true;
		break;
	}

	return false;
}

bool Boo::CanBornFromBlock()
{
	return true;
}

int Boo::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	switch (m_nState)
	{
	case STATE_FLY:
		return SPRIDX_BOO2; // Shy
		break;
	case STATE_MOVE:
		return SPRIDX_BOO1; // Normal
		break;
	}

	return SPRIDX_BOO1;
}

int Boo::GetZOrder()
{
	return EnemyObjectBase::GetZOrder();
}

SIZE Boo::GetSize()
{
	SIZE s;
	s.cx = 16 * m_fScaleX;
	s.cy = 16 * m_fScaleY;

	return s;
}
