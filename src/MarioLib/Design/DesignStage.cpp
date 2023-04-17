#include "DesignStage.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "RenderManager.h"
#include "GameObjectBase.h"
#include "Player.h"
#include "MapObjectBase.h"
#include "ThemeManager.h"
#include "FModManager.h"

#include "MapObjectBillBlaster.h"
#include "FireBar.h"
#include "MapObjectPipe.h"
#include "MapObjectPipeExit.h"
#include "Themes.h"

#include "UIBase.h"
#include "JsonManager.h"

DesignStage::DesignStage(Game *pGame)
	: Stage(pGame)
{
	m_bIsDesignStage = true;

	m_nCurToolType = TOOLTYPE_PICK;
	m_nCurType = -1;

	m_bDrag = false;
	m_pMapObject = nullptr;
	m_pEnemyObject = nullptr;
	m_pEventObject = nullptr;
	m_pVehicleObject = nullptr;
	m_bIsCurToolNotPlaced = true;

	m_pPlayBotObject = GameObjectBase::CreateInstance(pGame, this, PLAYEROBJECT_MARIO);
	m_pPlayBotObject->SetPosition(40, 9999);
	/*
	m_pPlayBotObject->m_fScaleX = 2.0f;
	m_pPlayBotObject->m_fScaleY = 2.0f;
	*/

	m_nThemeSet = STAGETHEMESET_OVERWORLD;

	m_bShowPlayedShadow = false;

	int nSizeTileData = m_sizeTile.cx * m_sizeTile.cy;
	m_pTileData = new BYTE[nSizeTileData];
	memset(m_pTileData, 0, sizeof(BYTE) * nSizeTileData);

	// Create Default Ground
	int nTileW = m_sizeTile.cx;
	int nTileH = m_sizeTile.cy;
	for (int x = 0; x < nTileW; x++)
	{
		// #TODO change to memset
		SetTileData(x, nTileH - 3, TILE_GROUND);
		SetTileData(x, nTileH - 2, TILE_GROUND);
		SetTileData(x, nTileH - 1, TILE_GROUND);
	}

	BuildBackground();

	m_strTransaction = L"";
}

DesignStage::~DesignStage()
{
	delete m_pPlayBotObject;
	m_pPlayBotObject = nullptr;
}

bool DesignStage::LoadFile(const char * lpszPathName, bool bDecryptIfEncrypted)
{
#if !defined(NDEBUG)
	Json::Value *j = JsonManager::LoadFile(lpszPathName, true);
#else
	Json::Value *j = JsonManager::LoadFile(lpszPathName, false);
#endif
	if (j)
	{
		LoadJsonMap(*j);
		delete j;
		return true;
	}

	return false;
}

void DesignStage::Render()
{
	if (m_pSprite == nullptr)
	{
		InitTheme();
	}
		
	if (m_pSpriteDesign == nullptr)
	{
		m_pSpriteDesign = SPR_MANAGER->Find(L"Design");
	}

	if (m_pSpriteBackground == nullptr)
	{
		m_pSpriteBackground = SPR_MANAGER->Find(L"TileBackground");
	}

	int nTileW = m_sizeTile.cx;
	int nTileH = m_sizeTile.cy;

	RECT rc = CAMERA->GetTileViewport();
	int nBeginX = rc.left;
	int nBeginY = rc.top;
	int nEndX = MIN(rc.right, nTileW);
	int nEndY = MIN(rc.bottom, nTileH);

	RenderFarBackground();

	// Draw Empty First
	for (int x = nBeginX; x < nEndX; x++)
	{
		for (int y = nBeginY; y < nEndY; y++)
		{
			int nData = GetTileData(x, y);
			if (nData == TILE_EMPTY)
				RenderTile(
					x * TILE_XS, 
					y * TILE_YS, 
					nData, 
					0x20ffffff);
		}
	}

	// Background Scene
	for (int x = nBeginX; x < nEndX; x++)
	{
		for (int y = nBeginY; y < nEndY; y++)
		{
			int nData = GetTileBackgroundData(x, y);
			if (nData != TILE_EMPTY)
				RenderTileBackground(x, y, nData);
		}
	}

	// Tile & Item
	for (int x = nBeginX; x < nEndX; x++)
	{
		for (int y = nBeginY; y < nEndY; y++)
		{
			int nData = GetTileData(x, y);
			if (nData != TILE_EMPTY)
				RenderTile(
					(x * TILE_XS), 
					(y * TILE_YS),
					nData);

			if (HasItem(x, y))
			{
				int nIdx = GetTileIndex(x, y);
				std::map<int, int>::iterator it = m_mapItem.find(nIdx);
				
				RenderItem(
					x * TILE_XS + (TILE_XS / 2),
					y * TILE_YS + TILE_YS,
					it->second);
			}
		}
	}

	// Render MapObjects
	{
		std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
		for (; it != m_vecMapObject.end(); ++it)
		{
			MapObjectBase *pObj = *it;
			if (pObj == m_pMapObject)
			{
				if (m_nCurMode == EDIT_DRAW || m_nCurMode == EDIT_DRAGMOVE)
				{
					pObj->Render(0x70ffffff, Z_MAP_SHADOW);
					
					NaRect rc = pObj->GetRect();
					int nX = pObj->m_nX;
					int nY = pObj->m_nY;

					// #TODO MapObject is AutoSizing Object
					pObj->m_nX = m_ptCursor.x - m_ptHandOffset.x;
					pObj->m_nY = m_ptCursor.y - m_ptHandOffset.y;
					pObj->Render(0xffffffff);

					pObj->m_nX = nX;
					pObj->m_nY = nY;
				}
				else
				{
					pObj->Render(0xffffffff);
				}
			}
			else
				pObj->Render();
		}
	}

	// Render Enemies (replaced function)
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
		for (; it != m_vecEnemy.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj == m_pEnemyObject)
			{
				if (m_nCurMode == EDIT_DRAW || m_nCurMode == EDIT_DRAGMOVE)
				{
					pObj->RenderDesign(0x70ffffff, Z_MAP_SHADOW);

					NaRect rc = pObj->GetRect();
					float fX = pObj->m_fX;
					float fY = pObj->m_fY;

					pObj->m_fX = m_ptCursor.x;
					pObj->m_fY = m_ptCursor.y + (rc.Height() / 2);
					pObj->RenderDesign(0xffffffff);

					pObj->m_fX = fX;
					pObj->m_fY = fY;
				}
				else
				{
					pObj->RenderDesign(0xffffffff);
				}
			}
			else
				pObj->RenderDesign();
		}
	}

	// Render EventObjects
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
		for (; it != m_vecEventObject.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj == m_pEventObject)
			{
				if (m_nCurMode == EDIT_DRAW || m_nCurMode == EDIT_DRAGMOVE)
				{
					pObj->RenderDesign(0x70ffffff, Z_MAP_SHADOW);

					NaRect rc = pObj->GetRect();
					float fX = pObj->m_fX;
					float fY = pObj->m_fY;

					pObj->m_fX = m_ptCursor.x - m_ptHandOffset.x;
					pObj->m_fY = m_ptCursor.y - m_ptHandOffset.y;
					pObj->RenderDesign(0xffffffff);

					pObj->m_fX = fX;
					pObj->m_fY = fY;
				}
				else
				{
					pObj->RenderDesign(0xffffffff);
				}
			}
			else
				pObj->RenderDesign();
		}
	}

	// Render VehicleObjects
	{
		std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
		for (; it != m_vecVehicle.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj == m_pVehicleObject)
			{
				if (m_nCurMode == EDIT_DRAW || m_nCurMode == EDIT_DRAGMOVE)
				{
					pObj->RenderDesign(0x70ffffff, Z_MAP_SHADOW);

					NaRect rc = pObj->GetRect();
					float fX = pObj->m_fX;
					float fY = pObj->m_fY;

					pObj->m_fX = m_ptCursor.x - m_ptHandOffset.x;
					pObj->m_fY = m_ptCursor.y - m_ptHandOffset.y;
					pObj->RenderDesign(0xffffffff);

					pObj->m_fX = fX;
					pObj->m_fY = fY;
				}
				else
				{
					pObj->RenderDesign(0xffffffff);
				}
			}
			else
				pObj->RenderDesign();
		}
	}

	// Render played shadow
	if (m_bShowPlayedShadow)
	{
		NaPointT<float> ptOld = { m_pPlayBotObject->m_fX, m_pPlayBotObject->m_fY };
		BEGIN_VECTOR_LOOP(NaPointT<float>, m_vecPlayedShadow) {
			m_pPlayBotObject->m_fX = pObj.x;
			m_pPlayBotObject->m_fY = pObj.y;

			m_pPlayBotObject->Render(0x50ffffff, Z_DESIGN_PLAYBOT);
		} END_VECTOR_LOOP();

		m_pPlayBotObject->m_fX = ptOld.x;
		m_pPlayBotObject->m_fY = ptOld.y;
	}

	// Render PlayBot
	if (GetStartPointInViewport())
		m_pPlayBotObject->Render(0x70ffffff, Z_DESIGN_PLAYBOT);
	else
		m_pPlayBotObject->Render(0xffffffff, Z_DESIGN_PLAYBOT);

	RenderPage();

	// Render Cursor
	RenderBrush();

	if (m_nCurMode != EDIT_DRAW && m_nCurMode != EDIT_DRAGMOVE)
	{
		RenderSelectionIndicator();
	}

	if (m_strTransaction.GetLength() > 0)
	{
		UIBase u(m_pGame);
		NaRect rcBox;
		rcBox.left = 0;
		rcBox.top = 0;
		rcBox.right = m_pGame->m_nWidth;
		rcBox.bottom = m_pGame->m_nHeight;

		u.RenderBox(rcBox, 0xbb000000, 998);
		RENDER_MANAGER->Text(0, 0, m_strTransaction, 0xffffffff, 1.0f, 999, true);
	}
}

