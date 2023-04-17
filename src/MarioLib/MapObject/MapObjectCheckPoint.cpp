#include "MapObjectCheckPoint.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "RenderManager.h"

MapObjectCheckPoint::MapObjectCheckPoint(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_CHECKPOINT;
}

MapObjectCheckPoint::~MapObjectCheckPoint()
{
}

void MapObjectCheckPoint::Install()
{
	
}

bool MapObjectCheckPoint::Tileize(int nPhase)
{
	/*
	// Deprecated classic checkpoint
	m_pStage->m_ptCheckPoint.x = m_nTX * TILE_XS + TILE_XS / 2;
	m_pStage->m_ptCheckPoint.y = m_nTY * TILE_YS + TILE_YS;
	*/

	m_pStage->ReserveCreateGameObject(m_nX + 8, m_nY + 16, EVENTOBJECT_CHECKPOINT_FLAG);

	return true;
}

SIZE MapObjectCheckPoint::GetSize()
{
	SIZE s;
	s.cx = 1;
	s.cy = 1;

	return s;
}

void MapObjectCheckPoint::Render(int nColor, int nZOrder)
{
	if (m_pSpritePlayer == nullptr)
	{
		m_pSpritePlayer = SPR_MANAGER->Find(L"Design");
	}

	int _x = x + (TILE_XS / 2) - CAMERA->m_ptPos.x;
	int _y = y + (TILE_YS)-CAMERA->m_ptPos.y;

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT_TOP;

	m_pSpritePlayer->RenderToQueue(_x, _y, SPRIDX_DESIGN_CHECKPOINT, 
		false, false, nColor, 1.0f, 1.0f, nZOrder);
}
