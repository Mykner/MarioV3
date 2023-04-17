#pragma once

#if defined(WIN32) && defined(SUPPORT_DIRECTX) 

#include "RenderManager.h"
#include <Windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "Common.h"
#include "Sprite.h"

class Game;
class Sprite;
class DirectXManager : public RenderManager
{
public:
	DirectXManager(Game *pGame);
	virtual ~DirectXManager();

	virtual void Init();
	virtual void Release();

	virtual void OnSize(int nWidth, int nHeight);

	virtual void BeginRenderFrame(COLORREF bgColor = 0);
	virtual void EndRenderFrame();

	virtual void LoadTexture(const wchar_t *szFilename, TextureBase *pTexture);
	virtual void ReleaseTexture(TextureBase *pTexture);
	virtual SIZE GetTextureSize(TextureBase *pTexture);
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
		);

	virtual void TextFont(int x, int y, NaString str);
	
	LPDIRECT3D9 m_pDX;
	LPDIRECT3DDEVICE9 m_pDevice;
	LPD3DXSPRITE m_pSprite;
	LPDIRECT3DTEXTURE9 m_pTexture;
	ID3DXFont *m_pFont;
};

#endif