void DesignStage::Process()
{
	std::map<int, int>::iterator it = m_mapHitFrame.begin();
	for (; it != m_mapHitFrame.end(); )
	{
		it->second--;
		if (it->second == 0)
		{
			// for fix Runtime Error -> Copy iterator
			m_mapHitFrame.erase(it++);
		}	
		else
		{
			++it;
		}
	}
}

void DesignStage::RenderTile(int x, int y, int nType, long lColor, int nZOrder)
{
	if (m_pSprite == nullptr)
		return;

	int tx = x / TILE_XS;
	int ty = y / TILE_YS;

	NaRect rc = GetTileRect(tx, ty);
	int nIdx = GetTileIndex(tx, ty);

	NaPointT<float> pt = CAMERA->GetPosition();
	rc.Offset(-pt.x, -pt.y);
	x -= pt.x;
	y -= pt.y;

	std::map<int, int>::iterator it = m_mapHitFrame.find(nIdx);
	if (it != m_mapHitFrame.end())
	{
		int nHitFrame = it->second;
		if (nHitFrame < 0)
			nHitFrame = 0;

		const int nHitAnimOffset[16] = { 0, 0, 1, -1, -3, -4, -5, -6, -7, -7, -7, -6, -6, -5, -3, -1 };
		rc.Offset(0, nHitAnimOffset[nHitFrame]);
	}

	nType = GetAnimatedTileIndex(nType);

	if (nType == TILE_EMPTY)
		nZOrder = Z_DESIGN_EMPTYTILE;

	bool bShadow = (nType != TILE_EMPTY) && (m_pGame->m_bModernStyle) && (nZOrder != Z_MAP_SHADOW);

	if (nType == TILE_GROUND)
		nType = GetAutoGroundTileIndex(tx, ty);
	
	m_pSprite->RenderToQueue(x + 8, y + 16, nType, false, false, lColor, 1.0f, 1.0f, nZOrder, bShadow);

	/*
	// For debug tile index
	if (nType != 0)
	{
		NaString str;
		str.Format(L"%d", nType);
		RENDER_MANAGER->Text(
			x,
			y,
			str, 0xffff0000, 1.0f);
	}
	*/
}

void DesignStage::RenderBrush()
{
	if (m_nCurMode == EDIT_SELECTED)
		return;

	// Render Current Tool
	switch (m_nCurToolType)
	{
	case TOOLTYPE_TILE:
		if (m_nCurMode != EDIT_SELECT)
			RenderBrushTile();
		break;
	case TOOLTYPE_MAPOBJECT:
	case TOOLTYPE_ENEMY:
	case TOOLTYPE_EVENTOBJECT:
		// Real Object is Brush
		break;
	case TOOLTYPE_ITEM:
		if (m_nCurMode != EDIT_SELECT)
			RenderBrushItem();
		break;
	}
}

void DesignStage::RenderBrushTile()
{
	// #TODO ZOrder = Shadow
	RenderTile(
		((m_ptCursor.x / TILE_XS) * TILE_XS),
		((m_ptCursor.y / TILE_YS) * TILE_YS),
		m_nCurType, 
		0x70ffffff);

	RenderTile(
		m_ptCursor.x - (TILE_XS / 2), 
		m_ptCursor.y - (TILE_YS / 2), 
		m_nCurType, 
		0xffffffff);
}

