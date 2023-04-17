#include "WarpZoneFlag.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "WarpZoneText.h"
#include "PipeInfo.h"

WarpZoneFlag::WarpZoneFlag(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_WARPZONEFLAG;
	m_nState = STATE_IDLE;
}

WarpZoneFlag::~WarpZoneFlag()
{
}

void WarpZoneFlag::CheckCollisionPlayer()
{
	
}

void WarpZoneFlag::Process()
{
	if (m_nState == STATE_DEAD)
		return;

	POINT pt = CAMERA->m_ptPos;

	int nMyPage = m_fX / GameDefaults::nPageWidth;
	int nCameraPage = (pt.x + (m_pGame->Width() - GameDefaults::nPageWidth)) / GameDefaults::nPageWidth;

	if (nCameraPage >= nMyPage)
	{
		int _x = (nMyPage * GameDefaults::nPageWidth) + 40;
		int _y = 112;

		WarpZoneText *pText = (WarpZoneText*)CUR_STAGE->ReserveCreateGameObject(_x, _y, EVENTOBJECT_WARPZONETEXT);
		pText->m_strText = L"WELCOME TO WARP ZONE!";

		BEGIN_VECTOR_LOOP(PipeInfo*, CUR_STAGE->m_vecPipeInfo) {
			if (pObj->m_nWarpType == PIPE_WARPZONE)
			{
				NaString strWorld = L"";
				int nIdx = pObj->m_strStageName.Find(L"-");
				if (nIdx != -1)
					strWorld = pObj->m_strStageName.Left(nIdx);

				_x = pObj->m_rcEnterance.left + 16;
				_y = pObj->m_rcEnterance.top - 8;

				WarpZoneText *pText = (WarpZoneText*)CUR_STAGE->ReserveCreateGameObject(_x, _y, EVENTOBJECT_WARPZONETEXT);
				pText->m_strText = strWorld;
			}
		} END_VECTOR_LOOP();

		// Clear Enemies
		m_pGame->m_pGameState->ClearEnemyInViewport();

		ChangeState(STATE_DEAD);
	}
}

void WarpZoneFlag::OnBeforeProcess()
{
}

void WarpZoneFlag::Render(int nColor, int nZOrder)
{
	
}

void WarpZoneFlag::RenderDesign(int nColor, int nZOrder)
{
	POINT pt = CAMERA->m_ptPos;
	int _x = x - pt.x;
	int _y = y - pt.y;

	Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
	pSpriteDesign->RenderToQueue(_x, _y, SPRIDX_DESIGN_BLUEBOX,
		false, false, nColor, 1.0f, 1.0f, Z_TEXT);
	
	_x = _x - 8;
	_y = _y - 16;
	NaString str;
	str.Format(L"WarpZonePage");
	RENDER_MANAGER->Text(_x, _y, str, nColor);
}

SIZE WarpZoneFlag::GetSize()
{
	SIZE s = { 16, 16 };
	return s;
}
