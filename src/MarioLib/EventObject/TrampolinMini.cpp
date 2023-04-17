#include "TrampolinMini.h"

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

// Property Interface
BEGIN_IMPL_PROPERTYMAP(TrampolinMini)
	PROP_BOOL("IsHorizontal", VT_BOOL, false, 0, "TrampolinMini"),
END_IMPL_PROPERTYMAP()

TrampolinMini::TrampolinMini(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_TRAMPOLINMINI;
	m_nState = STATE_IDLE;

	m_bCanLand = true;
	m_bCanCollide = true;

	m_bIsHorizontal = false;
	m_nSpringFrame = -1;
}

TrampolinMini::~TrampolinMini()
{
	m_vecPassenger.clear();
}

void TrampolinMini::CheckCollisionTrampolinH()
{
	NaRect rcMe = GetRect();
	rcMe.top += 4;

	m_vecWillPush.clear();

	std::vector<GameObjectBase*> vecAllGameObject;
	PASTE_VECTOR(vecAllGameObject, CUR_STAGE->m_vecItem);
	PASTE_VECTOR(vecAllGameObject, CUR_STAGE->m_vecEnemy);

	vecAllGameObject.push_back(CUR_PLAYER->GetLastControlObject());

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
		if (!pObj->CanBounceTrampolinMiniHorz())
			continue;


		if (rcMe.IsOverlapped(pObj->GetRect()))
		{
			pObj->m_fXS /= 2.0f;
			m_vecWillPush.push_back(pObj);
		}
	}
}

void TrampolinMini::Process()
{
	ProcessBase();

	CheckSpring();
	if (m_nSpringFrame != -1)
		ProcessSpring();
}

void TrampolinMini::OnBeforeProcess()
{
	CheckCollision();

	if (m_bIsHorizontal)
		CheckCollisionTrampolinH();
}

void TrampolinMini::OnAfterProcess()
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

	auto rc = GetRect();
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecPassenger) {
		auto rcObj = pObj->GetRect();
		if (pObj->m_nState == STATE_DEAD ||
			pObj->m_nNextState == STATE_DEAD ||
			!pObj->IsAlive() ||
			rc.left > rcObj.right + 4 ||
			rc.right < rcObj.left - 4 ||
			pObj->m_nState == STATE_CARRYBYHAND)
		{
			it = m_vecPassenger.erase(it);
		}
		else
		{
			pObj->m_fX += m_fXS;
			pObj->m_fY += m_fYS;

			++it;
		}
	} END_VECTOR_LOOP();
}

void TrampolinMini::Render(int nColor, int nZOrder)
{
	RenderTrampolinMini(m_fX, m_fY, nColor);
}

void TrampolinMini::RenderDesign(int nColor, int nZOrder)
{
	RenderTrampolinMini(m_fX, m_fY, nColor);
}

