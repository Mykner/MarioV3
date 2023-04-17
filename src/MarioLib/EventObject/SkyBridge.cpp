#include "SkyBridge.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(SkyBridge)
	PROP_INT("Length",			VT_I4, 3,					nullptr,			"SkyBridge"),
END_IMPL_PROPERTYMAP()

SkyBridge::SkyBridge(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_SKYBRIDGE;
	m_nState = STATE_IDLE;

	m_nLength = 3;

	m_nInitialX = 0;
	m_nInitialY = 0;

	m_bCanCollide = true;
	m_bCanLand = true;
}

SkyBridge::~SkyBridge()
{
	m_vecPassenger.clear();
}

void SkyBridge::Process()
{
	switch (m_nState)
	{
	case STATE_MOVE:
		OnMove();
		break;
	default:
		GameObjectBase::Process();
		return;
		break;
	}
}

void SkyBridge::OnBeforeProcess()
{
	//CheckCollision();
	//CheckCollisionPlayer();
}

void SkyBridge::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (IsAlive())
	{
		// Move with passengers 
		NaRect rcMe = GetRect();
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
			if (!pObj->IsAlive())
				continue;

			NaRect rcObj = pObj->GetRect();

			// Passenger has teleported :o!?
			if (m_fYS != 0 && abs((rcMe.top - m_fYS) - pObj->m_fY) > m_fYS * 2)
				continue;

			if (m_fXS != 0 &&
				((rcObj.right < rcMe.left) || (rcObj.left > rcMe.right) || pObj->m_nState == STATE_CLIMBIDLE))
				continue;

			if (m_fXS < 0 && pObj->m_bCollision[COLLISION_LEFT] == true)
			{
				if (rcObj.left < pObj->m_ptCollision[COLLISION_LEFT].x)
					pObj->m_fX = pObj->m_ptCollision[COLLISION_LEFT].x + (rcObj.Width() / 2);
			}
			else if (m_fXS > 0 && pObj->m_bCollision[COLLISION_RIGHT] == true)
			{
				if (rcObj.right > pObj->m_ptCollision[COLLISION_RIGHT].x)
					pObj->m_fX = pObj->m_ptCollision[COLLISION_RIGHT].x - (rcObj.Width() / 2);
			}
			else
				pObj->m_fX += m_fXS;

			if (m_fYS < 0 && pObj->m_bCollision[COLLISION_TOP] == true)
			{
				if (rcObj.top < pObj->m_ptCollision[COLLISION_TOP].y)
					pObj->m_fY = pObj->m_ptCollision[COLLISION_TOP].y + rcObj.Height();
			}
			else
				pObj->m_fY += m_fYS;
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

void SkyBridge::Render(int nColor, int nZOrder)
{
	RenderSkyBridge(m_fX, m_fY, nColor);
}

void SkyBridge::RenderDesign(int nColor, int nZOrder)
{
	RenderSkyBridge(m_fX, m_fY, nColor);

	Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	SIZE s = GetSize();
	pSpriteDesign->RenderToQueue(_x - s.cx / 2, _y - 4 - 8, SPRIDX_DESIGN_MOVE_RIGHT,
		false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);
}

void SkyBridge::RenderSkyBridge(int x, int y, int nColor)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;
	bool bShadow = (m_pGame->m_bModernStyle);

	SIZE s = GetSize();
	_x -= (s.cx / 2);
	_x += 8;

	for (int i = 0; i < m_nLength; i++)
	{
		(*m_ppSprite)->RenderToQueue(_x + (i * 16), _y, SPRIDX_MAPOBJ_SKYBRIDGE,
			false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ, bShadow);
	}
}

void SkyBridge::OnIdle()
{
	if (m_nStateFrame == 0)
	{
		m_nInitialX = m_fX;
		m_nInitialY = m_fY;
	}

	if (m_vecPassenger.size() > 0)
	{
		m_fXS = 1.0f;
		ChangeState(STATE_MOVE);
	}
}

void SkyBridge::OnMove()
{
	
}

void SkyBridge::AddPassenger(GameObjectBase * pObj)
{
	m_vecPassenger.push_back(pObj);
}

SIZE SkyBridge::GetSize()
{
	SIZE s = {
		16 * m_nLength,
		8
	};
	return s;
}
