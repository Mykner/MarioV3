#include "BackgroundUnderground.h"

#include "Stage.h"
#include <string.h>

BackgroundUnderground::BackgroundUnderground(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundUnderground::~BackgroundUnderground()
{
}

void BackgroundUnderground::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	int nGround = GetGroundY();
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;

		// Page1
		CreateDeadBush(nOffset + 7, nGround, 3);

		// Page2
		CreateDeadBone(nOffset + 17, nGround, 1);
		CreateDeadBush(nOffset + 22, nGround, 2);
		CreateGhostDeadMushroom(nOffset + 26, nGround, 1);
		CreateDeadBone(nOffset + 28, nGround, 1);

		// Page3
        CreateGhostDeadMushroom(nOffset + 39, nGround, 1);
	}
}