void DesignStage::RenderBrushItem()
{
	// #TODO ZOrder = Shadow
	RenderItem(
		((m_ptCursor.x / TILE_XS) * TILE_XS) + (TILE_XS / 2), 
		((m_ptCursor.y / TILE_YS) * TILE_YS) + (TILE_YS),
		m_nCurType, 
		0x70ffffff
	);
	RenderItem(
		m_ptCursor.x, 
		m_ptCursor.y + (TILE_YS / 2), 
		m_nCurType, 
		0xffffffff);
}

void DesignStage::RenderItem(int x, int y, int nType, int nColor)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;

	NaRect rc = GetTileRect(tx, ty);
	int nIdx = GetTileIndex(tx, ty);

	NaPointT<float> pt = CAMERA->GetPosition();
	rc.Offset(-pt.x, -pt.y);
	x -= pt.x;
	y -= pt.y;

	SIZE s = { 16, 16 };

	int nFrame = 0;
	switch (nType)
	{
	case ITEM_COIN:
		nFrame = SPRIDX_COIN;
		break;
	case ITEM_MUSHROOM:
		nFrame = SPRIDX_MUSHROOM;
		break;
	case ITEM_1UP:
		nFrame = SPRIDX_MUSHROOM1UP;
		break;
	case ITEM_FIREFLOWER:
		nFrame = SPRIDX_FLOWER;
		break;
	case ITEM_SUPERSTAR:
		nFrame = SPRIDX_STAR;
		break;
	case ITEM_VINE:
		nFrame = SPRIDX_VINEHEAD;
		break;
	case ITEM_GIANTMUSHROOM:
		nFrame = SPRIDX_MUSHROOM;
		s.cx *= 2;
		s.cy *= 2;
		break;
	case ITEM_YOSHIEGG:
		nFrame = SPRIDX_YOSHIEGG;
		break;
	case ITEM_CLOUD:
		nFrame = SPRIDX_CLOUD;
		break;
	case ITEM_DOORKEYCOIN:
		nFrame = SPRIDX_KEY_COIN;
		break;
	case ITEM_DOORKEY:
		nFrame = SPRIDX_KEY;
		break;
	case ITEM_TURTLESHELL:
		nFrame = SPRIDX_BLUETURTLE_SHELL;
		break;
	default:
		{
			if (nType > ITEM_MYSTERYMUSHROOM &&
				nType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
			{
				//nType
				rc.Offset(TILE_XS / 2, TILE_YS);

				bool bShadow = IS_MODERN;
				int nCostume = (nType - ITEM_MYSTERYMUSHROOM - 1);
				NaString strCostume;
				strCostume.Format(L"Costume%03d", nCostume);
				Sprite *pSprite = SPR_MANAGER->RequestCostume(L"Costume", strCostume.wstr());
				
				pSprite->RenderToQueue(
					x, y, //rc.left, rc.top, 
					0,
					false, false, nColor,
					1.0f * (s.cx / 16),
					1.0f * (s.cy / 16),
					Z_ITEM, bShadow);

				return;
			}
			else
			{
				nFrame = SPRIDX_MYSTERYMUSHROOM;
			}
		}
		break;
	}

	rc.Offset(TILE_XS / 2, TILE_YS);
	
	bool bShadow = IS_MODERN;	
	m_pSpriteItem->RenderToQueue(
		x, y, //rc.left, rc.top, 
		nFrame, 
		false, false, nColor, 
		1.0f * (s.cx / 16), 
		1.0f * (s.cy / 16), 
		Z_ITEM, bShadow);
}

void DesignStage::RenderPage()
{
	NaString str;
	for (int i = 0; i < m_nMaxPage; i++)
	{
		int x = i * GameDefaults::nPageWidth;
		int y = (GameDefaults::nPageTileHeight - 1) * TILE_YS;
		str.Format(L"Page %d", i + 1);
		
		x -= CAMERA->m_ptPos.x;
		m_pGame->m_pRenderManager->Text(x, y, str,
			-1, 1.0f, Z_TEXT, true);
	}
}

void DesignStage::RenderSelectionIndicator()
{
	NaRect rc = { -1, -1, -1, -1 };
	switch (m_nCurToolType)
	{
	case TOOLTYPE_PLAYBOT:
		// Force 16x16
		rc.left = m_pPlayBotObject->m_fX - 8;
		rc.right = m_pPlayBotObject->m_fX + 8;
		rc.top = m_pPlayBotObject->m_fY - 16;
		rc.bottom = m_pPlayBotObject->m_fY;
		break;
	case TOOLTYPE_TILE:
	case TOOLTYPE_ITEM:
		{
			if (m_nCurMode == EDIT_DRAW)
			{
				int x1 = m_ptCursor.x / TILE_XS;
				int y1 = m_ptCursor.y / TILE_YS;

				rc.left = x1 * TILE_XS;
				rc.top = y1 * TILE_YS;
				rc.right = (x1 + 1) * TILE_XS;
				rc.bottom = (y1 + 1) * TILE_YS;
			}
			else
			{
				int x1 = m_ptTileSelected.x;
				int y1 = m_ptTileSelected.y;
				rc.left = (x1 * TILE_XS);
				rc.top = (y1 * TILE_YS);
				rc.right = ((x1 + 1) * TILE_XS);
				rc.bottom = rc.top + TILE_YS;
			}
		}
		break;
	case TOOLTYPE_MAPOBJECT:
		rc = m_pMapObject->GetRect();
		break;
	case TOOLTYPE_ENEMY:
		rc = m_pEnemyObject->GetRect();
		break;
	case TOOLTYPE_EVENTOBJECT:
		rc = m_pEventObject->GetRect();
		break;
	case TOOLTYPE_VEHICLE:
		rc = m_pVehicleObject->GetRect();
		break;
	default:
		return;
		break;
	}

	POINT ptCam = CAMERA->m_ptPos;
	rc.Offset(-ptCam.x, -ptCam.y);

	m_pSpriteDesign->RenderToQueue(rc.left, rc.top, SPRIDX_DESIGN_ANCHOR_LT, false, false, -1L, 1.0f, 1.0f, Z_DESIGN_ANCHOR);
	m_pSpriteDesign->RenderToQueue(rc.right, rc.top, SPRIDX_DESIGN_ANCHOR_RT, false, false, -1L, 1.0f, 1.0f, Z_DESIGN_ANCHOR);
	m_pSpriteDesign->RenderToQueue(rc.left, rc.bottom, SPRIDX_DESIGN_ANCHOR_LB, false, false, -1L, 1.0f, 1.0f, Z_DESIGN_ANCHOR);
	m_pSpriteDesign->RenderToQueue(rc.right, rc.bottom, SPRIDX_DESIGN_ANCHOR_RB, false, false, -1L, 1.0f, 1.0f, Z_DESIGN_ANCHOR);
}

