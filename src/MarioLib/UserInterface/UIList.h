#pragma once

#include "UIContainer.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class UIList : public UIContainer
{
public:
	UIList(Game *pGame);
	virtual ~UIList();

	virtual void Process();
	virtual void Render();

	virtual UIBase* MoveFocusPrev();
	virtual UIBase* MoveFocusNext();

	virtual POINT GetChildOffset();

	bool NeedScrollBar();
	bool IsVisibleChild(UIBase *pChild);
	virtual void EnsureVisible(UIBase *pChild);

	virtual bool CanAcceptFocus();
	virtual NaRect GetRect();

	int m_nWidth;
	int m_nHeight;

	int m_nTargetScrollPos;
	int m_nCurrentScrollPos;

	int m_nItemHeight;
};

