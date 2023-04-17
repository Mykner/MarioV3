#include "BackgroundUnderWater.h"

#include "Stage.h"
#include "Vortex.h"

BackgroundUnderWater::BackgroundUnderWater(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundUnderWater::~BackgroundUnderWater()
{
}

void BackgroundUnderWater::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	if (m_pStage->m_pTileBackgroundData == nullptr)
		m_pStage->m_pTileBackgroundData = new BYTE[m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy];

	m_pData = m_pStage->m_pTileBackgroundData;
	memset(m_pData, 0, sizeof(BYTE) * m_pStage->m_sizeTile.cx * m_pStage->m_sizeTile.cy);

	int nVortexBegin = -1;
	int nTileH = m_pStage->m_sizeTile.cy;
	for (int i = 0; i < m_pStage->m_nMaxPage * GameDefaults::nPageTileWidth; i++)
	{
		SetData(i, 0, TILEBG_WATER_SKY);
		SetData(i, 1, TILEBG_WATER_SKY);
		SetData(i, 2, TILEBG_WATER_T);

		if (!m_pStage->m_bIsDesignStage)
		{
			if (m_pStage->GetTileData(i, nTileH - 1) == TILE_EMPTY)
			{
				if (nVortexBegin == -1)
					nVortexBegin = i;
			}
			else if (nVortexBegin != -1)
			{
				Vortex *pVortex = (Vortex*)m_pStage->CreateEventObject(nVortexBegin * TILE_XS, (nTileH - 1) * TILE_YS, EVENTOBJECT_VORTEX);
				pVortex->m_nWidth = i - nVortexBegin;
				pVortex->m_fX += ((float)pVortex->m_nWidth / 2.0f) * TILE_XS;

				nVortexBegin = -1;
			}
		}
	}
}
