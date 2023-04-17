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

    int nGround = GetGroundY();
    for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
    {
        int nOffset = i * GameDefaults::nPageTileWidth * 3;

        CreateLongCoral(nOffset + 3, nGround, 1);
        CreateLongCoral(nOffset + 16, nGround, 1);
        CreateShortCoral(nOffset + 11, nGround, 3);
        CreateShortCoral(nOffset + 23, nGround, 1);
        CreateShortCoral(nOffset + 41, nGround, 2);
    }

	int nVortexBegin = -1;
	int nTileH = m_pStage->m_sizeTile.cy;
	for (int i = 0; i < m_pStage->m_nMaxPage * GameDefaults::nPageTileWidth; i++)
	{
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

    CreateWaterSky();
}
