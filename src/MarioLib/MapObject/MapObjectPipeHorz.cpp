#include "MapObjectPipeHorz.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"
#include "TypeNames.h"
#include "RenderManager.h"
#include "MapObjectPipeExit.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectPipeHorz)
	PROP_INT("PipeType", VT_I4, PIPE_NONE, g_szPipeType, "PipeHorz"),
	PROP_STR("StageName", VT_ETC_NASTR, L"", 0, "PipeHorz"),
	PROP_INT("ExitIndex", VT_I4, -1, 0, "PipeHorz"),
	PROP_BOOL("LeftToRight", VT_BOOL, true, 0, "PipeHorz"),
	PROP_INT("UseAsExitIndex", VT_I4, -1, 0, "PipeHorz"),
END_IMPL_PROPERTYMAP()

MapObjectPipeHorz::MapObjectPipeHorz(Game *pGame, Stage *pStage) :
	MapObjectPipe(pGame, pStage)
{
	m_nType = MAPOBJECT_PIPEHORZ;

	m_nPipeType = PIPE_NONE;
	m_strStageName = L"";
	m_nExitIndex = -1;
	m_nUseAsExitIndex = -1;

	m_nDirection = PIPEENTRANCE_LEFTTORIGHT;
}

MapObjectPipeHorz::~MapObjectPipeHorz()
{
}

bool MapObjectPipeHorz::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		CalcSize();

		if (m_bLeftToRight)
		{
			m_pStage->SetTileData(m_nTX, m_nTY + 0, TILE_HPIPEHEAD_T);
			m_pStage->SetTileData(m_nTX, m_nTY + 1, TILE_HPIPEHEAD_B);
		}
		else
		{
			m_pStage->SetTileData(m_nTX, m_nTY + 0, TILE_HPIPERIGHT_T);
			m_pStage->SetTileData(m_nTX, m_nTY + 1, TILE_HPIPERIGHT_B);
		}
		return false;
	}
	else if (nPhase == 2)
	{
		// Create Exit Info
		if (m_nUseAsExitIndex > -1)
		{
			PipeExitInfo *pInfo = new PipeExitInfo;
			pInfo->m_nIndex = m_nUseAsExitIndex;
			pInfo->m_nType = GetPipeDir(this);
			pInfo->m_rcExit = GetEntranceRect();

			m_pStage->m_vecPipeExitInfo.push_back(pInfo);
		}
	}
	else if (nPhase == 3)
	{
		// Body
		if (m_bLeftToRight)
		{
			int i = m_nTX + 1;
			for (i = m_nTX + 1; i < m_nTX + GameDefaults::nPageTileWidth; i++)
			{
				if (m_pStage->GetTileData(i, m_nTY + 0) != TILE_EMPTY ||
					m_pStage->GetTileData(i, m_nTY + 1) != TILE_EMPTY)
					break;

				m_pStage->SetTileData(i, m_nTY + 0, TILE_HPIPEBODY_T);
				m_pStage->SetTileData(i, m_nTY + 1, TILE_HPIPEBODY_B);
			}

			// Junction
			if (m_pStage->GetTileData(i, m_nTY + 0) == TILE_PIPEBODY_L &&
				m_pStage->GetTileData(i, m_nTY + 1) == TILE_PIPEBODY_L)
			{
				m_pStage->SetTileData(i, m_nTY + 0, TILE_PIPEJUNCTION_T);
				m_pStage->SetTileData(i, m_nTY + 1, TILE_PIPEJUNCTION_B);
			}
		}
		else
		{
			int i = m_nTX - 1;
			for (i = m_nTX - 1; i >= m_nTX - GameDefaults::nPageTileWidth; i--)
			{
				if (m_pStage->GetTileData(i, m_nTY + 0) != TILE_EMPTY ||
					m_pStage->GetTileData(i, m_nTY + 1) != TILE_EMPTY)
					break;

				m_pStage->SetTileData(i, m_nTY + 0, TILE_HPIPEBODY_T);
				m_pStage->SetTileData(i, m_nTY + 1, TILE_HPIPEBODY_B);
			}

			/*
			// Junction
			if (m_pStage->GetTileData(i, m_nTY + 0) == TILE_PIPEBODY_L &&
				m_pStage->GetTileData(i, m_nTY + 1) == TILE_PIPEBODY_L)
			{
				m_pStage->SetTileData(i, m_nTY + 0, TILE_PIPEJUNCTION_T);
				m_pStage->SetTileData(i, m_nTY + 1, TILE_PIPEJUNCTION_B);
			}
			*/
		}
		
		// Event
		if (m_nPipeType != PIPE_NONE)
		{
			// New Code
			PipeInfo *pInfo = new PipeInfo;
			pInfo->m_rcEnterance = GetEntranceRect();
			pInfo->m_strStageName = m_strStageName;
			pInfo->m_nExitIndex = m_nExitIndex;
			if (m_bLeftToRight)
				pInfo->m_nType = PIPEENTRANCE_LEFTTORIGHT;
			else
				pInfo->m_nType = PIPEENTRANCE_RIGHTTOLEFT;
			pInfo->m_nWarpType = m_nPipeType;

			m_pStage->m_vecPipeInfo.push_back(pInfo);
		}

		return true;
	}

	return false;
}

