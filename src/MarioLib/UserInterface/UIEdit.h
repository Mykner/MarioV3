#pragma once

#include "UIText.h"

class Game;
class UIEdit : public UIText
{
public:
	UIEdit(Game *pGame);
	virtual ~UIEdit();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();
	virtual NaRect GetMaxRect();
	virtual NaRect GetScreenRect();

	NaString m_strEmptyText;
	NaString m_strTitleText;
	bool m_bAllowAll;
	bool m_bIsPassword;
};

