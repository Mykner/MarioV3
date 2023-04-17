
#pragma once

#include "NaCommon.h"
#include "SpriteIndex.h"

enum UITypes
{
	UI_BASE = 0, // BaseType
	UI_TEXT,
	UI_BUTTON,
	UI_IMAGEBUTTON,
	UI_EDIT,
	UI_SINGLESTAGEBUTTON,
	UI_CUSTOMGAMEBUTTON,
	UI_WORLDSTAGEBUTTON,
	UI_RECTANGLE,
	
	UI_CONTAINER, // Container BaseType
	UI_PANEL,
	UI_LIST,
};

class Game;
class NaRect;
class UIContainer;
class Sprite;
class UIBase
{
public:
	UIBase(Game *pGame);
	virtual ~UIBase();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();
	virtual NaRect GetMaxRect();
	virtual NaRect GetScreenRect();

	virtual bool CanAcceptFocus();
	virtual bool IsFocused();

	POINT GetScreenOffset();
	void RenderBox(NaRect rc, int nColor, int nZOrder = -1);
	void RenderRoundBox(NaRect rc, int nColor, int nZOrder = -1);

	int m_nID;
	int m_nX;
	int m_nY;
	bool m_bFocusAccept;
	bool m_bEnable;
	bool m_bVisible;
	int m_nType;

	UIContainer* m_pParent;
	Game* m_pGame;
	Sprite *m_pSprite;
};

