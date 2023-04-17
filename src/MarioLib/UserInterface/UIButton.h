#pragma once

#include "UIText.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class UIButton : public UIText
{
public:
	UIButton(Game *pGame);
	virtual ~UIButton();

	virtual void Process();
	virtual void Render();

	bool m_bShadow;
	bool m_bRenderBox;
};