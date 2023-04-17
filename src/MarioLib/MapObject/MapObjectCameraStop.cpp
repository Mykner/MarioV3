#include "MapObjectCameraStop.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "RenderManager.h"

MapObjectCameraStop::MapObjectCameraStop(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_CAMERASTOP;
}

MapObjectCameraStop::~MapObjectCameraStop()
{
}

void MapObjectCameraStop::Install()
{
	m_pStage->m_ptStopPoint.x = m_nTX * TILE_XS;
	m_pStage->m_ptStopPoint.y = m_nTY * TILE_YS + TILE_YS;
}

bool MapObjectCameraStop::Tileize(int nPhase)
{
	if (m_nTX == -1 || m_nTY == -1)
		return true;

	m_pStage->m_ptStopPoint.x = m_nTX * TILE_XS;
	m_pStage->m_ptStopPoint.y = m_nTY * TILE_YS + TILE_YS;

	return true;
}

SIZE MapObjectCameraStop::GetSize()
{
	SIZE s;
	s.cx = 1;
	s.cy = 1;

	return s;
}

void MapObjectCameraStop::Render(int nColor, int nZOrder)
{
	if (m_pSpritePlayer == nullptr)
	{
		m_pSpritePlayer = SPR_MANAGER->Find(L"Design");
	}

	int _x = x + (TILE_XS / 2) - CAMERA->m_ptPos.x;
	int _y = y + (TILE_YS)-CAMERA->m_ptPos.y;

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT_TOP;

	m_pSpritePlayer->RenderToQueue(_x, _y, SPRIDX_DESIGN_CAMERASTOP,
		false, false, nColor, 1.0f, 1.0f, nZOrder);
}
