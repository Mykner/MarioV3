#include "SuperStar.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "Stage.h"
#include "FModManager.h"

SuperStar::SuperStar(Game *pGame, Stage *pStage) :
	ItemObjectBase(pGame, pStage)
{
	m_nType = ITEM_SUPERSTAR;
	m_nState = STATE_ITEMPOP;
}

SuperStar::~SuperStar()
{
}

void SuperStar::OnWalk()
{
	ChangeState(STATE_JUMP);

	ItemObjectBase::OnWalk();
}

int SuperStar::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	int nFrame = SPRIDX_STAR;
	int nVal = (m_pGame->m_nGameFrame % 8);
	if (nVal < 2)
		nFrame = SPRIDX_STAR;
	else if (nVal < 4)
		nFrame = SPRIDX_STAR2;
	else if (nVal < 6)
		nFrame = SPRIDX_STAR3;
	else if (nVal < 8)
		nFrame = SPRIDX_STAR4;
	
	return nFrame;
}
