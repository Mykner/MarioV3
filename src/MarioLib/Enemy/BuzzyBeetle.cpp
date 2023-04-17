#include "BuzzyBeetle.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

#include "SpeechBubble.h"

BuzzyBeetle::BuzzyBeetle(Game *pGame, Stage *pStage) :
	Turtle(pGame, pStage)
{
	m_nType = ENEMY_BUZZYBEETLE;
	m_nState = STATE_IDLE;

	m_bFlip = true;
}

BuzzyBeetle::~BuzzyBeetle()
{
	m_vecPassenger.clear();
}

void BuzzyBeetle::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (m_nDamageCooltime > 0)
		m_nDamageCooltime--;

	if (m_nKickCooltime > 0)
		m_nKickCooltime--;

	if (IsAlive())
	{
		// Move with passengers 
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
			if (!CheckMoveWithMe(pObj))
				pObj->m_fY = GetRectFloat().top + 1;
		} END_VECTOR_LOOP();

		m_vecPassenger.clear();
	}

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void BuzzyBeetle::Render(int nColor, int nZOrder)
{
	Turtle::Render(nColor, nZOrder);

	/*
	{
		NaPointT<float> pt = CAMERA->GetPosition();
		float _x = x - pt.x;
		float _y = y - pt.y;

		NaString str;
		str.Format(L"%d", m_vecPassenger.size());

		NaRect rc = GetRect();
		SpeechBubble b(m_pGame);
		b.RenderWithTextAutoSized(
			_x, rc.top - 4, str
		);
	}
	*/
}

void BuzzyBeetle::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
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

void BuzzyBeetle::OnChangeState(int nState)
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

void BuzzyBeetle::OnFly()
{
	int nPhaseFrame = 20;
	int nVal = m_nStateFrame % (nPhaseFrame * 4);
	if (nVal < nPhaseFrame || (nVal >= nPhaseFrame * 3))
		m_fYS -= 0.05f;
	else if (nVal >= nPhaseFrame && nVal < nPhaseFrame * 3)
		m_fYS += 0.05f;
		
	if (IsOutOfStage())
		ChangeState(STATE_DEAD);
}

bool BuzzyBeetle::CanLand()
{
	if (m_bWinged)
		return true;

	return false;
}

bool BuzzyBeetle::CanPressed(int nPower)
{
	if (m_bWinged)
		return false;

	return Turtle::CanPressed(nPower);
}

bool BuzzyBeetle::CanDamageEnemy()
{
	if (m_bWinged)
		return false;

	return Turtle::CanDamageEnemy();
}

bool BuzzyBeetle::CanBurnt(int nType)
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

void BuzzyBeetle::AddPassenger(GameObjectBase * pObj)
{
	m_vecPassenger.push_back(pObj);
}

std::vector<POINT> BuzzyBeetle::GetWingPosition()
{
	POINT pt;
	SIZE s = GetSize();
	pt.x = -1;
	pt.y = -2;

	std::vector<POINT> vec;
	vec.push_back(pt);

	return vec;
}

int BuzzyBeetle::GetSpriteIndex()
{
	if (EnemyObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;
	switch (m_nState)
	{
	case STATE_SHELLAWAKE:
	case STATE_PRESSED:
	case STATE_SHELLIDLE:
	case STATE_CARRYBYHAND:
	case STATE_KICKEDUP:
	case STATE_PLACEDBYHAND:
		if (IS_MODERN)
			nFrame = SPRIDX_BUZZYBEETLE_SPIN1;
		else
			nFrame = SPRIDX_BUZZYBEETLE_SHELL;
		break;
	case STATE_SHELLRUN:
		if (m_pGame->m_bModernStyle)
		{
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_BUZZYBEETLE_SPIN1, 4, 3);
		}
		else
			nFrame = SPRIDX_BUZZYBEETLE_SHELL;
		break;
	case STATE_PUSHEDUP:
	case STATE_BURNT:
		nFrame = SPRIDX_BUZZYBEETLE_SHELL;
		break;
	default:
		if (m_bWinged)
			nFrame = SPRIDX_BUZZYBEETLE2;
		else
			nFrame = SPRIDX_BUZZYBEETLE1 + (m_nStateFrame / 8) % 2;
		if (m_nShape == TURTLESHAPE_SHELL || m_bDeadInside)
			nFrame = SPRIDX_BUZZYBEETLE_SHELL;
		break;
	}

	return nFrame;
}
