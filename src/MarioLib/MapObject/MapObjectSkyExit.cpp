#include "MapObjectSkyExit.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"

MapObjectSkyExit::MapObjectSkyExit(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_SKYEXIT;
}

MapObjectSkyExit::~MapObjectSkyExit()
{
}

bool MapObjectSkyExit::Tileize(int nPhase)
{
	m_pStage->m_ptSkyExit = 
	{ 
		m_nTX * TILE_XS + (TILE_XS / 2), 
		0
	};
	return true;
}

SIZE MapObjectSkyExit::GetSize()
{
	SIZE s = { 1, 1 };
	return s;
}

void MapObjectSkyExit::Render(int nColor, int nZOrder)
{
	if (m_pSpriteDesign == nullptr)
		m_pSpriteDesign = SPR_MANAGER->Find(L"Design");

	if (nZOrder == -1)
		nZOrder = Z_MAPOBJECT;

	m_pSpriteDesign->RenderToQueue(
		x + (TILE_XS / 2) - CAMERA->m_ptPos.x,
		y + TILE_YS - CAMERA->m_ptPos.y,
		SPRIDX_DESIGN_SKYEXIT,
		false, false, nColor, 1.0f, 1.0f, nZOrder
	);
}
