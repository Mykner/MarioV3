#include "Sprite.h"

#include "RenderManager.h"
#include "SpriteManager.h"
#include "NaPlatformAPI.h"
#include "ResourceHolder.h"
#include "Stage.h"
#include "PathManager.h"

Sprite::Sprite(Game *pGame)
{
	m_pGame = pGame;
	m_pTexture = nullptr;
	m_pTextureHD = nullptr;
	m_pSpritePiece = nullptr;

	m_strGameTheme = L"";
}

Sprite::~Sprite()
{
	if (m_pSpritePiece)
	{
		m_pSpritePiece->m_pTexture = nullptr;
		delete m_pSpritePiece;
	}

	if (m_pTexture)
	{
		if (m_pGame && m_pGame->m_pRenderManager)
		{
			m_pGame->m_pRenderManager->ReleaseTexture(m_pTexture);
		}
		else if (m_pTexture)
		{
			NaDebugOut(L"Error: Sprite has been lost Game* or RenderManager* !!\n");
		}

		delete m_pTexture;
		m_pTexture = nullptr;
	}
}

void Sprite::Load(const wchar_t * szName, const wchar_t* szBitmap, bool bWarn, int nMode)
{
	NaString strName = szName;
	NaString strBitmap;
	if (szBitmap)
		strBitmap = szBitmap;
	else
		strBitmap = szName;

	// Load Texture
	NaString strTexture;
	if (!LoadTexture(szName, strBitmap, strTexture, bWarn))
		return;

	switch (nMode)
	{
	case SPRITE_LOAD_DEFAULT:
		{
			// Load from file
			NaString strInfo;

			// Try Load GameThemed SpriteInfo
			bool bLoaded = false;
			if (m_strGameTheme.GetLength() > 0)
			{
				strInfo.Format(L"%ls\\%ls.sprite", PathManager::GetSpritePath(m_pGame, m_strGameTheme.wstr()), strName.wstr()); // Mario.sprite
				bLoaded = LoadInfo(strInfo);
			}

			// If fails, Load Base SpriteInfo
			if (bLoaded == false)
			{
				strInfo.Format(L"%ls\\%ls.sprite", PathManager::GetSpritePath(m_pGame), strName.wstr()); // Mario.sprite
				LoadInfo(strInfo);
			}
		}
		break;
	case SPRITE_LOAD_TILE16:
		MakeStageTileInfo();
		break;
	case SPRITE_LOAD_TILE16_WITH_PIECE:
		MakeStageTileInfo(true);
		break;
	case SPRITE_LOAD_TILE8:
		MakeFontTileInfo();
		break;
	case SPRITE_LOAD_LAYERIMAGE:
		MakeLayerImageInfo();
		break;
	}

	NaDebugOut(L"Load Sprite: %ls.sprite, %ls.bmp, %d frames",
		strName.wstr(),
		strBitmap.wstr(),
		(int)m_vecFrames.size());

	if (nMode != SPRITE_LOAD_DEFAULT)
		NaDebugOut(L" (Tile)");

	if (m_strGameTheme.GetLength() > 0)
		NaDebugOut(L" (%ls)", m_strGameTheme.wstr());

	NaDebugOut(L"\n");

	if (m_pTextureHD && !m_pTextureHD->IsEmpty())
	{
		NaDebugOut(L"-> HD Texture Found: %ls\n", strTexture.wstr());
	}
}

bool Sprite::LoadTexture(const wchar_t * szName, const wchar_t * szBitmap, NaString &strTexture, bool bWarn)
{
	NaString strBitmap = szBitmap;

	if (m_pTexture != nullptr)
		delete m_pTexture;
	m_pTexture = new TextureBase;

	// Try Load GameThemed Sprite
	if (m_strGameTheme.GetLength() > 0)
	{
		strTexture.Format(L"%ls\\%ls.bmp", PathManager::GetSpritePath(m_pGame, m_strGameTheme.wstr()), strBitmap.wstr());

		// #TODO exception handling. try, catch
		m_pGame->m_pRenderManager->LoadTexture(strTexture.wstr(), m_pTexture);
		if (!m_pTexture->IsEmpty())
		{
			m_pTexture->m_strFilePathName = strTexture;

			// Try Load HD Texture
			{
				strTexture.Format(L"%ls\\%ls@2x.bmp", PathManager::GetSpritePath(m_pGame, m_strGameTheme.wstr()), strBitmap.wstr());

				if (m_pTextureHD != nullptr)
					delete m_pTextureHD;
				m_pTextureHD = new TextureBase;
				m_pGame->m_pRenderManager->LoadTexture(strTexture.wstr(), m_pTextureHD);

				if (m_pTextureHD->IsEmpty())
				{
					delete m_pTextureHD;
					m_pTextureHD = nullptr;
				}
				else
					m_pTextureHD->m_strFilePathName = strTexture;
			}
		}
		else
		{
			// #TODO Must return false. if not load duplacated texture
			return false;
		}
	}

	// If fails, Load Base Sprite
	if (m_pTexture->IsEmpty())
	{
		strTexture.Format(L"%ls\\%ls.bmp", PathManager::GetSpritePath(m_pGame), strBitmap.wstr());
		m_pGame->m_pRenderManager->LoadTexture(strTexture.wstr(), m_pTexture);

		if (m_pTexture->IsEmpty())
		{
			if (bWarn)
				NaDebugOut(L"Error: Fail to load Texture: %ls (%ls) (%ls)\n", 
					szBitmap, szName, strTexture.wstr());
			return false;
		}
		m_pTexture->m_strFilePathName = strTexture;
	}

	// Try Load HD Texture
	if (m_strGameTheme.GetLength() == 0)
	{
		strTexture.Format(L"%ls\\%ls@2x.bmp", PathManager::GetSpritePath(m_pGame), strBitmap.wstr());

		if (m_pTextureHD != nullptr)
			delete m_pTextureHD;
		m_pTextureHD = new TextureBase;
		m_pGame->m_pRenderManager->LoadTexture(strTexture.wstr(), m_pTextureHD);

		if (m_pTextureHD->IsEmpty())
		{
			delete m_pTextureHD;
			m_pTextureHD = nullptr;
		}
		else
			m_pTextureHD->m_strFilePathName = strTexture;
	}

	return true;
}