void MapObjectPipeHorz::CalcSize()
{
	m_sizeHead.cx = 1;
	m_sizeHead.cy = 2;

	m_sizeBody.cx = 0;
	m_sizeBody.cy = 2;

	if (m_bLeftToRight)
	{
		for (int i = m_nTX + 1; i < m_nTX + GameDefaults::nPageTileWidth; i++)
		{
			if (m_pStage->GetTileData(i, m_nTY + 0) != TILE_EMPTY ||
				m_pStage->GetTileData(i, m_nTY + 1) != TILE_EMPTY)
				break;
			m_sizeBody.cx++;
		}
	}
	else
	{
		for (int i = m_nTX - 1; i >= m_nTX - GameDefaults::nPageTileWidth; i--)
		{
			if (m_pStage->GetTileData(i, m_nTY + 0) != TILE_EMPTY ||
				m_pStage->GetTileData(i, m_nTY + 1) != TILE_EMPTY)
				break;
			m_sizeBody.cx++;
		}
	}
}

SIZE MapObjectPipeHorz::GetSize()
{
	CalcSize();

	SIZE s;
	s.cx = m_sizeHead.cx + m_sizeBody.cx;
	s.cy = 2;

	return s;
}

RECT MapObjectPipeHorz::GetRect()
{
	SIZE s = GetSize();
	if (m_bLeftToRight)
	{
		NaRect rc(
			m_nTX * TILE_XS,
			m_nTY * TILE_YS,
			(m_nTX + s.cx) * TILE_XS,
			(m_nTY + s.cy) * TILE_YS
		);
		return rc;
	}
	else
	{
		NaRect rc(
			(m_nTX - s.cx + 1) * TILE_XS,
			m_nTY * TILE_YS,
			(m_nTX + 1) * TILE_XS,
			(m_nTY + s.cy) * TILE_YS
		);
		return rc;
	}
}

