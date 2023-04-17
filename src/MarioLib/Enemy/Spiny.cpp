#include "Spiny.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

Spiny::Spiny(Game *pGame, Stage *pStage) :
	Turtle(pGame, pStage)
{
	m_nType = ENEMY_SPINY;
	m_nState = STATE_IDLE;

	m_bFlip = true;
	m_nThrowSpikeCooltime = 0;
	m_bThrowSpikeDirection = false;
}

Spiny::~Spiny()
{
}

void Spiny::ChangeState(int nState, ChangeStateInfo * pInfo)
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
				if (m_bFlipV)
				{
					if (m_nState == STATE_SHELLIDLE || m_nState == STATE_SHELLAWAKE)
					{
						m_nShape = TURTLESHAPE_SHELLKICKED;
						OnShellKicked();

						SOUND_MANAGER->PlaySoundEffect(L"Kicked");
					}
					else if (m_nState == STATE_SHELLRUN)
					{
						m_nShape = TURTLESHAPE_SHELL;
						m_nAwakeTimer = 16 + 80;
						m_fXS = 0;
						StopCombo();

						SOUND_MANAGER->PlaySoundEffect(L"Pressed");
					}

					m_nShape = TURTLESHAPE_SHELL;
					m_nAwakeTimer = 16 + 80;
					m_fXS = 0;

					if (pInfo && pInfo->pFromObject)
						GiveScoreAndComboToAttacker(pInfo->pFromObject, 100);

					m_nNextState = nState;
					return;
				}
				break;
			}

			// Cannot press!
			return;
		}
		break;
	case STATE_WALK:
	case STATE_FALL:
	case STATE_JUMP:
		if (m_bWinged)
			nState = STATE_FLY;
		else
		{
			Turtle::ChangeState(nState, pInfo);
			return;
		}
	default:
		Turtle::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void Spiny::OnChangeState(int nState)
{
	switch (nState)
	{
	case STATE_FLY:
		if (m_bFlip)
			m_fXS = -1.0f;
		else
			m_fXS = 1.0f;
		break;
	default:
		Turtle::OnChangeState(nState);
		break;
	}
}

