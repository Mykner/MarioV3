#include "CheepCheep.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "SoundManager.h"

BEGIN_IMPL_PROPERTYMAP(CheepCheep)
	PROP_BOOL("PowerUp", VT_BOOL, false, nullptr, "CheepCheep"),
	PROP_BOOL("Winged", VT_BOOL, false, nullptr, "CheepCheep")
END_IMPL_PROPERTYMAP()

CheepCheep::CheepCheep(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_CHEEPCHEEP;
	m_nState = STATE_IDLE;

	m_bFlip = true;

	m_nJump = 0;
}

CheepCheep::~CheepCheep()
{
}

void CheepCheep::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	switch (nState)
	{	
	case STATE_PRESSED:
		if (m_bWinged)
		{
			m_fXS = 0;
			m_fYS = -0.5f;

			ReleaseWing();
			nState = STATE_FALL;

			SOUND_MANAGER->PlaySoundEffect(L"Pressed");
		}
		else
		{
			EnemyObjectBase::ChangeState(nState, pInfo);
			return;
		}
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
	default:
		EnemyObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void CheepCheep::OnChangeState(int nState)
{
	switch (nState)
	{
	case STATE_SWIM:
		if (m_bFlip)
			m_fXS = -0.25f;
		else
			m_fXS = 0.25f;
		break;
	case STATE_JUMP:
		if (m_nJump < 2)
		{
			if (m_nJump == 0)
			{
				if (IS_CLASSIC)
					m_fXS = (rand() % 15) / 10.0f;
				else
					m_fXS = 1.0f;

				if (rand() % 2 == 0)
					m_bFlip = !m_bFlip;
				if (m_bFlip)
					m_fXS *= -1;
			}
			m_fYS = -4.0f;
			m_nJump++;
		}
		else
		{
			if (IS_CLASSIC)
				m_fYS = -8.2f;
			else
				m_fYS = -7.0f;
			m_nJump = 0;
		}
		break;
	default:
		EnemyObjectBase::OnChangeState(nState);
		break;
	}
}

void CheepCheep::OnIdle()
{
	if (CanSwim())
		ChangeState(STATE_SWIM);
	else
	{
		// Change to flying mode
		ChangeState(STATE_FALL);
	}
}

void CheepCheep::OnSwim()
{
	if (m_nStateFrame % 160 == 0)
	{
		m_fYS = -1 + (rand() % 3);
		if (m_fYS != 0)
			m_fYS /= 4.0f;
	}

	if (m_fYS < 0 && m_fY < 48)
		m_fYS = 0.25f;		

	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);
}

void CheepCheep::OnJump()
{
	if (m_bWinged && m_nJump == 0 && m_fYS > 2.0f)
		ChangeState(STATE_FLY);

	OnFall();
}

void CheepCheep::OnFall()
{
	OnApplyGravity(0.5f, 4.0f);

	if (IsOutOfCamera())
	{
		ChangeState(STATE_DEAD);
		return;
	}

	NaRect rc = GetRect();
	if (m_nStateFrame > 8 &&
		rc.top > GameDefaults::nPageHeight)
	{
		CreateParameter param;
		param.fScaleX = m_fScaleX;
		param.fScaleY = m_fScaleY;
		CUR_STAGE->CreateEffect(m_fX, m_fY - 24, EFFECT_SPLASH, &param);

		ChangeState(STATE_JUMP);
	}
}

void CheepCheep::OnFly()
{
	if (m_nStateFrame < 90)
	{
		if (m_fYS > -2.0f)
			m_fYS -= 0.07f;
	}

	if (m_nStateFrame == 90)
		ChangeState(STATE_FALL);
}

void CheepCheep::OnPressed()
{
	if (m_nStateFrame > 40)
		ChangeState(STATE_DEAD);
}

bool CheepCheep::IsAlive()
{
	return EnemyObjectBase::IsAlive();
}

bool CheepCheep::CanPressed(int nPower)
{
	if (m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
	{
		if (CanSwim())
			return false;
	}

	return true;
}

bool CheepCheep::CanBounceTrampolinMiniHorz()
{
	return false;
}

std::vector<POINT> CheepCheep::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = 0;
	pt.y = -5;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int CheepCheep::GetWingSpriteIndex()
{
	int nFrame = SPRIDX_WING1;
	if (m_nState == STATE_FLY)
	{
		if (m_nStateFrame % 8 >= 4)
			nFrame = SPRIDX_WING2;
	}
	else if (m_fYS != 0)
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

int CheepCheep::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_PRESSED:
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_CHEEPCHEEP1;
		break;
	default:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_CHEEPCHEEP1, 2, 8);
		break;
	}

	return nFrame;
}
