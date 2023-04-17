#include "FireBar.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "Fireball.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(FireBar)
	PROP_INT("Length", VT_I4, 6, 0, "FireBar"),
	PROP_FLOAT("Speed", VT_R4, 1.875f, 0, "FireBar"),
	PROP_FLOAT("InitialAngle", VT_R4, 45.0f, 0, "FireBar"),
END_IMPL_PROPERTYMAP()

FireBar::FireBar(Game *pGame, Stage *pStage) :
	EnemyObjectBase(pGame, pStage)
{
	m_nType = ENEMY_FIREBAR;
	m_nState = STATE_IDLE;
	m_pSprite = SPR_MANAGER->Find(L"Bullet");

	m_bFlip = true;
	m_fAngle = 45.0f;
	m_nLength = 6;
	//m_fSpeed = 1.875f; // 360 / 192
	m_fSpeed = 360.0f / 208.0f; // 360 / 192

	CreateFireBalls(pGame, pStage);
	PositionFireBalls();
}

FireBar::~FireBar()
{
	SAFE_DELETE_OBJPTR_VECTOR(FireBall*, m_vecFireBall);
}

bool FireBar::IsOverlapped(GameObjectBase * pOther)
{
	int nLength = m_nLength;
	if (nLength > 100)
		nLength = 100;
	for (int i = 0; i < nLength; i++)
	{
		if (m_vecFireBall[i]->IsOverlapped(pOther))
			return true;
	}
	return false;
}

void FireBar::OnBeforeProcess()
{
	if (m_nNextState != -1)
	{
		// Affected by other
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;
	}
}

void FireBar::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int nLength = m_nLength;
	if (nLength > 100)
		nLength = 100;
	for (int i = 0; i < nLength; i++)
	{
		if (i == 0 &&
			(m_vecFireBall[i]->m_fX != m_fX || m_vecFireBall[i]->m_fY != m_fY))
		{
			// Design Issue
			PositionFireBalls();
		}

		FireBall *pFireBall = m_vecFireBall[i];

		int nFrame = pFireBall->GetSpriteIndex();
		if (m_pGame->m_bModernStyle)
		{
			float fScale = 1.0f;
			if (m_nState == STATE_STACKED && i == 0)
				fScale = 1.5f;
			
			bool bShadow = true;
			float fAngle = m_nStateFrame * 30;
			pFireBall->m_pSprite->RenderToQueue(
				pFireBall->m_fX - pt.x,
				pFireBall->m_fY - pt.y,
				nFrame,
				pFireBall->m_bFlip,
				pFireBall->m_bFlipV,
				nColor,
				fScale,
				fScale,
				Z_ENEMY,
				bShadow,
				fAngle
			);
		}
		else
		{
			pFireBall->m_pSprite->RenderToQueue(
				pFireBall->m_fX - pt.x,
				pFireBall->m_fY - pt.y,
				nFrame,
				pFireBall->m_bFlip,
				pFireBall->m_bFlipV,
				nColor,
				1.0f,
				1.0f,
				Z_ENEMY
			);
		}
	}
}

void FireBar::RenderDesign(int nColor, int nZOrder)
{
	Render(nColor);

	Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	if (m_fSpeed > 0)
	{
		pSpriteDesign->RenderToQueue(_x, _y - 8, SPRIDX_DESIGN_ROTATE_CW,
			false, false, nColor, 1.0f, 1.0f, Z_DESIGN_INDICATOR, true);
	}
	else if (m_fSpeed < 0)
	{
		pSpriteDesign->RenderToQueue(_x, _y - 8, SPRIDX_DESIGN_ROTATE_CCW,
			false, false, nColor, 1.0f, 1.0f, Z_DESIGN_INDICATOR, true);
	}
}

void FireBar::ChangeState(int nState, ChangeStateInfo *pInfo)
{
	EnemyObjectBase::ChangeState(nState, pInfo);
}