void DesignStage::FillTileData(int x1, int y1, int x2, int y2, int type)
{
	if (x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	for (int x = x1; x <= x2; x++)
	{
		for (int y = y1; y <= y2; y++)
		{
			SetTileData(x, y, type);
		}
	}
}

void DesignStage::FillItemData(int x1, int y1, int x2, int y2, int type)
{
	if (x1 > x2)
	{
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2)
	{
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}

	for (int x = x1; x <= x2; x++)
	{
		for (int y = y1; y <= y2; y++)
		{
			AddItem(x, y, type);
		}
	}
}

void* DesignStage::HitTest(int x, int y, int nToolType)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	int nIdx = GetTileIndex(tx, ty);

	// Vehicle
	if (nToolType == -1 || nToolType == TOOLTYPE_VEHICLE)
	{
		std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
		for (; it != m_vecVehicle.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
				return (void*)pObj;
		}
	}

	// PlayerBot
	if (nToolType == -1 || nToolType == TOOLTYPE_PLAYBOT)
	{
		if (m_pPlayBotObject->HitTest(x, y))
			return m_pPlayBotObject;
	}

	// EvenyObject
	if (nToolType == -1 || nToolType == TOOLTYPE_EVENTOBJECT)
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
		for (; it != m_vecEventObject.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
				return (void*)pObj;
		}
	}

	// Item
	if (nToolType == -1 || nToolType == TOOLTYPE_ITEM)
	{
		std::map<int, int>::iterator it = m_mapItem.find(nIdx);
		if (it != m_mapItem.end())
			return (void*)it->first;
	}

	// Enemy
	if (nToolType == -1 || nToolType == TOOLTYPE_ENEMY)
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
		for (; it != m_vecEnemy.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
				return (void*)pObj;
		}
	}

	// MapObject
	if (nToolType == -1 || nToolType == TOOLTYPE_MAPOBJECT)
	{
		std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
		for (; it != m_vecMapObject.end(); ++it)
		{
			MapObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
				return (void*)pObj;
		}
	}

	// Tile
	if (nToolType == -1 || nToolType == TOOLTYPE_TILE)
	{
		int nTileData = GetTileData(tx, ty);
		if (nTileData != TILE_EMPTY)
			return (void*)nTileData;
	}

	return nullptr;
}

bool DesignStage::Pick(int x, int y)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	int nIdx = GetTileIndex(tx, ty);

	// PlayerBot
	{
		if (m_pPlayBotObject->HitTest(x, y))
		{
			m_nCurToolType = TOOLTYPE_PLAYBOT;
			m_nCurType = PLAYEROBJECT_MARIO;

			return true;
		}
	}

	// EvenyObject
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
		for (; it != m_vecEventObject.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_EVENTOBJECT;
				m_nCurType = pObj->m_nType;
				m_pEventObject = pObj;

				return true;
			}
		}
	}

	// Vehicle
	{
		std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
		for (; it != m_vecVehicle.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_VEHICLE;
				m_nCurType = pObj->m_nType;
				m_pVehicleObject = pObj;

				return true;
			}
		}
	}

	// Item
	{
		std::map<int, int>::iterator it = m_mapItem.find(nIdx);
		if (it != m_mapItem.end())
		{
			m_nCurToolType = TOOLTYPE_ITEM;
			m_nCurType = it->second;

			m_mapItem.erase(it);
			return true;
		}
	}

	// Enemy
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
		for (; it != m_vecEnemy.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_pEnemyObject = pObj;
				m_nCurToolType = TOOLTYPE_ENEMY;
				m_nCurType = pObj->m_nType;

				return true;
			}
		}
	}

	// MapObject
	{
		std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
		for (; it != m_vecMapObject.end(); ++it)
		{
			MapObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_MAPOBJECT;
				m_nCurType = pObj->m_nType;
				m_pMapObject = pObj;

				return true;
			}
		}
	}

	// Tile
	{
		int nTileData = GetTileData(tx, ty);
		if (nTileData != TILE_EMPTY)
		{
			m_nCurToolType = TOOLTYPE_TILE;
			m_nCurType = nTileData;

			SetTileData(tx, ty, TILE_EMPTY);
			return true;
		}
	}

	return false;
}

void DesignStage::ChangeTool(int nToolType, int nType)
{
	// Detach old object
	if (m_pVehicleObject)
	{
		//if (m_pVehicleObject->m_fX == -1 && m_pVehicleObject->m_fY == -1)
		if (m_bIsCurToolNotPlaced)
		{
			std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
			for (; it != m_vecVehicle.end(); ++it)
			{
				if (m_pVehicleObject == *it)
				{
					delete m_pVehicleObject;
					m_vecVehicle.erase(it);
					break;
				}
			}
		}
		m_pVehicleObject = nullptr;
	}

	if (m_pEventObject)
	{
		//if (m_pEventObject->m_fX == -1 && m_pEventObject->m_fY == -1)
		if (m_bIsCurToolNotPlaced)
		{
			std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
			for (; it != m_vecEventObject.end(); ++it)
			{
				if (m_pEventObject == *it)
				{
					delete m_pEventObject;
					m_vecEventObject.erase(it);
					break;
				}
			}
		}
		m_pEventObject = nullptr;
	}

	if (m_pMapObject)
	{
		//if (m_pMapObject->m_nX == -1 && m_pMapObject->m_nY == -1)
		if (m_bIsCurToolNotPlaced)
		{
			std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
			for (; it != m_vecMapObject.end(); ++it)
			{
				if (m_pMapObject == *it)
				{
					delete m_pMapObject;
					m_vecMapObject.erase(it);
					break;
				}
			}
		}
		m_pMapObject = nullptr;
	}

	if (m_pEnemyObject)
	{
		//if (m_pEnemyObject->m_fX == -1 && m_pEnemyObject->m_fY == -1)
		if (m_bIsCurToolNotPlaced)
		{
			std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
			for (; it != m_vecEnemy.end(); ++it)
			{
				if (m_pEnemyObject == *it)
				{
					delete m_pEnemyObject;
					m_vecEnemy.erase(it);
					break;
				}
			}
		}
		m_pEnemyObject = nullptr;
	}

	// Set up
	m_nCurToolType = nToolType;
	m_nCurType = nType;

	m_pEnemyObject = nullptr;
	m_pEventObject = nullptr;
	m_pVehicleObject = nullptr;
	m_pMapObject = nullptr;
	m_bIsCurToolNotPlaced = true;

	// Create new object
	switch (nToolType)
	{
	case TOOLTYPE_MAPOBJECT:
		{
			m_pMapObject = AddMapObject(-1, -1, m_nCurType);
			
			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
		}
		break;
	case TOOLTYPE_ENEMY:
		{
			m_pEnemyObject = AddEnemy(-1, -1, m_nCurType);

			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
		}
		break;
	case TOOLTYPE_EVENTOBJECT:
		{
			m_pEventObject = CreateEventObject(-1, -1, m_nCurType);

			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
		}
		break;
	case TOOLTYPE_VEHICLE:
		{
			m_pVehicleObject = CreateVehicle(-1, -1, m_nCurType);

			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
		}
		break;
	}
}

