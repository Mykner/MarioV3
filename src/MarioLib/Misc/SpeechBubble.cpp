#include "SpeechBubble.h"

#include "Game.h"
#include "Macros.h"
#include "SpriteIndex.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Camera.h"
#include "RenderManager.h"

SpeechBubble::SpeechBubble(Game *pGame)
{
	m_pGame = pGame;
	m_pSprite = SPR_MANAGER->Find(L"Etc");

	m_nX = -1;
	m_nY = -1;
	m_nWidth = 0;
	m_nHeight = 0;
}

SpeechBubble::~SpeechBubble()
{
}

void SpeechBubble::SetPosition(int nX, int nY, int nWidth, int nHeight)
{
	m_nX = nX;
	m_nY = nY;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}

NaRect SpeechBubble::GetContentRect(int nX, int nY, int nWidth, int nHeight)
{
	if (nX == -1)
		nX = m_nX;
	if (nY == -1)
		nY = m_nY;
	if (nWidth == -1)
		nWidth = m_nWidth;
	if (nHeight == -1)
		nHeight = m_nHeight;

	NaRect rc;
	rc.left = nX - nWidth / 2;
	rc.right = nX + nWidth / 2;
	rc.top = nY - 6 - nHeight;
	rc.bottom = nY - 6;
	
	return rc;
}

void SpeechBubble::Render(int nColor)
{
	POINT ptCam = CAMERA->GetPosition();
	int _x = m_nX - ptCam.x;
	int _y = m_nY - ptCam.y;

	RenderBubble(_x, _y, m_nWidth, m_nHeight, nColor);
}

void SpeechBubble::RenderBubble(int nX, int nY, int nWidth, int nHeight, int nColor)
{
	int _x = nX;
	int _y = nY;
	int nZOrder = Z_SPEECHBUBBLE;

	// Bubble Tail
	m_pSprite->RenderToQueue(_x, _y, SPRIDX_ETC_SPEECHBUBBLE_TAIL,
		false, false, nColor, 1.0f, 1.0f, nZOrder, true, 0);

	float fScaleX, fScaleY;

	// Main Box
	fScaleX = nWidth - 8;
	fScaleY = nHeight;
	m_pSprite->RenderToQueue(
		_x - ((nWidth - 8) / 2), 
		_y - 6 - nHeight, 
		SPRIDX_ETC_DOT,
		false, false, nColor, fScaleX, fScaleY, nZOrder, true, 0);

	// LT
	m_pSprite->RenderToQueue(
		_x - ((nWidth - 8) / 2),
		_y - 6 - nHeight,
		SPRIDX_ETC_SPEECHBUBBLE_LT,
		false, false, nColor, 1.0f, 1.0f, nZOrder, true, 0);

	// L
	m_pSprite->RenderToQueue(
		_x - ((nWidth - 8) / 2) - 7,
		_y - 6 - 8,
		SPRIDX_ETC_DOT,

		false, false, nColor, 7.0f, 1.0f, nZOrder, true, 0);
	
	// LB
	m_pSprite->RenderToQueue(
		_x - ((nWidth - 8) / 2),
		_y - 6,
		SPRIDX_ETC_SPEECHBUBBLE_LB,
		false, false, nColor, 1.0f, 1.0f, nZOrder, true, 0);

	// RT
	m_pSprite->RenderToQueue(
		_x + ((nWidth - 8) / 2) + 1,
		_y - 6 - nHeight,
		SPRIDX_ETC_SPEECHBUBBLE_RT,
		false, false, nColor, 1.0f, 1.0f, nZOrder, true, 0);

	// R
	m_pSprite->RenderToQueue(
		_x + ((nWidth - 8) / 2) + 1,
		_y - 6 - 8,
		SPRIDX_ETC_DOT,

		false, false, nColor, 7.0f, 1.0f, nZOrder, true, 0);

	// RB
	m_pSprite->RenderToQueue(
		_x + ((nWidth - 8) / 2) + 1,
		_y - 6,
		SPRIDX_ETC_SPEECHBUBBLE_RB,
		false, false, nColor, 1.0f, 1.0f, nZOrder, true, 0);
}

void SpeechBubble::RenderWithTextAutoSized(int nX, int nY, NaString str, int nColor, int nTextColor)
{
	int _x = nX;
	int _y = nY;

	int nTextLen = str.GetLength();
	int nTextWidth = (nTextLen * 8) - 1;
	int nTextHeight = 7;
	int nWidth = nTextWidth + 8;
	int nHeight = nTextHeight + 8;

	RenderBubble(_x, _y, nWidth, nHeight, nColor);
	NaRect rc = GetContentRect(_x, _y, nTextWidth, nTextHeight);
	rc.Offset(0, -4);

	m_pGame->m_pRenderManager->Text(rc.left, rc.top, str,
		nTextColor, 1.0f, Z_SPEECHBUBBLE_CONTENT, false);
}
