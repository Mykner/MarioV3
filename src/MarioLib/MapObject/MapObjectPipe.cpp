#include "MapObjectPipe.h"

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
BEGIN_IMPL_PROPERTYMAP(MapObjectPipe)
	PROP_INT("PipeType",	VT_I4,			PIPE_NONE,				g_szPipeType,	"Pipe"),
	PROP_STR("StageName",	VT_ETC_NASTR,	L"",					0,				"Pipe"),
	PROP_INT("ExitIndex",	VT_I4,			-1,						0,				"Pipe"),
	PROP_BOOL("UpToDown",	VT_BOOL,		true,					0,				"Pipe"),
	PROP_INT("UseAsExitIndex",	VT_I4,		-1,						0,				"Pipe"),
	PROP_INT("Direction",	VT_I4,			PIPEENTRANCE_UPTODOWN,	g_szPipeDir,	"Pipe"),
END_IMPL_PROPERTYMAP()

MapObjectPipe::MapObjectPipe(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_PIPE;

	m_nPipeType = PIPE_NONE;
	m_strStageName = L"";
	m_nExitIndex = -1;
	m_nUseAsExitIndex = -1;
	m_nDirection = PIPEENTRANCE_UPTODOWN;

	// Deprecated
	m_bUpToDown = true;
}

MapObjectPipe::~MapObjectPipe()
{
}

bool MapObjectPipe::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		CalcSize();
	}

	int nDir = GetPipeDir(this);
	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
		TileizeUpToDown(nPhase);
		break;
	case PIPEENTRANCE_DOWNTOUP:
		TileizeDownToUp(nPhase);
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		TileizeLeftToRight(nPhase);
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		TileizeRightToLeft(nPhase);
		break;
	}

	if (nPhase == 2)
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
		return true;
	}

	return false;
}

void MapObjectPipe::TileizeUpToDown(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		m_pStage->SetTileData(m_nTX + 0, m_nTY, TILE_PIPEHEAD_L);
		m_pStage->SetTileData(m_nTX + 1, m_nTY, TILE_PIPEHEAD_R);

		// Body
		for (int i = m_nTY + 1; i < m_nTY + GameDefaults::nPageTileHeight; i++)
		{
			if (!IsEmptyTile(m_nTX + 0, i) || !IsEmptyTile(m_nTX + 1, i))
				break;

			m_pStage->SetTileData(m_nTX + 0, i, TILE_PIPEBODY_L);
			m_pStage->SetTileData(m_nTX + 1, i, TILE_PIPEBODY_R);
		}
	}
	else if (nPhase == 3)
	{
		// Event
		if (m_nPipeType != PIPE_NONE)
		{
			PipeInfo *pInfo = new PipeInfo;
			pInfo->m_rcEnterance = GetEntranceRect();
			pInfo->m_strStageName = m_strStageName;
			pInfo->m_nExitIndex = m_nExitIndex;
			pInfo->m_nType = PIPEENTRANCE_UPTODOWN;
			pInfo->m_nWarpType = m_nPipeType;

			m_pStage->m_vecPipeInfo.push_back(pInfo);
		}
	}
}

void MapObjectPipe::TileizeDownToUp(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		m_pStage->SetTileData(m_nTX + 0, m_nTY, TILE_PIPEBOTTOM_L);
		m_pStage->SetTileData(m_nTX + 1, m_nTY, TILE_PIPEBOTTOM_R);

		// Body
		for (int i = m_nTY - 1; i >= 0; i--)
		{
			if (!IsEmptyTile(m_nTX + 0, i) || !IsEmptyTile(m_nTX + 1, i))
				break;

			m_pStage->SetTileData(m_nTX + 0, i, TILE_PIPEBODY_L);
			m_pStage->SetTileData(m_nTX + 1, i, TILE_PIPEBODY_R);
		}
	}
	else if (nPhase == 3)
	{
		// Event
		if (m_nPipeType != PIPE_NONE)
		{
			PipeInfo *pInfo = new PipeInfo;
			pInfo->m_rcEnterance = GetEntranceRect();
			pInfo->m_strStageName = m_strStageName;
			pInfo->m_nExitIndex = m_nExitIndex;
			pInfo->m_nType = PIPEENTRANCE_DOWNTOUP;
			pInfo->m_nWarpType = m_nPipeType;

			m_pStage->m_vecPipeInfo.push_back(pInfo);
		}
	}
}

