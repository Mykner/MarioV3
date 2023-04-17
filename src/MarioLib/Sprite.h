#pragma once

#include "Common.h"

#include <vector>

#include "Game.h"

enum SpriteLoadModes
{
	SPRITE_LOAD_DEFAULT = 0,
	SPRITE_LOAD_TILE16,
	SPRITE_LOAD_TILE16_WITH_PIECE,
	SPRITE_LOAD_TILE8,
	SPRITE_LOAD_LAYERIMAGE,
};

class Game;
class SpriteFrame;
class SpriteRenderInfo;
class TextureBase;
class Sprite
{
public:
	Sprite(Game *pGame);
	virtual ~Sprite();

	void Load(const wchar_t* szName, const wchar_t* szBitmap = nullptr, bool bWarn = true, int nMode = SPRITE_LOAD_DEFAULT);
	bool LoadTexture(const wchar_t * szName, const wchar_t* szBitmap, NaString &strTexture, bool bWarn);
	bool LoadInfo(NaString strInfo);
	void ReloadTexture();
	void MakeStageTileInfo(bool bMakePieceSprite = false);
	void MakeFontTileInfo();
	void MakeLayerImageInfo();

	void SetName(NaString &strName);
	void SetGameTheme(NaString &strGameTheme);
	void Render(float x, float y, int nFrame, bool bFlip = false, bool bFlipV = false, long lColor = 0xffffffff, float fScaleX = 1.0f, float fScaleY = 1.0f, float fZ = 0, float fAngle = 0.0f, short nAxis = 0);
	void RenderToQueue(
		float x, 
		float y, 
		int nFrame, 
		bool bFlip = false, 
		bool bFlipV = false, 
		long lColor = 0xffffffff, 
		float fScaleX = 1.0f, 
		float fScaleY = 1.0f, 
		int nZOrder = 0, 
		bool bShadow = false, 
		float fAngle = 0.0f,
		short nAxis = 0
#if defined(_DEBUG)
		, char *szSourceFile = nullptr,
		int nSourceLine = -1
#endif
	);

	bool IsLoaded();

	Sprite *m_pSpritePiece;
	NaString m_strName;
	NaString m_strGameTheme;
	TextureBase *m_pTexture;
	TextureBase *m_pTextureHD;
	std::vector<SpriteFrame> m_vecFrames;
	Game *m_pGame;
};

class SpriteFrame
{
public:
	SpriteFrame() {
		m_rc = { 0, 0, 0, 0 };
		m_ptOffset = { 0, 0 };
	};
	virtual ~SpriteFrame() {};

	RECT m_rc;
	POINT m_ptOffset;
};

class SpriteRenderInfo
{
public:
	Sprite *pSprite;
	float x, y;
	int nFrame;
	bool bFlip, bFlipV;
	long lColor;
	float fScaleX, fScaleY;
	int nZOrder;
	float fAngle;
	short nAxis;
#if defined(_DEBUG)
	char* szSourceFile;
	int nSourceLine;
#endif

	// For sort
#ifdef WIN32
	int operator <(SpriteRenderInfo &other)
	{
		return nZOrder < other.nZOrder;
	};
#else
    bool operator <(const SpriteRenderInfo &other) const
    {
        return (nZOrder < other.nZOrder) ? true : false;
    };
#endif
};