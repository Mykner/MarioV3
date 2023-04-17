#pragma once

#include "UIText.h"

#include "NaString.h"
#include "NaRect.h"

class Game;
class Sprite;
class UIImageButton : public UIText
{
public:
	UIImageButton(Game *pGame);
	virtual ~UIImageButton();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();

	Sprite *m_pSprite;
	int m_nSpriteIndex;
	bool m_bText;
	bool m_bSpeechBubble;
	bool m_bRenderBox;
	int m_nWidth;
	int m_nHeight;
};