void FireBar::OnIdle()
{
	if (m_nLength > 100)
		m_nLength = 100;
	PositionFireBalls();

	for (int i = 0; i < m_nLength; i++)
	{
		m_vecFireBall[i]->Process();
	}

	if (m_pGame->m_bModernStyle)
	{
		// Smooth
		m_fAngle += m_fSpeed;
	}
	else
	{
		// No Smooth Concept
		if (m_nStateFrame % 6 == 5)
			m_fAngle += (m_fSpeed * 6);
	}

	if (m_fAngle >= 360)
		m_fAngle = 0;
}

void FireBar::OnStacked()
{
	OnIdle();

	if (m_pUnderMyFoot)
	{
		if (!m_pUnderMyFoot->IsAlive())
		{
			// Buggy condition
			m_pUnderMyFoot->ReleaseEnemyStack();
			return;
		}

		NaRect rc = GetRect();

		if (m_bCollision[COLLISION_LEFT])
		{
			if (m_ptCollision[COLLISION_LEFT].x - rc.left > 2)
			{
				ReleaseFootStack();

				m_fX = m_ptCollision[COLLISION_LEFT].x + (rc.Width() / 2);
				ChangeState(STATE_IDLE);
				return;
			}
		}

		if (m_bCollision[COLLISION_RIGHT])
		{
			if (rc.right - m_ptCollision[COLLISION_RIGHT].x > 2)
			{
				ReleaseFootStack();

				m_fX = m_ptCollision[COLLISION_RIGHT].x - (rc.Width() / 2);
				ChangeState(STATE_IDLE);
				return;
			}
		}

		NaRect rcStack = m_pUnderMyFoot->GetRect();
		m_fX = m_pUnderMyFoot->m_fX - m_pUnderMyFoot->m_fXS;
		m_fY = rcStack.top - 1;

		//m_bFlip = m_pUnderMyFoot->m_bFlip;
	}
}

bool FireBar::IsAlive()
{
	return true;
}

bool FireBar::IsInvinsible()
{
	return true;
}

bool FireBar::CanBurnt(int nType)
{
	return false;
}

bool FireBar::CanPressed(int nPower)
{
	return false;
}

bool FireBar::CanBornFromBlock()
{
	return false;
}

bool FireBar::CanEat()
{
	return false;
}

int FireBar::GetZOrder()
{
	return Z_AFTER_MAP;
}

std::vector<NaRect> FireBar::GetRectList()
{
	std::vector<NaRect> vecRect;
	int nLen = m_vecFireBall.size();
	for (int i = 0; i < nLen; i++)
	{
		NaRect rc = m_vecFireBall[i]->GetRect();
		vecRect.push_back(rc);
	}
	return vecRect;
}

void FireBar::CreateFireBalls(Game *pGame, Stage *pStage)
{
	for (int i = 0; i < m_nLength; i++)
	{
		FireBall *pFireBall = new FireBall(pGame, pStage);
		pFireBall->m_fXS = 0;
		pFireBall->m_fYS = 0;
		pFireBall->m_nState = STATE_IDLE;
		pFireBall->m_pParent = this;

		m_vecFireBall.push_back(pFireBall);
	}
}

void FireBar::PositionFireBalls()
{
	// Just Positioning Fireballs
	if (m_nLength != m_vecFireBall.size())
	{
		SAFE_DELETE_OBJPTR_VECTOR(FireBall*, m_vecFireBall);
		CreateFireBalls(m_pGame, m_pStage);
	}

	for (int i = 0; i < m_nLength; i++)
	{
		const float fToRadian = (1 / 180.0f) * (float)3.141592f;
		float rad = m_fAngle * fToRadian;
		float fDist = i * 6.5;

		float _x = (m_fX + 0) + fDist * cos(rad) - fDist * sin(rad);
		float _y = (m_fY - 4) + fDist * sin(rad) + fDist * cos(rad);

		m_vecFireBall[i]->m_fX = _x;
		m_vecFireBall[i]->m_fY = _y;
	}
}
