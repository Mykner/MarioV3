#pragma once

#include "UIText.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class UIRectangle : public UIText
{
public:
	UIRectangle(Game *pGame);
	virtual ~UIRectangle();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();
	virtual NaRect GetScreenRect();

	bool m_bShadow;
	bool m_bRenderBox;
	Sprite *m_pSprite;
	int m_nSpriteIndex;
	int m_nWidth;
	int m_nHeight;
	DWORD m_dwTextColor;
	DWORD m_dwFillColor;
};