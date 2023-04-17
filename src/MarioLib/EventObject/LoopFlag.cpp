#include "LoopFlag.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"

LoopFlag::LoopFlag(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_LOOPFLAG;
	m_nState = STATE_IDLE;

	m_nSleep = 0;
	m_pHandler = nullptr;
}

LoopFlag::~LoopFlag()
{
}

void LoopFlag::OnBeforeProcess()
{
	//CheckCollision();
	CheckCollisionPlayer();
}

void LoopFlag::OnAfterProcess()
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

void LoopFlag::Process()
{
	switch (m_nState)
	{
	case STATE_IDLE:
		OnIdle();
		break;
	}

	if (m_bCollisionPlayer && m_nSleep == 0)
	{
		if (m_pHandler)
		{
			m_pHandler->SetCheck(this);
		}
		m_nSleep = 36;
	}

	if (m_nSleep > 0)
		m_nSleep--;
}

void LoopFlag::RenderDesign(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	SIZE s = GetSize();

	Sprite *pSprite = SPR_MANAGER->Find(L"Design");
	pSprite->RenderToQueue(
		_x, _y, SPRIDX_DESIGN_LOOPHANDLER, false, false, nColor - 0x70000000,
		1.0f, 1.0f, Z_COMMON_OBJ
	);

	NaString str;
	str.Format(L"Loop\nImmune\nFlag\n");
	RENDER_MANAGER->Text(_x - (s.cx / 2), _y - (s.cy), str, nColor);
}

void LoopFlag::OnIdle()
{
	// Finding Nearest LoopHandler
	if (m_nStateFrame == 1)
	{
		BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEventObject) {
			if (pObj->m_nType == EVENTOBJECT_LOOPHANDLER)
			{
				if (m_fX > pObj->m_fX)
					continue;
				
				LoopHandler *pHandler = (LoopHandler*)pObj;
				if (pHandler->m_nImmunityFlag == 0)
					continue;

				if (m_pHandler == nullptr)
					m_pHandler = pHandler;
				else if (m_pHandler->m_fX > pObj->m_fX)
					m_pHandler = pHandler;
			}
		} END_VECTOR_LOOP();
	}
}

SIZE LoopFlag::GetSize()
{
	SIZE s = 
	{ 
		3 * TILE_XS,
		3 * TILE_YS 
	};
	return s;
}
