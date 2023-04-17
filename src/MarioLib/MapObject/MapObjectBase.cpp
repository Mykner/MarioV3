#include "MapObjectBase.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "ThemeManager.h"

#include "MapObjectPipe.h"
#include "MapObjectStartPoint.h"
#include "MapObjectPipeHorz.h"
#include "MapObjectPipeBody.h"
#include "MapObjectPipeExit.h"
#include "MapObjectCameraStop.h"
#include "MapObjectIsland.h"
#include "MapObjectRiver.h"
#include "MapObjectSkyExit.h"
#include "MapObjectRiverBridge.h"
#include "MapObjectEnemyStop.h"
#include "MapObjectBillBlaster.h"
#include "MapObjectMushroomIsland.h"
#include "MapObjectWall.h"
#include "MapObjectCheckPoint.h"
#include "MapObjectSemiSolid.h"
#include "MapObjectMessageBlock.h"

MapObjectBase::MapObjectBase(Game *pGame, Stage *pStage)
{
	m_pGame = pGame;
	m_pStage = pStage;

	m_pSprite = pStage->m_pSprite;
	m_nType = MAPOBJECT_NONE;

	m_strOption = L"";
}

MapObjectBase::~MapObjectBase()
{
}

// Hit Test by Game Coord
bool MapObjectBase::HitTest(int x, int y)
{
	NaRect rc = GetRect();
	if (rc.ptInRect(x, y))
		return true;

	return false;
}

SIZE MapObjectBase::GetSize()
{
	SIZE s = { 1, 1 };
	return s;
}

RECT MapObjectBase::GetRect()
{
	SIZE s = GetSize();
	NaRect rc(
		m_nTX * TILE_XS, 
		m_nTY * TILE_YS, 
		(m_nTX + s.cx) * TILE_XS, 
		(m_nTY + s.cy) * TILE_YS
	);

	return rc;
}

bool MapObjectBase::IsSingletonType(int nType)
{
	switch (nType)
	{
	case MAPOBJECT_STARTPOINT:
		return true;
		break;
	}

	return false;
}

// MapObject Factory
MapObjectBase * MapObjectBase::CreateInstance(Game *pGame, Stage *pStage, int nType)
{
	MapObjectBase *pObj = nullptr;
	switch (nType)
	{
	case MAPOBJECT_PIPE:			pObj = new MapObjectPipe(pGame, pStage);			break;
	case MAPOBJECT_STARTPOINT:		pObj = new MapObjectStartPoint(pGame, pStage);		break;
	case MAPOBJECT_PIPEHORZ:		pObj = new MapObjectPipeHorz(pGame, pStage);		break;
	case MAPOBJECT_PIPEBODY:		pObj = new MapObjectPipeBody(pGame, pStage);		break;
	case MAPOBJECT_PIPEEXIT:		pObj = new MapObjectPipeExit(pGame, pStage);		break;
	case MAPOBJECT_CAMERASTOP:		pObj = new MapObjectCameraStop(pGame, pStage);		break;
	case MAPOBJECT_ISLAND:			pObj = new MapObjectIsland(pGame, pStage);			break;
	case MAPOBJECT_RIVER:			pObj = new MapObjectRiver(pGame, pStage);			break;
	case MAPOBJECT_SKYEXIT:			pObj = new MapObjectSkyExit(pGame, pStage);			break;
	case MAPOBJECT_RIVERBRIDGE:		pObj = new MapObjectRiverBridge(pGame, pStage);		break;
	case MAPOBJECT_ENEMYSTOP:		pObj = new MapObjectEnemyStop(pGame, pStage);		break;
	case MAPOBJECT_BILLBLASTER:		pObj = new MapObjectBillBlaster(pGame, pStage);		break;
	case MAPOBJECT_MUSHROOMISLAND:	pObj = new MapObjectMushroomIsland(pGame, pStage);	break;
	case MAPOBJECT_WALL:			pObj = new MapObjectWall(pGame, pStage);			break;
	case MAPOBJECT_CHECKPOINT:		pObj = new MapObjectCheckPoint(pGame, pStage);		break;
	case MAPOBJECT_SEMISOLID:		pObj = new MapObjectSemiSolid(pGame, pStage);		break;
	case MAPOBJECT_MESSAGEBLOCK:	pObj = new MapObjectMessageBlock(pGame, pStage);	break;
	}

	if (pObj)
	{
		pObj->BuildPropertyMap();
	}

	//NaDebugOut(L"MabObjectBase::CreateInstance: %d, 0x%08x\n", nType, pObj);
	return pObj;
}
