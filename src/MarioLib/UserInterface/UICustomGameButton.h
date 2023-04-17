#pragma once

#include "UIButton.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class UICustomGameButton : public UIButton
{
public:
	UICustomGameButton(Game *pGame);
	virtual ~UICustomGameButton();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();

	NaString m_strCreator;
	NaString m_strLastStage;
	int m_nLife;
	int m_nCoin;
	int m_nShape;
};

