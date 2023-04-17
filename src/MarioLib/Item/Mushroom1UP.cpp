#include "Mushroom1UP.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"

Mushroom1UP::Mushroom1UP(Game *pGame, Stage *pStage) :
	Mushroom(pGame, pStage)
{
	m_nType = ITEM_1UP;
	m_nState = STATE_ITEMPOP;
}

Mushroom1UP::~Mushroom1UP()
{
}

int Mushroom1UP::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return 3;
}
