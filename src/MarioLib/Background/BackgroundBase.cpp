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
	m_pData = nullptr;
}

BackgroundBase::~BackgroundBase()
{
}

void BackgroundBase::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	if (m_pStage->m_pTileBackgroundData == nullptr)
		m_pStage->m_pTileBackgroundData = new BYTE[m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy];

	m_pData = m_pStage->m_pTileBackgroundData;
	memset(m_pData, 0, sizeof(BYTE) * m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy);

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

	SetData(x, y + 0, TILEBG_CLOUD_LT);
	SetData(x++, y + 1, TILEBG_CLOUD_LB);
	for (int i = 0; i < size; i++)
	{
		SetData(x, y + 0, TILEBG_CLOUD_T);
		SetData(x++, y + 1, TILEBG_CLOUD_B);
	}
	SetData(x, y + 0, TILEBG_CLOUD_RT);
	SetData(x, y + 1, TILEBG_CLOUD_RB);
}

void BackgroundBase::CreateRedCloud(int x, int y, int size)
{
	if (m_pStage->m_pSpriteFarBackground)
		return;

	SetData(x, y + 0, TILEBG_REDCLOUD_LT);
	SetData(x++, y + 1, TILEBG_REDCLOUD_LB);
	for (int i = 0; i < size; i++)
	{
		SetData(x, y + 0, TILEBG_REDCLOUD_T);
		SetData(x++, y + 1, TILEBG_REDCLOUD_B);
	}
	SetData(x, y + 0, TILEBG_REDCLOUD_RT);
	SetData(x, y + 1, TILEBG_REDCLOUD_RB);
}

void BackgroundBase::CreateMountain(int x, int y, int size)
{
	int nWidth = (size * 2) + 1;
	if (IsContainHole(x, nWidth))
		return;

	SetData(x + size, y++, TILEBG_MOUNTAIN_T);
	for (int i = 1; i <= size; i++)
	{
		int _x = x + (size - i);
		SetData(_x++, y, TILEBG_MOUNTAIN_L);
		for (int j = 0; j < (i * 2) - 1; j++)
		{
			if (j % 2 == 0)
				SetData(_x++, y, TILEBG_MOUNTAIN_TREE);
			else
				SetData(_x++, y, TILEBG_MOUNTAIN_C);
		}
		SetData(_x, y++, TILEBG_MOUNTAIN_R);
	}
}

void BackgroundBase::CreateBush(int x, int y, int size)
{
	int nWidth = size + 2;
	if (IsContainHole(x, nWidth))
		return;

	SetData(x++, y + 0, TILEBG_BUSH_L);
	for (int i = 0; i < size; i++)
	{
		SetData(x++, y + 0, TILEBG_BUSH_C);
	}
	SetData(x, y + 0, TILEBG_BUSH_R);
}

void BackgroundBase::CreateFence(int x, int y, int size)
{
	for (int i = 0; i < size; i++)
	{
		if (IsContainHole(x + i, 1))
			return;
		SetData(x + i, y + 0, TILEBG_FENCE);
	}
}

void BackgroundBase::CreateTree(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_TREE_B);
	if (size > 1)
	{
		for (int i = 0; i < size - 1; i++)
		{
			SetData(x, y--, TILEBG_TREE_C);
		}
		SetData(x, y, TILEBG_TREE_T);
	}
	else
		SetData(x, y, TILEBG_TREE_T1);
}

void BackgroundBase::CreateSnowTree(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_TREE_B);
	if (size > 1)
	{
		for (int i = 0; i < size - 1; i++)
		{
			SetData(x, y--, TILEBG_SNOWTREE_C);
		}
		SetData(x, y, TILEBG_SNOWTREE_T);
	}
	else
		SetData(x, y, TILEBG_SNOWTREE_T1);
}

void BackgroundBase::CreateDeadMushroom(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y, TILEBG_DEADMUSHROOM);
}

void BackgroundBase::CreateDeadBush(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	if (size == 1)
	{
		SetData(x, y, TILEBG_DEADBUSH_R);
	}
	else if (size == 2)
	{
		SetData(x, y - 1, TILEBG_DEADBUSH_R);
		SetData(x, y, TILEBG_DEADBUSH_LR);
	}
	else
	{
		SetData(x, y - 2, TILEBG_DEADBUSH_R);
		SetData(x, y - 1, TILEBG_DEADBUSH_LR);
		SetData(x, y, TILEBG_DEADBUSH_BODY);
	}
}

void BackgroundBase::CreateDeadBone(int x, int y, int size)
{
	if (IsContainHole(x, 3))
		return;

	SetData(x++, y, TILEBG_DEADBONE_L);
	SetData(x++, y, TILEBG_DEADBONE_C);
	SetData(x++, y, TILEBG_DEADBONE_R);
}

void BackgroundBase::CreateBowserJrStatue(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_BOWSERJR_STATUE_B);
	SetData(x, y, TILEBG_BOWSERJR_STATUE_T);
}

void BackgroundBase::CreateBowserStatue(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_BOWSER_STATUE_BODY);
	SetData(x, y--, TILEBG_BOWSER_STATUE_B);
	SetData(x, y, TILEBG_BOWSER_STATUE_T);
}

void BackgroundBase::CreateCastleFence(int x, int y, int size)
{
	if (IsContainHole(x, size))
		return;

	for (int i = 0; i < size; i++)
	{
		if (IsContainHole(x + i, 1))
			return;

		if (i == 0)
			SetData(x + i, y + 0, TILEBG_CASTLE_FENCE_L);
		else if (i == size - 1)
			SetData(x + i, y + 0, TILEBG_CASTLE_FENCE_R);
		else
			SetData(x + i, y + 0, TILEBG_CASTLE_FENCE_C);
	}
}

void BackgroundBase::CreateGhostDeadMushroom(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y, TILEBG_GHOSTHOUSE_DEADMUSHROOM);
}

void BackgroundBase::CreateLamp(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_GHOSTHOUSE_LAMP_B);
	SetData(x, y, TILEBG_GHOSTHOUSE_LAMP_T);
}

void BackgroundBase::CreateClock(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_GHOSTHOUSE_CLOCK_B);
	SetData(x, y--, TILEBG_GHOSTHOUSE_CLOCK_C);
	SetData(x, y, TILEBG_GHOSTHOUSE_CLOCK_T);
}

void BackgroundBase::CreateMiniBolt(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y, TILEBG_AIRSHIP_MINIBOLT);
}

void BackgroundBase::CreateMiniPipe(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_AIRSHIP_MINIPIPE_B);
	SetData(x, y, TILEBG_AIRSHIP_MINIPIPE_T);
}

void BackgroundBase::CreateMiniFlag(int x, int y, int size)
{
	if (IsContainHole(x, 1))
		return;

	SetData(x, y--, TILEBG_AIRSHIP_MINIFLAG_B);
	SetData(x, y--, TILEBG_AIRSHIP_MINIFLAG_C);
	SetData(x, y, TILEBG_AIRSHIP_MINIFLAG_T);
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

void BackgroundBase::SetData(int x, int y, int value)
{
	if (x < 0 || y < 0 || x >= m_pStage->m_sizeTile.cx || y >= m_pStage->m_sizeTile.cy)
		return;

	int nTileW = GameDefaults::nPageTileWidth * m_pStage->m_nMaxPage;
	int nIdx = y * nTileW + x;
	m_pData[nIdx] = value;
	m_pStage->m_pTileBackgroundData[nIdx] = value;
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
