#include "MapObjectStartPoint.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "RenderManager.h"

MapObjectStartPoint::MapObjectStartPoint(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_STARTPOINT;
}

MapObjectStartPoint::~MapObjectStartPoint()
{
}

void MapObjectStartPoint::Install()
{
	m_pStage->m_ptStartPoint.x = m_nTX * TILE_XS + TILE_XS / 2;
	m_pStage->m_ptStartPoint.y = m_nTY * TILE_YS + TILE_YS;
}

bool MapObjectStartPoint::Tileize(int nPhase)
{
	m_pStage->m_ptStartPoint.x = m_nTX * TILE_XS + TILE_XS / 2;
	m_pStage->m_ptStartPoint.y = m_nTY * TILE_YS + TILE_YS;

	return true;
}

SIZE MapObjectStartPoint::GetSize()
{
	SIZE s = { 1, 1 };

	return s;
}

void MapObjectStartPoint::Render(int nColor, int nZOrder)
{
	if (m_pSpritePlayer == nullptr)
	{
		m_pSpritePlayer = SPR_MANAGER->Find(L"Design");
	}

	int _x = x + (TILE_XS / 2) - CAMERA->m_ptPos.x;
	int _y = y + (TILE_YS) - CAMERA->m_ptPos.y;

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT_TOP;
	
	m_pSpritePlayer->RenderToQueue(_x, _y, 0, 
		false, false, nColor, 1.0f, 1.0f, nZOrder);
}
