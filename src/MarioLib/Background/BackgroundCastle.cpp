#include "BackgroundCastle.h"

#include "Stage.h"
#include <string.h>

BackgroundCastle::BackgroundCastle(Stage * pStage) :
	BackgroundBase(pStage)
{
	
}

BackgroundCastle::~BackgroundCastle()
{
}

void BackgroundCastle::BuildBackground()
{
	if (m_pStage == nullptr)
		return;

	// Loop 3page pattern
	int nGround = GetGroundY();
	for (int i = 0; i < (m_pStage->m_nMaxPage / 3) + 1; i++)
	{
		int nOffset = i * GameDefaults::nPageTileWidth * 3;

		CreateBowserJrStatue(nOffset + 4, nGround, 3);
		CreateBowserStatue(nOffset + 5, nGround, 3);
		CreateBowserJrStatue(nOffset + 6, nGround, 3);

		CreateCastleFence(nOffset + 16, nGround, 4);

		CreateBowserJrStatue(nOffset + 11, nGround, 3);
		CreateBowserStatue(nOffset + 12, nGround, 3);
		CreateBowserJrStatue(nOffset + 13, nGround, 3);

		CreateCastleFence(nOffset + 36, nGround, 3);
		CreateCastleFence(nOffset + 41, nGround, 3);
	}

    CreateLavaLayer();
}
