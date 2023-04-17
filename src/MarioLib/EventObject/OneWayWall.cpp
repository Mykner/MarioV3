#include "OneWayWall.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "SoundManager.h"

#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(OneWayWall)
	PROP_INT("Direction", VT_I4, 5, g_szOneWayWallDir, "OneWayWall"),
END_IMPL_PROPERTYMAP()

OneWayWall::OneWayWall(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_ONEWAYWALL;
	m_nState = STATE_IDLE;

	m_nDirection = 0;

	m_bCanCollide = true;
	m_bCanLand = false;
}

OneWayWall::~OneWayWall()
{
	m_vecPassenger.clear();
}

void OneWayWall::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	bool bShadow = (m_pGame->m_bModernStyle);

	if (nZOrder == -1)
		nZOrder = Z_SEMISOLID_BACKGROUND;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	float fAngle = 0;
	switch (m_nDirection)
	{
	case 0: fAngle = 0; break;
	case 1: fAngle = 90; break;
	case 2: fAngle = 180; break;
	case 3: fAngle = 270; break;
	}

	int nSpriteIndex = GetSpriteIndex();
	(*m_ppSprite)->RenderToQueue(_x, _y, nSpriteIndex,
		m_bFlip, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow, fAngle);
}

void OneWayWall::Process()
{
	
}

void OneWayWall::OnBeforeProcess()
{
	//CheckCollision();
	//CheckCollisionPlayer();
}

void OneWayWall::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	if (IsAlive())
	{
		// Move with passengers 
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger)
			pObj->m_fX += m_fXS;
		pObj->m_fY += m_fYS;
		END_VECTOR_LOOP();
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

bool OneWayWall::CanPushedUp()
{
	return false;
}

bool OneWayWall::CanCollide(int nDir)
{
	switch (nDir)
	{
	case COLLISION_RIGHT:
		if (m_nDirection == 0)
			return true;
		break;
	case COLLISION_BOTTOM:
		if (m_nDirection == 1)
			return true;
		break;
	case COLLISION_LEFT:
		if (m_nDirection == 2)
			return true;
		break;
	case COLLISION_TOP:
		if (m_nDirection == 3)
			return true;
		break;
	}

	return false;
}

int OneWayWall::GetSpriteIndex()
{
	if ((m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN) &&
		m_nStateFrame < 30)
		return -1;

	return SPRITE_INDEX_CYCLE(SPRIDX_MAPOBJ_ONEWAYWALL1, 5, 8);
}

SIZE OneWayWall::GetSize()
{
	SIZE s = {
		32,
		32
	};
	return s;
}