void TrampolinMini::RenderTrampolinMini(float x, float y, int nColor)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	int nFrame = GetSpriteIndex();
	bool bShadow = m_pGame->m_bModernStyle;

	int nZOrder = GetZOrder();
	if (nZOrder == 0)
		nZOrder = Z_COMMON_OBJ;

	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		false, false, nColor, 1.0f, 1.0f, nZOrder, bShadow);

	/*
	if (!m_bIsHorizontal)
	{
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

void TrampolinMini::ChangeState(int nState, ChangeStateInfo * pInfo)
{
	switch (nState)
	{
	case STATE_PRESSED:
		{
			// Hold Passengers
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
				if (pObj->IsAlive())
					pObj->ChangeState(STATE_TRAMPOLINPRESS);
			} END_VECTOR_LOOP();
		}
		break;
	case STATE_SPRING:
		{
			// Jump Passengers
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
				if (!m_bIsHorizontal)
				{
					pObj->m_fYS = 0;
					pObj->m_fY -= 6.0f;
				}
			} END_VECTOR_LOOP();
		}
		break;
	default:
		EventObjectBase::ChangeState(nState, pInfo);
		return;
		break;
	}

	m_nNextState = nState;

	if (pInfo)
	{
		m_ChangeStateInfo = *pInfo;
		m_ChangeStateInfo.nNextState = nState;
	}
	else
	{
		m_ChangeStateInfo.nNextState = -1;
	}
}

void TrampolinMini::OnIdle()
{
	CheckFalling();
}

void TrampolinMini::OnReachWall(bool bIsLeftWall)
{
	SIZE s = GetSize();

	if (bIsLeftWall)
		m_fX = (m_ptCollision[COLLISION_LEFT].x + 1) + s.cx / 2 + 1;
	else
		m_fX = (m_ptCollision[COLLISION_RIGHT].x) - s.cx / 2;

	m_fXS *= -0.5f;
}

void TrampolinMini::OnLanding()
{
	m_fY = m_ptCollision[COLLISION_BOTTOM].y;

	if (abs(m_fYS) > 1.0f)
	{
		m_fXS *= 0.7f;
		m_fYS = -0.5f * m_fYS;
	}
	else
	{
		m_fXS = 0;
		m_fYS = 0;
	}

	if (m_bStandOnJumpBlock)
		ChangeState(STATE_BLOCKJUMP);
	else
		ChangeState(STATE_STAND);
}

void TrampolinMini::CheckSpring()
{
	if (m_nSpringFrame != -1)
		return;

	if (!m_bIsHorizontal)
	{
		if (m_vecPassenger.size() > 0)
			m_nSpringFrame = 0;
	}
	else
	{
		m_bCanCollide = false;

		if (m_vecWillPush.size() > 0)
			m_nSpringFrame = 0;
	}
}

void TrampolinMini::ProcessSpring()
{
	if (m_nSpringFrame == -1)
		return;

	if (!m_bIsHorizontal)
	{
		auto rc = GetRect();
		if (m_nSpringFrame <= 4)
		{
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
				if (pObj->IsAlive() &&
					(pObj->m_nState != STATE_TRAMPOLINJUMP && m_fYS >= 0) &&
					pObj->m_nState != STATE_TRAMPOLINPRESS &&
					pObj->m_nState != STATE_CARRYBYHAND)
				{
					auto rcObj = pObj->GetRect();
					if (rc.left > rcObj.right + 4)
						continue;

					if (rc.right < rcObj.left - 4)
						continue;

					pObj->ChangeState(STATE_TRAMPOLINPRESS);
				}
			} END_VECTOR_LOOP();

			MovePassengers();
		}

		if (m_nSpringFrame == 6)
		{
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
				pObj->m_fY -= 8.0f;
			} END_VECTOR_LOOP();
		}

		if (m_nSpringFrame == 7)
		{
			PushUpPassengers();
			m_nSpringFrame = -1;
			return;
		}
	}
	else
	{
		if (m_nSpringFrame == 3)
		{
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecWillPush) {
				pObj->OnBounceTrampolinMiniHorz(this);
			} END_VECTOR_LOOP();

			if (m_vecWillPush.size() > 0)
			{
				SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
			}
		}

		if (m_nSpringFrame == 7)
		{
			m_nSpringFrame = -1;
			return;
		}
	}

	m_nSpringFrame++;
}

bool TrampolinMini::IsAlive()
{
	return true;
}

bool TrampolinMini::CanBornFromBlock()
{
	return true;
}

bool TrampolinMini::CanCarried()
{
	if (m_bIsHorizontal)
		return false;

	if (m_nSpringFrame != -1)
		return false;

	switch (m_nState)
	{
	case STATE_ITEMPOP:
	case STATE_ITEMPOPDOWN:
		return false;
		break;
	}
	
	return true;
}

void TrampolinMini::AddPassenger(GameObjectBase * pNewObj)
{
	if (pNewObj->m_pVehicle)
		return;

	switch (pNewObj->m_nState)
	{
	case STATE_CARRYBYHAND:
	case STATE_RIDE:
		return;
		break;
	}

	if (pNewObj->m_fYS < 0 &&
		pNewObj->m_nState == STATE_TRAMPOLINJUMP &&
		pNewObj->m_nStateFrame <= 10)
		return;

	// Search
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
		if (pNewObj == pObj)
			return;
	} END_VECTOR_LOOP();

	m_vecPassenger.push_back(pNewObj);
}

void TrampolinMini::MovePassengers()
{
	NaRect rc = GetRect();

	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
		pObj->m_fY = rc.top + 2;
	} END_VECTOR_LOOP();
}

void TrampolinMini::PushUpPassengers()
{
	bool bPushed = false;
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
		if (pObj->IsAlive())
		{
			pObj->ChangeState(STATE_TRAMPOLINJUMP);
			bPushed = true;
		}
	} END_VECTOR_LOOP();

	m_vecPassenger.clear();

	if (bPushed)
	{
		SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
	}
}

int TrampolinMini::GetSpriteIndex()
{
	if (EventObjectBase::GetSpriteIndex() == -1)
		return -1;

	int nFrame;

	if (!m_bIsHorizontal)
	{
		nFrame = SPRIDX_MAPOBJ_TRAMPOLIN_MINI1;

		if (m_nSpringFrame != -1)
		{
			int nFrames[] = { 1, 1, 2, 2, 1, 1, 1, 1 };

			nFrame = SPRIDX_MAPOBJ_TRAMPOLIN_MINI1 + nFrames[m_nSpringFrame];
		}
	}
	else
	{
		nFrame = SPRIDX_MAPOBJ_TRAMPOLIN_H_MINI1;
		
		if (m_nSpringFrame != -1)
		{
			int nFrames[] = { 1, 2, 1, 0, 3, 0, 1, 0  };

			nFrame = SPRIDX_MAPOBJ_TRAMPOLIN_H_MINI1 + nFrames[m_nSpringFrame];
		}
	}

	return nFrame;
}

SIZE TrampolinMini::GetSize()
{
	SIZE s = { 16, 16 };

	switch (m_nState)
	{
	case STATE_PRESSED:
		if (m_nStateFrame < 8)
		{
			if (m_bIsHorizontal)
				s.cx = 16 - (m_nStateFrame);
			else
				s.cy = 16 - (m_nStateFrame);
		}
		break;
	case STATE_SPRING:
		if (m_nStateFrame < 4)
		{
			if (!m_bIsHorizontal)
				s.cy = 8;
		}
		break;
	}

	return s;
}