void MapObjectPipe::TileizeLeftToRight(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		m_pStage->SetTileData(m_nTX, m_nTY + 0, TILE_HPIPEHEAD_T);
		m_pStage->SetTileData(m_nTX, m_nTY + 1, TILE_HPIPEHEAD_B);
	}
	else if (nPhase == 3)
	{
		int i = m_nTX + 1;
		for (i = m_nTX + 1; i < m_nTX + GameDefaults::nPageTileWidth; i++)
		{
			if (!IsEmptyTile(i, m_nTY + 0) || !IsEmptyTile(i, m_nTY + 1))
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

		// Event
		if (m_nPipeType != PIPE_NONE)
		{
			// New Code
			PipeInfo *pInfo = new PipeInfo;
			pInfo->m_rcEnterance = GetEntranceRect();
			pInfo->m_strStageName = m_strStageName;
			pInfo->m_nExitIndex = m_nExitIndex;
			pInfo->m_nType = PIPEENTRANCE_LEFTTORIGHT;
			pInfo->m_nWarpType = m_nPipeType;

			m_pStage->m_vecPipeInfo.push_back(pInfo);
		}
	}
}

void MapObjectPipe::TileizeRightToLeft(int nPhase)
{
	if (nPhase == 0)
	{
		// Head
		m_pStage->SetTileData(m_nTX, m_nTY + 0, TILE_HPIPERIGHT_T);
		m_pStage->SetTileData(m_nTX, m_nTY + 1, TILE_HPIPERIGHT_B);
	}
	else if (nPhase == 3)
	{
		int i = m_nTX - 1;
		for (i = m_nTX - 1; i >= m_nTX - GameDefaults::nPageTileWidth; i--)
		{
			if (!IsEmptyTile(i, m_nTY + 0) || !IsEmptyTile(i, m_nTY + 1))
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

		// Event
		if (m_nPipeType != PIPE_NONE)
		{
			// New Code
			PipeInfo *pInfo = new PipeInfo;
			pInfo->m_rcEnterance = GetEntranceRect();
			pInfo->m_strStageName = m_strStageName;
			pInfo->m_nExitIndex = m_nExitIndex;
			pInfo->m_nType = PIPEENTRANCE_RIGHTTOLEFT;
			pInfo->m_nWarpType = m_nPipeType;

			m_pStage->m_vecPipeInfo.push_back(pInfo);
		}
	}
}

bool MapObjectPipe::IsEmptyTile(int x, int y)
{
	int nData = m_pStage->GetTileData(x, y);
	switch (nData)
	{
	case TILE_EMPTY:
		return true;
		break;
	}

	if (m_pStage->IsBackgroundTile(nData))
		return true;

	return false;
}

void MapObjectPipe::CalcSize()
{
	int nDir = GetPipeDir(this);
	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
	case PIPEENTRANCE_DOWNTOUP:
		m_sizeHead.cx = 2;
		m_sizeHead.cy = 1;

		m_sizeBody.cx = 2;
		m_sizeBody.cy = 0;
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
	case PIPEENTRANCE_RIGHTTOLEFT:
		m_sizeHead.cx = 1;
		m_sizeHead.cy = 2;

		m_sizeBody.cx = 0;
		m_sizeBody.cy = 2;
		break;
	}

	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
		{
			for (int i = m_nTY + 1; i < m_nTY + GameDefaults::nPageTileHeight; i++)
			{
				if (m_pStage->GetTileData(m_nTX + 0, i) != TILE_EMPTY ||
					m_pStage->GetTileData(m_nTX + 1, i) != TILE_EMPTY)
					break;
				m_sizeBody.cy++;
			}
		}
		break;
	case PIPEENTRANCE_DOWNTOUP:
		{
			for (int i = m_nTY - 1; i >= 0; i--)
			{
				if (m_pStage->GetTileData(m_nTX + 0, i) != TILE_EMPTY ||
					m_pStage->GetTileData(m_nTX + 1, i) != TILE_EMPTY)
					break;
				m_sizeBody.cy++;
			}
		}
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		{
			for (int i = m_nTX + 1; i < m_nTX + GameDefaults::nPageTileWidth; i++)
			{
				if (m_pStage->GetTileData(i, m_nTY + 0) != TILE_EMPTY ||
					m_pStage->GetTileData(i, m_nTY + 1) != TILE_EMPTY)
					break;
				m_sizeBody.cx++;
			}
		}
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		{
			for (int i = m_nTX - 1; i >= m_nTX - GameDefaults::nPageTileWidth; i--)
			{
				if (m_pStage->GetTileData(i, m_nTY + 0) != TILE_EMPTY ||
					m_pStage->GetTileData(i, m_nTY + 1) != TILE_EMPTY)
					break;
				m_sizeBody.cx++;
			}
		}
		break;
	}
}

