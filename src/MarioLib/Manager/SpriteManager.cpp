#include "SpriteManager.h"
#include "RenderManager.h"

#include "Stage.h"

#include <algorithm>	// sort
#include <functional>	// sort

SpriteManager::SpriteManager(Game *pGame)
{
	m_pGame = pGame;
}

SpriteManager::~SpriteManager()
{
}

void SpriteManager::Release()
{
	m_vecRenderQueue.clear();

	std::vector<Sprite*>::iterator it = m_vecSprites.begin();
	for (; it != m_vecSprites.end(); )
	{
		Sprite *pSprite = *it;
		delete pSprite;
		it = m_vecSprites.erase(it);
	}

	m_vecSprites.clear();
}

void SpriteManager::Load(const wchar_t * name, int nMode)
{
	NaString strName = name;

	Sprite *pSprite = new Sprite(m_pGame);
	pSprite->SetName(strName);
	pSprite->Load(strName, nullptr, true, nMode);

	m_vecSprites.push_back(pSprite);
}

void SpriteManager::LoadMarioSprite(const wchar_t * name)
{
	wchar_t* szParts[] =
	{
		L"Body",
		L"Tunic",
		L"Pants",
		nullptr,
	};

	for (int i = 0; ; i++)
	{
		if (szParts[i] == nullptr)
			break;

		NaString strName = name;
		NaString strNameWithTheme;
		strNameWithTheme.Format(L"%ls%ls", strName.wstr(), szParts[i]);

		Sprite *pSprite = new Sprite(m_pGame);
		pSprite->SetName(strNameWithTheme);
		pSprite->Load(strName, strNameWithTheme);

		m_vecSprites.push_back(pSprite);
	}
}

void SpriteManager::LoadThemeSprite(const wchar_t * name)
{
	wchar_t* szThemes[] =
	{
		L"Overworld",
		L"Underground",
		L"Castle",
		L"Underwater",
		L"CastleUnderwater",
		L"Snow",
		L"Mushroom",
		L"Gray",
		nullptr,
	};

	for (int i = 0; ; i++)
	{
		if (szThemes[i] == nullptr)
			break;

		NaString strName = name;
		NaString strNameWithTheme;
		strNameWithTheme.Format(L"%ls%ls", strName.wstr(), szThemes[i]);

		Sprite *pSprite = new Sprite(m_pGame);
		pSprite->SetName(strNameWithTheme);
		pSprite->Load(strName, strNameWithTheme, false);
		if (pSprite->m_pTexture->IsEmpty())
		{
			// Load Failed
			delete pSprite;
			continue;
		}
		m_vecSprites.push_back(pSprite);
	}
}

Sprite * SpriteManager::Find(const wchar_t * name, bool bWarn)
{
	std::vector<Sprite*>::iterator it = m_vecSprites.begin();
	for (; it != m_vecSprites.end(); ++it)
	{
		Sprite *pSprite = *it;
		if (pSprite->m_strName == name)
		{
			return pSprite;
		}
	}

	if (bWarn)
	{
		NaDebugOut(L"Error: Cannot Find Sprite: %ls\n", name);
		DebugBreak();
	}

	return nullptr;
}

Sprite * SpriteManager::Request(const wchar_t * name, const wchar_t* bitmap_name, int nMode)
{
	Sprite *pSprite = RequestBase(name, bitmap_name, true, nMode);
	
	if (pSprite == nullptr)
		pSprite = RequestBase(name, bitmap_name, false, nMode);

	return pSprite;
}

Sprite * SpriteManager::RequestBase(const wchar_t * name, const wchar_t * bitmap_name, bool bWithGameTheme, int nMode)
{
	NaString strGameTheme = L"";
	if (m_pGame->m_pCurStage && bWithGameTheme)
		strGameTheme = m_pGame->m_pCurStage->m_strGameTheme;

	std::vector<Sprite*>::iterator it = m_vecSprites.begin();
	for (; it != m_vecSprites.end(); ++it)
	{
		Sprite *pSprite = *it;
		if (pSprite->m_strGameTheme.Compare(strGameTheme) != 0)
			continue;
		if (bitmap_name == nullptr && pSprite->m_strName == name)
			return pSprite;
		if (bitmap_name != nullptr && pSprite->m_strName == bitmap_name)
			return pSprite;
	}

	// If finding fails, try load
	Sprite *pSprite = new Sprite(m_pGame);
	NaString strName(name);
	if (bitmap_name == nullptr)
	{
		pSprite->SetName(strName);
		pSprite->SetGameTheme(strGameTheme);
		pSprite->Load(strName, nullptr, true, nMode);
	}
	else
	{
		NaString strBitmapName;
		strBitmapName.Format(L"%ls", bitmap_name);
		pSprite->SetName(strBitmapName);
		pSprite->SetGameTheme(strGameTheme);
		pSprite->Load(strName, strBitmapName, true, nMode);
	}

	if (pSprite->IsLoaded())
	{
		m_vecSprites.push_back(pSprite);
	}
	else
	{
		delete pSprite;
		pSprite = nullptr;
	}

	return pSprite;
}

