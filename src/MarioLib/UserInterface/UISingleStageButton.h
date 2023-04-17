#pragma once

#include "UIButton.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class UISingleStageButton : public UIButton
{
public:
	UISingleStageButton(Game *pGame);
	virtual ~UISingleStageButton();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();

	NaString m_strBestTime;
	NaString m_strBestScore;

	NaString m_strFolder;
	NaString m_strFile;
	NaString m_strPathName;
};

