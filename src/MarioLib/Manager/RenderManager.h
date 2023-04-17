#pragma once

#define USE_OPENGL

#include "Sprite.h"
#include "SpriteManager.h"

#include <vector>
#include <fstream>

class TextureBase
{
public:
	TextureBase()
	{
		m_glTexture = 0;
		m_pDXTexture = nullptr;
		m_strFilePathName = L"";
	};
	union 
	{
		unsigned int m_glTexture;
		void* m_pDXTexture;
	};

	bool IsEmpty()
	{
		if (m_glTexture == 0 && m_pDXTexture == nullptr)
			return true;
		return false;
	};

	NaString m_strFilePathName;
};

class Game;
class RenderManager
{
public:
	RenderManager(Game *pGame)
	{
		m_pGame = pGame;
		m_pFontSprite = nullptr;
	}
	virtual ~RenderManager() {};

	virtual void Init() {};
	virtual void Release() {};
	
	virtual void OnSize(int nWidth, int nHeight) {};

	virtual void BeginRenderFrame(COLORREF bgColor = 0) {};
	virtual void EndRenderFrame() {};

	virtual void LoadTexture(const wchar_t* filename, TextureBase *pTexture) {};
	virtual void ReleaseTexture(TextureBase *pTexture) {};
	virtual SIZE GetTextureSize(TextureBase *pTexture) { return SIZE(); };

	virtual void SetDefaultShader() {};
	virtual void SetShadowShader() {};

	virtual void RenderTexture(TextureBase *pTexture,
		float x, float y,
		RECT &rc,
		int offsetX, int offsetY,
		bool bFlip, bool bFlipV,
		int nColor,
		float fScaleX, float fScaleY,
		float fZ,
		float fAngle,
		int nRotationAxis
	) {};

	virtual void TextFont(int x, int y, NaString str) {};
	virtual void Text(
		float x, 
		float y, 
		NaString str, 
		int nColor = 0xffffffff, 
		float fScale = 1.0f, 
		int nZOrder = Z_TEXT, 
		bool bShadow = false)
	{
		if (m_pFontSprite == nullptr)
		{
			m_pFontSprite = m_pGame->m_pSpriteManager->Find(L"Font");
		}

		float fFirstX = x;
		int nLen = str.GetLength();
		int nGameFrame = m_pGame->m_nGameFrame;

		for (int i = 0; i < nLen; i++)
		{
			int n = str[i];

			// $ == Coin
			// # == (c)
			// & == Mushroom
			// ` == Clock
			if (str[i] == L'$') // Animation Coin
			{
				if (nGameFrame % 60 < 10)
					n = 1;
				else if (nGameFrame % 60 < 20)
					n = 2;
				else if (nGameFrame % 60 < 50)
					n = 3;
				else if (nGameFrame % 60 < 60)
					n = 2;
			}
			else if (str[i] == L'\n')
				n = L' ';

			if (bShadow)
			{
				m_pFontSprite->RenderToQueue(x + 1, y + 1, n, false, false, 0x90000000,
					fScale, fScale, nZOrder - 1);
			}
			m_pFontSprite->RenderToQueue(x, y, n, false, false, nColor,
				fScale, fScale, nZOrder);

			x += (8.0f * fScale);

			// new line
			if (str[i] == L'\n')
			{
				y += (8.0f * fScale);
				x = fFirstX;
			}
		}
	};

	Game *m_pGame;
	Sprite* m_pFontSprite;
};