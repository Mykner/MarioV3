#include "BackgroundSnowFence.h"

#include "Stage.h"

BackgroundSnowFence::BackgroundSnowFence(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundSnowFence::~BackgroundSnowFence()
{
}

void BackgroundSnowFence::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	int nGround = GetGroundY();
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;
		
		// Page1
		CreateCloud(nOffset + 0, 3, 2);
		CreateSnowTree(nOffset + 11, nGround, 1);
		CreateSnowTree(nOffset + 13, nGround, 2);
		CreateFence(nOffset + 14, nGround, 4);

		// Page2
		CreateCloud(nOffset + 18, 3, 1);
		CreateSnowTree(nOffset + 21, nGround, 2);
		CreateSnowTree(nOffset + 23, nGround, 1);
		CreateSnowTree(nOffset + 24, nGround, 1);
		CreateCloud(nOffset + 27, 2, 1);
		CreateCloud(nOffset + 30, 3, 2);

		// Page3
		CreateFence(nOffset + 38, nGround, 2);
		CreateSnowTree(nOffset + 40, nGround, 1);
		CreateFence(nOffset + 41, nGround, 1);
		CreateSnowTree(nOffset + 43, nGround, 2);
		CreateCloud(nOffset + 45, 2, 1);
	}
}