bool Sprite::LoadInfo(NaString strInfo)
{
	FILE *fp;
	NaPlatform::FileOpen(&fp, strInfo.cstr(), "rb");
	if (fp != nullptr)
	{
		for (; ; )
		{
			int nTemp;
			SpriteFrame f;
			if (EOF ==
#ifdef WIN32
				fscanf_s(
#else
					fscanf(
#endif
						fp, "%d %d %d %d %d %d %d",
						&f.m_rc.left,
						&f.m_rc.top,
						&f.m_rc.right,
						&f.m_rc.bottom,
						&f.m_ptOffset.x,
						&f.m_ptOffset.y,
						&nTemp))
				break;

			m_vecFrames.push_back(f);
		}
		fclose(fp);
		return true;
	}

	return false;
}

void Sprite::ReloadTexture()
{
	if (m_pTexture && !m_pTexture->IsEmpty())
	{
		m_pGame->m_pRenderManager->ReleaseTexture(m_pTexture);
		m_pGame->m_pRenderManager->LoadTexture(m_pTexture->m_strFilePathName.wstr(), m_pTexture);
	}

	if (m_pTextureHD && !m_pTexture->IsEmpty())
	{
		m_pGame->m_pRenderManager->ReleaseTexture(m_pTextureHD);
		m_pGame->m_pRenderManager->LoadTexture(m_pTextureHD->m_strFilePathName.wstr(), m_pTextureHD);
	}
}

void Sprite::MakeStageTileInfo(bool bMakePieceSprite)
{
	// 1: 16x16 Full Tile Sprite
	SIZE s = m_pGame->m_pRenderManager->GetTextureSize(m_pTexture);
	int nTextureW = s.cx;
	int nTextureH = s.cy;
	int nTileW = 16;
	int nTileH = 16;
	for (int y = 0; y < (nTextureH / nTileH); y++)
	{
		for (int x = 0; x < (nTextureW / nTileW); x++)
		{
			SpriteFrame f;
			f.m_rc.left = x * nTileW;
			f.m_rc.top = y * nTileH;
			f.m_rc.right = (x + 1) * nTileW;
			f.m_rc.bottom = (y + 1) * nTileH;
			f.m_ptOffset.x = 8;
			f.m_ptOffset.y = 16;

			m_vecFrames.push_back(f);
		}
	}

	// 2: 8x8 Tile Piece Sprite
	if (bMakePieceSprite)
	{
		m_pSpritePiece = new Sprite(m_pGame);
		m_pSpritePiece->m_strName = m_strName;
		m_pSpritePiece->m_pTexture = m_pTexture;
		nTileW = 16;
		nTileH = 16;
		for (int y = 0; y < (nTextureH / nTileH); y++)
		{
			for (int x = 0; x < (nTextureW / nTileW); x++)
			{
				for (int y2 = 0; y2 < 2; y2++)
				{
					for (int x2 = 0; x2 < 2; x2++)
					{
						SpriteFrame f;
						f.m_rc.left = x * nTileW + x2 * 8;
						f.m_rc.top = y * nTileH + y2 * 8;
						f.m_rc.right = f.m_rc.left + 8;
						f.m_rc.bottom = f.m_rc.top + 8;
						f.m_ptOffset.x = 0;
						f.m_ptOffset.y = 0;

						m_pSpritePiece->m_vecFrames.push_back(f);
					}
				}
			}
		}
	}
}

void Sprite::MakeFontTileInfo()
{
	SIZE s = m_pGame->m_pRenderManager->GetTextureSize(m_pTexture);
	int nTextureW = s.cx;
	int nTextureH = s.cy;
	int nTileW = 8;
	int nTileH = 8;
	for (int y = 0; y < (nTextureH / nTileH); y++)
	{
		for (int x = 0; x < (nTextureW / nTileW); x++)
		{
			SpriteFrame f;
			f.m_rc.left = x * nTileW;
			f.m_rc.top = y * nTileH;
			f.m_rc.right = (x + 1) * nTileW;
			f.m_rc.bottom = (y + 1) * nTileH;
			f.m_ptOffset.x = 0;
			f.m_ptOffset.y = 0;

			m_vecFrames.push_back(f);
		}
	}
}

void Sprite::MakeLayerImageInfo()
{
	SIZE s = m_pGame->m_pRenderManager->GetTextureSize(m_pTexture);
	
	SpriteFrame f;
	f.m_rc.left = 0;
	f.m_rc.top = 0;
	f.m_rc.right = s.cx;
	f.m_rc.bottom = s.cy;
	f.m_ptOffset.x = 0;
	f.m_ptOffset.y = 0;

	m_vecFrames.push_back(f);
}

void Sprite::SetName(NaString & strName)
{
	m_strName = strName;
}

void Sprite::SetGameTheme(NaString & strGameTheme)
{
	m_strGameTheme = strGameTheme;
}

void Sprite::Render(float x, float y, int nFrame, bool bFlip, bool bFlipV, long lColor, float fScaleX, float fScaleY, float fZ, float fAngle, short nAxis)
{
	if (nFrame < 0)
		return;

	if (nFrame >= m_vecFrames.size())
	{
		NaDebugOut(L"Error: SpriteFrame index Out of Range! (name=%ls, index=%d, size=%d)\n", 
			m_strName.wstr(), 
			nFrame, 
			m_vecFrames.size());
		DebugBreak();
		return;
	}

	SpriteFrame frame = m_vecFrames[nFrame];
	/*
	if (fScaleX > 1.0f && fScaleY > 1.0f &&
		m_pTextureHD && !m_pTextureHD->IsEmpty())
		*/

	if (m_pTextureHD && !m_pTextureHD->IsEmpty())
	{
		RECT rc = frame.m_rc;
		rc.left *= 2;
		rc.right *= 2;
		rc.top *= 2;
		rc.bottom *= 2;
		m_pGame->m_pRenderManager->RenderTexture(
			m_pTextureHD,
			x, y,
			rc,
			frame.m_ptOffset.x * 2, frame.m_ptOffset.y * 2,
			bFlip, bFlipV,
			lColor, fScaleX * 0.5f, fScaleY * 0.5f, fZ,
			fAngle,
			nAxis
		);
	}
	else
	{
		m_pGame->m_pRenderManager->RenderTexture(
			m_pTexture,
			x, y,
			frame.m_rc,
			frame.m_ptOffset.x, frame.m_ptOffset.y,
			bFlip, bFlipV,
			lColor, fScaleX, fScaleY, fZ,
			fAngle,
			nAxis
		);
	}	
}

void Sprite::RenderToQueue(
	float x, 
	float y,
	int nFrame, 
	bool bFlip, 
	bool bFlipV, 
	long lColor, 
	float fScaleX, 
	float fScaleY, 
	int nZOrder, 
	bool bShadow, 
	float fAngle,
	short nAxis
#if defined(_DEBUG)
	, char *szSourceFile,
	int nSourceLine
#endif
)
{
	SpriteRenderInfo info;
	info.pSprite = this;
	info.x = x;
	info.y = y;
	info.nFrame = nFrame;
	info.bFlip = bFlip;
	info.bFlipV = bFlipV;
	info.lColor = lColor;
	info.fScaleX = fScaleX;
	info.fScaleY = fScaleY;
	info.nZOrder = nZOrder;
	info.fAngle = fAngle;
	info.nAxis = nAxis;
#if defined(_DEBUG)
	info.szSourceFile = szSourceFile;
	info.nSourceLine = nSourceLine;
#endif

	if (this == nullptr || !IsLoaded() || nFrame == -1)
		return;

	// Temp; SpriteIndex Validation
	if (nFrame >= m_vecFrames.size())
	{
#if defined(USE_FRAMERANGE_BREAK)
		DebugBreak();
#else
		return;
#endif
	}

	m_pGame->m_pSpriteManager->m_vecRenderQueue.push_back(info);

	if (bShadow)
	{
		info.x += 3;
		info.y += 3;

		// #TODO Refactoring shadow code.
		if (m_pGame->m_pResourceHolder && m_pGame->m_pResourceHolder->m_bUseDirectX)
			info.lColor = SpriteDefaults::dwColorShadow;
		else
		{
			if (CUR_STAGE->m_lBackgroundColor == 0 ||
				CUR_STAGE->m_lBackgroundColor == 0xff000000)
				info.lColor = 0xa0555555;
			else
				info.lColor = SpriteDefaults::dwColorShadow;
		}

		info.nZOrder = Z_MAP_SHADOW;

		m_pGame->m_pSpriteManager->m_vecRenderQueue.push_back(info);
	}
}

bool Sprite::IsLoaded()
{
	if (!m_pTexture->IsEmpty() && m_vecFrames.size() > 0)
		return true;

	return false;
}
