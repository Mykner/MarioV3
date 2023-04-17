#include "BackgroundRedCloud.h"

#include "Stage.h"

BackgroundRedCloud::BackgroundRedCloud(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundRedCloud::~BackgroundRedCloud()
{
}

void BackgroundRedCloud::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;
		
		CreateRedCloud(nOffset + 3, 3, 2);
		CreateRedCloud(nOffset + 9, 7, 1);
		CreateRedCloud(nOffset + 18, 2, 2);
		CreateRedCloud(nOffset + 28, 11, 1);
		CreateRedCloud(nOffset + 35, 7, 1);
		CreateRedCloud(nOffset + 38, 6, 1);
		CreateRedCloud(nOffset + 46, 11, 1);
	}
}