SIZE MapObjectPipe::GetSize()
{
	CalcSize();

	SIZE s = { 1, 1 };

	int nDir = GetPipeDir(this);
	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
	case PIPEENTRANCE_DOWNTOUP:
		s.cx = 2;
		s.cy = m_sizeHead.cy + m_sizeBody.cy;
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
	case PIPEENTRANCE_RIGHTTOLEFT:
		s.cx = m_sizeHead.cx + m_sizeBody.cx;
		s.cy = 2;
		break;
	}

	return s;
}

RECT MapObjectPipe::GetRect()
{
	NaRect rc = { 0, 0, 0, 0 };
	SIZE s = GetSize();
	int nDir = GetPipeDir(this);
	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
		rc = NaRect(
			m_nTX * TILE_XS,
			m_nTY * TILE_YS,
			(m_nTX + s.cx) * TILE_XS,
			(m_nTY + s.cy) * TILE_YS
		);
		break;
	case PIPEENTRANCE_DOWNTOUP:
		rc = NaRect(
			m_nTX * TILE_XS,
			(m_nTY - s.cy + 1) * TILE_YS,
			(m_nTX + s.cx) * TILE_XS,
			(m_nTY + 1) * TILE_YS
		);
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		rc = NaRect(
			m_nTX * TILE_XS,
			m_nTY * TILE_YS,
			(m_nTX + s.cx) * TILE_XS,
			(m_nTY + s.cy) * TILE_YS
		);
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		rc = NaRect(
			(m_nTX - s.cx + 1) * TILE_XS,
			m_nTY * TILE_YS,
			(m_nTX + 1) * TILE_XS,
			(m_nTY + s.cy) * TILE_YS
		);
		break;
	}

	return rc;
}

void MapObjectPipe::Render(int nColor, int nZOrder)
{
	if (m_pSpriteDesign == nullptr)
		m_pSpriteDesign = SPR_MANAGER->Find(L"Design");

	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT;

	int nDir = GetPipeDir(this);
	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
		RenderUpToDown(nColor, nZOrder);
		break;
	case PIPEENTRANCE_DOWNTOUP:
		RenderDownToUp(nColor, nZOrder);
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		RenderLeftToRight(nColor, nZOrder);
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		RenderRightToLeft(nColor, nZOrder);
		break;
	}
}

