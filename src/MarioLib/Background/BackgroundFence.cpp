#include "BackgroundFence.h"

#include "Stage.h"

BackgroundFence::BackgroundFence(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundFence::~BackgroundFence()
{
}

void BackgroundFence::BuildBackground()
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
		CreateCloud(nOffset + 0, 3, 2);
		CreateTree(nOffset + 11, nGround, 1);
		CreateTree(nOffset + 13, nGround, 2);
		CreateFence(nOffset + 14, nGround, 4);

		// Page2
		CreateCloud(nOffset + 18, 3, 1);
		CreateTree(nOffset + 21, nGround, 2);
		CreateTree(nOffset + 23, nGround, 1);
		CreateTree(nOffset + 24, nGround, 1);
		CreateCloud(nOffset + 27, 2, 1);
		CreateCloud(nOffset + 30, 3, 2);

		// Page3
		CreateFence(nOffset + 38, nGround, 2);
		CreateTree(nOffset + 40, nGround, 1);
		CreateFence(nOffset + 41, nGround, 1);
		CreateTree(nOffset + 43, nGround, 2);
		CreateCloud(nOffset + 45, 2, 1);
	}
}
