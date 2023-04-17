#pragma once

#include "UIBase.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class UIText : public UIBase
{
public:
	UIText(Game *pGame);
	virtual ~UIText();

	virtual void Render();

	virtual NaRect GetRect();
	virtual NaRect GetScreenRect();

	NaString m_strText;
	DWORD m_dwTextColor;
};
