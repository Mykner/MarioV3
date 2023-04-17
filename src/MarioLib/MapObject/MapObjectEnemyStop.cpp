#include "MapObjectEnemyStop.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "RenderManager.h"

MapObjectEnemyStop::MapObjectEnemyStop(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_ENEMYSTOP;
}

MapObjectEnemyStop::~MapObjectEnemyStop()
{
}

void MapObjectEnemyStop::Install()
{
	m_pStage->m_ptEnemyStop.x = m_nTX * TILE_XS;
	m_pStage->m_ptEnemyStop.y = m_nTY * TILE_YS + TILE_YS;
}

bool MapObjectEnemyStop::Tileize(int nPhase)
{
	m_pStage->m_ptEnemyStop.x = m_nTX * TILE_XS;
	m_pStage->m_ptEnemyStop.y = m_nTY * TILE_YS + TILE_YS;

	return true;
}

SIZE MapObjectEnemyStop::GetSize()
{
	SIZE s;
	s.cx = 1;
	s.cy = 1;

	return s;
}

void MapObjectEnemyStop::Render(int nColor, int nZOrder)
{
	if (m_pSpritePlayer == nullptr)
	{
		m_pSpritePlayer = SPR_MANAGER->Find(L"Design");
	}

	int _x = x + (TILE_XS / 2) - CAMERA->m_ptPos.x;
	int _y = y + (TILE_YS)-CAMERA->m_ptPos.y;

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT_TOP;

	m_pSpritePlayer->RenderToQueue(_x, _y, SPRIDX_DESIGN_ENEMYSTOP,
		false, false, nColor, 1.0f, 1.0f, nZOrder);
}
