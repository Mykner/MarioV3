#include "LoopHandler.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(LoopHandler)
	PROP_INT("NextPage", VT_I4,	-1,	0, "LoopHandler"),
	PROP_INT("ImmunityFlagCount", VT_I4, -1, 0, "LoopHandler"),
END_IMPL_PROPERTYMAP()

LoopHandler::LoopHandler(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_LOOPHANDLER;
	m_nState = STATE_IDLE;

	m_nNextPage = -1;
	m_nImmunityFlag = 0;
	m_nSleep = 0;
}

LoopHandler::~LoopHandler()
{
}

void LoopHandler::OnBeforeProcess()
{
	//CheckCollision();
	CheckCollisionPlayer();
}

void LoopHandler::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

    ProcessState();
}

void LoopHandler::Process()
{
	if (m_bCollisionPlayer && m_nSleep == 0)
	{
		if (m_nNextPage != -1)
		{
			if ((m_nImmunityFlag > 0 && m_vecCheckedFlag.size() < m_nImmunityFlag) ||
				m_nImmunityFlag == 0)
			{
				NaDebugOut(L"LoopHandler(0x%08x): Player Checked\n", this);
				NaDebugOut(L"- ImmunityFlag: %d\n", m_nImmunityFlag);
				NaDebugOut(L"- Checked Flag: %d\n", m_vecCheckedFlag.size());

				CUR_STAGE->SetPageJump(m_fX / GameDefaults::nPageWidth, m_nNextPage);
				m_nSleep = 36;

				m_vecCheckedFlag.clear();
			}
			else
			{
				// Immune!
				m_nSleep = 36;
			}
		}
	}

	if (m_nSleep > 0)
		m_nSleep--;
}

void LoopHandler::RenderDesign(int nColor, int nZOrder)
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
	if (m_nNextPage == -1)
		str.Format(L"Loop\nRegion\n");
	else
		str.Format(L"Loop\nRegion\n\n\nTo\nPage%02d", m_nNextPage);
	RENDER_MANAGER->Text(_x - (s.cx / 2), _y - (s.cy), str, nColor);
}

SIZE LoopHandler::GetSize()
{
	SIZE s = 
	{ 
		3 * TILE_XS,
		3 * TILE_YS 
	};
	return s;
}

void LoopHandler::SetCheck(GameObjectBase * pLoopFlag)
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecCheckedFlag) {
		if (pObj == pLoopFlag)
			return;
	} END_VECTOR_LOOP();
		 
	m_vecCheckedFlag.push_back(pLoopFlag);
	NaDebugOut(L"LoopHandler(0x%08x): Flag(0x%08x) Checked.\n", this, pLoopFlag);
}
