#pragma once

#include "UIBase.h"
#include "Common.h"
#include <vector>

class Game;
class Sprite;
class UIContainer : public UIBase
{
public:
	UIContainer(Game *pGame);
	virtual ~UIContainer();

	virtual void Process();
	virtual void Render();
	
	void Clear();
	virtual void AddUI(UIBase *pUI);

	int GetFocusedIndex();

	virtual POINT GetChildOffset();

	virtual UIBase* MoveFocusPrev();
	virtual UIBase* MoveFocusNext();

	virtual UIBase* GetFirstFocusAcceptableUI();
	virtual UIBase* GetLastFocusAcceptableUI();

	virtual void EnsureVisible(UIBase *pChild);

	std::vector<UIBase*> m_vecUI;
	
	NaRect m_rc;
	UIBase *m_pFocused;
};

