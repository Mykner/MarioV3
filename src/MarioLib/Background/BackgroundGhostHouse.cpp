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

	// Loop 3page pattern
	int nGround = GetGroundY();
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;

		// Page1
        CreateGhostHouseFence(nOffset, nGround, 3);
		CreateLamp(nOffset + 4, nGround, 3);
		CreateGhostDeadMushroom(nOffset + 5, nGround, 1);
		CreateLamp(nOffset + 10, nGround, 3);

		// Page2
		CreateClock(nOffset + 12, nGround, 1);
		CreateLamp(nOffset + 17, nGround, 2);
        CreateGhostHouseFence(nOffset + 19, nGround, 3);
		CreateGhostDeadMushroom(nOffset + 24, nGround, 1);
		CreateClock(nOffset + 27, nGround, 1);
        CreateGhostHouseFence(nOffset + 29, nGround, 3);

		// Page3
		CreateLamp(nOffset + 35, nGround, 1);
	}
}
