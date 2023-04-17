#include "Bridge.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"

#include "SpeechBubble.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Bridge)
	PROP_BOOL("IsShortSize",	VT_BOOL,	false,				nullptr,			"Bridge"),
	PROP_INT("Length",			VT_I4,		3,					nullptr,			"Bridge"),
	PROP_INT("BridgeType",		VT_I4,		BRIDGETYPE_NORMAL,	g_szBridgeTypes,	"Bridge"),
	PROP_INT("MoveDistance",	VT_I4,		104,				nullptr,			"Bridge"),
	PROP_INT("AnchorHeight",	VT_I4,		80,					nullptr,			"Bridge"),
END_IMPL_PROPERTYMAP()

Bridge::Bridge(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_BRIDGE;
	m_nState = STATE_IDLE;

	m_bIsShort = false;
	m_nLength = 3;
	m_nBridgeType = BRIDGETYPE_NORMAL;
	m_nDistance = 104;
	m_nAnchorHeight = 80;
	m_nAnchorPositionY = -1;
	m_nLinkedPositionX = -1;

	m_nInitialX = 0;
	m_nInitialY = 0;

	m_bCanLand = true;
	m_pLinkedBridge = nullptr;
}

Bridge::~Bridge()
{
	m_vecPassenger.clear();
}

void Bridge::Process()
{
	switch (m_nState)
	{
	case STATE_FALL:
		OnFall();
		break;
	case STATE_MOVE:
		OnMove();
		break;
	default:
		GameObjectBase::Process();
		return;
		break;
	}
}

void Bridge::OnBeforeProcess()
{
	//CheckCollision();
	//CheckCollisionPlayer();
}

void Bridge::OnAfterProcess()
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

		if (m_nBridgeType == BRIDGETYPE_BALANCE_LEFT || m_nBridgeType == BRIDGETYPE_BALANCE_RIGHT)
		{
			if (m_nState == STATE_MOVE)
				m_nAnchorHeight += m_fYS;
		}

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

void Bridge::Render(int nColor, int nZOrder)
{
	RenderBridge(m_fX, m_fY, nColor);

	if (m_nBridgeType == BRIDGETYPE_BALANCE_LEFT || m_nBridgeType == BRIDGETYPE_BALANCE_RIGHT)
	{
		RenderLineAndAnchor(nColor);
	}

	/*
	{
		auto rc = GetRectFloat();
		NaString str;
		str.Format(L"T:%.2f", rc.top, rc.bottom);

		SpeechBubble b(m_pGame);
		b.RenderWithTextAutoSized(
			m_fX + m_nLength * 8, m_fY - 8, str
		);
	}
	*/
}

