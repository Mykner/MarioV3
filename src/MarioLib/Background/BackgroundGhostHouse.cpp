#include "BackgroundGhostHouse.h"

#include "Stage.h"
#include <string.h>

BackgroundGhostHouse::BackgroundGhostHouse(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundGhostHouse::~BackgroundGhostHouse()
{
}

void BackgroundGhostHouse::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	if (m_pStage->m_pTileBackgroundData == nullptr)
		m_pStage->m_pTileBackgroundData = new BYTE[m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy];

	m_pData = m_pStage->m_pTileBackgroundData;
    memset(m_pData, 0, sizeof(BYTE) * m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy);

	// Loop 3page pattern
	int nGround = m_pStage->m_sizeTile.cy - 4;
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;

		// Page1
		CreateLamp(nOffset + 4, nGround, 3);
		CreateGhostDeadMushroom(nOffset + 5, nGround, 1);
		CreateLamp(nOffset + 10, nGround, 3);

		// Page2
		CreateClock(nOffset + 12, nGround, 1);
		CreateLamp(nOffset + 17, nGround, 2);
		CreateGhostDeadMushroom(nOffset + 24, nGround, 1);
		CreateClock(nOffset + 27, nGround, 1);

		// Page3
		CreateLamp(nOffset + 35, nGround, 1);
	}
}