void MapObjectPipe::RenderUpToDown(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	// Head
	m_pStage->RenderTile(x + (TILE_XS * 0), y, TILE_PIPEHEAD_L, nColor, nZOrder);
	m_pStage->RenderTile(x + (TILE_XS * 1), y, TILE_PIPEHEAD_R, nColor, nZOrder);

	// Body
	for (int i = 1; i < s.cy; i++)
	{
		ty = (y + i) / TILE_YS;
		if (m_pStage->GetTileData(tx + 0, ty) != TILE_EMPTY ||
			m_pStage->GetTileData(tx + 1, ty) != TILE_EMPTY)
			break;

		m_pStage->RenderTile(x + (TILE_XS * 0), y + (i * TILE_YS), TILE_PIPEBODY_L, nColor, nZOrder);
		m_pStage->RenderTile(x + (TILE_XS * 1), y + (i * TILE_YS), TILE_PIPEBODY_R, nColor, nZOrder);
	}

	if (m_nPipeType != PIPE_NONE)
	{
		NaRect rc = GetRect();
		m_pSpriteDesign->RenderToQueue(
			x + rc.Width() / 2 - CAMERA->m_ptPos.x,
			y - CAMERA->m_ptPos.y,
			SPRIDX_DESIGN_ARROW_DOWN,
			false, false, -1L, 1.0f, 1.0f, Z_MAPOBJECT);

		NaString str;
		str.Format(L"%ls", m_strStageName.wstr());
		DRAWTEXT(
			x - CAMERA->m_ptPos.x,
			y - CAMERA->m_ptPos.y - 24,
			str
		);
	}

	if (m_nUseAsExitIndex != -1)
	{
		int nOffSetY = 0;
		if (m_nPipeType != PIPE_NONE)
			nOffSetY = -4;

		NaRect rc = GetRect();
		m_pSpriteDesign->RenderToQueue(
			x + rc.Width() / 2 - CAMERA->m_ptPos.x,
			y - CAMERA->m_ptPos.y + nOffSetY,
			SPRIDX_DESIGN_ARROW_UP,
			false, false, nColor, 1.0f, 1.0f, Z_MAPOBJECT);

		NaString str;
		str.Format(L"%d", m_nUseAsExitIndex);
		m_pGame->m_pRenderManager->Text(
			x + TILE_XS - CAMERA->m_ptPos.x - 4,
			y - CAMERA->m_ptPos.y + 4,
			str,
			nColor & 0xa0ff0000
		);
	}
}

void MapObjectPipe::RenderDownToUp(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

	// Head
	m_pStage->RenderTile(x + (TILE_XS * 0), y, TILE_PIPEBOTTOM_L, nColor, nZOrder);
	m_pStage->RenderTile(x + (TILE_XS * 1), y, TILE_PIPEBOTTOM_R, nColor, nZOrder);

	// Body
	for (int i = 1; i < s.cy; i++)
	{
		ty = (y - i) / TILE_YS;
		if (m_pStage->GetTileData(tx + 0, ty) != TILE_EMPTY ||
			m_pStage->GetTileData(tx + 1, ty) != TILE_EMPTY)
			break;

		m_pStage->RenderTile(x + (TILE_XS * 0), y - (i * TILE_YS), TILE_PIPEBODY_L, nColor, nZOrder);
		m_pStage->RenderTile(x + (TILE_XS * 1), y - (i * TILE_YS), TILE_PIPEBODY_R, nColor, nZOrder);
	}

	if (m_nPipeType != PIPE_NONE)
	{
		NaRect rc = GetRect();
		m_pSpriteDesign->RenderToQueue(
			x + rc.Width() / 2 - CAMERA->m_ptPos.x,
			y - CAMERA->m_ptPos.y + 28,
			SPRIDX_DESIGN_ARROW_DOWN,
			false, true, -1L, 1.0f, 1.0f, Z_MAPOBJECT);

		NaString str;
		str.Format(L"%ls", m_strStageName.wstr());
		DRAWTEXT(
			x - CAMERA->m_ptPos.x,
			y - CAMERA->m_ptPos.y + 32,
			str
		);
	}

	if (m_nUseAsExitIndex != -1)
	{
		int nOffSetY = 0;
		if (m_nPipeType != PIPE_NONE)
			nOffSetY = 4;

		NaRect rc = GetRect();
		m_pSpriteDesign->RenderToQueue(
			x + rc.Width() / 2 - CAMERA->m_ptPos.x,
			y - CAMERA->m_ptPos.y + 28 + nOffSetY,
			SPRIDX_DESIGN_ARROW_UP,
			false, true, -1L, 1.0f, 1.0f, Z_MAPOBJECT);

		NaString str;
		str.Format(L"%d", m_nUseAsExitIndex);
		m_pGame->m_pRenderManager->Text(
			x + TILE_XS - CAMERA->m_ptPos.x - 4,
			y - CAMERA->m_ptPos.y + 4,
			str,
			nColor & 0xa0ff0000
		);
	}
}

