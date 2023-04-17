#include "Trampolin.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"

#include "UIBase.h"

Trampolin::Trampolin(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_TRAMPOLIN;
	m_nState = STATE_IDLE;

	m_bCanLand = true;
	m_bCanCollide = true;
}

Trampolin::~Trampolin()
{
	m_vecPassenger.clear();
}

void Trampolin::OnBeforeProcess()
{
	//CheckCollision();
	//CheckCollisionPlayer();
}

void Trampolin::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

    ProcessState();

	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecPassenger) {
		if (pObj->m_nState == STATE_DEAD ||
			pObj->m_nNextState == STATE_DEAD ||
			!pObj->IsAlive() ||
			pObj->m_nState == STATE_CARRYBYHAND)
		{
			it = m_vecPassenger.erase(it);
		}
		else
			++it;
	} END_VECTOR_LOOP();
}

void Trampolin::Render(int nColor, int nZOrder)
{
	RenderTrampolin(m_fX, m_fY, nColor);

	/*
	{
		POINT ptCamera = CAMERA->GetPosition();
		UIBase u(m_pGame);
		NaRect rc = GetRect();
		rc.Offset(-ptCamera.x, -ptCamera.y);

		u.RenderBox(rc, 0x900000ff);
	}
	//*/
}

void Trampolin::RenderDesign(int nColor, int nZOrder)
{
	RenderTrampolin(m_fX, m_fY, nColor);
}

void Trampolin::RenderTrampolin(float x, float y, int nColor)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;

	int nFrame = GetSpriteIndex();
	(*m_ppSprite)->RenderToQueue(_x, _y, nFrame,
		false, false, nColor, 1.0f, 1.0f, Z_COMMON_OBJ);

	/*
	NaString str;
	str.Format(L"P:%d\nS:%d", m_vecPassenger.size(), m_nStateFrame);
	DRAWTEXT(_x + 8, _y - 16, str);
	//*/
}

void Trampolin::ChangeState(int nState, ChangeStateInfo * pInfo)
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
				pObj->m_fYS = 0;
				pObj->m_fY -= 6.0f;
			} END_VECTOR_LOOP();
		}
		break;
	}

	m_nNextState = nState;
}

void Trampolin::OnIdle()
{
	if (m_vecPassenger.size() > 0)
	{
		ChangeState(STATE_PRESSED);
	}
}

void Trampolin::OnPressed()
{
	if (m_nStateFrame <= 7)
	{
		// Hold Passengers
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
			if (pObj->IsAlive() &&
				(pObj->m_nState != STATE_TRAMPOLINJUMP && m_fYS >= 0) &&
				pObj->m_nState != STATE_TRAMPOLINPRESS)
				pObj->ChangeState(STATE_TRAMPOLINPRESS);
		} END_VECTOR_LOOP();

		MovePassengers();
	}

	if (m_nStateFrame == 7)
	{
		ChangeState(STATE_SPRING);
	}
}

void Trampolin::OnSpring()
{
	if (m_nStateFrame == 3)
	{
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
			pObj->m_fY -= 8.0f;
		} END_VECTOR_LOOP();
	}

	if (m_nStateFrame == 7)
	{
		PushUpPassengers();

		ChangeState(STATE_IDLE);
	}
}

bool Trampolin::IsAlive()
{
	return true;
}

void Trampolin::AddPassenger(GameObjectBase * pNewObj)
{
	if (pNewObj->m_pVehicle)
		return;


	switch (pNewObj->m_nState)
	{
	case STATE_CARRYBYHAND:
		return;
		break;
	}

	// Search
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
		if (pNewObj == pObj)
			return;
	} END_VECTOR_LOOP();

	m_vecPassenger.push_back(pNewObj);
}

void Trampolin::MovePassengers()
{
	NaRect rc = GetRect();

	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
		if (m_nState == STATE_PRESSED)
			pObj->m_fY = rc.top + 2;
	} END_VECTOR_LOOP();
}

void Trampolin::PushUpPassengers()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecPassenger) {
		if (pObj->IsAlive() &&
			pObj->m_nState != STATE_CARRYBYHAND)
			pObj->ChangeState(STATE_TRAMPOLINJUMP);
	} END_VECTOR_LOOP();

	m_vecPassenger.clear();
}

int Trampolin::GetSpriteIndex()
{
	int nFrame = SPRIDX_MAPOBJ_TRAMPOLIN1;
	switch (m_nState)
	{
	case STATE_PRESSED:
		if (m_nStateFrame < 4)
			nFrame = SPRIDX_MAPOBJ_TRAMPOLIN2;
		else if (m_nStateFrame < 8)
			nFrame = SPRIDX_MAPOBJ_TRAMPOLIN3;
		break;
	case STATE_SPRING:
		if (m_nStateFrame < 4)
			nFrame = SPRIDX_MAPOBJ_TRAMPOLIN2;
		break;
	}

	return nFrame;
}

SIZE Trampolin::GetSize()
{
	SIZE s = {
		16,
		32
	};

	switch (m_nState)
	{
	case STATE_PRESSED:
		if (m_nStateFrame < 8)
			s.cy = 32 - (m_nStateFrame*2);
		break;
	case STATE_SPRING:
		if (m_nStateFrame < 4)
			s.cy = 24;
		break;
	}

	return s;
}
