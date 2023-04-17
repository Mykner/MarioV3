#include "BackgroundAirShip.h"

#include "Stage.h"
#include <string.h>

BackgroundAirShip::BackgroundAirShip(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundAirShip::~BackgroundAirShip()
{
}

void BackgroundAirShip::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	int nGround = GetGroundY();
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;

		CreateMiniPipe(nOffset + 4, nGround, 3);
		CreateMiniFlag(nOffset + 7, nGround, 3);
        CreateMiniFence(nOffset + 9, nGround, 3);
		CreateMiniPipe(nOffset + 11, nGround, 3);

		CreateMiniBolt(nOffset + 16, nGround, 4);

		CreateMiniFlag(nOffset + 19, nGround, 3);
        CreateMiniFence(nOffset + 20, nGround, 3);
		CreateMiniPipe(nOffset + 25, nGround, 3);

		CreateMiniBolt(nOffset + 36, nGround, 3);
		CreateMiniBolt(nOffset + 41, nGround, 3);
	}
}
