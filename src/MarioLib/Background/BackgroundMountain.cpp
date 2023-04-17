#include "BackgroundMountain.h"

#include "Stage.h"
#include <string.h>

BackgroundMountain::BackgroundMountain(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundMountain::~BackgroundMountain()
{
}

void BackgroundMountain::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	int nGround = GetGroundY();
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;
		
		if (m_pStage->m_sizeTile.cy > 16)
		{
			for (int j = 0; j < m_pStage->m_sizeTile.cy - 16; j += 16)
			{
				CreateCloud(nOffset + (rand() % 32), (j) + 3, (rand() % 3) + 1);
				CreateCloud(nOffset + (rand() % 32), (j) + 6, (rand() % 3) + 1);
				CreateCloud(nOffset + (rand() % 32), (j) + 9, (rand() % 3) + 1);
				CreateCloud(nOffset + (rand() % 32), (j) + 12, (rand() % 3) + 1);
			}
		}

		CreateMountain(nOffset + 0, nGround, 2);
		CreateMountain(nOffset + 16, nGround, 1);
		CreateCloud(nOffset + 8, nGround - 9, 1);
		CreateCloud(nOffset + 19, nGround - 10, 1);
		CreateCloud(nOffset + 27, nGround - 9, 3);
		CreateCloud(nOffset + 36, nGround - 10, 2);
		CreateBush(nOffset + 11, nGround, 3);
		CreateBush(nOffset + 23, nGround, 1);
		CreateBush(nOffset + 41, nGround, 2);
	}
}