void MapObjectPipe::RenderLeftToRight(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

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

void MapObjectPipe::RenderRightToLeft(int nColor, int nZOrder)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	SIZE s = GetSize();

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

MapObjectBase* MapObjectPipe::FindExit(int nIndex)
{
	MapObjectBase *pExitObj = nullptr;

	// Old Style
	int i = 0;
	std::vector<MapObjectBase*>::iterator it = m_pStage->m_vecMapObject.begin();
	for (; it != m_pStage->m_vecMapObject.end(); ++it)
	{
		if (this == *it)
			continue;
		if ((*it)->m_nType == MAPOBJECT_PIPEEXIT)
		{
			if (nIndex == i)
			{
				pExitObj = *it;
				return pExitObj;
			}

			i++;
		}
	}

	// New Style
	pExitObj = nullptr;
	it = m_pStage->m_vecMapObject.begin();
	for (; it != m_pStage->m_vecMapObject.end(); ++it)
	{
		if ((*it)->m_nType == MAPOBJECT_PIPE ||
			(*it)->m_nType == MAPOBJECT_PIPEHORZ)
		{
			MapObjectPipe *pPipe = (MapObjectPipe*)*it;
			if (pPipe->m_nUseAsExitIndex != -1 &&
				pPipe->m_nUseAsExitIndex == nIndex)
			{
				pExitObj = *it;
				return pExitObj;
			}
		}
	}

	return nullptr;
}

NaRect MapObjectPipe::GetEntranceRect()
{
	NaRect rc = GetRect();
	int nDir = GetPipeDir(this);

	switch (nDir)
	{
	case PIPEENTRANCE_UPTODOWN:
		rc.bottom = rc.top + 1;
		break;
	case PIPEENTRANCE_DOWNTOUP:
		rc.bottom = rc.top + 1;
		rc.Offset(0, 16);
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		rc.right = rc.left + 1;
		rc.left -= 1;
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		rc.left = rc.right - 1;
		rc.right += 1;
		break;
	}

	return rc;
}

int MapObjectPipe::GetPipeDir(MapObjectPipe * pPipe)
{
	if (m_bUseNewStyle == false)
	{
		if (pPipe->m_nType == MAPOBJECT_PIPE && pPipe->m_bUpToDown)
			return PIPEENTRANCE_UPTODOWN;
		else if (pPipe->m_nType == MAPOBJECT_PIPE && !pPipe->m_bUpToDown)
			return PIPEENTRANCE_DOWNTOUP;
		else if (pPipe->m_nType == MAPOBJECT_PIPEHORZ && pPipe->m_bLeftToRight)
			return PIPEENTRANCE_LEFTTORIGHT;
		else if (pPipe->m_nType == MAPOBJECT_PIPEHORZ && !pPipe->m_bLeftToRight)
			return PIPEENTRANCE_RIGHTTOLEFT;
	}
	else
	{
		switch (pPipe->m_nDirection)
		{
		case PIPEENTRANCE_UPTODOWN:
		case PIPEENTRANCE_DOWNTOUP:
		case PIPEENTRANCE_LEFTTORIGHT:
		case PIPEENTRANCE_RIGHTTOLEFT:
			return pPipe->m_nDirection;
			break;
		}
	}

	return -1;
}
