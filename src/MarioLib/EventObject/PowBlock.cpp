#include "PowBlock.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"
#include "SoundManager.h"

#include "UIBase.h"
#include "SpeechBubble.h"

PowBlock::PowBlock(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_POWBLOCK;
	m_nState = STATE_IDLE;

	m_bCanLand = true;
	m_bCanCollide = true;

	m_bKicked = false;
}

PowBlock::~PowBlock()
{
}

void PowBlock::Process()
{
	ProcessBase();
}

void PowBlock::OnBeforeProcess()
{
	CheckCollision();
	CheckCollisionEnemy();
}

void PowBlock::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void PowBlock::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_EXPLODE)
	{
		if (m_nStateFrame % 2 == 1)
			return;
	}

	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	int nFrame = GetSpriteIndex();
	bool bShadow = m_pGame->m_bModernStyle;
	if (nZOrder == -1)
		nZOrder = GetZOrder();
	
	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		false, false, nColor, fScaleX, fScaleY, nZOrder, bShadow);
}

void PowBlock::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_KICKED:
	case STATE_KICKEDUP:
		m_bKicked = true;
		
		if (pInfo && pInfo->pFromObject)
		{
			m_nDamageGroup = pInfo->pFromObject->m_nDamageGroup;
		}
		break;
	case STATE_PRESSED:
		break;
	case STATE_CARRYBYHAND:
		m_bKicked = false;
		m_nDamageGroup = DAMAGEGROUP_NEUTRAL;

		EventObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	default:
		EventObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void PowBlock::OnIdle()
{
	CheckFalling();
}

void PowBlock::OnReachWall(bool bIsLeftWall)
{
	/*
	SIZE s = GetSize();

	if (bIsLeftWall)
		m_fX = (m_ptCollision[COLLISION_LEFT].x + 1) + s.cx / 2 + 1;
	else
		m_fX = (m_ptCollision[COLLISION_RIGHT].x) - s.cx / 2;

	m_fXS *= -0.5f;
	*/

	ChangeState(STATE_EXPLODE);
}

void PowBlock::OnPressed()
{
	if (m_bCollision[COLLISION_BOTTOM])
	{
		if (m_fY > m_ptCollision[COLLISION_BOTTOM].y)
		{
			m_fY = m_ptCollision[COLLISION_BOTTOM].y;
		}

		m_fYS = 0;
	}
	else 
	{
		OnApplyGravity(1, 4.0f);
	}
}

void PowBlock::OnPushedUp()
{
	m_fXS = 0;
	m_fYS = 0;

	ChangeState(STATE_EXPLODE);
}

void PowBlock::OnKicked()
{
	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
		if (pEnemy->IsAlive() &&
			//pEnemy->CanCollide(-1) == false &&
			!pEnemy->IsInvinsible() &&
			pEnemy->CanBurnt(BURN_STARMAN))
		{
			if ((m_fXS > 0 && pEnemy->m_fX > m_fX) ||
				(m_fXS < 0 && pEnemy->m_fX < m_fX))
			{
				if (m_pKickedBy)
				{
					AddCombo();
					if (m_pKickedBy->m_pController)
					{
						int nScore = CalcScoreFrom(100);
						m_pKickedBy->m_pController->AddScoreWithEffect(nScore, pEnemy);
					}
				}
				pEnemy->ChangeState(STATE_BURNT);
				NaDebugOut(L"%s Hits %s(0x%08x)\n",
					GetTypeName(), pEnemy->GetTypeName(), pEnemy);
			}
		}
	}

	EventObjectBase::OnKicked();
}

void PowBlock::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;
	
	m_fXS = 0;
	m_fYS = 0;

	if (m_bKicked)
	{
		ChangeState(STATE_EXPLODE);
	}
}

void PowBlock::OnExplode()
{
	if (m_nStateFrame == 0)
	{
		// Destroy near enemy
		{
			NaRect rc = GetRect();
			rc.left -= 16;
			rc.top -= 16;
			rc.right += 16;
			rc.bottom += 16;

			// Destroy Enemies
			BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEnemy) {
				NaRect rcObj = pObj->GetRect();
				if (!rc.IsOverlapped(rcObj))
					continue;

				if (pObj->IsAlive() && pObj->CanBurnt(BURN_STARMAN))
				{
					ChangeStateInfo info;
					info.nNextState = STATE_BURNT;
					info.pFromObject = this;
					info.nPower = BURN_STARMAN;
					pObj->ChangeState(STATE_BURNT, &info);
				}
			} END_VECTOR_LOOP();
		}

		// Make earthquake
		CUR_STAGE->MakeEarthquake(POWER_EARTHQUAKE_POWERBLOCK);
		SOUND_MANAGER->PlaySoundEffect(L"Cannon");
	}

	if (m_nStateFrame == 24)
	{
		ChangeState(STATE_DEAD);
	}
}

bool PowBlock::IsAlive()
{
	return (m_nState != STATE_DEAD);
}

bool PowBlock::CanBornFromBlock()
{
	return true;
}

bool PowBlock::CanCarried()
{
	switch (m_nState)
	{
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
	case STATE_EXPLODE:
		return false;
		break;
	}

	return true;
}

bool PowBlock::CanBurnt(int nType)
{
	switch (nType)
	{
	case BURN_THWOMPSMASH:
		return true;
		break;
	}

	return false;
}

void PowBlock::AddPassenger(GameObjectBase * pNewObj)
{
	if (pNewObj->m_nType >= BULLET_BEGIN && pNewObj->m_nType < BULLET_END)
		return;

	switch (m_nState)
	{
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
	case STATE_PIPEUP:
	case STATE_PIPEDOWN:
	case STATE_PIPELEFT:
	case STATE_PIPERIGHT:
		return;
		break;
	}
}

int PowBlock::GetSpriteIndex()
{
	if (EventObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame = SPRIDX_MAPOBJ_POWBLOCK;

	return SPRIDX_MAPOBJ_POWBLOCK;
}

int PowBlock::GetZOrder()
{
	int nZOrder = GameObjectBase::GetZOrder();
	if (nZOrder != 0)
		return nZOrder;

	if (m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN)
		return Z_BEHIND_MAP;

	return Z_COMMON_OBJ;
}

void PowBlock::GetScale(float & fScaleX, float & fScaleY)
{
	fScaleX = m_fScaleX;
	fScaleY = m_fScaleY;

	if (m_nState == STATE_EXPLODE)
	{
		int nFactor = m_nStateFrame % 4;
		if (nFactor == 0)
		{
			fScaleX = m_fScaleX * (1.0f + (m_nStateFrame / 90.0f));
			fScaleY = fScaleX;
		}
		else if (nFactor == 2)
		{
			fScaleX = m_fScaleX * (1.0f + (m_nStateFrame / 30.0f));
			fScaleY = fScaleX;
		}
	}
}

SIZE PowBlock::GetSize()
{
	SIZE s = { 16, 16 };

	return s;
}