void Bridge::RenderDesign(int nColor, int nZOrder)
{
	RenderBridge(m_fX, m_fY, nColor);

	Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	SIZE s = GetSize();

	switch(m_nBridgeType)
	{
	case BRIDGETYPE_CYCLEUP:
		pSpriteDesign->RenderToQueue(_x, _y - s.cy - 4, SPRIDX_DESIGN_MOVE_UP,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		RenderBridge(m_fX, m_fY + 8, COLOR_ARGB(50, 255, 255, 255));
		RenderBridge(m_fX, m_fY + 16, COLOR_ARGB(25, 255, 255, 255));
		break;
	case BRIDGETYPE_CYCLEDOWN:
		pSpriteDesign->RenderToQueue(_x, _y + 4, SPRIDX_DESIGN_MOVE_DOWN,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		RenderBridge(m_fX, m_fY - 8, COLOR_ARGB(50, 255, 255, 255));
		RenderBridge(m_fX, m_fY - 16, COLOR_ARGB(25, 255, 255, 255));
		break;
	case BRIDGETYPE_DOWNUP:
		pSpriteDesign->RenderToQueue(_x, _y + 4, SPRIDX_DESIGN_MOVE_DOWN,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		pSpriteDesign->RenderToQueue(_x, _y + m_nDistance - s.cy - 4, SPRIDX_DESIGN_MOVE_UP,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);
		RenderBridge(m_fX, m_fY + m_nDistance, COLOR_ARGB(150, 255, 255, 255));
		break;
	case BRIDGETYPE_RIGHTLEFT:
		pSpriteDesign->RenderToQueue(_x - s.cx / 2, _y - 4 - 8, SPRIDX_DESIGN_MOVE_RIGHT,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		pSpriteDesign->RenderToQueue(_x + m_nDistance + s.cx / 2, _y - 4 - 8, SPRIDX_DESIGN_MOVE_LEFT,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);
		RenderBridge(m_fX + m_nDistance, m_fY, COLOR_ARGB(150, 255, 255, 255));
		break;
	case BRIDGETYPE_UPDOWN:
		pSpriteDesign->RenderToQueue(_x, _y - m_nDistance + 4, SPRIDX_DESIGN_MOVE_DOWN,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		pSpriteDesign->RenderToQueue(_x, _y - s.cy - 4, SPRIDX_DESIGN_MOVE_UP,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);
		RenderBridge(m_fX, m_fY - m_nDistance, COLOR_ARGB(150, 255, 255, 255));
		break;
	case BRIDGETYPE_LEFTRIGHT:
		pSpriteDesign->RenderToQueue(_x + s.cx / 2, _y - 4 - 8, SPRIDX_DESIGN_MOVE_LEFT,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);

		pSpriteDesign->RenderToQueue(_x - m_nDistance - s.cx / 2, _y - 4 - 8, SPRIDX_DESIGN_MOVE_RIGHT,
			false, false, -1L, 1.0f, 1.0f, Z_COMMON_OBJ);
		RenderBridge(m_fX - m_nDistance, m_fY, COLOR_ARGB(150, 255, 255, 255));
		break;
	case BRIDGETYPE_BALANCE_LEFT:
	case BRIDGETYPE_BALANCE_RIGHT:
		FindLinkedBridge();
		RenderLineAndAnchor(nColor);
		break;
	}
}

void Bridge::RenderBridge(float x, float y, int nColor)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = (m_pGame->m_bModernStyle);

	SIZE s = GetSize();
	if (m_bIsShort)
	{
		_x -= (s.cx / 2);
		_x += 4;

		for (int i = 0; i < m_nLength; i++)
		{
			(*m_ppSprite)->RenderToQueue(_x + (i * 8), _y, SPRIDX_MAPOBJ_BRIDGE_SHORT,
				false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ, bShadow);
		}
	}
	else
	{
		_x -= (s.cx / 2);
		_x += 8;

		for (int i = 0; i < m_nLength; i++)
		{
			(*m_ppSprite)->RenderToQueue(_x + (i * 16), _y, SPRIDX_MAPOBJ_BRIDGE,
				false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ, bShadow);
		}
	}
}

void Bridge::RenderLineAndAnchor(int nColor)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = m_fX - pt.x;
	float _y = m_fY - pt.y;

	SIZE s = GetSize();

	// Vertical Line
	for (int i = 0; i < m_nAnchorHeight; i += 8)
	{
		(*m_ppSprite)->RenderToQueue(_x, m_nAnchorPositionY + i, SPRIDX_MAPOBJ_LINE_VERT,
			false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP);
	}

	if (m_nBridgeType == BRIDGETYPE_BALANCE_LEFT)
	{
		// Anchor
		(*m_ppSprite)->RenderToQueue(_x, m_nAnchorPositionY - 8, SPRIDX_MAPOBJ_BRIDGE_ANCHOR_L,
			false, false, nColor, 1.0f, 1.0f, Z_AFTER_MAP);

		// Horizontal Line
		for (int i = m_fX + 10; i < m_nLinkedPositionX; i += 8)
		{
			(*m_ppSprite)->RenderToQueue(i - pt.x, m_nAnchorPositionY - 7 - 8, SPRIDX_MAPOBJ_LINE_HORZ,
				false, false, nColor, 1.0f, 1.0f, Z_BEHIND_MAP);
		}
	}
	else if (m_nBridgeType == BRIDGETYPE_BALANCE_RIGHT)
	{
		// Anchor
		(*m_ppSprite)->RenderToQueue(_x, m_nAnchorPositionY - 8, SPRIDX_MAPOBJ_BRIDGE_ANCHOR_R,
			false, false, nColor, 1.0f, 1.0f, Z_AFTER_MAP);
	}
}

void Bridge::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_FALL:
		m_fYS = 0;
		break;
	default:
		EventObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;
}

void Bridge::OnIdle()
{
	if (m_nStateFrame == 0)
	{
		m_nInitialX = m_fX;
		m_nInitialY = m_fY;

		if (m_nBridgeType == BRIDGETYPE_BALANCE_LEFT || m_nBridgeType == BRIDGETYPE_BALANCE_RIGHT)
		{
			FindLinkedBridge();
		}
		
		ChangeState(STATE_MOVE);
	}
}

void Bridge::OnFall()
{
	m_fYS += 0.33f;
	if (m_fYS > 4.0f)
		m_fYS = 4.0f;
}

void Bridge::OnMove()
{
	switch (m_nBridgeType)
	{
	case BRIDGETYPE_NORMAL:
		break;
	case BRIDGETYPE_CYCLEUP:
		OnMoveCycleUp();
		break;
	case BRIDGETYPE_CYCLEDOWN:
		OnMoveCycleDown();
		break;
	case BRIDGETYPE_DOWNUP:
		OnMoveDownUp();
		break;
	case BRIDGETYPE_UPDOWN:
		OnMoveUpDown();
		break;
	case BRIDGETYPE_RIGHTLEFT:
		OnMoveRightLeft();
		break;
	case BRIDGETYPE_LEFTRIGHT:
		OnMoveLeftRight();
		break;
	case BRIDGETYPE_BALANCE_LEFT:
	case BRIDGETYPE_BALANCE_RIGHT:
		if (m_pLinkedBridge)
		{
			if (m_vecPassenger.size() > 0 &&
				m_vecPassenger.size() > m_pLinkedBridge->m_vecPassenger.size())
			{
				m_fYS = 2.0f;
				m_pLinkedBridge->m_fYS = -2.0f;
			}
			else if (m_vecPassenger.size() == m_pLinkedBridge->m_vecPassenger.size())
			{
				m_fYS = 0;
				m_pLinkedBridge->m_fYS = 0;
			}

			if (m_nAnchorHeight <= 0)
			{
				ChangeState(STATE_FALL);

				m_pLinkedBridge->ChangeState(STATE_FALL);

				BEGIN_VECTOR_LOOP(GameObjectBase*, m_pLinkedBridge->m_vecPassenger) {
					if (pObj->m_pController)
					{
						pObj->m_pController->AddScoreWithEffect(1000, pObj);
					}
				} END_VECTOR_LOOP();

				InitAnchorPositionY();
				m_pLinkedBridge->InitAnchorPositionY();

				// Unlink
				m_pLinkedBridge->m_pLinkedBridge = nullptr;
				m_pLinkedBridge = nullptr;
			}
		}
		break;
	case BRIDGETYPE_FALL:
		if (m_vecPassenger.size() > 0)
			m_fYS = 2.0f;
		else
			m_fYS = 0;
		break;
	}
}

void Bridge::OnMoveCycleUp()
{
	if (m_fY < -16)
		m_fY = 256;
	m_fYS = -1.0f;
}

void Bridge::OnMoveCycleDown()
{
	if (m_fY > 256)
		m_fY = -16;
	m_fYS = 1.0f;
}

void Bridge::OnMoveDownUp()
{
	// Original = 420 frame
	// Current = 360 frame

	// #TODO Modify Speed Function

	float fCur = (m_nDistance / 2.0f) * sin((-90 + m_nStateFrame) * fToRadian);
	float fNew = (m_nDistance / 2.0f) * sin((-90 + (m_nStateFrame + 1)) * fToRadian);
	m_fYS = fNew - fCur;
}

void Bridge::OnMoveUpDown()
{
	float fCur = (m_nDistance / 2.0f) * sin((90 + m_nStateFrame) * fToRadian);
	float fNew = (m_nDistance / 2.0f) * sin((90 + (m_nStateFrame + 1)) * fToRadian);
	m_fYS = fNew - fCur;
}

void Bridge::OnMoveRightLeft()
{
	float fCur = (m_nDistance / 2.0f) * sin((-90 + m_nStateFrame) * fToRadian);
	float fNew = (m_nDistance / 2.0f) * sin((-90 + (m_nStateFrame + 1)) * fToRadian);
	m_fXS = fNew - fCur;
}

void Bridge::OnMoveLeftRight()
{
	float fCur = (m_nDistance / 2.0f) * sin((90 + m_nStateFrame) * fToRadian);
	float fNew = (m_nDistance / 2.0f) * sin((90 + (m_nStateFrame + 1)) * fToRadian);
	m_fXS = fNew - fCur;
}

void Bridge::FindLinkedBridge()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEventObject) {
		if (pObj->m_nType == EVENTOBJECT_BRIDGE)
		{
			if (abs(pObj->m_fX - m_fX) > GameDefaults::nPageWidth / 2)
				continue;

			if (m_nBridgeType == BRIDGETYPE_BALANCE_LEFT)
			{
				Bridge *pBridge = (Bridge*)pObj;
				if (pBridge->m_nBridgeType == BRIDGETYPE_BALANCE_RIGHT &&
					m_fX < pBridge->m_fX)
				{
					m_nLinkedPositionX = pBridge->m_fX;
					m_pLinkedBridge = pBridge;

					m_pLinkedBridge->m_nAnchorHeight = m_nAnchorHeight - (m_fY - m_pLinkedBridge->m_fY);
					break;
				}
			}
			else if (m_nBridgeType == BRIDGETYPE_BALANCE_RIGHT)
			{
				Bridge *pBridge = (Bridge*)pObj;
				if (pBridge->m_nBridgeType == BRIDGETYPE_BALANCE_LEFT &&
					m_fX > pBridge->m_fX)
				{
					m_pLinkedBridge = pBridge;
					break;
				}
			}
		}
	} END_VECTOR_LOOP();

	if (m_pLinkedBridge != nullptr)
	{
		InitAnchorPositionY();
	}
}

void Bridge::InitAnchorPositionY()
{
	m_nAnchorPositionY = m_fY - m_nAnchorHeight;
}

void Bridge::AddPassenger(GameObjectBase * pObj)
{
	m_vecPassenger.push_back(pObj);
}

SIZE Bridge::GetSize()
{
	SIZE s;
	s = {
		(m_bIsShort ? 8 : 16) * m_nLength,
		9
	};
	return s;
}