bool DesignStage::Select(int x, int y)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	int nIdx = GetTileIndex(tx, ty);

	m_pEnemyObject = nullptr;
	m_pMapObject = nullptr;
	m_pEventObject = nullptr;
	m_pVehicleObject = nullptr;

	// PlayerBot
	{
		if (m_pPlayBotObject->HitTest(x, y))
		{
			m_nCurToolType = TOOLTYPE_PLAYBOT;
			m_nCurType = PLAYEROBJECT_MARIO;

			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
			m_bIsCurToolNotPlaced = false;
			return true;
		}
	}

	// Vehicle
	{
		std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
		for (; it != m_vecVehicle.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_VEHICLE;
				m_nCurType = pObj->m_nType;
				m_pVehicleObject = pObj;

				m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
				m_bIsCurToolNotPlaced = false;
				return true;
			}
		}
	}

	// EventObject
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
		for (; it != m_vecEventObject.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_EVENTOBJECT;
				m_nCurType = pObj->m_nType;
				m_pEventObject = pObj;

				m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
				m_bIsCurToolNotPlaced = false;
				return true;
			}
		}
	}

	// Item
	{
		std::map<int, int>::iterator it = m_mapItem.find(nIdx);
		if (it != m_mapItem.end())
		{
			m_nCurToolType = TOOLTYPE_ITEM;
			m_nCurType = it->second;
			m_ptTileSelected = { tx, ty };

			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
			m_bIsCurToolNotPlaced = false;
			return true;
		}
	}

	// Enemy
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
		for (; it != m_vecEnemy.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_ENEMY;
				m_nCurType = pObj->m_nType;
				m_pEnemyObject = pObj;

				m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
				m_bIsCurToolNotPlaced = false;
				return true;
			}
		}
	}

	// MapObject
	{
		std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
		for (; it != m_vecMapObject.end(); ++it)
		{
			MapObjectBase *pObj = *it;
			if (pObj->HitTest(x, y))
			{
				m_nCurToolType = TOOLTYPE_MAPOBJECT;
				m_nCurType = pObj->m_nType;
				m_pMapObject = pObj;

				m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
				m_bIsCurToolNotPlaced = false;
				return true;
			}
		}
	}

	// Tile
	{
		int nTileData = GetTileData(tx, ty);
		if (nTileData != TILE_EMPTY)
		{
			m_nCurToolType = TOOLTYPE_TILE;
			m_nCurType = nTileData;
			m_ptTileSelected = { tx, ty };

			m_ptHandOffset = GetDefaultHandOffset(m_nCurType);
			return true;
		}
	}

	m_nCurToolType = TOOLTYPE_PICK;
	m_nCurType = -1;

	return false;
}

void DesignStage::MoveSelected(int x, int y)
{
	const int nDefaultGridSize = 8;
	
	int _x = x - m_ptHandOffset.x;
	int _y = y - m_ptHandOffset.y;

	int tx = _x / TILE_XS;
	int ty = _y / TILE_YS;

	switch (m_nCurToolType)
	{
	case TOOLTYPE_VEHICLE:
		{
			SIZE s = m_pVehicleObject->GetSize();
			m_pVehicleObject->m_fX = ((_x / nDefaultGridSize) * nDefaultGridSize);// +(s.cx / 2);
			m_pVehicleObject->m_fY = ((_y / nDefaultGridSize) * nDefaultGridSize);// +s.cy;
		}
		break;
	case TOOLTYPE_PLAYBOT:
		m_pPlayBotObject->m_fX = _x;
		m_pPlayBotObject->m_fY = _y;
		break;
	case TOOLTYPE_EVENTOBJECT:
		{
			SIZE s = m_pEventObject->GetSize();
			m_pEventObject->m_fX = ((_x / nDefaultGridSize) * nDefaultGridSize);// +(s.cx / 2);
			m_pEventObject->m_fY = ((_y / nDefaultGridSize) * nDefaultGridSize);// +s.cy;
		}
		break;
	case TOOLTYPE_ENEMY:
		m_pEnemyObject->m_fX = _x / (TILE_XS / 2) * (TILE_XS / 2) + (TILE_XS / 2);
		m_pEnemyObject->m_fY = ty * TILE_YS + TILE_YS;
		break;
	case TOOLTYPE_MAPOBJECT:
		m_pMapObject->m_nX = _x / TILE_XS * TILE_XS;
		m_pMapObject->m_nY = _y / TILE_YS * TILE_YS;

		m_pMapObject->m_nTX = _x / TILE_XS;
		m_pMapObject->m_nTY = _y / TILE_YS;
		break;
	case TOOLTYPE_ITEM:
	case TOOLTYPE_TILE:
		m_ptTileSelected = { tx, ty };
		break;
	}
}

void DesignStage::RemoveSelected()
{
	switch (m_nCurToolType)
	{
	case TOOLTYPE_VEHICLE:
		{
			std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
			for (; it != m_vecVehicle.end(); ++it)
			{
				if (m_pVehicleObject == *it)
				{
					m_vecVehicle.erase(it);
					delete m_pVehicleObject;
					m_pVehicleObject = nullptr;

					PLAYSOUND(L"Kicked");
					break;
				}
			}
		}
		break;
	case TOOLTYPE_EVENTOBJECT:
		{
			std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
			for (; it != m_vecEventObject.end(); ++it)
			{
				if (m_pEventObject == *it)
				{
					m_vecEventObject.erase(it);
					delete m_pEventObject;
					m_pEventObject = nullptr;

					PLAYSOUND(L"BreakBlock");
					break;
				}
			}
		}
		break;
	case TOOLTYPE_ENEMY:
		{
			std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
			for (; it != m_vecEnemy.end(); ++it)
			{
				if (m_pEnemyObject == *it)
				{
					m_vecEnemy.erase(it);
					delete m_pEnemyObject;
					m_pEnemyObject = nullptr;

					PLAYSOUND(L"Kicked");
					break;
				}
			}
		}
		break;
	case TOOLTYPE_MAPOBJECT:
		{
			std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
			for (; it != m_vecMapObject.end(); ++it)
			{
				if (m_pMapObject == *it)
				{
					m_vecMapObject.erase(it);
					delete m_pMapObject;
					m_pMapObject = nullptr;

					PLAYSOUND(L"BreakBlock");
					break;
				}
			}
		}
		break;
	case TOOLTYPE_ITEM:
		{
			int nIdx = GetTileIndex(m_ptTileSelected.x, m_ptTileSelected.y);
			std::map<int, int>::iterator it = m_mapItem.find(nIdx);
			if (it != m_mapItem.end())
			{
				m_mapItem.erase(it);

				PLAYSOUND(L"Kicked");
			}
		}
		break;
	case TOOLTYPE_TILE:
		if (GetTileData(m_ptTileSelected.x, m_ptTileSelected.y) != TILE_EMPTY)
		{
			SetTileData(m_ptTileSelected.x, m_ptTileSelected.y, TILE_EMPTY);
			
			PLAYSOUND(L"BreakBlock");
		}
		break;
	}

	m_ptTileSelected = { -1, -1 };
	m_nCurToolType = TOOLTYPE_PICK;
	m_nCurType = -1;
}

