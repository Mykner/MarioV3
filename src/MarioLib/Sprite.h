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
class SpriteAnimation;
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
    void LoadNewJsonInfo(Json::Value &j, NaString &strInfo, double dVersion);
    void LoadOldJsonInfo(Json::Value &j, NaString &strInfo, double dVersion);
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
    std::map<NaString, SpriteAnimation> m_mapAnimations;
    NaString m_strSubPositions;
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
    std::map<NaString, NaPoint> m_mapSubPositions;
};

class SpriteAnimation
{
public:
    SpriteAnimation() {
        m_nDelay = -1;
        m_bSomething1 = true;
        m_bSomething2 = true;
        m_nSomething3 = 0;
        m_nLength = 0;
    };
    virtual ~SpriteAnimation() {};

    int m_nDelay;
    bool m_bSomething1;
    bool m_bSomething2;
    int m_nSomething3;
    int m_nStartFrame;
    int m_nLength;
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