void Spiny::OnThrown()
{
	if (m_nStateFrame < 8)
		return;

	if (!m_bCollision[COLLISION_BOTTOM])
	{
		OnApplyGravity(0.5f, 3.0f);
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

void Spiny::OnFly()
{
	int nPhaseFrame = 20;
	int nVal = m_nStateFrame % (nPhaseFrame * 4);
	if (nVal < nPhaseFrame || (nVal >= nPhaseFrame * 3))
		m_fYS -= 0.05f;
	else if (nVal >= nPhaseFrame && nVal < nPhaseFrame * 3)
		m_fYS += 0.05f;

	if (m_nThrowSpikeCooltime > 0)
	{
		m_nThrowSpikeCooltime--;
		if (m_nThrowSpikeCooltime == 30)
		{
			CreateParameter param;
			param.pFromObject = this;
			param.nType = BULLET_SPINYSPIKE;
			param.fScaleX = m_fScaleX;
			param.fScaleY = m_fScaleY;
			
			NaRect rc = GetRect();
			if (m_bThrowSpikeDirection)
			{
				param.fXS = 1.0f;
				param.fYS = 1.0f;
				CUR_STAGE->CreateBullet(rc.right, rc.bottom, BULLET_SPINYSPIKE, &param);

				param.fXS = -1.0f;
				param.fYS = 1.0f;
				CUR_STAGE->CreateBullet(rc.left, rc.bottom, BULLET_SPINYSPIKE, &param);

				param.fXS = 1.0f;
				param.fYS = -1.0f;
				CUR_STAGE->CreateBullet(rc.right, rc.top, BULLET_SPINYSPIKE, &param);

				param.fXS = -1.0f;
				param.fYS = -1.0f;
				CUR_STAGE->CreateBullet(rc.left, rc.top, BULLET_SPINYSPIKE, &param);
			}
			else
			{
				param.fXS = 1.5f;
				param.fYS = 0;
				CUR_STAGE->CreateBullet(rc.right, rc.Middle(), BULLET_SPINYSPIKE, &param);

				param.fXS = -1.5f;
				param.fYS = 0;
				CUR_STAGE->CreateBullet(rc.left, rc.Middle(), BULLET_SPINYSPIKE, &param);

				param.fXS = 0;
				param.fYS = 1.5f;
				CUR_STAGE->CreateBullet(rc.Center(), rc.bottom, BULLET_SPINYSPIKE, &param);

				param.fXS = 0;
				param.fYS = -1.5f;
				CUR_STAGE->CreateBullet(rc.Center(), rc.top, BULLET_SPINYSPIKE, &param);
			}

			m_bThrowSpikeDirection = !m_bThrowSpikeDirection;
		}
	}
	else
	{
		if (m_nStateFrame % 160 == 80)
			ThrowSpike();
	}
}

void Spiny::OnLanding()
{
	Turtle::OnLanding();

	if (m_nShape == TURTLESHAPE_NORMAL)
	{
		if (CUR_PLAYER_OBJ->m_fX < m_fX)
			m_bFlip = true;
		else
			m_bFlip = false;

		if (m_fXS > 0 && m_bFlip)
			m_fXS *= -1;
		else if (m_fXS < 0 && m_bFlip == false)
			m_fXS *= -1;
	}
}

void Spiny::ThrowSpike()
{
	m_nThrowSpikeCooltime = 80;
}

bool Spiny::CanPressed(int nPower)
{
	if (m_nShape < TURTLESHAPE_NORMAL)
		return true;

	switch (nPower)
	{
	case POWER_PRESS_YOSHI:
	case POWER_PRESS_GOOMBASHOES:
		return true;
		break;
	}

	return false;
}

std::vector<POINT> Spiny::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -1;
	pt.y = -2;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int Spiny::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_SHELLIDLE:
	case STATE_SHELLAWAKE:
	case STATE_CARRYBYHAND:
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		nFrame = SPRIDX_SPINY_SPIN1;
		break;
	case STATE_PUSHEDUP:
		if (IS_CLASSIC)
			nFrame = SPRIDX_SPINY1;
		else
			nFrame = SPRIDX_SPINY_SPIN1;
		break;
	case STATE_THROWN:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_SPINY_EGG1, 2, 6);
		break;
	case STATE_BURNT:
		nFrame = SPRIDX_SPINY1;
		break;
	case STATE_SHELLRUN:
		if (m_pGame->m_bModernStyle)
		{
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_SPINY_SPIN1, 4, 3);
		}
		else
			nFrame = SPRIDX_SPINY_SPIN1;
		break;
	default:
		if (m_bWinged)
		{
			if (m_nThrowSpikeCooltime > 0 && m_nThrowSpikeCooltime < 75)
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_SPINY_EGG1, 2, 16);
			else
				nFrame = SPRIDX_SPINY_FLY;
		}
		else 
			nFrame = SPRIDX_SPINY1 + (m_nStateFrame / 8) % 2;
		if (m_nShape < TURTLESHAPE_NORMAL || m_bDeadInside)
			nFrame = SPRIDX_SPINY_SPIN1;
		break;
	}

	return nFrame;
}

void Spiny::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nThrowSpikeCooltime > 75)
	{
		fScaleX *= (1.0f - ((80 - m_nThrowSpikeCooltime) / 10.0f));
		fScaleY *= (1.0f - ((80 - m_nThrowSpikeCooltime) / 10.0f));
	}

	if (m_nThrowSpikeCooltime <= 5 && m_nThrowSpikeCooltime != 0)
	{
		fScaleX *= (1.0f - ((5 - m_nThrowSpikeCooltime) / 10.0f));
		fScaleY *= (1.0f - ((5 - m_nThrowSpikeCooltime) / 10.0f));
	}
}
