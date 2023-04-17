#include "DryBone.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "EventObjectBase.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(DryBone)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "DryBone"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "DryBone"),
END_IMPL_PROPERTYMAP()

DryBone::DryBone(Game *pGame, Stage *pStage) :
	RedTurtle(pGame, pStage)
{
	m_nType = ENEMY_DRYBONE;
}

DryBone::~DryBone()
{
}

void DryBone::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_PUSHEDUP:
		SplitParts();
		nState = STATE_DEAD;

		ReleaseWing();

		SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
		break;
	case STATE_BURNT:
		SplitParts();

		ReleaseWing();

		SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
		break;
	case STATE_PRESSED:
		{
			if (m_bPowerUp)
				pInfo = nullptr;

			ReleaseEnemyStack();

			int nPower = POWER_PRESS_NORMAL;
			if (pInfo && pInfo->nPower)
				nPower = pInfo->nPower;

			switch (nPower)
			{
			default:
				if (m_bPowerUp)
				{
					m_nAwakeTimer = 16 + 80;

					SOUND_MANAGER->PlaySoundEffect(L"Pressed");
					return;
				}
				else if (m_bWinged)
				{
					m_fYS = 0;
					nState = STATE_WALK;

					ReleaseWing();

					SOUND_MANAGER->PlaySoundEffect(L"Pressed");
				}					
				else
				{
					m_nShape = TURTLESHAPE_SHELL;
					m_nAwakeTimer = 16 + 80;
					m_fXS = 0;

					SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
				}

				m_nDamageCooltime = 8;
				break;
			}

			if (pInfo && pInfo->pFromObject)
				GiveScoreAndComboToAttacker(pInfo->pFromObject, 100, true);
		}
		break;
	case STATE_SPITTED:
	case STATE_SPITTED_SAFE:
		SplitParts();
		nState = STATE_DEAD;

		ReleaseWing();
		break;
	default:
		RedTurtle::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void DryBone::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	if (nZOrder == -1)
		nZOrder = GetZOrder();

	// #TODO Offset option?
	if ((m_nState == STATE_SHELLIDLE && m_nStateFrame > 200) ||
		(m_nState == STATE_SHELLAWAKE && m_nStateFrame < 6))
	{
		if (m_nStateFrame % 8 < 4)
			pt.x -= 1;
	}

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

		float _x = x - pt.x;
		float _y = y - pt.y;

        if (m_nState == STATE_STACKED)
            _x += GetStackXOffset();

		(*m_ppSprite)->RenderToQueue(_x, _y, nFrame, m_bFlip, m_bFlipV, nColor,
			fScaleX, fScaleY, nZOrder, bShadow, fAngle);

		if (m_bWinged)
			RenderWing(nColor, nZOrder);
	}
}

void DryBone::OnShellIdle()
{
	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(1, 4.0f);
	}
	else
	{
		if ((int)m_fY <= m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fYS = 0;
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}
	}

	NaRect rc = GetRect();
	if (m_bCollision[COLLISION_LEFT])
	{
		m_fX = m_ptCollision[COLLISION_LEFT].x + (rc.Width() / 2) + 2; 
	}
	else if (m_bCollision[COLLISION_RIGHT])
	{
		m_fX = m_ptCollision[COLLISION_RIGHT].x - (rc.Width() / 2) - 1;
	}

	if ((!m_bPowerUp && m_nStateFrame > 245) ||
		(m_bPowerUp && m_nStateFrame == 4))
	{
		ChangeState(STATE_SHELLAWAKE);
	}
}

void DryBone::OnShellAwake()
{
	if ((!m_bPowerUp &&m_nStateFrame == 24) ||
		(m_bPowerUp &&m_nStateFrame == 4))
	{
		if (m_bWinged)
			ChangeState(STATE_MOVE);
		else
			ChangeState(STATE_WALK);
	}
}

void DryBone::OnMove()
{
	if (m_nAwakeTimer > 0)
		m_nAwakeTimer--;

	RedTurtle::OnMove();
}

void DryBone::SplitParts()
{
	int nFlip = m_bFlip ? -1 : 1;
	float fScale = m_bPowerUp ? 2.0f : 1.0f;

	CreateParameter param;	
	param.pFromObject = this;

	// Head
	param.nType = 0;
	param.fScaleX = fScale;
	param.fScaleY = fScale;
	param.fXS = 0.7f * nFlip;
	param.fYS = -2.0f;
	CUR_STAGE->CreateEffect(m_fX + (nFlip * 4 * fScale), m_fY - (6 * fScale), EFFECT_DRYBONEDEBRIS, &param);

	// Shell
	param.nType = 1;
	param.fXS = -0.4f * nFlip;
	param.fYS = -3.0f;
	CUR_STAGE->CreateEffect(m_fX, m_fY - (2 * fScale), EFFECT_DRYBONEDEBRIS, &param);

	// Spine
	param.nType = 2;
	param.fXS = 0.2f * nFlip;
	param.fYS = -2.0f;
	CUR_STAGE->CreateEffect(m_fX - (nFlip * -1 * fScale), m_fY, EFFECT_DRYBONEDEBRIS, &param);
}

bool DryBone::CanPressed(int nPower)
{
	if (m_nState == STATE_SHELLIDLE || m_nState == STATE_SHELLAWAKE)
		return false;

	return true;
}

bool DryBone::CanKicked()
{
	return false;
}

bool DryBone::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_FIREBALL:
	case BURN_BOWSERFIRE:
		return false;
		break;
	case BURN_TURTLESHELL:
	case BURN_YOSHIFIRE:
	case BURN_STARMAN:
	case BURN_POWBLOCK:
		return true;
		break;
	}

	return false;
}

bool DryBone::CanCarried()
{
	return false;
}

int DryBone::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_SHELLAWAKE:
		if (!m_bPowerUp)
			nFrame = SPRIDX_DRYBONE_DIE4 - (m_nStateFrame / 6); 
		else
			nFrame = SPRIDX_DRYBONE_DIE2 - (m_nStateFrame / 2);
		break;
	case STATE_PRESSED:
	case STATE_SHELLIDLE:
		if (m_nStateFrame < 9)
			nFrame = SPRIDX_DRYBONE_DIE1 + (m_nStateFrame / 3);
		else
			nFrame = SPRIDX_DRYBONE_DIE4;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = -1;
		break;
	case STATE_HELDBYTONGUE:
		if (nFrame == 0)
			nFrame = SPRIDX_DRYBONE_DIE4;
		break;
	case STATE_MOVE:
		{
			int nAwakeTimer = 80 + 16 - m_nAwakeTimer;
			if (m_bWinged && nAwakeTimer < 10)
			{
				if (nAwakeTimer < 6)
					nFrame = SPRIDX_DRYBONE_DIE1 + ((nAwakeTimer) / 3);
				else if (nAwakeTimer < 10)
					nFrame = SPRIDX_DRYBONE_DIE2 - ((nAwakeTimer - 6) / 2);

				NaDebugOut(L"nFrame = %d (m_nAwakeTimer: %d)\n", nFrame, nAwakeTimer);
			}
			else
				nFrame = SPRIDX_DRYBONE1 + (m_nStateFrame / 8) % 2;
		}
		break;
	default:
		nFrame = SPRIDX_DRYBONE1 + (m_nStateFrame / 8) % 2;
		if (m_nShape == TURTLESHAPE_SHELL || m_bDeadInside)
			nFrame = SPRIDX_DRYBONE_DIE4;
		
		break;
	}

	m_nSpriteIndex = nFrame;

	return nFrame;
}
