#include "MapObjectBillBlaster.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Stage.h"
#include "Camera.h"
#include "PipeInfo.h"
#include "TypeNames.h"
#include "RenderManager.h"
#include "BillBlaster.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectBillBlaster)
	PROP_INT("FireType",	VT_I4,	-1,								g_szThrowableTypes, "MapObjectBillBlaster"),
	PROP_INT("Interval",	VT_I4,	10,								0,					"MapObjectBillBlaster"),
	PROP_INT("Chance",		VT_I4,	100,							0,					"MapObjectBillBlaster"),
	PROP_INT("MaxCount",	VT_I4,	5,								0,					"MapObjectBillBlaster"),
	PROP_INT("Height",		VT_I4,	-1,								0,					"MapObjectBillBlaster"),
END_IMPL_PROPERTYMAP()

MapObjectBillBlaster::MapObjectBillBlaster(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_BILLBLASTER;

	m_nFireTypeIndex = -1;
	m_nFireType = ENEMY_BULLETBILL;
	for (int i = 0; ; i++)
	{
		if (g_nThrowableTypes[i] == ENEMY_BULLETBILL)
		{
			m_nFireTypeIndex = i;
			break;
		}
	}

	m_nInterval = 10;
	m_nChance = 100;
	m_nMaxCount = 5;
	m_nHeight = -1;
}

MapObjectBillBlaster::~MapObjectBillBlaster()
{
}

bool MapObjectBillBlaster::Tileize(int nPhase)
{
	if (nPhase == 0)
	{
		// Create EventObject who fires BulletBill
		auto size = GetSize();
		BillBlaster *pObj = (BillBlaster*)m_pStage->CreateEventObject(
			(m_nTX * TILE_XS) + (TILE_XS / 2), 
			(m_nTY + size.cy) * TILE_YS, 
			EVENTOBJECT_BILLBLASTER);
		if (pObj)
		{
			pObj->m_nFireType = GetFireType();
			pObj->m_nInterval = m_nInterval;
			pObj->m_nChance = m_nChance;
			pObj->m_nMaxCount = m_nMaxCount;
			pObj->m_nLength = GetSize().cy;
		}
	}
	
	// Changed body to object (2017.04.12)

	return true;
}

void MapObjectBillBlaster::CalcSize()
{
	m_sizeHead.cx = 1;
	m_sizeHead.cy = 1;

	m_sizeBody.cx = 1;
	m_sizeBody.cy = 0;

	if (m_nHeight < -1)
		m_nHeight = -1;
	if (m_nHeight == -1)
	{
		for (int i = m_nTY + 1; i < TILE_YS; i++)
		{
			if (m_pStage->GetTileData(m_nTX, i) != TILE_EMPTY)
				break;
			m_sizeBody.cy++;
		}
	}	
	else
	{
		m_sizeBody.cy = m_nHeight;
	}
}

SIZE MapObjectBillBlaster::GetSize()
{
	CalcSize();

	SIZE s;
	s.cx = 1;
	s.cy = m_sizeHead.cy + m_sizeBody.cy;

	return s;
}

void MapObjectBillBlaster::Render(int nColor, int nZOrder)
{
	SIZE s = GetSize();

	if (nZOrder == -1)
		nZOrder = Z_MAP;

	// Head
	m_pStage->RenderTile(x, y, TILE_CANNONHEAD_T, nColor, nZOrder);

	if (nZOrder == Z_MAP)
		nZOrder = Z_BEHIND_MAP;

	// Body
	for (int i = 1; i < s.cy; i++)
	{
		int tx = x / TILE_XS;
		int ty = (y / TILE_YS) + i;
		if (m_pStage->GetTileData(tx, ty) != TILE_EMPTY)
			break;

		if (i == 1)
			m_pStage->RenderTile(x, y + (i * TILE_YS), TILE_CANNONHEAD_B, nColor, nZOrder);
		else
			m_pStage->RenderTile(x, y + (i * TILE_YS), TILE_CANNONBODY, nColor, nZOrder);
	}
}

int MapObjectBillBlaster::GetFireType()
{
	return g_nThrowableTypes[m_nFireTypeIndex];
}
