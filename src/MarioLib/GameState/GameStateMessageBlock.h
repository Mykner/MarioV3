#pragma once

#include "GameStateBase.h"

class Sprite;
class GameStateMessageBlock : public GameStateBase
{
public:
	GameStateMessageBlock(Game *pGame);
	virtual ~GameStateMessageBlock();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	NaString m_strMessageDisplay;
	NaString m_strMessage;
	int m_nCurrentIndex;
	int m_nColumn;
	int m_nRow;
	bool m_bWaitNextPage;

	Sprite *m_pSprite;
};