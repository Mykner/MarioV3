#include "MysteryMushroom.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "FModManager.h"

MysteryMushroom::MysteryMushroom(Game *pGame, Stage *pStage) :
	Mushroom(pGame, pStage)
{
	m_nType = ITEM_MYSTERYMUSHROOM;
	m_nState = STATE_ITEMPOP;
}

MysteryMushroom::~MysteryMushroom()
{
}

int MysteryMushroom::GetSpriteIndex()
{
	if (m_nState == STATE_ITEMPOP && m_nStateFrame < 30)
		return -1;

	if (m_nState == STATE_ITEMPOPDOWN && m_nStateFrame < 4)
		return -1;

	return SPRIDX_MYSTERYMUSHROOM;
}

const wchar_t* MysteryMushroom::GetTypeName()
{
	const wchar_t* sz = L"MysteryMushroom";
	return sz;
}