GameObjectBase * DesignStage::GetSelectedObject()
{
	switch (m_nCurToolType)
	{
	case TOOLTYPE_PLAYBOT:
		return m_pPlayBotObject;
		break;
	case TOOLTYPE_TILE:
	case TOOLTYPE_ITEM:
		return nullptr;
		break;
	case TOOLTYPE_MAPOBJECT:
		return nullptr; // m_pMapObject
		break;
	case TOOLTYPE_ENEMY:
		return m_pEnemyObject;
		break;
	case TOOLTYPE_EVENTOBJECT:
		return m_pEventObject;
		break;
	case TOOLTYPE_VEHICLE:
		return m_pVehicleObject;
		break;
	default:
		break;
	}

	return nullptr;
}

void DesignStage::SetSelectedCostume(int nType)
{
	if (m_nCurToolType != TOOLTYPE_ITEM)
		return;

	if (m_nCurType >= ITEM_MYSTERYMUSHROOM &&
		m_nCurType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
	{
		int nIdx = GetTileIndex(m_ptTileSelected.x, m_ptTileSelected.y);
		std::map<int, int>::iterator it = m_mapItem.find(nIdx);
		if (it != m_mapItem.end())
		{
			m_mapItem.erase(it);
			m_mapItem.insert(
				std::pair<int, int>(nIdx, nType)
			);
			m_nCurType = nType;

			PLAYSOUND(L"PowerUp");
		}
	}
}

bool DesignStage::Draw()
{
	switch (m_nCurToolType)
	{
	case TOOLTYPE_TILE:
		{
			SetTileData(
				m_ptCursor.x / TILE_XS,
				m_ptCursor.y / TILE_YS,
				m_nCurType
			);

			return false;
		}
		break;
	case TOOLTYPE_ITEM:
		{
			AddItem(
				m_ptCursor.x / TILE_XS,
				m_ptCursor.y / TILE_YS,
				m_nCurType);			
		}
		break;
	case TOOLTYPE_ERASER:
		{
			int x = m_ptCursor.x;
			int y = m_ptCursor.y;

			// Tile
			SetTileData(
				x / TILE_XS, y / TILE_YS,
				TILE_EMPTY
			);

			// Item
			int nIdx = GetTileIndex(x / TILE_XS, y / TILE_YS);
			std::map<int, int>::iterator it = m_mapItem.find(nIdx);
			if (it != m_mapItem.end())
			{
				m_mapItem.erase(it);
				
				PLAYSOUND(L"Kicked");
			}

			// EvenyObject
			{
				std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
				for (; it != m_vecEventObject.end(); )
				{
					GameObjectBase *pObj = *it;
					if (pObj->HitTest(x, y))
					{
						it = m_vecEventObject.erase(it);
						delete pObj;

						PLAYSOUND(L"BreakBlock");
					}
					else
						++it;
				}
			}

			// Vehicle
			{
				std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
				for (; it != m_vecVehicle.end(); )
				{
					GameObjectBase *pObj = *it;
					if (pObj->HitTest(x, y))
					{
						it = m_vecVehicle.erase(it);
						delete pObj;

						PLAYSOUND(L"Kicked");
					}
					else
						++it;
				}
			}

			// Enemy
			{
				std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
				for (; it != m_vecEnemy.end(); )
				{
					GameObjectBase *pObj = *it;
					if (pObj->HitTest(x, y))
					{
						it = m_vecEnemy.erase(it);
						delete pObj;

						PLAYSOUND(L"Kicked");
					}
					else
						++it;
				}
			}

			// MapObject
			{
				std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
				for (; it != m_vecMapObject.end(); )
				{
					MapObjectBase *pObj = *it;
					if (pObj->HitTest(x, y))
					{
						it = m_vecMapObject.erase(it);
						delete pObj;

						PLAYSOUND(L"BreakBlock");
					}
					else
						++it;
				}
			}
		}
		break;
	case TOOLTYPE_ENEMY:
	case TOOLTYPE_EVENTOBJECT:
		// Nothing to do
		break;
	case TOOLTYPE_MAPOBJECT:
		m_pMapObject->Install();
		break;	
	}

	return true;
}

void DesignStage::DragMove(int x, int y)
{
	int tx = x / TILE_XS;
	int ty = y / TILE_YS;
	switch (m_nCurToolType)
	{
	case TOOLTYPE_MAPOBJECT:
		m_pMapObject->Install();
		break;
	case TOOLTYPE_ITEM:
		AddItem(tx, ty, m_nCurType);
		m_ptTileSelected = { tx, ty };
		break;
	case TOOLTYPE_TILE:
		SetTileData(tx, ty, m_nCurType);
		m_ptTileSelected = { tx, ty };
		break;
	}
}

void DesignStage::OnDragMove()
{

}

POINT DesignStage::GetDefaultHandOffset(int nType)
{
	POINT pt = { 0, 0 };

	switch (nType)
	{
		// MapObject
	case MAPOBJECT_PIPE: 
	case MAPOBJECT_PIPEBODY:
		pt = { 16, 8 }; 
		break;
	case MAPOBJECT_PIPEHORZ: 
		pt = { 8, 16 }; 
		break;
	case MAPOBJECT_ISLAND:
	case MAPOBJECT_MUSHROOMISLAND:
		pt = { 24, 8 };
		break;
	case MAPOBJECT_BILLBLASTER:
	case MAPOBJECT_RIVER:
	case MAPOBJECT_WALL:
	case MAPOBJECT_STARTPOINT:
	case MAPOBJECT_CHECKPOINT:
	case MAPOBJECT_CAMERASTOP:
	case MAPOBJECT_ENEMYSTOP:
	case MAPOBJECT_RIVERBRIDGE:
	case MAPOBJECT_SEMISOLID:
	case MAPOBJECT_MESSAGEBLOCK:
		pt = { 8, 8 };
		break;
		// EventObject
	case EVENTOBJECT_FLAGPOLE:
	case EVENTOBJECT_BOSSBRIDGE:
	case EVENTOBJECT_BOSSBRIDGE_AXE:
	case EVENTOBJECT_PRINCESS:
	case EVENTOBJECT_TRAMPOLIN:
	case EVENTOBJECT_LOOPHANDLER:
	case EVENTOBJECT_LOOPFLAG:
	case EVENTOBJECT_WARPZONEFLAG:
	case EVENTOBJECT_TRAMPOLINMINI:
		pt = { 0, -8 };
		break;
	case EVENTOBJECT_GOALPOINT:
		pt = { 0, -16 };
		break;
	case EVENTOBJECT_BRIDGE:
	case EVENTOBJECT_BOWSERFIRESHOOTER:
	case EVENTOBJECT_SKYBRIDGE:
		pt = { 0, -4 };
		break;
	case EVENTOBJECT_WARPZONETEXT:
		pt = { -8, -16 };
		break;
	case EVENTOBJECT_ENEMYGENERATOR:
		pt = { 8, 8 };
		break;
		// PlayerObject
	case PLAYEROBJECT_MARIO:
		pt = { 0, -8 };
		break;
	}

	return pt;
}

void DesignStage::MovePlayBot(float x, float y)
{
	m_pPlayBotObject->m_fX += x;
	m_pPlayBotObject->m_fY += y;
}

void DesignStage::MovePlayBotToSafePlace()
{
	int nCurrentY = m_pPlayBotObject->m_fY;
	int nGround = (m_sizeTile.cy * GameDefaults::nPageTileHeight) - (TILE_YS * 3);
	if (m_pPlayBotObject->m_fY >= nGround)
		m_pPlayBotObject->m_fY = nGround - 1;

	for ( ; ; )
	{
		if (HitTest(m_pPlayBotObject->m_fX, m_pPlayBotObject->m_fY, TOOLTYPE_MAPOBJECT) || 
			HitTest(m_pPlayBotObject->m_fX, m_pPlayBotObject->m_fY, TOOLTYPE_ENEMY) ||
			HitTest(m_pPlayBotObject->m_fX, m_pPlayBotObject->m_fY, TOOLTYPE_TILE) ||
			HitTest(m_pPlayBotObject->m_fX, m_pPlayBotObject->m_fY, TOOLTYPE_EVENTOBJECT))
			m_pPlayBotObject->m_fY -= 1;
		else
			break;

		if (m_pPlayBotObject->m_fY < 0)
		{
			m_pPlayBotObject->m_fY = nCurrentY;
			break;
		}
	}

	BEGIN_VECTOR_LOOP(Stage*, m_vecSubStage) {
		((DesignStage*)pObj)->MovePlayBotToSafePlace();
	} END_VECTOR_LOOP();
}

MapObjectBase * DesignStage::FindPipeExit(int nIndex)
{
	MapObjectBase *pExitObj = nullptr;

	// Old Style
	int i = 0;
	std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
	for (; it != m_vecMapObject.end(); ++it)
	{
		if ((*it)->m_nType == MAPOBJECT_PIPEEXIT)
		{
			if (nIndex == i)
			{
				pExitObj = *it;
				return pExitObj;
			}

			i++;
		}
	}

	// New Style
	pExitObj = nullptr;
	it = m_vecMapObject.begin();
	for (; it != m_vecMapObject.end(); ++it)
	{
		if ((*it)->m_nType == MAPOBJECT_PIPE ||
			(*it)->m_nType == MAPOBJECT_PIPEHORZ)
		{
			MapObjectPipe *pPipe = (MapObjectPipe*)*it;
			if (pPipe->m_nUseAsExitIndex != -1 &&
				pPipe->m_nUseAsExitIndex == nIndex)
			{
				pExitObj = *it;
				return pExitObj;
			}
		}
	}

	return nullptr;
}

void DesignStage::MovePlayBotToPipeExit(MapObjectBase * pPipeObj)
{
	MapObjectPipe *pPipe = (MapObjectPipe *)pPipeObj;
	if (pPipe)
	{
		if (pPipe->m_nPipeType == PIPE_WARP_TO_BONUSZONE)
		{
			PLAYSOUND(L"PowerDown");
			return;
		}

		if (pPipe->m_nExitIndex == -1)
		{

		}
		else
		{
			MapObjectBase *pExit = pPipe->FindExit(pPipe->m_nExitIndex);
			if (pExit)
			{
				if (pExit->m_nType == MAPOBJECT_PIPEEXIT)
				{
					switch (((MapObjectPipeExit*)pExit)->m_nExitType)
					{
					case PIPEENTRANCE_UPTODOWN:
					default:
						m_pPlayBotObject->SetPosition(pExit->m_nX + 16, pExit->m_nY + 16 - 8);
						break;
					case PIPEENTRANCE_DOWNTOUP:
						m_pPlayBotObject->SetPosition(pExit->m_nX + 16, pExit->m_nY + 16 + 24);
						break;
					case PIPEENTRANCE_LEFTTORIGHT:
						m_pPlayBotObject->SetPosition(pExit->m_nX, pExit->m_nY + 32);
						break;
					case PIPEENTRANCE_RIGHTTOLEFT:
						m_pPlayBotObject->SetPosition(pExit->m_nX + 32, pExit->m_nY + 32);
						break;
					}
				}
				else
				{
					MapObjectPipe *pExitPipe = ((MapObjectPipe*)pExit);
					NaRect rcExit = pExitPipe->GetEntranceRect();
					int nDir = pExitPipe->GetPipeDir(pExitPipe);
					switch (nDir)
					{
					case PIPEENTRANCE_UPTODOWN:
					default:
						m_pPlayBotObject->SetPosition(rcExit.Center(), rcExit.top);
						break;
					case PIPEENTRANCE_DOWNTOUP:
						m_pPlayBotObject->SetPosition(rcExit.Center(), rcExit.bottom + 24);
						break;
					case PIPEENTRANCE_LEFTTORIGHT:
						m_pPlayBotObject->SetPosition(rcExit.left - 24, rcExit.bottom);
						break;
					case PIPEENTRANCE_RIGHTTOLEFT:
						m_pPlayBotObject->SetPosition(rcExit.right + 24, rcExit.bottom);
						break;
					}
				}

				PLAYSOUND(L"PowerDown");
			}
		}
	}
}

MapObjectBase * DesignStage::GetStartPointInViewport()
{
	NaRect rcView = m_pGame->m_pCamera->GetViewport();

	BEGIN_VECTOR_LOOP(MapObjectBase*, m_vecMapObject) {
		if (pObj->m_nType == MAPOBJECT_STARTPOINT)
		{
			if (rcView.ptInRect(
				pObj->m_nTX * TILE_XS + TILE_XS / 2,
				pObj->m_nTY * TILE_YS + TILE_YS))
			{
				return pObj;
			}
			else
				break;
		}
	} END_VECTOR_LOOP();

	return nullptr;
}

void DesignStage::ClearGroundBlocks()
{
	int nTileW = m_sizeTile.cx;
	int nTileH = m_sizeTile.cy;
	for (int x = 0; x < nTileW; x++)
	{
		SetTileData(x, nTileH - 3, TILE_EMPTY);
		SetTileData(x, nTileH - 2, TILE_EMPTY);
		SetTileData(x, nTileH - 1, TILE_EMPTY);
	}
}

bool DesignStage::QuickPowerUp()
{
	if (m_nCurType == -1)
		return false;

	switch (m_nCurToolType)
	{
	case TOOLTYPE_ENEMY:
	case TOOLTYPE_VEHICLE:
		{
			auto pObj = GetSelectedObject();
			if (pObj->HasProperty(L"PowerUp"))
			{
				pObj->m_bPowerUp = !pObj->m_bPowerUp;
				pObj->OnPowerUp();

				if (pObj->m_bPowerUp)
				{
					PLAYSOUND(L"PowerUp");
				}
				else
				{
					PLAYSOUND(L"PowerDown");
				}
				return true;
			}
		}
		break;
	}
	return false;
}

bool DesignStage::QuickToggleWing()
{
	if (m_nCurType == -1)
		return false;

	switch (m_nCurToolType)
	{
	case TOOLTYPE_ENEMY:
	case TOOLTYPE_VEHICLE:
		{
			auto pObj = GetSelectedObject();
			if (pObj->HasProperty(L"Winged"))
			{
				pObj->m_bWinged = !pObj->m_bWinged;
				if (pObj->m_bWinged)
				{
					PLAYSOUND(L"PowerUp");
				}
				else
				{
					PLAYSOUND(L"PowerDown");
				}
				return true;
			}
		}
		break;
	}
	return false;
}

Stage* DesignStage::CreateSubStage()
{
	DesignStage *pNewStage = new DesignStage(m_pGame);

	NaString strName;
	strName.Format(L"Sub%d", (m_vecSubStage.size() + 1));

	pNewStage->m_strName = strName;
	pNewStage->CreateDefaultGround();
	pNewStage->m_pParentStage = this;
	pNewStage->m_bIsSubStage = true;

	AddSubStage(pNewStage);

	return pNewStage;
}

int DesignStage::GetSubStageCount()
{
	Stage *pTopStage = GetTopStage();
	
	return pTopStage->m_vecSubStage.size();
}

int DesignStage::MakeNextPipeExitIndex()
{
	int nIdx = -1;

	MapObjectBase *pExitObj = nullptr;
	int i = 0;
	std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
	for (; it != m_vecMapObject.end(); ++it)
	{
		if ((*it)->m_nType == MAPOBJECT_PIPEEXIT)
		{		
			i++;
		}
	}

	if (i == 0)
	{
		int nMax = -1;
		pExitObj = nullptr;
		it = m_vecMapObject.begin();
		for (; it != m_vecMapObject.end(); ++it)
		{
			if ((*it)->m_nType == MAPOBJECT_PIPE ||
				(*it)->m_nType == MAPOBJECT_PIPEHORZ)
			{
				MapObjectPipe *pPipe = (MapObjectPipe*)*it;
				if (pPipe->m_nUseAsExitIndex >= 0)
				{
					if (pPipe->m_nUseAsExitIndex > nMax)
						nMax = pPipe->m_nUseAsExitIndex;
				}
			}
		}

		nIdx = nMax + 1;
	}
	else
	{
		nIdx = i;
	}

	return nIdx;
}

void* DesignStage::CreateInstance()
{
	return (new DesignStage(m_pGame));
}

void DesignStage::OnMaxVertPageChanged()
{
	Stage::OnMaxVertPageChanged();
}

void DesignStage::SetEditMode(int nMode)
{
	m_nCurMode = nMode;
}

void DesignStage::CheckValid()
{
	// Fix Wrong positioned objects, etc.

	int nFixedPiranha = 0;
	int nFixedBillBlaster = 0;
	BEGIN_VECTOR_LOOP_NOINC(MapObjectBase*, m_vecMapObject) {
		switch (pObj->m_nType)
		{
		case MAPOBJECT_PIPE:
			{
				// Piranha Plant Position

				NaRect rc = pObj->GetRect();
				GameObjectBase *pPiranha = (GameObjectBase*)HitTest(rc.Center(), rc.top - 1, TOOLTYPE_ENEMY);
				if (pPiranha && pPiranha->m_nType == ENEMY_PIRANHA_PLANT)
				{
					if (pPiranha->m_fY == rc.top - 1)
					{
						pPiranha->m_fY = rc.top;
						nFixedPiranha++;
					}
				}
			}
			break;
		case MAPOBJECT_BILLBLASTER:
			{
				MapObjectBillBlaster *pBillBlaster = (MapObjectBillBlaster*)pObj;
				if (pBillBlaster->GetFireType() == ENEMY_BULLETBILL)
				{
					pBillBlaster->m_nChance = 50;
					pBillBlaster->m_nInterval = 5;

					nFixedBillBlaster++;
				}
			}
			break;
		case MAPOBJECT_CAMERASTOP:
			{
				if (pObj->m_nTX == -1 || pObj->m_nTY == -1)
				{
					NaDebugOut(L"Fixed CameraStop point (removed)\n");
					delete pObj;
					it = m_vecMapObject.erase(it);
					continue;
				}
			}
			break;
		}

		++it;
	} END_VECTOR_LOOP();

	int nFixedFireBar = 0;
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEnemy) {
		switch (pObj->m_nType)
		{
		case ENEMY_FIREBAR:
			{
				FireBar* pFireBar = (FireBar*)pObj;
				if (pFireBar->m_fSpeed == 1.875f)
				{
					pFireBar->m_fSpeed = 360.0f / 208.0f;
					nFixedFireBar++;
				}
				else if (pFireBar->m_fSpeed == -1.875f)
				{
					pFireBar->m_fSpeed = -(360.0f / 208.0f);
					nFixedFireBar++;
				}
			}
			break;
		}
	} END_VECTOR_LOOP();

	NaDebugOut(L"Fixed Piranha Plant Position: %d\n", nFixedPiranha);
	NaDebugOut(L"Fixed BillBlaster Chance: %d\n", nFixedBillBlaster);
	NaDebugOut(L"Fixed FireBar Speed: %d\n", nFixedFireBar);
}
