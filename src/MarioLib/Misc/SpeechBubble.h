#pragma once

#include "NaRect.h"
#include "NaString.h"

class Game;
class Sprite;
class SpeechBubble
{
public:
	SpeechBubble(Game *pGame);
	virtual ~SpeechBubble();

	void SetPosition(int nX, int nY, int nWidth, int nHeight);
	NaRect GetContentRect(int nX = -1, int nY = -1, int nWidth = -1, int nHeight = -1);
	void Render(int nColor = 0xffffffff);
	void RenderBubble(int nX, int nY, int nWidth, int nHeight, int nColor = 0xffffffff);
	void RenderWithTextAutoSized(int nX, int nY, NaString str, int nColor = 0xffffffff, int nTextColor = 0xff000000);

	union
	{
		int m_nX;
		int x;
	};
	union 
	{
		int m_nY;
		int y;
	};
	int m_nWidth;
	int m_nHeight;
	Sprite *m_pSprite;

	Game *m_pGame;
};

