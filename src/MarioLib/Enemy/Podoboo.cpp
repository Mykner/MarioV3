#include "Podoboo.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "InputManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(Podoboo)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "Podoboo"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "Podoboo"),
END_IMPL_PROPERTYMAP()

Podoboo::Podoboo(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_PODOBOO;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nInitialY = -1;
	m_fAngle = 0;

	m_nBaseState = STATE_IDLE;
}

Podoboo::~Podoboo()
{
}

void Podoboo::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (nFrame != -1)
	{
		bool bShadow = m_pGame->m_bModernStyle && nZOrder != Z_MAP_SHADOW;
		float fAngle = 0.0f;

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

		if (m_nState == STATE_FLY || m_bWinged)
			fAngle = m_fAngle;

		(*m_ppSprite)->RenderToQueue(x - pt.x, y - pt.y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);
	}
}

void Podoboo::RenderWing(int nColor, int nZOrder)
{
}

void Podoboo::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{
	case STATE_FALL:
		nState = STATE_MOVEDOWN;
		if (m_nInitialY == -1)
			m_nInitialY = m_fY;
		m_bFlipV = true;
		break;
	case STATE_FLY:
		m_fXS = -0.7f;
		m_fYS = 0.7f;
		m_fAngle = 45 + 90;
		m_bFlip = false;
		break;
	case STATE_MOVEUP:
		m_fYS = -4.6f;
		m_bFlipV = false;
		break;
	case STATE_MOVEDOWN:
		m_fYS = 0;
		m_bFlipV = true;
		break;
	case STATE_TRAMPOLINJUMP:
		if (m_bWinged)
		{
			if (m_fYS > 0)
				m_fYS *= -1;
			m_nState = STATE_FLY;
			m_nNextState = -1;
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

void Podoboo::OnChangeState(int nState)
{
	ChangeStateInfo *pInfo = nullptr;
	if (m_ChangeStateInfo.nNextState != -1)
		pInfo = &m_ChangeStateInfo;

	switch (nState)
	{
	case STATE_TRAMPOLINJUMP:
		if (!m_bWinged)
		{
			m_nNextState = -1;
			if (m_fYS > 0)
				m_fYS *= -1;
			EnemyObjectBase::OnChangeState(STATE_MOVEUP);
		}
		return;
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void Podoboo::OnIdle()
{
	if (m_nInitialY == -1)
		m_nInitialY = m_fY;

	if (m_bWinged == false)
		ChangeState(STATE_MOVEDOWN);
	else
		ChangeState(STATE_FLY);
}

void Podoboo::OnMoveUp()
{
	if (IS_CLASSIC)
	{
		m_fYS += (0.5f / 8.0f);
		if (m_fYS >= 0)
			ChangeState(STATE_MOVEDOWN);
	}
	else
	{
		if (m_fY < m_nInitialY + 40)
			m_fYS *= 0.9f;

		if (m_fY < m_nInitialY)
		{
			m_fY = m_nInitialY;
			ChangeState(STATE_MOVEDOWN);
		}
	}
}

void Podoboo::OnMoveDown()
{
	if (m_fYS < 4.0f)
		m_fYS += 0.1f;
	if (m_fY > GameDefaults::nPageHeight + (m_fScaleY * 16))
	{
		m_fXS = 0;
		m_fYS = 0;

		if (IS_CLASSIC)
		{
			if (rand() % 16 == 0)
				ChangeState(STATE_MOVEUP);
		}
		else
		{
			if (m_pGame->m_nGameFrame % 72 == 0)
				ChangeState(STATE_MOVEUP);
		}
	}
}

void Podoboo::OnFly()
{
	if (m_fXS < 0 && m_bCollision[COLLISION_LEFT])
		m_fXS *= -1;
	else if (m_fXS > 0 && m_bCollision[COLLISION_RIGHT])
		m_fXS *= -1;

	if (m_fYS < 0 && m_bCollision[COLLISION_TOP])
		m_fYS *= -1;
	else if (m_fYS > 0 && m_bCollision[COLLISION_BOTTOM])
		m_fYS *= -1;

	int nAngle = 0;
	if (m_fXS > 0 && m_fYS < 0)
		nAngle = 45; 
	else if (m_fXS > 0 && m_fYS > 0)
		nAngle = 45 + 90;
	else if (m_fXS < 0 && m_fYS > 0)
		nAngle = 45 + 180;
	else if (m_fXS < 0 && m_fYS < 0)
		nAngle = 45 + 270;
	
	m_bFlip = false;
	m_bFlipV = false;

	while (true)
	{
		if (nAngle - m_fAngle > 180)
			nAngle -= 360;
		else if (nAngle - m_fAngle < -180)
			nAngle += 360;
		else
			break;
	}

	while (m_fAngle == nAngle && m_fAngle > 360)
	{
		m_fAngle -= 360;
	}

	if (m_fAngle < nAngle)
		m_fAngle += 10;
	if (m_fAngle > nAngle)
		m_fAngle -= 10;
}

void Podoboo::OnTrampolinJump()
{
	if (!m_bWinged)
	{
		ChangeState(STATE_MOVEUP);
	}
	else
	{
		ChangeState(STATE_FLY);
	}
}

void Podoboo::OnBounceTrampolinMiniHorz(GameObjectBase * pTrampolin)
{
	NaRect rc = pTrampolin->GetRect();
	bool bLeft = false;
	if (pTrampolin->m_fX < m_fX)
		bLeft = true;

	if (bLeft)
	{
		m_bCollision[COLLISION_LEFT] = true;
		m_ptCollision[COLLISION_LEFT].x = rc.right;
	}
	else
	{
		m_bCollision[COLLISION_RIGHT] = true;
		m_ptCollision[COLLISION_RIGHT].x = rc.left;
	}

	OnReachWall(bLeft);

	m_fXS = 3.0f;
	m_fYS = -4.0f;

	if (!bLeft)
	{
		m_fXS *= -1;
	}
}

bool Podoboo::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool Podoboo::CanSpit()
{
	return true;
}

bool Podoboo::CanPressed(int nPower)
{
	return false;
}

bool Podoboo::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_TURTLESHELL:
	case BURN_BOBOMB:
	case BURN_STARMAN:
		return true;
		break;
	}

	return false;
}

std::vector<POINT> Podoboo::GetWingPosition()
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

int Podoboo::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_nStateFrame % 16 >= 8)
		nFrame = SPRIDX_WING2;
	
	return nFrame;
}

int Podoboo::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	if (m_bWinged)
	{
		if (GetWingSpriteIndex() == SPRIDX_WING1)
			return SPRITE_INDEX_CYCLE(SPRIDX_PODOBOO1_WING1, 2, 4);
		else
			return SPRITE_INDEX_CYCLE(SPRIDX_PODOBOO1_WING2, 2, 4);
	}

	return SPRITE_INDEX_CYCLE(SPRIDX_PODOBOO1, 2, 4);
}

int Podoboo::GetZOrder()
{
	return Z_AFTER_MAP;
}

void Podoboo::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_STACKED)
	{
		float fFactor = (m_nStateFrame % 24) / 100.0f;
		fScaleX *= (1.0f + (fFactor));
		fScaleY *= (1.0f + (fFactor));
	}
}
