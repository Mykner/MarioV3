#include "YoshiTongue.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"

#include "Yoshi.h"

YoshiTongue::YoshiTongue(Game *pGame, Stage *pStage) :
	BulletObjectBase(pGame, pStage)
{
	m_nType = BULLET_YOSHITONGUE;
	m_nState = STATE_WALK;

	m_pOwner = nullptr;
	m_pHold = nullptr;
	m_nLength = 0;
	m_bUsed = false;
}

YoshiTongue::~YoshiTongue()
{
}

void YoshiTongue::ParseParameter(CreateParameter * pParam)
{
	if (pParam == nullptr)
		return;
	
	if (pParam->pFromPlayer != nullptr)
		m_nDamageGroup = DAMAGEGROUP_PLAYER;

	if (pParam->pFromObject)
	{
		m_pOwner = pParam->pFromObject;

		m_pController = m_pOwner->m_pController;
		m_bFlip = m_pOwner->m_bFlip;
	}

	if (m_bFlip)
		m_fXS *= -1;
}

void YoshiTongue::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	switch (m_nState)
	{
	case STATE_WALK:
		OnWalk();
		break;
	}

	if (m_vecOverlappedEnemy.size() > 0)
		OnDamage();
	
	if (IsOutOfCamera())
		ChangeState(STATE_DEAD);
}

void YoshiTongue::OnBeforeProcess()
{
	if (m_nNextState != -1)
	{
		// Affected by other
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;
	}

	if (m_nDamageGroup == DAMAGEGROUP_PLAYER)
		CheckCollisionEnemy();
	else
		CheckCollisionPlayer();
}

void YoshiTongue::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	GameObjectBase::ChangeState(nState);
}

void YoshiTongue::CheckCollisionEnemy()
{
	if (!IsAlive())
		return;

	if (m_pHold || m_bUsed)
		return;

	m_fX += m_fXS;
	m_fY += m_fYS;

	NaRect rcMe = GetRect();

	m_vecPressedEnemy.clear();
	m_vecOverlappedEnemy.clear();

	std::vector<GameObjectBase*> vecAllGameObject;
	PASTE_VECTOR(vecAllGameObject, CUR_STAGE->m_vecItem);
	PASTE_VECTOR(vecAllGameObject, CUR_STAGE->m_vecEnemy);
	PASTE_VECTOR(vecAllGameObject, CUR_STAGE->m_vecBullet);

	std::vector<GameObjectBase*>::iterator it = vecAllGameObject.begin();
	for (; it != vecAllGameObject.end(); ++it)
	{
		GameObjectBase *pObj = *it;
		if (pObj == this)
			continue;
		if (!pObj->IsAlive())
			continue;
		if (pObj->m_nState == STATE_IDLE && pObj->m_nStateFrame == 0)
			continue;
		if (abs(m_fX - pObj->m_fX) > GameDefaults::nPageWidth)
			continue;

		if (IsOverlapped(pObj))
		{
			m_vecOverlappedEnemy.push_back(pObj);
		}
	}

	m_fX -= m_fXS;
	m_fY -= m_fYS;
}

void YoshiTongue::OnWalk()
{
	if (m_nStateFrame < 8 && m_pHold == nullptr)
		m_nLength += 4;
	else if (m_nStateFrame > 12 && m_nLength > 0)
		m_nLength -= 4;

	// This is Tongue... Relative position
	m_bFlip = m_pOwner->m_bFlip;
	if (!m_bFlip)
		m_fX = m_pOwner->m_fX + 19;
	else
		m_fX = m_pOwner->m_fX - 19;
	m_fY = m_pOwner->m_fY - 10;

	// Held Item, Enemy, ..
	if (m_pHold)
	{
		if (!m_pHold->IsAlive())
		{
			m_pHold = nullptr;
			return;
		}

		m_pHold->SetPosition(
			m_fX + ((m_nLength + 4) * (m_bFlip ? -1 : 1)),
			m_fY + 4
		);
	}

	if (m_nStateFrame == 21)
	{
		if (m_pHold)
		{
			if (m_pOwner->m_nType == VEHICLE_YOSHI)
			{
				Yoshi *pYoshi = (Yoshi*)m_pOwner;
				pYoshi->OnKeepInMouth(m_pHold);
			}
			else
				m_pHold->ChangeState(STATE_DEAD);

			m_pHold = nullptr;
		}
		ChangeState(STATE_DEAD);
	}
}

void YoshiTongue::OnDamage()
{
	if (m_pHold || m_bUsed)
		return;

	std::vector<GameObjectBase*>::iterator it = m_vecOverlappedEnemy.begin();
	for (; it != m_vecOverlappedEnemy.end(); ++it)
	{
		GameObjectBase *pEnemy = *it;
		if (pEnemy->IsAlive())
		{
			// Damage Only One Target
			if (pEnemy->CanEat())
			{
				m_pHold = pEnemy;

				ChangeStateInfo info;
				info.nNextState = STATE_HELDBYTONGUE;
				info.pFromObject = this;
				pEnemy->ChangeState(STATE_HELDBYTONGUE, &info);
		
				m_bUsed = true;
				break;
			}
		}
	}
}

NaRect YoshiTongue::GetRect()
{
	NaRect rc;
	SIZE s = GetSize();
	rc.left = m_fX;
	rc.right = m_fX;
	if (!m_bFlip)
		rc.right += s.cx;
	else
		rc.left -= s.cx;

	rc.top = m_fY - s.cy + 1;
	rc.bottom = m_fY;

	return rc;
}

SIZE YoshiTongue::GetSize()
{
	SIZE s = { 4 + m_nLength, 4 };
	return s;
}

void YoshiTongue::Render(int nColor, int nZOrder)
{
	if (m_nState == STATE_DEAD)
		return;

	NaPointT<float> pt = CAMERA->GetPosition();
	int nFrame = GetSpriteIndex();

	bool bShadow = m_pGame->m_bModernStyle;

	int nOffset = 0;
	for (int i = 0; i < m_nLength; i += 4)
	{
		nOffset = (m_bFlip ? -i : i);
		m_pSprite->RenderToQueue(
			x - pt.x + nOffset,
			y - pt.y,
			SPRIDX_BULLET_YOSHITONGUE_BODY,
			m_bFlip, m_bFlipV, nColor, 1.0f, 1.0f, Z_BULLET, bShadow);
	}

	nOffset = (m_bFlip ? -m_nLength : m_nLength);
	m_pSprite->RenderToQueue(
		x - pt.x + nOffset,
		y - pt.y, 
		SPRIDX_BULLET_YOSHITONGUE_HEAD,
		m_bFlip, m_bFlipV, nColor, 1.0f, 1.0f, Z_BULLET, bShadow);
}
