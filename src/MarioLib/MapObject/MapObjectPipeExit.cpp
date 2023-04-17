#include "MapObjectPipeExit.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"
#include "MapObjectPipe.h"
#include "MapObjectPipeHorz.h"
#include "RenderManager.h"

MapObjectPipeExit::MapObjectPipeExit(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_PIPEEXIT;

	m_pPipe = nullptr;
	m_nExitType = -1;
}

MapObjectPipeExit::~MapObjectPipeExit()
{
}

bool MapObjectPipeExit::Tileize(int nPhase)
{
	if (nPhase == 1)
	{
		m_pPipe = FindPipe();
	}
	if (nPhase == 2)
	{
		PipeExitInfo *pInfo = new PipeExitInfo;
		if (m_pPipe)
		{
			pInfo->m_rcExit = ((MapObjectPipe*)m_pPipe)->GetEntranceRect();
			pInfo->m_nType = GetPipeDir((MapObjectPipe*)m_pPipe);
		}
		else
		{
			pInfo->m_rcExit = GetRect();
			pInfo->m_nType = PIPEENTRANCE_UPTODOWN;
		}
		
		// Will be deprecated.
		pInfo->m_nIndex = m_pStage->m_vecPipeExitInfo.size();
		m_pStage->m_vecPipeExitInfo.push_back(pInfo);
	}
	if (nPhase == 5)
		return true;

	return false;
}

void MapObjectPipeExit::Install()
{
	MapObjectPipe *pPipe = (MapObjectPipe*)FindPipe();
	if (pPipe)
	{
		switch (GetPipeDir(pPipe))
		{
		case PIPEENTRANCE_UPTODOWN:
			m_nTX = pPipe->m_nTX;
			m_nTY = pPipe->m_nTY - 1;
			break;
		case PIPEENTRANCE_DOWNTOUP:
			m_nTX = pPipe->m_nTX;
			m_nTY = pPipe->m_nTY;
			break;
		case PIPEENTRANCE_LEFTTORIGHT:
			m_nTX = pPipe->m_nTX - 1;
			m_nTY = pPipe->m_nTY;
			break;
		case PIPEENTRANCE_RIGHTTOLEFT:
			m_nTX = pPipe->m_nTX;
			m_nTY = pPipe->m_nTY;
			break;
		}
	}
}

SIZE MapObjectPipeExit::GetSize()
{
	SIZE s = { 2, 2 };
	return s;
}

