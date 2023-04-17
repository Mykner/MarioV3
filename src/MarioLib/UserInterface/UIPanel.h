#pragma once

#include "UIContainer.h"
#include "Common.h"
#include <vector>

class Game;
class Sprite;
class UIPanel : public UIContainer
{
public:
	UIPanel(Game *pGame);
	virtual ~UIPanel();

	virtual void Process();
	virtual void Render();
	void RenderBoxUI();

	virtual void AddUI(UIBase *pUI);

	void RecalcBoxSize();
	void RecalcAlignPos();

	virtual UIBase* MoveFocusPrev();
	virtual UIBase* MoveFocusNext();

	virtual NaRect GetRect();

	bool m_bAutoAlignX;
	bool m_bAutoAlignY;
	bool m_bAutoSize;
	int m_nPanelType;

	enum PanelType
	{
		PANEL_NORMAL,
		PANEL_ROUNDBOX,
		PANEL_ROUNDSHADOWBOX,
		PANEL_MODAL,
	};
};

