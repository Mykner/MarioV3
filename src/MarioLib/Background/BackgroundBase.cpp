#include "BackgroundBase.h"

#include "Types.h"
#include "Stage.h"
#include "Themes.h"

#include "BackgroundMountain.h"
#include "BackgroundCloud.h"
#include "BackgroundUnderWater.h"
#include "BackgroundFence.h"
#include "BackgroundSnowFence.h"
#include "BackgroundRedCloud.h"
#include "BackgroundRedCloud.h"
#include "BackgroundUnderground.h"
#include "BackgroundCastle.h"
#include "BackgroundGhostHouse.h"
#include "BackgroundAirShip.h"

BackgroundBase::BackgroundBase(Stage * pStage)
{
	m_pStage = pStage;
}

BackgroundBase::~BackgroundBase()
{
}

void BackgroundBase::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;
		
		CreateCloud(nOffset + 3, 3, 2);
		CreateCloud(nOffset + 9, 7, 1);
		CreateCloud(nOffset + 18, 2, 2);
		CreateCloud(nOffset + 28, 11, 1);
		CreateCloud(nOffset + 35, 7, 1);
		CreateCloud(nOffset + 38, 6, 1);
		CreateCloud(nOffset + 46, 11, 1);
	}
}

void BackgroundBase::CreateCloud(int x, int y, int size)
{
	if (m_pStage->m_pSpriteFarBackground)
		return;

    m_pStage->AddBackgroundObject(x, y, size + 4);
}

void BackgroundBase::CreateRedCloud(int x, int y, int size)
{
	if (m_pStage->m_pSpriteFarBackground)
		return;

    m_pStage->AddBackgroundObject(x, y, size + 7);
}

void BackgroundBase::CreateMountain(int x, int y, int size)
{
	int nWidth = (size * 2) + 1;
	if (IsContainHole(x, nWidth))
		return;

    m_pStage->AddBackgroundObject(x, y, size + 2);
}

void BackgroundBase::CreateBush(int x, int y, int size)
{
	int nWidth = size + 2;
	if (IsContainHole(x, nWidth))
		return;

    m_pStage->AddBackgroundObject(x, y, size - 1);
}

void BackgroundBase::CreateFence(int x, int y, int size)
{
	if (IsContainHole(x, size))
		return;

    m_pStage->AddBackgroundObject(x, y, size + 15);
}

void BackgroundBase::CreateTree(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, size + 10);
}

void BackgroundBase::CreateSnowTree(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, size + 13);
}

void BackgroundBase::CreateShortCoral(int x, int y, int size)
{
    if (IsContainHole(x, 1))
        return;

    m_pStage->AddBackgroundObject(x, y, size + 29);
}

void BackgroundBase::CreateLongCoral(int x, int y, int size)
{
    if (IsContainHole(x, 3))
        return;

    m_pStage->AddBackgroundObject(x, y, 33);
}

void BackgroundBase::CreateWaterSky()
{
    m_pStage->AddBackgroundObject(0, 0, 20);
}

void BackgroundBase::CreateDeadBush(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, size + 34);
}

void BackgroundBase::CreateDeadBone(int x, int y, int size)
{
	if (IsContainHole(x, 3))
		return;

    m_pStage->AddBackgroundObject(x, y, 34);
}

void BackgroundBase::CreateBowserJrStatue(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 38);
}

void BackgroundBase::CreateBowserStatue(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 39);
}

void BackgroundBase::CreateCastleFence(int x, int y, int size)
{
	if (IsContainHole(x, size))
		return;

    m_pStage->AddBackgroundObject(x, y, size + 37);
}

void BackgroundBase::CreateLavaLayer()
{
    BackgroundObjectBase *pObj = m_pStage->AddBackgroundObject(0, 0, 25);

    NaRect rc;
    rc.left = -TILE_XS;
    rc.top = TILE_YS * (m_pStage->m_sizeTile.cy - 1) - 8;
    rc.right = TILE_XS * (m_pStage->m_sizeTile.cx + 2);
    rc.bottom = rc.top + TILE_YS;

    m_pStage->AddHurtArea(rc);
}

void BackgroundBase::CreateGhostDeadMushroom(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 35);
}

void BackgroundBase::CreateLamp(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 42);
}

void BackgroundBase::CreateClock(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 43);
}

void BackgroundBase::CreateGhostHouseFence(int x, int y, int size)
{
    if (IsContainHole(x, 3))
        return;

    m_pStage->AddBackgroundObject(x, y, 44);
}

void BackgroundBase::CreateMiniBolt(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 45);
}

void BackgroundBase::CreateMiniPipe(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 46);
}

void BackgroundBase::CreateMiniFlag(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

    m_pStage->AddBackgroundObject(x, y, 47);
}

void BackgroundBase::CreateMiniFence(int x, int y, int size)
{
    if (IsContainHole(x, 3))
        return;

    m_pStage->AddBackgroundObject(x, y, 48);
}

int BackgroundBase::GetGroundY()
{
    return m_pStage->m_sizeTile.cy - 3;
}

bool BackgroundBase::IsContainHole(int x, int width)
{
	if (m_pStage->m_bIsDesignStage)
		return false;

	int nGround = m_pStage->m_sizeTile.cy - 3;
	for (int i = 0; i < width; i++)
	{
		if (m_pStage->GetTileData(x + i, nGround) == TILE_EMPTY)
			return true;
		if (m_pStage->GetTileData(x + i, nGround) == TILE_RIVER_T)
			return true;
	}
	return false;
}

BackgroundBase * BackgroundBase::CreateInstance(int nType, Stage * pStage)
{
	BackgroundBase *pObj = nullptr;
	switch (nType)
	{
	case STAGEBGTHEME_MOUNTAIN:		pObj = new BackgroundMountain(pStage);		break;
	case STAGEBGTHEME_CLOUD:		pObj = new BackgroundCloud(pStage);			break;
	case STAGEBGTHEME_UNDERWATER:	pObj = new BackgroundUnderWater(pStage);	break;
	case STAGEBGTHEME_FENCE:		pObj = new BackgroundFence(pStage);			break;
	case STAGEBGTHEME_NIGHT:		pObj = new BackgroundMountain(pStage);		break;
	case STAGEBGTHEME_SNOW:			pObj = new BackgroundSnowFence(pStage);		break;
	case STAGEBGTHEME_SNOWNIGHT:	pObj = new BackgroundSnowFence(pStage);		break;
	case STAGEBGTHEME_REDCLOUD:		pObj = new BackgroundRedCloud(pStage);		break;
	case STAGEBGTHEME_UNDERGROUND:	pObj = new BackgroundUnderground(pStage);	break;
	case STAGEBGTHEME_CASTLE:		pObj = new BackgroundCastle(pStage);		break;
	case STAGEBGTHEME_GHOSTHOUSE:	pObj = new BackgroundGhostHouse(pStage);	break;
	case STAGEBGTHEME_AIRSHIP:		pObj = new BackgroundAirShip(pStage);		break;
	}

	return pObj;
}