void MapObjectPipeHorz::Render(int nColor, int nZOrder)
{
	if (m_pSpriteDesign == nullptr)
		m_pSpriteDesign = SPR_MANAGER->Find(L"Design");

	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	if (m_bLeftToRight)
	{
		// Head
		m_pStage->RenderTile(x, y + (TILE_YS * 0), TILE_HPIPEHEAD_T, nColor, nZOrder);
		m_pStage->RenderTile(x, y + (TILE_YS * 1), TILE_HPIPEHEAD_B, nColor, nZOrder);

		// Body
		for (int i = 1; i < s.cx; i++)
		{
			tx = (x + i) / TILE_XS;
			if (m_pStage->GetTileData(tx, ty + 0) != TILE_EMPTY ||
				m_pStage->GetTileData(tx, ty + 1) != TILE_EMPTY)
				break;

			m_pStage->RenderTile(x + (i * TILE_XS), y + (TILE_XS * 0), TILE_HPIPEBODY_T, nColor, nZOrder);
			m_pStage->RenderTile(x + (i * TILE_XS), y + (TILE_XS * 1), TILE_HPIPEBODY_B, nColor, nZOrder);
		}

		if (m_nPipeType != PIPE_NONE)
		{
			NaRect rc = GetRect();
			m_pSpriteDesign->RenderToQueue(
				x - CAMERA->m_ptPos.x,
				y + rc.Height() / 2 - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_RIGHT,
				false, false, -1L, 1.0f, 1.0f,
				Z_MAPOBJECT);

			NaString str;
			str.Format(L"%ls", m_strStageName.wstr());
			DRAWTEXT(
				x - CAMERA->m_ptPos.x - 16,
				y - CAMERA->m_ptPos.y,
				str
			)
		}

		if (m_nUseAsExitIndex != -1)
		{
			int nOffSetX = 0;
			if (m_nPipeType != PIPE_NONE)
				nOffSetX = -4;

			NaRect rc = GetRect();
			m_pSpriteDesign->RenderToQueue(
				x - CAMERA->m_ptPos.x - 12 + nOffSetX,
				y + rc.Height() / 2 - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_LEFT,
				false, false, nColor, 1.0f, 1.0f, Z_MAPOBJECT);

			NaString str;
			str.Format(L"%d", m_nUseAsExitIndex);
			m_pGame->m_pRenderManager->Text(
				x + TILE_XS - CAMERA->m_ptPos.x - 12,
				y - CAMERA->m_ptPos.y + 12,
				str,
				nColor & 0xa0ff0000
			);
		}
	}
	else
	{
		// Head
		m_pStage->RenderTile(x, y + (TILE_YS * 0), TILE_HPIPERIGHT_T, nColor, nZOrder);
		m_pStage->RenderTile(x, y + (TILE_YS * 1), TILE_HPIPERIGHT_B, nColor, nZOrder);

		// Body
		for (int i = 1; i < s.cx; i++)
		{
			tx = (x - i) / TILE_XS;
			if (m_pStage->GetTileData(tx, ty + 0) != TILE_EMPTY ||
				m_pStage->GetTileData(tx, ty + 1) != TILE_EMPTY)
				break;

			m_pStage->RenderTile(x - (i * TILE_XS), y + (TILE_XS * 0), TILE_HPIPEBODY_T, nColor, nZOrder);
			m_pStage->RenderTile(x - (i * TILE_XS), y + (TILE_XS * 1), TILE_HPIPEBODY_B, nColor, nZOrder);
		}

		if (m_nPipeType != PIPE_NONE)
		{
			NaRect rc = GetRect();
			m_pSpriteDesign->RenderToQueue(
				x - CAMERA->m_ptPos.x + 16,
				y + rc.Height() / 2 - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_RIGHT,
				true, false, -1L, 1.0f, 1.0f,
				Z_MAPOBJECT);

			NaString str;
			str.Format(L"%ls", m_strStageName.wstr());
			DRAWTEXT(
				x - CAMERA->m_ptPos.x + 16,
				y - CAMERA->m_ptPos.y,
				str
			)
		}

		if (m_nUseAsExitIndex != -1)
		{
			int nOffSetX = 0;
			if (m_nPipeType != PIPE_NONE)
				nOffSetX = 4;

			NaRect rc = GetRect();
			m_pSpriteDesign->RenderToQueue(
				x - CAMERA->m_ptPos.x + 28 + nOffSetX,
				y + rc.Height() / 2 - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_RIGHT,
				false, true, -1L, 1.0f, 1.0f, Z_MAPOBJECT);

			NaString str;
			str.Format(L"%d", m_nUseAsExitIndex);
			m_pGame->m_pRenderManager->Text(
				x + TILE_XS - CAMERA->m_ptPos.x - 12,
				y - CAMERA->m_ptPos.y + 12,
				str,
				nColor & 0xa0ff0000
			);
		}
	}

	/*
	NaRect rcMe = GetRect();

	std::vector<MapObjectBase*>::iterator it = m_pStage->m_vecMapObject.begin();
	for (; it != m_pStage->m_vecMapObject.end(); ++it)
	{
		if (this == *it)
			continue;
		if (//(*it)->m_nType == MAPOBJECT_PIPE ||
			(*it)->m_nType == MAPOBJECT_PIPEBODY)
		{
			NaRect rcOther = (*it)->GetRect();
			if (rcMe.IsOverlapped(rcOther))
			{
				//m_pStage->RenderTile(i, 
			}
		}
	}
	*/
}