void MapObjectPipeExit::Render(int nColor, int nZOrder)
{
	if (m_pSpriteDesign == nullptr)
		m_pSpriteDesign = SPR_MANAGER->Find(L"Design");

	int tx = x / TILE_XS;
	int ty = y / TILE_YS;

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT;

	MapObjectPipe *pPipe = (MapObjectPipe*)FindPipe();

	if (pPipe)
	{
		SIZE sizePipe = pPipe->GetSize();
		m_nExitType = GetPipeDir(pPipe);
		switch (m_nExitType)
		{
		case PIPEENTRANCE_UPTODOWN:
			m_pSpriteDesign->RenderToQueue(
				(pPipe->m_nTX + (sizePipe.cx / 2)) * TILE_XS - CAMERA->m_ptPos.x,
				pPipe->m_nTY * TILE_YS - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_UP,
				false, false, nColor, 1.0f, 1.0f, nZOrder
			);
			break;
		case PIPEENTRANCE_DOWNTOUP:
			m_pSpriteDesign->RenderToQueue(
				(pPipe->m_nTX + (sizePipe.cx / 2)) * TILE_XS - CAMERA->m_ptPos.x,
				(pPipe->m_nTY + 2) * TILE_YS - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_DOWN,
				false, false, nColor, 1.0f, 1.0f, nZOrder
			);
			break;
		case PIPEENTRANCE_LEFTTORIGHT:
			m_pSpriteDesign->RenderToQueue(
				(pPipe->m_nTX - 1) * TILE_XS - CAMERA->m_ptPos.x,
				(pPipe->m_nTY + 1) * TILE_YS - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_LEFT,
				false, false, nColor, 1.0f, 1.0f, nZOrder
			);
			break;
		case PIPEENTRANCE_RIGHTTOLEFT:
			m_pSpriteDesign->RenderToQueue(
				(pPipe->m_nTX + 2) * TILE_XS - CAMERA->m_ptPos.x,
				(pPipe->m_nTY + 1) * TILE_YS - CAMERA->m_ptPos.y,
				SPRIDX_DESIGN_ARROW_RIGHT,
				false, false, nColor, 1.0f, 1.0f, nZOrder
			);
			break;
		default:
			return;
			break;
		}
	}
	else
	{
		m_pSpriteDesign->RenderToQueue(
			x + (TILE_XS) - CAMERA->m_ptPos.x,
			y + (TILE_YS) - CAMERA->m_ptPos.y,
			SPRIDX_DESIGN_ARROW_UP,
			false, false, nColor, 1.0f, 1.0f, nZOrder
		);
	}

	int nIndex = 0;
	std::vector<MapObjectBase*>::iterator it = m_pStage->m_vecMapObject.begin();
	for (; it != m_pStage->m_vecMapObject.end(); ++it)
	{
		if ((*it)->m_nType == MAPOBJECT_PIPEEXIT)
		{
			if (this == *it)
			{
				NaString str;
				str.Format(L"%d", nIndex);
				m_pGame->m_pRenderManager->Text(
					x + TILE_XS - CAMERA->m_ptPos.x - 4,
					y + TILE_YS - CAMERA->m_ptPos.y - 4,
					str,
					0xa0ff0000
				);
				break;
			}
			nIndex++;
		}
	}
}

MapObjectBase * MapObjectPipeExit::FindPipe()
{
	MapObjectBase *pPipe = nullptr;
	std::vector<MapObjectBase*>::iterator it = m_pStage->m_vecMapObject.begin();
	for (; it != m_pStage->m_vecMapObject.end(); ++it)
	{
		MapObjectBase *pObj = *it;
		if (pObj == this)
			continue;

		if (pObj->m_nType != MAPOBJECT_PIPE &&
			pObj->m_nType != MAPOBJECT_PIPEHORZ)
			continue;

		MapObjectPipe *pPipe = (MapObjectPipe*)pObj;
		if ((pObj->HitTest((m_nTX + 0) * TILE_XS, (m_nTY + 0) * TILE_YS) ||
			 pObj->HitTest((m_nTX + 1) * TILE_XS, (m_nTY + 0) * TILE_YS) ||
			 pObj->HitTest((m_nTX + 1) * TILE_XS, (m_nTY + 1) * TILE_YS) ||
			 pObj->HitTest((m_nTX + 1) * TILE_XS, (m_nTY + 1) * TILE_YS)))
		{
			return pObj;
		}
	}

	return nullptr;
}

int MapObjectPipeExit::GetPipeDir(MapObjectPipe * pPipe)
{
	if (pPipe->m_nType == MAPOBJECT_PIPE && pPipe->m_bUpToDown)
	{
		return PIPEENTRANCE_UPTODOWN;
	}
	else if (pPipe->m_nType == MAPOBJECT_PIPE && !pPipe->m_bUpToDown)
	{
		return PIPEENTRANCE_DOWNTOUP;
	}
	else if (pPipe->m_nType == MAPOBJECT_PIPEHORZ && pPipe->m_bLeftToRight)
	{
		return PIPEENTRANCE_LEFTTORIGHT;
	}
	else if (pPipe->m_nType == MAPOBJECT_PIPEHORZ && !pPipe->m_bLeftToRight)
	{
		return PIPEENTRANCE_RIGHTTOLEFT;
	}

	return -1;
}
