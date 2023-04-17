#include "MapObjectSemiSolid.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(MapObjectSemiSolid)
	PROP_INT("Width",		VT_I4, 3, 0, "MapObjectSemiSolid"),
	PROP_INT("SubType",		VT_I4, 1, 0, "MapObjectSemiSolid"),
END_IMPL_PROPERTYMAP()

MapObjectSemiSolid::MapObjectSemiSolid(Game *pGame, Stage *pStage) :
	MapObjectBase(pGame, pStage)
{
	m_nType = MAPOBJECT_SEMISOLID;
	m_pSprite = SPR_MANAGER->Find(L"MapObject");

	m_nWidth = 3;
	m_nHeight = 1;

	m_nSubType = 1;
}

MapObjectSemiSolid::~MapObjectSemiSolid()
{
}

bool MapObjectSemiSolid::Tileize(int nPhase)
{
	int nSkinIndex = m_nSubType;
	if (nSkinIndex < 0)
		nSkinIndex = 0;
	if (nSkinIndex > 2)
		nSkinIndex = 2;

	if (nPhase == 0)
	{
		CalcSize();

		// Head
		for (int i = m_nTX; i < m_nTX + m_nWidth; i++)
		{
			/*
			int nIdx = GetSkinTileIndex(1) + (nSkinIndex * 3);
			if (i == m_nTX)
				nIdx = GetSkinTileIndex(0) + (nSkinIndex * 3);
			else if (i == m_nTX + m_nWidth - 1)
				nIdx = GetSkinTileIndex(2) + (nSkinIndex * 3);
			*/

			int nIdx = GetCorrectTileIndex(i - m_nTX, 0);
			m_pStage->SetTileData(i, m_nTY, nIdx);
		}

		return false;
	}
	else if (nPhase == 1)
	{
		// Body
		for (int j = m_nTY + 1; j < m_nTY + m_nHeight; j++)
		{
			for (int i = m_nTX; i < m_nTX + m_nWidth; i++)
			{
				if (m_pStage->GetTileData(i, j) != TILE_EMPTY &&
					m_pStage->GetTileData(i, j) != TILE_HIDDENITEM)
					continue;

				/*
				int nIdx = GetSkinTileIndex(4) + (nSkinIndex * 3);
				if (i == m_nTX)
					nIdx = GetSkinTileIndex(3) + (nSkinIndex * 3);
				else if (i == m_nTX + m_nWidth - 1)
					nIdx = GetSkinTileIndex(5) + (nSkinIndex * 3);
				*/

				int nIdx = GetCorrectTileIndex(i - m_nTX, j - m_nTY);
				m_pStage->SetTileData(i, j, nIdx);
			}
		}
	}

	return true;
}

void MapObjectSemiSolid::Install()
{
	CalcSize();
}

void MapObjectSemiSolid::CalcSize()
{
	m_nHeight = 1;

	for (int j = m_nTY + 1; j < m_nTY + GameDefaults::nPageHeight; j++)
	{
		int nStop = 0;
		for (int i = m_nTX; i < m_nTX + m_nWidth; i++)
		{
			if (m_pStage->GetTileData(i, j) != TILE_EMPTY)
			{
				nStop++;
			}
		}

		if (nStop == m_nWidth)
			break;

		m_nHeight++;
	}
}

SIZE MapObjectSemiSolid::GetSize()
{
	CalcSize();

	SIZE s = { m_nWidth, m_nHeight };

	return s;
}

void MapObjectSemiSolid::Render(int nColor, int nZOrder)
{
	bool bNotShadow = false;
	if (nZOrder == -1)
	{
		bNotShadow = true;
		nZOrder = Z_MAP;
	}

	int nSkinIndex = m_nSubType;
	if (nSkinIndex < 0)
		nSkinIndex = 0;
	if (nSkinIndex > 2)
		nSkinIndex = 2;

	for (int j = 0; j < m_nHeight; j++)
	{
		for (int i = 0; i < m_nWidth; i++)
		{
			/*
			int nIdx;
			if (j == 0)
			{
				nIdx = GetSkinTileIndex(1) + (nSkinIndex * 3);
				if (i == 0)
					nIdx = GetSkinTileIndex(0) + (nSkinIndex * 3);
				else if (i == m_nWidth - 1)
					nIdx = GetSkinTileIndex(2) + (nSkinIndex * 3);

				m_pStage->RenderTile(x + (i * TILE_XS), y + (j * TILE_YS), nIdx, nColor, nZOrder);
			}
			else
			{
				if (bNotShadow)
					nZOrder = Z_BEHIND_MAP;

				nIdx = GetSkinTileIndex(4) + (nSkinIndex * 3);
				if (i == 0)
					nIdx = GetSkinTileIndex(3) + (nSkinIndex * 3);
				else if (i == m_nWidth - 1)
					nIdx = GetSkinTileIndex(5) + (nSkinIndex * 3);

				m_pStage->RenderTile(x + (i * TILE_XS), y + (j * TILE_YS), nIdx, nColor, nZOrder);
			}
			*/

			if (j != 0 && bNotShadow)
				nZOrder = Z_BEHIND_MAP;

			int nIdx = GetCorrectTileIndex(i, j);
			m_pStage->RenderTile(x + (i * TILE_XS), y + (j * TILE_YS), nIdx, nColor, nZOrder);
		}
	}
}

int MapObjectSemiSolid::GetSubTypedTileIndex(int nIndex)
{
	int nSet[12] =
	{
		TILE_ISLANDHEAD_L,
		TILE_ISLANDHEAD_C,
		TILE_ISLANDHEAD_R,
		TILE_ISLANDBODY_L, // semi solid1 ltt
		TILE_ISLANDBODY_C,
		TILE_ISLANDBODY_R, // semi solid1 rtt
		TILE_SEMI_SOLID1_LBT, // semi solid1 lbt
		TILE_SEMI_SOLID1_CBT, // semi solid1 cbt
		TILE_SEMI_SOLID1_RBT, // semi solid1 rbt
		TILE_SEMI_SOLID1_LBB, // semi solid1 lbb
		TILE_SEMI_SOLID1_CBB, // semi solid1 cbb
		TILE_SEMI_SOLID1_RBB, // semi solid1 rbb
	};

	int nSkinIndex = m_nSubType;
	if (nSkinIndex < 0)
		nSkinIndex = 0;
	if (nSkinIndex > 2)
		nSkinIndex = 2;

	return nSet[nIndex] + (nSkinIndex * 3);
}

int MapObjectSemiSolid::GetCorrectTileIndex(int x, int y)
{
	if (y == 0)
	{
		if (x == 0)
			return GetSubTypedTileIndex(0);
		else if (x == m_nWidth - 1)
			return GetSubTypedTileIndex(2);
		else
			return GetSubTypedTileIndex(1);
	}
	else if (y == m_nHeight - 1)
	{
		if (x == 0)
			return GetSubTypedTileIndex(9);
		else if (x == m_nWidth - 1)
			return GetSubTypedTileIndex(11);
		else
			return GetSubTypedTileIndex(10);
	}
	else
	{
		if (x == 0)
			return GetSubTypedTileIndex(3);
		else if (x == m_nWidth - 1)
			return GetSubTypedTileIndex(5);
		else
		{
			if ((x + y) % 2 == 1)
				return GetSubTypedTileIndex(4);
			else
				return GetSubTypedTileIndex(7);
		}
	}

	return 0;
}
