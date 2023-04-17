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

	if (m_pStage->m_pTileBackgroundData == nullptr)
		m_pStage->m_pTileBackgroundData = new BYTE[m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy];

	m_pData = m_pStage->m_pTileBackgroundData;
    memset(m_pData, 0, sizeof(BYTE) * m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy);

	// Loop 3page pattern
	int nGround = m_pStage->m_sizeTile.cy - 4;
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;

		CreateMiniPipe(nOffset + 4, nGround, 3);
		CreateMiniFlag(nOffset + 5, nGround, 3);
		CreateMiniPipe(nOffset + 6, nGround, 3);

		CreateMiniBolt(nOffset + 16, nGround, 4);

		CreateMiniPipe(nOffset + 11, nGround, 3);
		CreateMiniFlag(nOffset + 12, nGround, 3);
		CreateMiniPipe(nOffset + 13, nGround, 3);

		CreateMiniBolt(nOffset + 36, nGround, 3);
		CreateMiniBolt(nOffset + 41, nGround, 3);
	}
}