Sprite * SpriteManager::RequestThemed(const wchar_t * name, const wchar_t * theme, bool bWithGameTheme, int nMode)
{
	NaString strGameTheme = L"";
	if (m_pGame->m_pCurStage && bWithGameTheme)
		strGameTheme = m_pGame->m_pCurStage->m_strGameTheme;
	
	NaString strName(name);
	NaString strNameWithTheme;
	strNameWithTheme.Format(L"%ls%ls", strName.wstr(), theme);

	std::vector<Sprite*>::iterator it = m_vecSprites.begin();
	for (; it != m_vecSprites.end(); ++it)
	{
		Sprite *pSprite = *it;
		if (pSprite->m_strGameTheme.Compare(strGameTheme) != 0)
			continue;
		if (pSprite->m_strName == name || pSprite->m_strName == strNameWithTheme)
			return pSprite;
	}

	// If finding fails, try load
	Sprite *pSprite = new Sprite(m_pGame);

	pSprite->SetName(strNameWithTheme);
	pSprite->SetGameTheme(strGameTheme);
	pSprite->Load(strName.wstr(), strNameWithTheme.wstr(), false, nMode);
	if (pSprite->IsLoaded())
	{
		m_vecSprites.push_back(pSprite);
	}
	else
	{
		delete pSprite;
		pSprite = nullptr;
	}

	return pSprite;
}

Sprite * SpriteManager::RequestCostume(const wchar_t * name, const wchar_t * bitmap_name)
{
	bool bWithGameTheme = true;
	for (int i = 0; i < 2; i++)
	{
		if (i == 1)
			bWithGameTheme = false;

		NaString strGameTheme = L"";
		if (m_pGame->m_pCurStage && bWithGameTheme)
			strGameTheme = m_pGame->m_pCurStage->m_strGameTheme;

		std::vector<Sprite*>::iterator it = m_vecSprites.begin();
		for (; it != m_vecSprites.end(); ++it)
		{
			Sprite *pSprite = *it;
			if (pSprite->m_strGameTheme.Compare(strGameTheme) != 0)
				continue;
			if (bitmap_name != nullptr && pSprite->m_strName == bitmap_name)
				return pSprite;
		}

		// If finding fails, try load
		Sprite *pSprite = new Sprite(m_pGame);
		
		NaString strName;
		strName.Format(L"%ls", name);

		NaString strBitmapName;
		strBitmapName.Format(L"Costumes\\%ls", bitmap_name);

		NaString strIndexName;
		strIndexName.Format(L"%ls", bitmap_name);

		pSprite->SetName(strIndexName);
		pSprite->SetGameTheme(strGameTheme);
		pSprite->Load(strName, strBitmapName, true, 0);

		if (pSprite->IsLoaded())
		{
			m_vecSprites.push_back(pSprite);
		}
		else
		{
			delete pSprite;
			pSprite = nullptr;
		}
		
		if (pSprite)
			return pSprite;
	}

	return nullptr;
}

void SpriteManager::RenderQueue()
{
	if (m_vecRenderQueue.size() == 0)
		return;

	//sort(m_vecRenderQueue.begin(), m_vecRenderQueue.end());
	stable_sort(m_vecRenderQueue.begin(), m_vecRenderQueue.end(),
		[](SpriteRenderInfo lhs, SpriteRenderInfo rhs) -> bool
		{
			if (lhs.nZOrder < rhs.nZOrder)
				return true;
			return false;
		});

	std::vector<SpriteRenderInfo>::iterator it = m_vecRenderQueue.begin();
	for (; it != m_vecRenderQueue.end(); ++it)
	{
		SpriteRenderInfo info = *it;
		if (info.pSprite == nullptr)
		{
			// ?!
			continue;
		}

		if (info.nZOrder == Z_MAP_SHADOW)
			m_pGame->m_pRenderManager->SetShadowShader();
		else
			m_pGame->m_pRenderManager->SetDefaultShader();

		info.pSprite->Render(
			info.x,
			info.y,
			info.nFrame,
			info.bFlip,
			info.bFlipV,
			info.lColor,
			info.fScaleX,
			info.fScaleY,
			0,
			info.fAngle,
			info.nAxis
		);
	}

	m_vecRenderQueue.clear();
}
