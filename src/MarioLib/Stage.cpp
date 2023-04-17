#include "Stage.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "RenderManager.h"
#include "GameObjectBase.h"
#include "MapObjectBase.h"
#include "Player.h"
#include "PipeInfo.h"
#include "FModManager.h"
#include "PropertyMap.h"
#include "MapObjectPipe.h"
#include "json.h"
#include "TypeNames.h"
#include "ThemeManager.h"
#include "BackgroundBase.h"
#include "ItemObjectBase.h"
#include "NaPlatformAPI.h"
#include "VehicleObjectBase.h"
#include "EnemyObjectBase.h"
#include "SoundManager.h"
#include "EnemyGenerator.h"
#include "Themes.h"
#include "PathManager.h"
#include "JsonManager.h"
#include <sstream>
#include <iostream>
#include <iomanip>

#include "UIBase.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(Stage)
	PROP_INT("ThemeSet",			VT_I4,			STAGETHEMESET_OVERWORLD,		g_szStageThemeSet, "Theme"),
	PROP_INT("BackgroundTheme",		VT_I4,			STAGEBGTHEME_MOUNTAIN,			g_szStageBackgroundTheme, "Theme"),
	PROP_FLOAT("Gravity",			VT_R4,			GameDefaults::fDefaultGravity,	0, "Map"),
	PROP_LONG("BackgroundColor",	VT_ETC_COLOR,	0xFF5080FF,						0, "Theme"),
	PROP_INT("MaxPage",				VT_I4,			16,								0, "Map"),
	PROP_INT("MaxVerticalPage",		VT_I4,			1,								0, "Map"),
	PROP_INT("Theme",				VT_I4,			STAGETHEME_OVERWORLD,			g_szStageTheme, "Map"),
	PROP_BOOL("CanGoBack",			VT_BOOL,		true,							0, "Map"),
	PROP_INT("AutoScroll",			VT_I4,			AUTOSCROLL_NONE,				g_szAutoScrollType, "Map"),
	PROP_INT("EntranceType",		VT_I4,			STAGEENTRANCE_NONE,				g_szStageEntranceType, "Map"),
	PROP_STR("NextStageName",		VT_ETC_NASTR,	L"",							0, "Map"),
	PROP_STR("SkyStageName",		VT_ETC_NASTR,	L"",							0, "Map"),
	PROP_INT("Time",				VT_I4,			400,							0, "Map"),
	PROP_STR("Creator",				VT_ETC_NASTR,	L"Unknown",						0, "Map"),
	PROP_INT("AbilityFlag",			VT_I4,			0,								0, "Map"),
#if !defined(NDEBUG)
	PROP_INT("InitialShape",		VT_I4,			0,								0, "Map"),
#endif
	PROP_STR("UniqueId",			VT_ETC_NASTR,	L"",							0, "Map"),
	PROP_STR("Title",				VT_ETC_NASTR,	L"",							0, "Map"),
	PROP_INT("Revision",			VT_I4,			1,								0, "Map"),
	PROP_STR("CustomThemeName",		VT_ETC_NASTR,	L"",							0, "Customize"),
	PROP_STR("GameTheme",			VT_ETC_NALIST,	L"",							0, "Customize"),
END_IMPL_PROPERTYMAP()

Stage::Stage(Game *pGame)
{
	m_pGame = pGame;

	m_pTileData = nullptr;
	m_pTileBackgroundData = nullptr;

	m_pSprite = nullptr;
	m_pSpritePiece = nullptr;
	m_pSpriteBackground = nullptr;
	m_pSpriteFarBackground = nullptr;
	m_pSpriteItem = nullptr;
	m_pSpriteEnemy = nullptr;
	m_pSpriteEffect = nullptr;
	m_pSpriteMapObject = nullptr;

	m_nMaxPage = 16;
	m_nMaxVertPage = 1;

	m_sizeTile.cx = GameDefaults::nPageTileWidth * m_nMaxPage;
	m_sizeTile.cy = GameDefaults::nPageTileHeight * m_nMaxVertPage;
	m_bIsDesignStage = false;

	m_fGravity = GameDefaults::fDefaultGravity;
	m_lBackgroundColor = 0xff7686ff; // Changed from 0xFF5080FF (SMB1)
	m_bCanGoBack = true;
	m_nEntranceType = STAGEENTRANCE_NONE;

	m_strGameTheme = L"";
	m_nThemeSet = STAGETHEMESET_OVERWORLD;
	m_nTheme = STAGETHEME_OVERWORLD;
	m_nBackgroundTheme = STAGEBGTHEME_MOUNTAIN;
	m_strCustomThemeName = L"";

    m_pParentStage = nullptr;
	m_pCurPipe = nullptr;
	m_strNextStageName = L"";
	m_strSkyStageName = L"";

	m_nTime = 400;
	m_strCreator = L"Unknown";	

	m_nPrevJumpPage = -1;
	m_nNextJumpPage = -1;

	m_strLastCheckPoint = L"";
	m_ptLastCheckPoint = { -1, -1 };
	m_nAutoScroll = 0;
	m_nAbilityFlag = ABILITY_NONE;
	m_nInitialShape = -1;
	m_nStateFrame = 0;

	m_dwBeginTime = 0;
	m_dwPlayTime = 0;
	m_nTryCount = 0;
	m_bIsSubStage = false;
	m_nMaxDoorKey = 0;
	m_bSwapCoinBlock = false;
	m_nReqSwapCoinBlock = 0;

	// For Stage Server
	m_strUniqueId = L"";
	m_strTitle = L"";
	m_nRevision = 1;

	BuildPropertyMap();
	m_ThemeSetting.BuildPropertyMap();
}

Stage::~Stage()
{
	Clear();
}

void Stage::Clear()
{
	if (m_strGameTheme.GetLength() > 0)
		m_ThemeSetting.Clear();

	if (CAMERA)
		CAMERA->StopEarthquake();

	m_strUniqueId = L"";
	m_strTitle = L"";
	m_nRevision = 1;

	m_nReqSwapCoinBlock = 0;
	m_bSwapCoinBlock = false;
	m_nMaxPage = 16;
	m_nMaxVertPage = 1;
	m_sizeTile.cx = GameDefaults::nPageTileWidth * m_nMaxPage;
	m_sizeTile.cy = GameDefaults::nPageTileHeight * m_nMaxVertPage;

	m_pCurPipe = nullptr;
	m_vecSwimArea.clear();
	m_strSkyStageName = L"";
	m_strNextStageName = L"";
	m_ptEnemyStop.x = GameDefaults::nMaxCameraPosX;
	
	m_nPrevJumpPage = -1;
	m_nNextJumpPage = -1;

	m_strLastCheckPoint = L"";
	m_ptLastCheckPoint = { -1, -1 };

	m_dwPlayTime = 0;
	m_nTryCount = 0;
	m_nTime = 400;
	m_strCreator = L"Unknown";

	m_strGameTheme = L"";
	m_nThemeSet = STAGETHEMESET_CUSTOM;
	m_nTheme = STAGETHEME_OVERWORLD;
	m_nBackgroundTheme = STAGEBGTHEME_MOUNTAIN;
	m_nMaxDoorKey = 0;
	m_nAutoScroll = 0;
	m_nAbilityFlag = ABILITY_NONE;
	m_nStateFrame = 0;

	ClearSubStages();

	SAFE_DELETE(m_pTileData);
	SAFE_DELETE(m_pTileBackgroundData);

	m_mapItem.clear();
	m_mapHitFrame.clear();
	m_mapPressFrame.clear();
	m_mapMessage.clear();
	
	SAFE_DELETE_OBJPTR_VECTOR(PipeInfo*, m_vecPipeInfo);
	SAFE_DELETE_OBJPTR_VECTOR(PipeExitInfo*, m_vecPipeExitInfo);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecEnemy);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecItem);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecEffect);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecBullet);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecEventObject);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecVehicle);
	SAFE_DELETE_OBJPTR_VECTOR(GameObjectBase*, m_vecReservedGameObject);
}

bool Stage::Load(const wchar_t * name)
{
	NaString strName;
	strName.Format(L"%ls\\%ls\\%ls.map", 
		PathManager::GetMapPath(m_pGame),
		m_pGame->m_strStagePack.wstr(),
		name);
	NaDebugOut(L"Load Stage: %ls (%ls)\n", m_strName.wstr(), strName.wstr());

	Clear();

	m_strName = name;

	// Init TileData
	CreateDefaultGround();

	m_ptStartPoint = { -1, -1 };
	m_ptStopPoint = { GameDefaults::nDefaultStopPointX, 0 };
	m_ptEnemyStop.x = GameDefaults::nMaxCameraPosX;

	m_nPrevJumpPage = -1;
	m_nNextJumpPage = -1;

	if (LoadFile(strName.cstr()))
	{
		OnLoad();
		return true;
	}

	return false;
}

bool Stage::LoadFile(const char* lpszPathName, bool bDecryptIfEncrypted /*= true*/)
{
	Json::Value *j = JsonManager::LoadFile(lpszPathName, bDecryptIfEncrypted);
	if (j)
	{
		LoadJsonMap(*j);
		delete j;

		return true;
	}

	return false;
}

void Stage::LoadJsonMap(Json::Value & j)
{
	if (m_pTileData)
		delete[] m_pTileData;

	// Lets Load
	NaString strVersionKey = L"Neoarc's Mario Map v";
	double dVersion = 0;

	std::string strVer = j["Version"].asString();
	NaString strVersion = strVer.c_str();
	if (strVersion.Left(strVersionKey.GetLength()).Compare(strVersionKey) == 0)
	{
		int nIdx = strVersionKey.GetLength();
		strVersion = strVersion.Mid(nIdx);
		dVersion = atof(strVersion.cstr());
	}

	// Base
	JsonManager::LoadJsonObjectProperty(this, j);
	m_sizeTile.cx = m_nMaxPage * GameDefaults::nPageTileWidth;
	m_sizeTile.cy = m_nMaxVertPage * GameDefaults::nPageTileHeight;
	
	if (m_nTime >= 0 && m_nTime < 10)
		m_nTime = 10;

	// Init TileData
	int nSizeTileData = m_sizeTile.cx * m_sizeTile.cy;
	m_pTileData = new BYTE[nSizeTileData];
	memset(m_pTileData, 0, sizeof(BYTE) * nSizeTileData);

	// TileData
	std::string strTileData = j["TileData"].asString();
	for (int i = 0; i < nSizeTileData; i++)
	{
		if (dVersion > 1.0)
		{
			BYTE nValue = 0;
			NaString strHex;
			strHex.Format("%c%c",
				strTileData[i * 2],
				strTileData[i * 2 + 1]);

			for (int j=0; j<2; j++)
			{
				nValue *= 16;
				if (strHex[j] >= L'0' && strHex[j] <= L'9')
					nValue += strHex[j] - '0';
				else if (strHex[j] >= L'a' && strHex[j] <= L'f')
					nValue += (10 + strHex[j] - 'a');
			}

			/*
			std::stringstream ss;
			ss << std::hex << strHex.cstr();
			ss >> nValue;
			*/

			m_pTileData[i] = nValue;
		}
		else
		{
			m_pTileData[i] = strTileData[i] - L'A';
		}

		if (dVersion < 1.0)
		{
			MigrateTileData(m_pTileData[i], dVersion);
		}
	}

	// MapObject
	Json::Value jGroup = j["MapObject"];
	for (int i = 0; i < jGroup.size(); i++)
	{
		Json::Value jObj = jGroup[i];
		AddMapObject(jObj["X"].asInt(), jObj["Y"].asInt(), jObj["Type"].asInt());
		MapObjectBase *pObj = m_vecMapObject[m_vecMapObject.size() - 1];

		JsonManager::LoadJsonObjectProperty(pObj, jObj);
	}

	// Item
	jGroup = j["Item"];
	for (int i = 0; i < jGroup.size(); i++)
	{
		Json::Value jObj = jGroup[i];
		AddItem(
			jObj["Index"].asInt() % m_sizeTile.cx, 
			jObj["Index"].asInt() / m_sizeTile.cx, 
			jObj["Type"].asInt());
	}

	// Enemy
	jGroup = j["Enemy"];
	for (int i = 0; i < jGroup.size(); i++)
	{
		Json::Value jObj = jGroup[i];
		AddEnemy(jObj["X"].asInt(), jObj["Y"].asInt(), jObj["Type"].asInt());
		GameObjectBase *pObj = m_vecEnemy[m_vecEnemy.size() - 1];

		pObj->LoadProperty(jObj);
	}

	// EventObject
	jGroup = j["EventObject"];
	for (int i = 0; i < jGroup.size(); i++)
	{
		Json::Value jObj = jGroup[i];
		CreateEventObject(jObj["X"].asInt(), jObj["Y"].asInt(), jObj["Type"].asInt());
		GameObjectBase *pObj = m_vecEventObject[m_vecEventObject.size() - 1];

		pObj->LoadProperty(jObj);
	}

	// Vehicle
	jGroup = j["Vehicle"];
	for (int i = 0; i < jGroup.size(); i++)
	{
		Json::Value jObj = jGroup[i];
		CreateVehicle(jObj["X"].asInt(), jObj["Y"].asInt(), jObj["Type"].asInt());
		GameObjectBase *pObj = m_vecVehicle[m_vecVehicle.size() - 1];

		pObj->LoadProperty(jObj);
	}

	// SubStage
	jGroup = j["SubStage"];
	for (int i = 0; i < jGroup.size(); i++)
	{
		Json::Value jObj = jGroup[i];

		Stage *pSubStage = (Stage*)CreateInstance();

		pSubStage->LoadJsonMap(jObj);
		pSubStage->m_pParentStage = this;
		pSubStage->m_bIsSubStage = true;

		pSubStage->m_strName.Format("%s", jObj["Name"].asString().c_str());

		if (pSubStage->m_nMaxDoorKey > 0)
			m_nMaxDoorKey += pSubStage->m_nMaxDoorKey;

		AddSubStage(pSubStage);
	}
}

// BinaryMap will be deprecated
void Stage::LoadBinaryMap(FILE * fp)
{
	if (m_pTileData)
		delete[] m_pTileData;

	// Init TileData
	int nSizeTileData = m_sizeTile.cx * m_sizeTile.cy;
	m_pTileData = new BYTE[nSizeTileData];
	memset(m_pTileData, 0, sizeof(BYTE) * nSizeTileData);

	fseek(fp, 0, SEEK_SET);

	NaString str;
	str = ReadString(fp);

	NaString strHeader(L"Neoarc's Mario Map v");
	if (str.Left(strHeader.GetLength()).Compare(strHeader) != 0)
	{
		DebugBreak();
		return;
	}

	double dVersion = 0;
	int nMajor = 0;
	int nMinor = 0;
	NaString strVersion = str.Right(str.GetLength() - strHeader.GetLength());
#ifdef WIN32
	swscanf_s(strVersion, L"%d.%d", &nMajor, &nMinor);
#else
	scanf(strVersion, L"%d.%d", &nMajor, &nMinor);
#endif
	dVersion = nMajor + (nMinor / 10.0);

	size_t size;
	int nX, nY;
	unsigned int nIndex, nType;
	float fX, fY;
	while (true)
	{
		str = ReadString(fp);
		NaDebugOut(L"- Token: %ls\n", str.wstr());

		if (str.Compare(L"TileData") == 0)
		{
			fread(m_pTileData, sizeof(BYTE), nSizeTileData, fp);
		}
		else if (str.Compare(L"MapObject") == 0)
		{
			fread(&size, sizeof(size_t), 1, fp);
			NaDebugOut(L"- MapObjects: %d\n", size);
			for (int i = 0; i < size; i++)
			{
				fread(&nX, sizeof(int), 1, fp);
				fread(&nY, sizeof(int), 1, fp);
				fread(&nType, sizeof(int), 1, fp);
				AddMapObject(nX, nY, nType);
				MapObjectBase *pObj = m_vecMapObject[m_vecMapObject.size() - 1];

				if (dVersion >= 0.2 && dVersion < 0.7)
				{
					NaString strOp;
					strOp = ReadString(fp);
					pObj->m_strOption = strOp;
				}

				if (dVersion >= 0.6)
				{
					PropertyMap *pMap = pObj->GetPropertyMap();
					std::map<NaString, PropertyInfo>::iterator it2 = pMap->begin();
					for (; it2 != pMap->end(); ++it2)
					{
						PropertyInfo info = it2->second;
						void *pRefValue = pObj->GetRefPropertyValue(info.strPropName);

						NaString strName;

						switch (info.vt)
						{
						case VT_I4:
							strName = ReadString(fp);
							fread(&*(long*)pRefValue, sizeof(int), 1, fp);
							break;
						case VT_ETC_NASTR:
							if (dVersion >= 0.7)
							{
								strName = ReadString(fp);

								NaString strVal;
								strVal = ReadString(fp);
								NaString *pRefStr = (NaString*)pRefValue;
								pRefStr->Format(L"%ls", strVal.wstr());
							}
							else if (nType == MAPOBJECT_PIPE ||
								nType == MAPOBJECT_PIPEHORZ)
							{
								MapObjectPipe *pPipe = (MapObjectPipe*)pObj;
								pPipe->m_strStageName = pObj->m_strOption;
							}
							break;
						}
					}
				}
			}
		}
		else if (str.Compare(L"Item") == 0)
		{
			fread(&size, sizeof(size_t), 1, fp);
			NaDebugOut(L"- Items: %d\n", size);
			for (int i = 0; i < size; i++)
			{
				fread(&nIndex, sizeof(int), 1, fp);
				fread(&nType, sizeof(int), 1, fp);
				NaDebugOut(L"   - Index: %d, Type: %d\n", nIndex, nType);
				AddItem(
					nIndex % m_sizeTile.cx,
					nIndex / m_sizeTile.cx,
					nType);
			}
		}
		else if (str.Compare(L"Enemy") == 0)
		{
			fread(&size, sizeof(size_t), 1, fp);
			NaDebugOut(L"- Enemies: %d\n", size);
			for (int i = 0; i < size; i++)
			{
				fread(&fX, sizeof(float), 1, fp);
				fread(&fY, sizeof(float), 1, fp);
				fread(&nType, sizeof(int), 1, fp);
				AddEnemy(fX, fY, nType);
			}
		}
		else if (str.Compare(L"EventObject") == 0)
		{
			fread(&size, sizeof(size_t), 1, fp);
			NaDebugOut(L"- EventObjects: %d\n", size);
			for (int i = 0; i < size; i++)
			{
				fread(&fX, sizeof(float), 1, fp);
				fread(&fY, sizeof(float), 1, fp);
				fread(&nType, sizeof(int), 1, fp);
				CreateEventObject(fX, fY, nType);
				GameObjectBase *pObj = m_vecEventObject[m_vecEventObject.size() - 1];

				if (dVersion >= 0.5)
				{
					PropertyMap *pMap = pObj->GetPropertyMap();
					std::map<NaString, PropertyInfo>::iterator it2 = pMap->begin();
					for (; it2 != pMap->end(); ++it2)
					{
						PropertyInfo info = it2->second;
						void *pRefValue = pObj->GetRefPropertyValue(info.strPropName);

						NaString strName;
						strName = ReadString(fp);

						switch (info.vt)
						{
						case VT_I4:
							fread(&*(long*)pRefValue, sizeof(int), 1, fp);
							break;
						}
					}
				}
			}
		}
		else if (str.Compare(L"Theme") == 0)
		{
			fread(&m_nTheme, sizeof(int), 1, fp);
		}
		else if (str.Compare(L"BackgroundTheme") == 0)
		{
			fread(&m_nBackgroundTheme, sizeof(int), 1, fp);
		}
		else if (str.Compare(L"BackgroundColor") == 0)
		{
			fread(&m_lBackgroundColor, sizeof(long), 1, fp);
		}
		else if (str.Compare(L"CanGoBack") == 0)
		{
			fread(&m_bCanGoBack, sizeof(bool), 1, fp);
		}
		else if (str.Compare(L"EntranceType") == 0)
		{
			fread(&m_nEntranceType, sizeof(int), 1, fp);
		}
		else if (str.Compare(L"NextStageName") == 0)
		{
			NaString strName;
			strName = ReadString(fp);
			m_strNextStageName = strName;
		}
		else if (str.Compare(L"EndOfMap") == 0)
		{
			break;
		}
		else
		{
			NaDebugOut(L"Error: Unknown Map Token: %ls\n", str.wstr());
			DebugBreak();
		}
	}
}

void Stage::OnLoad()
{
	// StopPoint validation 
	if (m_ptStopPoint.x > GameDefaults::nTilePerScreenWidth * m_nMaxPage * GameDefaults::nTileWidth)
		m_ptStopPoint.x = GameDefaults::nTilePerScreenWidth * m_nMaxPage * GameDefaults::nTileWidth;

	// Find Map Sprite
	InitTheme();

	m_pGame->m_pThemeManager->RequestTileSprite(L"TileBackground", -1, &m_pSpriteBackground, nullptr);

	// #TODO SPR_MANAGER->Find(L"TileBackground")
	//m_pSpriteBackground = SPR_MANAGER->Find(L"TileBackground");

	// MapObject to Tile
	TileizeMapObjects();

	// StartPoint validation
	if (m_ptStartPoint.x == -1 && m_ptStartPoint.x == -1)
	{
		m_ptStartPoint.x = 40;
		m_ptStartPoint.y = 9999;

		int nGround = (m_sizeTile.cy * GameDefaults::nPageTileHeight) - (TILE_YS * 3);
		if (m_ptStartPoint.y >= nGround)
			m_ptStartPoint.y = nGround - 1;

		for (; ; )
		{
			int nData = GetTileData(m_ptStartPoint.x / TILE_XS, m_ptStartPoint.y / TILE_YS);
			int nType = GetDataType(nData);
			if (nType == TILETYPE_EMPTY)
				break;

			m_ptStartPoint.y -= 1;
		}
	}

	InitItems();

	InitObjectIntoPipe();

	InitEnemyStack();

	// Build Background
	BuildBackground();

	// Recursive - Sub Stage
	BEGIN_VECTOR_LOOP(Stage*, m_vecSubStage) {
		pObj->OnLoad();

		if (pObj->m_nMaxDoorKey > 0)
			m_nMaxDoorKey += pObj->m_nMaxDoorKey;
	} END_VECTOR_LOOP();

	if (CUR_PLAYER && CUR_PLAYER_OBJ && m_nInitialShape != -1)
	{
		switch (m_nInitialShape)
		{
		case SHAPE_BIG:
		case SHAPE_FIRE:
			CUR_PLAYER_OBJ->OnChangeShape(m_nInitialShape);
			break;
		}
	}
}

void Stage::SaveJsonMap(Json::Value & j)
{
	// Header
	j["Version"] = "Neoarc's Mario Map v1.1";

	if (m_bIsSubStage)
	{
		j["Name"] = m_strName.cstr();
	}

	// Base
	JsonManager::SaveJsonObjectProperty(this, j);

	int nSizeTileData = (m_sizeTile.cx * m_sizeTile.cy);

	// TileData
	{
		auto ByteToHexString = [](BYTE a, char *c)
		{
			short b[2] = { a / 16, a % 16 };
			for (int i = 0; i < 2; i++)
			{
				if (b[i] >= 0 && b[i] <= 9)
					*c = '0' + b[i];
				else
					*c = 'a' + (b[i] - 10);
				c += 1;
			}
		};

		char *szTileData = new char[(nSizeTileData * 2) + 1];
		for (int i = 0; i < nSizeTileData; i++)
		{
			// Changed to 2byte string (2017.04.07)
			ByteToHexString(m_pTileData[i], &szTileData[i * 2]);

			/*
			// old 2 (slow)
			NaString strHex;
			strHex.Format(L"%02x", m_pTileData[i]);
			auto szHex = strHex.cstr();
			
			szTileData[(i * 2) + 0] = szHex[0];
			szTileData[(i * 2) + 1] = szHex[1];
			*/

			// old
			//szTileData[i] = (L'A' + m_pTileData[i]);
		}
		szTileData[nSizeTileData * 2] = '\0';

		j["TileData"] = szTileData;
		delete[] szTileData;
	}

	// MapObject
	{
		j["MapObject"] = Json::Value();

		std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
		for (; it != m_vecMapObject.end(); ++it)
		{
			MapObjectBase *pObj = *it;

			Json::Value jObj;
			jObj["X"] = pObj->m_nTX;
			jObj["Y"] = pObj->m_nTY;
			jObj["Type"] = pObj->m_nType;
			JsonManager::SaveJsonObjectProperty(pObj, jObj);

			j["MapObject"].append(jObj);
		}
	}

	// Item
	{
		j["Item"] = Json::Value();

		std::map<int, int>::iterator it = m_mapItem.begin();
		for (; it != m_mapItem.end(); ++it)
		{
			Json::Value jObj;
			jObj["Index"] = it->first;
			jObj["Type"] = it->second;

			j["Item"].append(jObj);
		}
	}

	// Enemy
	{
		j["Enemy"] = Json::Value();

		std::vector<GameObjectBase*>::iterator it = m_vecEnemy.begin();
		for (; it != m_vecEnemy.end(); ++it)
		{
			GameObjectBase *pObj = *it;

			Json::Value jObj;
			jObj["X"] = pObj->m_fX;
			jObj["Y"] = pObj->m_fY;
			jObj["Type"] = pObj->m_nType;
			JsonManager::SaveJsonObjectProperty(pObj, jObj);

			j["Enemy"].append(jObj);
		}
	}

	// EventObject
	{
		j["EventObject"] = Json::Value();

		std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
		for (; it != m_vecEventObject.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			if (pObj->m_nType == EVENTOBJECT_VORTEX)
				continue;

			Json::Value jObj;
			jObj["X"] = pObj->m_fX;
			jObj["Y"] = pObj->m_fY;
			jObj["Type"] = pObj->m_nType;
			JsonManager::SaveJsonObjectProperty(pObj, jObj);

			j["EventObject"].append(jObj);
		}
	}

	// Vehicle
	{
		j["Vehicle"] = Json::Value();

		std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
		for (; it != m_vecVehicle.end(); ++it)
		{
			GameObjectBase *pObj = *it;
			Json::Value jObj;
			jObj["X"] = pObj->m_fX;
			jObj["Y"] = pObj->m_fY;
			jObj["Type"] = pObj->m_nType;
			JsonManager::SaveJsonObjectProperty(pObj, jObj);

			j["Vehicle"].append(jObj);
		}
	}

	// SubStage
	{
		j["SubStage"] = Json::Value();

		std::vector<Stage*>::iterator it = m_vecSubStage.begin();
		for (; it != m_vecSubStage.end(); ++it)
		{
			Stage *pObj = *it;

			Json::Value jObj;
			pObj->SaveJsonMap(jObj);

			j["SubStage"].append(jObj);
		}
	}
}

NaString Stage::ReadString(FILE * fp)
{
	BYTE nByte;
	WORD nWord;
	BYTE *buf = nullptr;
	NaString str;

	// unicode tag
	fread(&nByte, sizeof(BYTE), 1, fp); // ff
	fread(&nWord, sizeof(WORD), 1, fp); // fe ff
	fread(&nByte, sizeof(BYTE), 1, fp);

	buf = new BYTE[(nByte + 1) * sizeof(wchar_t)];
	fread(buf, sizeof(wchar_t), nByte, fp);
	*((wchar_t*)buf + nByte) = 0;
	str = (wchar_t*)buf;
	delete[] buf;

	return str;
}

void Stage::MigrateTileData(BYTE & nTileData, double dVersionFrom)
{
#define MIGRATE_FROMTO(from, to) case from: nTileData = to; break;

	if (dVersionFrom < 1.0)
	{
		switch (nTileData)
		{
		MIGRATE_FROMTO(TILE_V0_9_EMPTY, TILE_EMPTY)
		MIGRATE_FROMTO(TILE_V0_9_GROUND, TILE_GROUND)
		MIGRATE_FROMTO(TILE_V0_9_BRICK, TILE_BRICK)
		MIGRATE_FROMTO(TILE_V0_9_ITEM, TILE_ITEM)
		MIGRATE_FROMTO(TILE_V0_9_TAKENITEM, TILE_TAKENITEM)
		MIGRATE_FROMTO(TILE_V0_9_PIPEHEAD_L, TILE_PIPEHEAD_L)
		MIGRATE_FROMTO(TILE_V0_9_PIPEHEAD_R, TILE_PIPEHEAD_R)
		MIGRATE_FROMTO(TILE_V0_9_PIPEBODY_L, TILE_PIPEBODY_L)
		MIGRATE_FROMTO(TILE_V0_9_PIPEBODY_R, TILE_PIPEBODY_R)
		MIGRATE_FROMTO(TILE_V0_9_HARDBRICK, TILE_HARDBRICK)
		MIGRATE_FROMTO(TILE_V0_9_ISLANDHEAD_L, TILE_ISLANDHEAD_L)
		MIGRATE_FROMTO(TILE_V0_9_ISLANDHEAD_C, TILE_ISLANDHEAD_C)
		MIGRATE_FROMTO(TILE_V0_9_ISLANDHEAD_R, TILE_ISLANDHEAD_R)
		MIGRATE_FROMTO(TILE_V0_9_ISLANDBODY, TILE_ISLANDBODY_C)
		MIGRATE_FROMTO(TILE_V0_9_MUSHROOMHEAD_L, TILE_MUSHROOMHEAD_L)
		MIGRATE_FROMTO(TILE_V0_9_MUSHROOMHEAD_C, TILE_MUSHROOMHEAD_C)
		MIGRATE_FROMTO(TILE_V0_9_MUSHROOMHEAD_R, TILE_MUSHROOMHEAD_R)
		MIGRATE_FROMTO(TILE_V0_9_MUSHROOMBODY_T, TILE_MUSHROOMBODY_T)
		MIGRATE_FROMTO(TILE_V0_9_MUSHROOMBODY_B, TILE_MUSHROOMBODY_B)
		MIGRATE_FROMTO(TILE_V0_9_CLOUD, TILE_CLOUD)
		MIGRATE_FROMTO(TILE_V0_9_BRIDGE_T, TILE_BRIDGE_T)
		MIGRATE_FROMTO(TILE_V0_9_BRIDGE_B, TILE_BRIDGE_B)
		MIGRATE_FROMTO(TILE_V0_9_CANNONHEAD_T, TILE_CANNONHEAD_T)
		MIGRATE_FROMTO(TILE_V0_9_CANNONHEAD_B, TILE_CANNONHEAD_B)
		MIGRATE_FROMTO(TILE_V0_9_CANNONBODY, TILE_CANNONBODY)
		MIGRATE_FROMTO(TILE_V0_9_HPIPEHEAD_T, TILE_HPIPEHEAD_T)
		MIGRATE_FROMTO(TILE_V0_9_HPIPEHEAD_B, TILE_HPIPEHEAD_B)
		MIGRATE_FROMTO(TILE_V0_9_HPIPEBODY_T, TILE_HPIPEBODY_T)
		MIGRATE_FROMTO(TILE_V0_9_HPIPEBODY_B, TILE_HPIPEBODY_B)
		MIGRATE_FROMTO(TILE_V0_9_PIPEJUNCTION_T, TILE_PIPEJUNCTION_T)
		MIGRATE_FROMTO(TILE_V0_9_PIPEJUNCTION_B, TILE_PIPEJUNCTION_B)
		//MIGRATE_FROMTO(TILE_V0_9_ITEM2, TILE_ITEM2)
		//MIGRATE_FROMTO(TILE_V0_9_ITEM3, TILE_ITEM3)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_HEAD1, TILE_CASTLE_HEAD1)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_HEAD2, TILE_CASTLE_HEAD2)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_WINDOW1, TILE_CASTLE_WINDOW1)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_BRICK, TILE_CASTLE_BRICK)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_WINDOW2, TILE_CASTLE_WINDOW2)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_DOOR_T, TILE_CASTLE_DOOR_T)
		MIGRATE_FROMTO(TILE_V0_9_CASTLE_DOOR_B, TILE_CASTLE_DOOR_B)
		MIGRATE_FROMTO(TILE_V0_9_RIVER_T, TILE_RIVER_T)
		MIGRATE_FROMTO(TILE_V0_9_RIVER_B, TILE_RIVER_B)
		MIGRATE_FROMTO(TILE_V0_9_HIDDENITEM, TILE_HIDDENITEM)
		//MIGRATE_FROMTO(TILE_V0_9_TAKINGITEM, TILE_TAKINGITEM)
		MIGRATE_FROMTO(TILE_V0_9_VERTICAL_LINE, TILE_VERTICAL_LINE)
		MIGRATE_FROMTO(TILE_V0_9_SPIKE, TILE_SPIKE)
		MIGRATE_FROMTO(TILE_V0_9_JUMP, TILE_JUMP)
		MIGRATE_FROMTO(TILE_V0_9_SEMI_SOLID_T, TILE_SEMI_SOLID_CT)
		MIGRATE_FROMTO(TILE_V0_9_SEMI_SOLID_B, TILE_SEMI_SOLID2_CTB)
		//MIGRATE_FROMTO(TILE_V0_9_SPIKE2, TILE_SPIKE2)
		//MIGRATE_FROMTO(TILE_V0_9_RIVER_T2, TILE_RIVER_T2)
		//MIGRATE_FROMTO(TILE_V0_9_RIVER_T3, TILE_RIVER_T3)
		//MIGRATE_FROMTO(TILE_V0_9_RIVER_T4, TILE_RIVER_T4)
		//MIGRATE_FROMTO(TILE_V0_9_RIVER_T5, TILE_RIVER_T5)
		MIGRATE_FROMTO(TILE_V0_9_PIPEBOTTOM_L, TILE_PIPEBOTTOM_L)
		MIGRATE_FROMTO(TILE_V0_9_PIPEBOTTOM_R, TILE_PIPEBOTTOM_R)
		MIGRATE_FROMTO(TILE_V0_9_HPIPERIGHT_T, TILE_HPIPERIGHT_T)
		MIGRATE_FROMTO(TILE_V0_9_HPIPERIGHT_B, TILE_HPIPERIGHT_B)
		MIGRATE_FROMTO(TILE_V0_9_MESSAGE, TILE_MESSAGE)
		};
	}
}

void Stage::CreatePresetStage(NaString strName)
{
	Clear();

	// Init TileData
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

	m_ptStartPoint.x = 40;
	m_ptStopPoint.x = GameDefaults::nDefaultStopPointX;
	m_ptStopPoint.y = 0;
	m_ptEnemyStop.x = GameDefaults::nMaxCameraPosX;
	m_nPrevJumpPage = -1;
	m_nNextJumpPage = -1;

	if (strName.Compare(L"CommonEntrance") == 0)
	{
		m_nThemeSet = STAGETHEMESET_OVERWORLD;

		CreateEventObject(40, 208, EVENTOBJECT_GOALPOINT);

		MapObjectPipe *pPipe = nullptr;

		pPipe = (MapObjectPipe*)AddMapObject(12, 9, MAPOBJECT_PIPE);
		pPipe->m_nPipeType = 0;

		pPipe = (MapObjectPipe*)AddMapObject(10, 11, MAPOBJECT_PIPEHORZ);
		pPipe->m_nPipeType = 1;

		AddMapObject(3, 12, MAPOBJECT_STARTPOINT);

		AddMapObject(16, 12, MAPOBJECT_CAMERASTOP);

		SetTileData(16, 13, TILE_EMPTY);
		SetTileData(16, 14, TILE_EMPTY);
		SetTileData(16, 15, TILE_EMPTY);

		SetTileData(17, 13, TILE_EMPTY);
		SetTileData(17, 14, TILE_EMPTY);
		SetTileData(17, 15, TILE_EMPTY);

		SetTileData(18, 13, TILE_EMPTY);
		SetTileData(18, 14, TILE_EMPTY);
		SetTileData(18, 15, TILE_EMPTY);

		SetTileData(19, 13, TILE_EMPTY);
		SetTileData(19, 14, TILE_EMPTY);
		SetTileData(19, 15, TILE_EMPTY);

		SetTileData(21, 9, TILE_ITEM);
	}
	else if (strName.Compare(L"Title") == 0)
	{
		m_pGame->m_strStagePack = L".";
		m_pGame->m_nStagePackType = STAGEPACK_SINGLE;
		if (Load(L"_Title"))
			return;
		
		m_nThemeSet = (rand() % 6) + 1;
		for (int i = 2; i < 13; i++)
			SetTileData(0, i, TILE_BRICK);

		AddMapObject(3, 7, MAPOBJECT_PIPE);
		AddMapObject(23, 10, MAPOBJECT_PIPE);
	}

	// Find Map Sprite
	InitTheme();

	if (strName.Compare(L"Title") == 0)
	{
		if (m_lBackgroundColor == 0xff000000)
		{
			switch (rand() % 2)
			{
			case 0:
				m_lBackgroundColor = 0xffffbc1c;
				break;
			case 1:
				m_lBackgroundColor = 0xff1ca7ff;
				break;
			}
		}
	}

	InitItems();

	m_pGame->m_pThemeManager->RequestTileSprite(L"TileBackground", -1, &m_pSpriteBackground, nullptr);
	
	// MapObject to Tile
	TileizeMapObjects();

	// Build Background
	BuildBackground();
}

void Stage::CreateDefaultGround()
{
	if (m_pTileData != nullptr)
	{
		delete[] m_pTileData;
		m_pTileData = nullptr;
	}

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
}

void Stage::OnMaxPageChanged()
{
	if (m_nMaxPage != m_sizeTile.cx / GameDefaults::nPageTileWidth)
	{
		int nCurMaxPage = m_sizeTile.cx / GameDefaults::nPageTileWidth;
		NaDebugOut(L"MaxPageChanged: %d->%d\n", nCurMaxPage, m_nMaxPage);

		int nNewSize = 
			(m_nMaxPage * GameDefaults::nPageTileWidth) * 
			(m_nMaxVertPage * GameDefaults::nPageTileHeight);

		BYTE* pNewTileData = new BYTE[nNewSize];
		memset(pNewTileData, 0, sizeof(BYTE) * nNewSize);

		// Copy Current
		for (int i=0; i<GameDefaults::nPageTileHeight * m_nMaxVertPage; i++)
		{
			int nRowSize = nCurMaxPage * GameDefaults::nPageTileWidth;
			if (nCurMaxPage > m_nMaxPage)
			{
				// Some Data Lost
				nRowSize = m_nMaxPage * GameDefaults::nPageTileWidth;
			}

			memcpy(
				pNewTileData + i * (m_nMaxPage * GameDefaults::nPageTileWidth),
				m_pTileData + i * (nCurMaxPage * GameDefaults::nPageTileWidth),
				nRowSize * sizeof(BYTE)
			);
		}

		delete[] m_pTileData;

		m_pTileData = pNewTileData;
		m_sizeTile.cx = m_nMaxPage * GameDefaults::nPageTileWidth;
		m_sizeTile.cy = m_nMaxVertPage * GameDefaults::nPageTileHeight;

		// Move Items
		{
			int nCurCX = nCurMaxPage * GameDefaults::nPageTileWidth;
			std::map<int, int> mapItemNew;
			
			// Copy
			std::map<int, int>::iterator it = m_mapItem.begin();
			for (; it != m_mapItem.end(); ++it)
			{
				int nCurIdx = it->first;
				int nCurX = nCurIdx % nCurCX;
				int nCurY = nCurIdx / nCurCX;

				int nNewIdx = GetTileIndex(nCurX, nCurY);
				mapItemNew.insert(
					std::pair<int, int>(nNewIdx, it->second)
				);
			}

			m_mapItem.clear();

			// Paste
			it = mapItemNew.begin();
			for (; it != mapItemNew.end(); ++it)
			{
				m_mapItem.insert(
					std::pair<int, int>(it->first, it->second)
				);
			}
		}

		BuildBackground();
	}
}

void Stage::OnMaxVertPageChanged()
{
	int nOldPage = m_sizeTile.cy / GameDefaults::nPageTileHeight;
	if (m_nMaxVertPage != nOldPage)
	{
		int nCurMaxPage = nOldPage;
		NaDebugOut(L"MaxVerticalPageChanged: %d->%d\n", nCurMaxPage, m_nMaxVertPage);

		int nOldSize = m_sizeTile.cx * m_sizeTile.cy;
		int nNewSize =
			(m_nMaxPage * GameDefaults::nPageTileWidth) *
			(m_nMaxVertPage * GameDefaults::nPageTileHeight);

		BYTE* pNewTileData = new BYTE[nNewSize];
		memset(pNewTileData, 0, sizeof(BYTE) * nNewSize);

		// Copy Current
		int nSrcRow = 0;
		int nDstRow = 0;
		int nRowSize = m_sizeTile.cy / GameDefaults::nPageTileHeight;
		int nPageBufSize = 
			m_nMaxPage *
			GameDefaults::nPageTileWidth * 
			GameDefaults::nPageTileHeight;

		if (nOldPage < m_nMaxVertPage)
		{
			nDstRow = m_nMaxVertPage - nOldPage;
		}
		else
		{
			nSrcRow = (nOldPage - m_nMaxVertPage);
			nRowSize = m_nMaxVertPage;
		}

		memcpy(
			pNewTileData + (nDstRow * nPageBufSize),
			m_pTileData + (nSrcRow * nPageBufSize),
			nRowSize * nPageBufSize * sizeof(BYTE)
		);

		NaDebugOut(L"-> Old buffer: 0x%08x\n", m_pTileData);
		delete[] m_pTileData;

		m_pTileData = pNewTileData;
		NaDebugOut(L"-> New buffer: 0x%08x\n", m_pTileData);

		m_sizeTile.cx = m_nMaxPage * GameDefaults::nPageTileWidth;
		m_sizeTile.cy = m_nMaxVertPage * GameDefaults::nPageTileHeight;

		/*
		// Move Items
		{
			int nCurCX = nCurMaxPage * GameDefaults::nPageTileWidth;
			std::map<int, int> mapItemNew;

			// Copy
			std::map<int, int>::iterator it = m_mapItem.begin();
			for (; it != m_mapItem.end(); ++it)
			{
				int nCurIdx = it->first;
				int nCurX = nCurIdx % nCurCX;
				int nCurY = nCurIdx / nCurCX;

				int nNewIdx = GetTileIndex(nCurX, nCurY);
				mapItemNew.insert(
					std::pair<int, int>(nNewIdx, it->second)
				);
			}

			m_mapItem.clear();

			// Paste
			it = mapItemNew.begin();
			for (; it != mapItemNew.end(); ++it)
			{
				m_mapItem.insert(
					std::pair<int, int>(it->first, it->second)
				);
			}
		}
		*/

		BuildBackground();
	}
}

void Stage::InitEnemyStack()
{
	std::vector<GameObjectBase*>::iterator it1 = m_vecEnemy.begin();
	for ( ; it1 != m_vecEnemy.end(); ++it1) 
	{
		EnemyObjectBase *pE1 = (EnemyObjectBase*)*it1;
		if (!pE1->IsAlive())
			continue;

		NaRect rc1 = pE1->GetRect();
		std::vector<GameObjectBase*>::iterator it2 = m_vecEnemy.begin();
		for ( ; it2 != m_vecEnemy.end(); ++it2)
		{
			EnemyObjectBase *pE2 = (EnemyObjectBase*)*it2;
			if (pE1 == pE2)
				continue;
			if (!pE2->IsAlive())
				continue;
			if (pE1->m_fX == pE2->m_fX)
			{
				int nHeight = rc1.Height();
				if (nHeight % 16 != 0)
				{
					nHeight = ((nHeight / 16) + 1) * 16;
				}

				if (pE1->m_fY - nHeight == pE2->m_fY)
				{
					((EnemyObjectBase*)pE1)->m_pOnMyHead = (EnemyObjectBase*)pE2;
					((EnemyObjectBase*)pE2)->m_pUnderMyFoot = (EnemyObjectBase*)pE1;
					pE2->SetState(STATE_STACKED);
					break;
				}
			}
		}
	}
}

void Stage::MakeEarthquake(int nPower)
{
	switch(nPower)
	{
	case POWER_EARTHQUAKE_SLEDGEBROTHERS:
		{
			GameObjectBase *pObj = CUR_PLAYER->GetLastControlObject();
			if (pObj && 
				pObj->m_bCollision[COLLISION_BOTTOM] && 
				pObj->IsAlive())
			{
				pObj->ChangeState(STATE_PARALYZE);
			}
		}
		CAMERA->MakeEarthquake();
		break;
	case POWER_EARTHQUAKE_GIANTMARIOJUMP:
		{
			int nX = CAMERA->GetPosition().x - 150;
			int nMax = nX + GameDefaults::nPageWidth + 600;
			for (int i = nX; i < nMax; i += 10)
			{
				if (rand() % 4 == 0)
				{
					auto pObj = CUR_STAGE->CreateItem(i, -16, ITEM_THROWNCOIN);
					pObj->m_fYS = ((rand() % 200) / 100.0f);
				}
			}
		}
		CAMERA->MakeEarthquake();
		break;
	case POWER_EARTHQUAKE_POWERBLOCK:
		{
			m_pGame->m_pGameState->SetEnemyInViewportState(STATE_POWED);

			// Make falling static coins
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecItem) {
				if (pObj->m_nType != ITEM_COIN)
					continue;

				if (!pObj->IsOutOfCamera())
				{
					pObj->ChangeState(STATE_DEAD);
					ReserveCreateGameObject(pObj->m_fX, pObj->m_fY, ITEM_THROWNCOIN);
				}
			} END_VECTOR_LOOP();
		}
		CAMERA->MakeEarthquake(8);
		break;
	default:
		CAMERA->MakeEarthquake();
		break;
	}
}

void Stage::RenderBackground()
{
	int nTileW = m_sizeTile.cx;
	int nTileH = m_sizeTile.cy;

	RECT rc = CAMERA->GetTileViewport();
	int nBeginX = rc.left;
	int nBeginY = rc.top;
	int nEndX = MIN(rc.right, nTileW);
	int nEndY = MIN(rc.bottom, nTileH);

	for (int x = nBeginX; x < nEndX; x++)
	{
		for (int y = nBeginY; y < nEndY; y++)
		{
			int nData = GetTileBackgroundData(x, y);
			if (nData != TILE_EMPTY)
				RenderTileBackground(x, y, nData);
		}
	}
}

void Stage::Render()
{
	int nTileW = m_sizeTile.cx;
	int nTileH = m_sizeTile.cy;

	RECT rc = CAMERA->GetTileViewport();
	int nBeginX = rc.left;
	int nBeginY = rc.top;
	int nEndX = MIN(rc.right, nTileW);
	int nEndY = MIN(rc.bottom, nTileH);

	RenderFarBackground();
	RenderEventObjects();
	RenderItems();
	RenderEnemies();
	RenderVehicles();

	for (int x = nBeginX; x < nEndX; x++)
	{
		for (int y = nBeginY; y < nEndY; y++)
		{
			int nData = GetTileData(x, y);
			if (nData != TILE_EMPTY && nData != TILE_HIDDENITEM)
				RenderTile(x, y, nData);

			// For debug tile coord
			/*
			if (y == 0)
			{
				NaPointT<float> ptCamera = CAMERA->GetPosition();
				NaString str;
				str.Format(L"%d", x);
				RENDER_MANAGER->Text((float)(x*16) - ptCamera.x, (float)(y*16) - ptCamera.y, str, -1, 0.5f);

				int x2 = x, y2 = y;
				CalcPageJumpedPos(x2, y2);
				str.Format(L"%d", x2);
				RENDER_MANAGER->Text((float)(x * 16) - ptCamera.x, (float)(y * 16) + 8 - ptCamera.y, str, 0xffff0000, 0.5f);
			}
			*/
		}
	}

	RenderBullets();	

	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecReservedGameObject) {
		if (!pObj->IsOutOfCamera())
			pObj->Render();
	} END_VECTOR_LOOP();

	// For debug pipe warp
	/*
	{
		UIBase u(m_pGame);
		POINT ptCamera = CAMERA->GetPosition();
		BEGIN_VECTOR_LOOP(PipeInfo*, m_vecPipeInfo) {
			NaRect rc = pObj->m_rcEnterance;
			rc.Offset(-ptCamera.x, -ptCamera.y);
			if (pObj->m_nType == PIPEENTRANCE_EXITONLY)
				u.RenderBox(rc, 0xc00000ff);
			else
				u.RenderBox(rc, 0xc0ff0000);
		} END_VECTOR_LOOP();

		BEGIN_VECTOR_LOOP(PipeExitInfo*, m_vecPipeExitInfo) {
			NaRect rc = pObj->m_rcExit;
			rc.Offset(-ptCamera.x, -ptCamera.y);
			u.RenderBox(rc, 0xc00000ff);
		} END_VECTOR_LOOP();
	}
	//*/

	// For debug swim area
	/*
	{
		UIBase u(m_pGame);
		POINT ptCamera = CAMERA->GetPosition();
		BEGIN_VECTOR_LOOP(NaRect, m_vecSwimArea) {
			auto rc = pObj;
			rc.Offset(-ptCamera.x, -ptCamera.y);
			u.RenderBox(rc, 0xc00000ff);
		} END_VECTOR_LOOP();
	}
	//*/

	/*
	{
		NaString s;
		s.Format(L"Switch: %d", CUR_PLAYER->m_nPSwitchTime);
		RENDER_MANAGER->Text(0, 0, s);
	}
	//*/
}

void Stage::RenderTile(int x, int y, int nType, long lColor, int nZOrder)
{
	NaRect rc = GetTileRect(x, y);
	int nIdx = GetTileIndex(x, y);

	// Offset includes from GetTileRect()
	//pt = GetHitAnimatedTileOffset(x, y);
	//rc.Offset(pt.x, pt.y);

	nType = GetAnimatedTileIndex(nType);

	bool bShadow = (nType != TILE_EMPTY) && (m_pGame->m_bModernStyle) && (nZOrder != Z_MAP_SHADOW);
	/*
	if (bShadow)
	{
		if (GetTileData(x + 1, y) != TILE_EMPTY &&
			GetTileData(x, y + 1) != TILE_EMPTY &&
			GetTileData(x + 1, y + 1) != TILE_EMPTY)
			bShadow = false;
	}
	*/

	if (nType == TILE_GROUND)
		nType = GetAutoGroundTileIndex(x, y);

	if (nZOrder == Z_MAP && rc.top % TILE_YS != 0)
		nZOrder = Z_MAP_ANIMATING;

	if (nZOrder == Z_MAP)
	{
		if (IsBackgroundTile(nType))
			nZOrder = Z_SEMISOLID_BACKGROUND;
	}
	
	float fScale = 1.0f;
	if (rc.top % 16 != 0)
	{
		POINT ptOffset = GetHitAnimatedTileOffset(x, y);
		fScale += abs(ptOffset.y) / 40.0f;
	}

	NaPointT<float> ptCamera = CAMERA->GetPosition();

	m_pSprite->RenderToQueue(
		(float)rc.left + 8 - ptCamera.x, 
		(float)rc.top + 16 - ptCamera.y, 
		nType, false, false, lColor, fScale, fScale, nZOrder, bShadow);
}

void Stage::RenderTileBackground(int x, int y, int nType)
{
	NaRect rc = GetTileRect(x, y, false);

	if (nType == -1)
		return;

	long lAlpha = -1L;
	if (m_pGame->m_bModernStyle)
	{
		// Removed 2016.11.07
		// Changed BG Tileset SMB1 to SMM

		// lAlpha = 0xb0ffffff;

		nType = GetAnimatedBackgroundTileIndex(nType);
	}

	NaPointT<float> ptCamera = CAMERA->GetPosition(CAMERA_PIN_BACKGROUND);

	m_pSpriteBackground->RenderToQueue(
		(float)rc.left + 8 - ptCamera.x,
		(float)rc.top + 16 - ptCamera.y,
		nType, false, false, lAlpha, 1.0f, 1.0f, Z_BACKGROUND);
}

void Stage::RenderFarBackground()
{
	// #TODO #ASAP FarBackground
	if (m_pSpriteFarBackground == nullptr)
		return;

	NaPointT<float> ptCamera = CAMERA->GetPosition(CAMERA_PIN_FARBACKGROUND);

	int w = m_pSpriteFarBackground->m_vecFrames[0].m_rc.right;
	int h = m_pSpriteFarBackground->m_vecFrames[0].m_rc.bottom;
	int x = -(ptCamera.x / 3);

	int nCameraMaxY = (m_sizeTile.cy * TILE_YS) - (16 * TILE_YS);// m_pGame->m_nHeight;
	int y;
	if (nCameraMaxY == 0)
		y = (m_pGame->m_nHeight - h);
	else
		y = (m_pGame->m_nHeight - h) * (ptCamera.y / nCameraMaxY);

	while (x + w < 0)
	{
		x += w;
	}


	while (x < m_pGame->m_nWidth)
	{
		m_pSpriteFarBackground->RenderToQueue(
			x, y,
			0,
			false, false, 0xffffffff, 1.0f, 1.0f, Z_FAR_BACKGROUND);

		x += w;
	}
}

void Stage::RenderItems()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecItem)
		if (!pObj->IsOutOfCamera())
			pObj->Render();
	END_VECTOR_LOOP();
}

void Stage::RenderEnemies()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEnemy)
		if (!pObj->IsOutOfCamera())
			pObj->Render();
	END_VECTOR_LOOP();
}

void Stage::RenderEffects()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEffect)
		pObj->Render();
	END_VECTOR_LOOP();
}

void Stage::RenderBullets()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecBullet)
		if (!pObj->IsOutOfCamera())
			pObj->Render();
	END_VECTOR_LOOP();
}

void Stage::RenderEventObjects()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEventObject)
		if (!pObj->IsOutOfCamera())
			pObj->Render();
	END_VECTOR_LOOP();
}

void Stage::RenderVehicles()
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecVehicle)
		if (!pObj->IsOutOfCamera())
			pObj->Render();
	END_VECTOR_LOOP();
}

long Stage::GetBackgroundColor()
{
	return m_lBackgroundColor;
}

void Stage::OnBeforeProcess()
{
	// Vehicle
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecVehicle)
		if (!CAMERA->IsOutOfCamera(pObj))
			pObj->OnBeforeProcess();
	END_VECTOR_LOOP();

	// EventObject
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEventObject)
		if (!pObj->IsOutOfCamera())
			pObj->OnBeforeProcess();
	END_VECTOR_LOOP();

	// Items
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecItem)
		if (!pObj->IsOutOfCamera())
			pObj->OnBeforeProcess();
	END_VECTOR_LOOP();

	// Enemy
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEnemy)
		if (!CAMERA->IsOutOfCamera(pObj) ||
			(pObj->m_fXS != 0 || pObj->m_fYS != 0))
			pObj->OnBeforeProcess();
	END_VECTOR_LOOP();

	// Bullet
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecBullet)
		pObj->OnBeforeProcess();
	END_VECTOR_LOOP();

	// Animation
	// ..
}

void Stage::Process()
{
	// Process Virtual Page
	if (m_nNextJumpPage != -1)
	{
		NaRect rcView = CAMERA->GetViewport();

		// Out from Previous Page of Virtual Page
		// Move All Object in Current ViewPort to JumpedPage
		if (rcView.left > ((m_nPrevJumpPage + 1) * GameDefaults::nPageWidth) - 1)
		{
			int nOffsetX = (m_nNextJumpPage - m_nPrevJumpPage - 1) * GameDefaults::nPageWidth;

			CUR_PLAYER_OBJ->m_fX += nOffsetX;
			CAMERA->m_ptPos.x += nOffsetX;

			std::vector<GameObjectBase*> vecAllGameObject;
			PASTE_VECTOR(vecAllGameObject, m_vecItem);
			PASTE_VECTOR(vecAllGameObject, m_vecEnemy);
			PASTE_VECTOR(vecAllGameObject, m_vecBullet);
			PASTE_VECTOR(vecAllGameObject, m_vecEventObject);
			PASTE_VECTOR(vecAllGameObject, m_vecVehicle);

			BEGIN_VECTOR_LOOP(GameObjectBase*, vecAllGameObject) {
				NaRect rcObj = pObj->GetRect();
				if (pObj->m_fX >= rcView.left && 
					pObj->m_fX <= rcView.right) //rcView.IsOverlapped(rcObj))
				{
					pObj->m_fX += nOffsetX;
				}
			} END_VECTOR_LOOP();

			BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecJumpedObject) {
				if (pObj->m_nType >= ENEMY_BEGIN && pObj->m_nType <= ENEMY_END)
					m_vecEnemy.push_back(pObj);
				else if (pObj->m_nType >= ITEM_BEGIN && pObj->m_nType <= ITEM_END)
					m_vecItem.push_back(pObj);
				else if (pObj->m_nType >= EFFECT_BEGIN && pObj->m_nType <= EFFECT_END)
					m_vecEffect.push_back(pObj);
				else if (pObj->m_nType >= BULLET_BEGIN && pObj->m_nType <= BULLET_END)
					m_vecBullet.push_back(pObj);
				else if (pObj->m_nType >= EVENTOBJECT_BEGIN && pObj->m_nType <= EVENTOBJECT_END)
					m_vecEventObject.push_back(pObj);
				else if (pObj->m_nType >= VEHICLE_BEGIN && pObj->m_nType <= VEHICLE_END)
					m_vecVehicle.push_back(pObj);
			} END_VECTOR_LOOP();
			m_vecJumpedObject.clear();

			// Clear
			m_nPrevJumpPage = -1;
			m_nNextJumpPage = -1;

			NaDebugOut(L"Clear PageJump\n");
		}
	}
	
	// Process InStage Objects
	ProcessBase();

	// Process AirShip
	if (m_nTheme == STAGETHEME_AIRSHIP)
	{
		// #TODO Change to Function
		float fOffset = (15.0f) * sin((-90 + (m_nStateFrame)) * fToRadian);

		CAMERA->m_ptAirShipOffset.x = 0;
		CAMERA->m_ptAirShipOffset.y = fOffset;
	}
	else
	{
		CAMERA->m_ptAirShipOffset.x = 0;
		CAMERA->m_ptAirShipOffset.y = 0;
	}

	m_nStateFrame++;
}

void Stage::ProcessBase()
{
	// Vehicle
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecVehicle)
		if (!CAMERA->IsOutOfCamera(pObj))
			pObj->Process();
	END_VECTOR_LOOP();

	// EventObject
	{
		// Use temporarily copied vector
		std::vector<GameObjectBase*> vecEventObject;
		vecEventObject.insert(vecEventObject.end(), m_vecEventObject.begin(), m_vecEventObject.end());

		BEGIN_VECTOR_LOOP(GameObjectBase*, vecEventObject)
			if (!pObj->IsOutOfCamera())
				pObj->Process();
		END_VECTOR_LOOP();
	}

	// Items
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecItem)
		if (!pObj->IsOutOfCamera())
			pObj->Process();
	END_VECTOR_LOOP();
	
	// Enemy
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEnemy) {
		bool bOutOfCamera = CAMERA->IsOutOfCamera(pObj);
		if (!bOutOfCamera || (bOutOfCamera && pObj->m_nState != STATE_IDLE))
			pObj->Process();
	} END_VECTOR_LOOP();

	// Bullet
	BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecBullet)
		pObj->Process();
	END_VECTOR_LOOP();

	// Animation
	{
		for (std::map<int, int>::iterator it = m_mapHitFrame.begin();
			it != m_mapHitFrame.end(); 
			++it)
		{
			it->second--;
		}

		for (std::map<int, int>::iterator it = m_mapPressFrame.begin();
			it != m_mapPressFrame.end();
			++it)
		{
			it->second--;
		}
	}

	if (m_nReqSwapCoinBlock != 0)
	{
		bool bIsInitial;
		if (m_nReqSwapCoinBlock < 0)
			bIsInitial = false;
		else
			bIsInitial = true;

		Stage *pStage = CUR_STAGE;
		while (pStage)
		{
			BEGIN_VECTOR_LOOP(Stage*, pStage->m_vecSubStage) {
				if (pObj->m_bSwapCoinBlock != bIsInitial)
					pObj->SwapCoinAndBrick();
			} END_VECTOR_LOOP();

			if (pStage->m_bSwapCoinBlock != bIsInitial)
				pStage->SwapCoinAndBrick();

			pStage = pStage->m_pParentStage;
		}

		m_nReqSwapCoinBlock = 0;
	}
}

void Stage::OnAfterProcess()
{
	std::vector<GameObjectBase*> vecDeads;

	// Vehicle
	{
		std::vector<GameObjectBase*>::iterator it = m_vecVehicle.begin();
		for (; it != m_vecVehicle.end(); )
		{
			GameObjectBase *pObj = *it;
			if (!CAMERA->IsOutOfCamera(pObj))
				pObj->OnAfterProcess();

			// Now vehicle handled by player
			if (((VehicleObjectBase*)pObj)->m_pPassenger != nullptr)
			{
				it = m_vecVehicle.erase(it);
			}
			else if (pObj->m_nState == STATE_DEAD)
			{
				//delete pObj;
				vecDeads.push_back(pObj);
				it = m_vecVehicle.erase(it);
			}
			else
				++it;
		}
	}

	// EventObject
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEventObject.begin();
		for (; it != m_vecEventObject.end(); )
		{
			GameObjectBase *pObj = *it;
			if (!pObj->IsOutOfCamera())
				pObj->OnAfterProcess();

			if (pObj->m_nState == STATE_DEAD)
			{
				//delete pObj;
				vecDeads.push_back(pObj);
				it = m_vecEventObject.erase(it);
			}
			else if (pObj->m_nState == STATE_CARRYBYHAND)
			{
				it = m_vecEventObject.erase(it);
			}
			else
				++it;
		}
	}

	// Items
	{
		std::vector<GameObjectBase*>::iterator it = m_vecItem.begin();
		for (; it != m_vecItem.end(); )
		{
			GameObjectBase *pObj = *it;
			if (!pObj->IsOutOfCamera())
				pObj->OnAfterProcess();

			if (pObj->m_nState == STATE_DEAD)
			{
				//delete pObj;
				vecDeads.push_back(pObj);
				it = m_vecItem.erase(it);
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
			bool bOutOfCamera = CAMERA->IsOutOfCamera(pObj);
			if (!bOutOfCamera || (bOutOfCamera && pObj->m_nState != STATE_IDLE))
				pObj->OnAfterProcess();

			if (pObj->m_nState == STATE_DEAD)
			{
				//delete pObj;
				vecDeads.push_back(pObj);
				it = m_vecEnemy.erase(it);
			}
			else if (pObj->m_nState == STATE_CARRYBYHAND)
			{
				it = m_vecEnemy.erase(it);
			}
			else
				++it;
		}		
	}

	// Bullet
	{
		std::vector<GameObjectBase*>::iterator it = m_vecBullet.begin();
		for (; it != m_vecBullet.end(); )
		{
			GameObjectBase *pObj = *it;
			pObj->OnAfterProcess();

			if (pObj->m_nState == STATE_DEAD)
			{
				//delete pObj;
				vecDeads.push_back(pObj);
				it = m_vecBullet.erase(it);
			}
			else
				++it;
		}
	}

	// Animation
	{
		for (std::map<int, int>::iterator it = m_mapHitFrame.begin(); it != m_mapHitFrame.end(); )
		{
			if (it->second == 0)
				// for fix Runtime Error -> Copy iterator
				m_mapHitFrame.erase(it++);
			else
				++it;
		}

		for (std::map<int, int>::iterator it = m_mapPressFrame.begin(); it != m_mapPressFrame.end(); )
		{
			if (it->second == 0)
				m_mapPressFrame.erase(it++);
			else
				++it;
		}
	}

	// Reserved GameObjects
	{
		// When vector size changed, iterator will be broken
		std::vector<GameObjectBase*>::iterator it;
		it = m_vecReservedGameObject.begin();
		for (; it != m_vecReservedGameObject.end(); )
		{
			GameObjectBase *pObj = *it;
			if (pObj->m_nType >= ITEM_BEGIN && pObj->m_nType <= ITEM_END)
				m_vecItem.push_back(pObj);
			else if (pObj->m_nType >= EFFECT_BEGIN && pObj->m_nType <= EFFECT_END)
				m_vecEffect.push_back(pObj);
			else if (pObj->m_nType >= ENEMY_BEGIN && pObj->m_nType <= ENEMY_END)
				m_vecEnemy.push_back(pObj);
			else if (pObj->m_nType >= EVENTOBJECT_BEGIN && pObj->m_nType <= EVENTOBJECT_END)
				m_vecEventObject.push_back(pObj);
			else if (pObj->m_nType >= VEHICLE_BEGIN && pObj->m_nType <= VEHICLE_END)
				m_vecVehicle.push_back(pObj);

			it = m_vecReservedGameObject.erase(it);
		}
	}

	// cleaning Deads
	{
		std::vector<GameObjectBase*>::iterator it = vecDeads.begin();
		for (; it != vecDeads.end(); )
		{
			GameObjectBase *pObj = *it;
			
			NaDebugOut(L"Dead: %ls (0x%08x)\n", pObj->GetTypeName(), pObj);
			delete pObj;
			it = vecDeads.erase(it);
		}
	}
}

void Stage::ProcessEffects()
{
	// Effect
	{
		std::vector<GameObjectBase*>::iterator it = m_vecEffect.begin();
		for (; it != m_vecEffect.end(); )
		{
			GameObjectBase *pEffect = *it;
			pEffect->OnBeforeProcess();
			pEffect->Process();
			pEffect->OnAfterProcess();

			if (pEffect->m_nState == STATE_DEAD)
			{
				delete pEffect;
				it = m_vecEffect.erase(it);
			}
			else
				++it;
		}
	}
}

void Stage::HitBlock(int x, int y, int nPower, GameObjectBase *pWho, GameObjectBase *pOwner)
{
	// POWER_HITBLOCK_NORMAL: Hit Brick
	// POWER_HITBLOCK_BIGMARIO: Destroy SoftBrick
	// POWER_HITBLOCK_GIANTSHELL: 
	// POWER_HITBLOCK_GIANTMARIO: Destroy All Block

	int nIdx = GetTileIndex(x, y);
	int nData = GetTileData(x, y);
	int nType = GetDataType(nData);
	bool bHitAnimation = false;

	POINT ptBlockCenter =
	{
		x * TILE_XS + TILE_XS / 2,
		y * TILE_YS - 1
	};

	if (nType == TILETYPE_ITEM || HasItem(x, y))
	{
		// Popup Item
		PopItemUp(x, y, pOwner);
		bHitAnimation = true;
	}
	else if (nType == TILETYPE_SOFTHARD && nPower >= POWER_HITBLOCK_BIGMARIO)
	{
		CreateParameter param;
		param.fXS = -1.0f;
		param.fYS = -6.0f;
		CreateEffect(ptBlockCenter.x, ptBlockCenter.y + (TILE_YS), EFFECT_BRICKDEBRIS, &param);
		param.fXS = 1.0f;
		param.fYS = -6.0f;
		CreateEffect(ptBlockCenter.x, ptBlockCenter.y + (TILE_YS), EFFECT_BRICKDEBRIS, &param);
		param.fXS = -1.0f;
		param.fYS = -4.0f;
		CreateEffect(ptBlockCenter.x, ptBlockCenter.y + (TILE_YS), EFFECT_BRICKDEBRIS, &param);
		param.fXS = 1.0f;
		param.fYS = -4.0f;
		CreateEffect(ptBlockCenter.x, ptBlockCenter.y + (TILE_YS), EFFECT_BRICKDEBRIS, &param);

		DestroyTile(x, y);
		bHitAnimation = false;

		//nType = TILETYPE_EMPTY;
		//nData = TILE_EMPTY;

		SOUND_MANAGER->PlaySoundEffect(L"BreakBlock");
	}
	else if (nType == TILETYPE_SOFTHARD || nType == TILETYPE_JUMP || nType == TILETYPE_ITEM ||
		nData == TILE_HIDDENITEM || nData == TILE_MESSAGE)
		bHitAnimation = true;
	
	// Process Animation & Type Change
	if (nType == TILETYPE_SOFTHARD || nType == TILETYPE_JUMP || nType == TILETYPE_ITEM ||
		nData == TILE_HIDDENITEM || nData == TILE_MESSAGE || bHitAnimation)
	{
		NaRect rcBlock = GetTileRect(x, y);
		rcBlock.top -= 1;
		rcBlock.bottom = rcBlock.top + 1;
		rcBlock.left += 1;
		rcBlock.right -= 1;

		if (bHitAnimation)
		{
			m_mapHitFrame.insert(std::pair<int, int>(nIdx, 15));
		}

		std::vector<GameObjectBase*> vecGameObjects;
		vecGameObjects.push_back(CUR_PLAYER->GetLastControlObject());
		vecGameObjects.insert(vecGameObjects.end(), m_vecItem.begin(), m_vecItem.end());
		vecGameObjects.insert(vecGameObjects.end(), m_vecEnemy.begin(), m_vecEnemy.end());
		vecGameObjects.insert(vecGameObjects.end(), m_vecEventObject.begin(), m_vecEventObject.end());

		std::vector<GameObjectBase*>::iterator it = vecGameObjects.begin();
		for (; it != vecGameObjects.end(); ++it )
		{
			GameObjectBase *pObj = *it;
			NaRect rcObj = pObj->GetRect();
			if (pObj == pWho)
				continue; 
			if (pObj->m_nState == STATE_ITEMPOP)
				continue;
			if (pObj->CanPushedUp() == false)
				continue;

			if (pObj->IsAlive() && rcBlock.IsOverlapped(rcObj))
			{
				if (pObj->m_fX < ptBlockCenter.x && pObj->m_fXS > 0)
				{
					if (pObj->m_fXS > 0)
						pObj->m_fXS *= -1;
					pObj->m_bFlip = true;
				}

				if (pObj->m_fX > ptBlockCenter.x && pObj->m_fXS < 0)
				{
					if (pObj->m_fXS < 0)
						pObj->m_fXS *= -1;
					pObj->m_bFlip = false; 
				}

				ChangeStateInfo info;
				info.pFromObject = pWho;
				pObj->ChangeState(STATE_PUSHEDUP, &info);
			}
		}

		if (nData == TILE_MESSAGE)
		{
			NaDebugOut(L"MessageBlock: %d,%d / idx: %d\n", x, y, nIdx);

			std::map<int, NaString>::iterator it = m_mapMessage.find(nIdx);
			if (it != m_mapMessage.end())
			{
				GameStateChangeParameter param;
				param.m_strOption = it->second;
				m_pGame->ChangeState(GAMESTATE_MESSAGEBLOCK, &param);
			}
		}
	}

	if (pWho)
	{
		if (!pWho->IsOutOfCamera())
		{
			SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
		}
	}
	else
	{
		SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
	}
}

void Stage::CrushBlock(int x, int y, GameObjectBase * pWho)
{
	int nData = GetTileData(x, y);
	int nType = GetDataType(nData);
	POINT ptBlockCenter =
	{
		x * TILE_XS + TILE_XS / 2,
		y * TILE_YS - 1
	};

	int nPipeLT = -1;
	switch (nData)
	{
	case TILE_PIPEHEAD_L:
		nPipeLT = x;
		break;
	case TILE_PIPEHEAD_R:
		nPipeLT = x - 1;
		break;
	}

	DestroyTile(x, y);

	if (nPipeLT != -1)
	{
		bool bStop = false;
		for (int _y = y + 1; _y < GameDefaults::nPageTileHeight; _y++)
		{
			for (int _x = nPipeLT; _x < nPipeLT + 2; _x++)
			{
				int nPipeData = GetTileData(_x, _y);
				if (nPipeData != TILE_PIPEHEAD_L &&
					nPipeData != TILE_PIPEHEAD_R &&
					nPipeData != TILE_PIPEBODY_L &&
					nPipeData != TILE_PIPEBODY_R)
				{
					bStop = true;
					break;
				}

				CrushBlock(_x, _y, pWho);
			}
			if (bStop)
				break;
		}

		SetTileData(nPipeLT, y, TILE_EMPTY);
		SetTileData(nPipeLT + 1, y, TILE_EMPTY);

		// Remove EnemyGenerator too
		auto rcTile = GetTileRect(x, y, false);
		BEGIN_VECTOR_LOOP(GameObjectBase*, m_vecEventObject) {
			auto rcObj = pObj->GetRect();
			if (rcTile.IsOverlapped(rcObj))
			{
				pObj->ChangeState(STATE_DEAD);
			}
		} END_VECTOR_LOOP();
	}

	if (nType == TILETYPE_ITEM || HasItem(x, y))
	{
		int nIdx = GetTileIndex(x, y);
		std::map<int, int>::iterator it = m_mapItem.find(nIdx);
		if (it != m_mapItem.end())
		{
			it->second = ITEM_COIN;
		}

		PopItemUp(x, y, pWho);
	}

	CreateParameter param;
	for (int i = 0; i < 4; i++)
	{
		if (nData != TILE_BRICK)
			param.nType = (nData * 4) + i;
		if (i % 2 == 0)
			param.fXS = pWho->m_fXS + ((rand() % 30) / 10.0f);
		else
			param.fXS = pWho->m_fXS - ((rand() % 30) / 10.0f);
		param.fYS = -(((rand() % 60) / 10.0f) + 1);
		CreateEffect(ptBlockCenter.x, ptBlockCenter.y + (TILE_YS), EFFECT_BRICKDEBRIS, &param);
	}
}

void Stage::PressBlock(int x, int y, int nPower, GameObjectBase * pWho)
{
	int nData = GetTileData(x, y);
	int nType = GetDataType(nData);

	int nPipeLT = -1;

	switch (nData)
	{
	case TILE_BRICK:
	case TILE_ITEM:
	case TILE_TAKENITEM:
	case TILE_HARDBRICK:
		if (nPower >= POWER_HITBLOCK_GIANTMARIO && CanCrush(x, y, nPower))
			CrushBlock(x, y, pWho);
		else if (nPower == POWER_HITBLOCK_NORMAL)
			;
		break;
	case TILE_PIPEHEAD_L:
		nPipeLT = x;
		break;
	case TILE_PIPEHEAD_R:
		nPipeLT = x - 1;
		break;
	}

	if (nPower == POWER_HITBLOCK_NORMAL)
	{
		int nIdx = GetTileIndex(x, y);
		int nFrame = GetJumpTileFrame(x, y);
		if (nFrame == -1)
		{
			m_mapPressFrame.insert(
				std::pair<int, int>(nIdx, 13)
			);

			if (HasItem(x, y))
			{
				PopItemUp(x, y, pWho, COLLISION_BOTTOM);
			}
		}
		else if (nFrame < 1) // changed from 8
		{
			std::map<int, int>::iterator it = m_mapPressFrame.find(nIdx);

			// Reset
			it->second = 13;
		}
	}

	if (nPower == POWER_HITBLOCK_GIANTMARIO && nPipeLT != -1)
	{
		for (int i=0; ; i++)
		{
			int nDataPipe = GetTileData(nPipeLT + i, y);
			SetTileData(nPipeLT + i, y, TILE_EMPTY);

			if (nDataPipe == TILE_PIPEHEAD_L)
				SetTileData(nPipeLT + i, y + 1, TILE_PIPEHEAD_L);
			if (nDataPipe == TILE_PIPEHEAD_R)
				SetTileData(nPipeLT + i, y + 1, TILE_PIPEHEAD_R);

			if (nDataPipe == TILE_PIPEHEAD_R)
				break;

			SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
		}

		NaRect rcTile = GetTileRect(nPipeLT, y);

		BEGIN_VECTOR_LOOP(PipeInfo*, CUR_STAGE->m_vecPipeInfo) {
			if (pObj->m_rcEnterance.IsOverlapped(rcTile))
			{
				pObj->m_rcEnterance.Offset(0, 16);
			}
		} END_VECTOR_LOOP();

		BEGIN_VECTOR_LOOP(PipeExitInfo*, CUR_STAGE->m_vecPipeExitInfo) {
			if (pObj->m_rcExit.IsOverlapped(rcTile))
			{
				pObj->m_rcExit.Offset(0, 16);
			}
		} END_VECTOR_LOOP();
	}
}

void Stage::PopItemUp(int x, int y, GameObjectBase * pWho, int nDir)
{
	int nIdx = GetTileIndex(x, y);
	bool bTakenItem = false;

	POINT ptBlockCenter =
	{
		x * TILE_XS + TILE_XS / 2,
		y * TILE_YS - 1
	};

	std::map<int, int>::iterator it = m_mapItem.find(nIdx);
	if (it != m_mapItem.end())
	{
		NaDebugOut(L"Item found: %d\n", it->second);
		int nItemType = it->second;
		bool bCreate = true;
		bool bGetCoin = false;
		int nItemState = -1;

		switch (nItemType)
		{
		case ITEM_MUSHROOM:
			if (pWho && pWho->HasMushroom())
				nItemType = ITEM_FIREFLOWER;

			if ((m_pGame->m_nSpecialEvents & SPECIALEVENT_GIANTMUSHROOM) &&
				m_pGame->m_nStagePackType == STAGEPACK_CLASSIC)
			{
				if (rand() % 100 <= 30)
					nItemType = ITEM_GIANTMUSHROOM;
			}
			break;
		case ITEM_COIN:
		case ITEM_COINS1:
			bGetCoin = true;
			
			bCreate = false;
			bTakenItem = true;
			break;
		case ITEM_COINS2: case ITEM_COINS3: case ITEM_COINS4:
		case ITEM_COINS5: case ITEM_COINS6: case ITEM_COINS7:
		case ITEM_COINS8: case ITEM_COINS9: case ITEM_COINS10:
		case ITEM_COINS:
			bGetCoin = true;
			
			it->second -= 1;
			bCreate = false;
			break;
		case ITEM_DOORKEYCOIN:
			nItemState = STATE_JUMP;
			pWho->OnApplyItem(nItemType);

			bCreate = true;
			bTakenItem = true;
			break;
		}

		if (bGetCoin)
		{
			CreateParameter param;
			if (pWho && pWho->m_pController)
			{
			
				param.pFromPlayer = GET_CONTROLLER_IF_PLAYER(pWho);
				pWho->m_pController->AddCoin();
			}
			CreateEffect(ptBlockCenter.x, ptBlockCenter.y, EFFECT_COIN, &param);
		}

		if (bCreate)
		{
			GameObjectBase *pObj = ReserveCreateGameObject((x * TILE_XS) + (TILE_XS / 2), (y * TILE_YS) + TILE_YS, nItemType);
			if (nItemState == -1)
			{
				if (nDir == COLLISION_BOTTOM)
				{
					pObj->m_nState = STATE_ITEMPOPDOWN;
					pObj->m_fY += 8;
				}
				else
					pObj->m_nState = STATE_ITEMPOP;
			}
			else
			{
				pObj->m_nState = nItemState;
				pObj->ChangeState(nItemState);
			}

			bTakenItem = true;
		}

		if (bTakenItem)
			m_mapItem.erase(it);
	}
	else
	{
		NaDebugOut(L"Item not found -> Default item\n");

		CreateParameter param;
		if (pWho && pWho->m_pController)
		{
			param.pFromPlayer = GET_CONTROLLER_IF_PLAYER(pWho);
			pWho->m_pController->AddCoin();
		}
		CreateEffect(ptBlockCenter.x, ptBlockCenter.y, EFFECT_COIN, &param);

		bTakenItem = true;
	}

	if (bTakenItem)
	{
		int nData = GetTileData(x, y);
		if (nData != TILE_JUMP)
			SetTileData(x, y, TILE_TAKENITEM);
	}
}

bool Stage::PressJumpBlock(int x, int y, GameObjectBase * pWho)
{
	bool bPressed = false;
	if (IsJumpTile(x, y))
	{
		int nIdx = GetTileIndex(x, y);
		int nFrame = GetJumpTileFrame(x, y);
		if (nFrame == -1)
		{
			m_mapPressFrame.insert(
				std::pair<int, int>(nIdx, 13)
			);

			if (HasItem(x, y))
			{
				PopItemUp(x, y, pWho, COLLISION_BOTTOM);
			}

			bPressed = true;
		}
		else if (nFrame < 1) // changed from 8
		{
			std::map<int, int>::iterator it = m_mapPressFrame.find(nIdx);

			// Reset
			it->second = 13;
			bPressed = true;
		}
	}

	if (bPressed)
	{
		SOUND_MANAGER->PlaySoundEffect(L"HitBlock");
	}

	return bPressed;
}

bool Stage::CanCrush(int x, int y, int nPower)
{
	int nData = GetTileData(x, y);
	switch (nData)
	{
	case TILE_GROUND:
		if (GetTileData(x + 1, y - 1) == TILE_HPIPEHEAD_B)
			return false;
		if (nPower >= POWER_HITBLOCK_GIANTMARIO && y <= 12)
			return true;
		return false;
		break;
	case TILE_BRICK:
	case TILE_ITEM:
	case TILE_TAKENITEM:
	case TILE_HARDBRICK:
		{
			if (GetTileData(x + 1, y - 1) == TILE_HPIPEHEAD_B)
				return false;

			int nDataOver = GetTileData(x, y - 1);
			if (nDataOver == TILE_PIPEHEAD_L ||
				nDataOver == TILE_PIPEHEAD_R ||
				nDataOver == TILE_PIPEBODY_L ||
				nDataOver == TILE_PIPEBODY_R)
				return false;

			if (GetTileData(x - 1, y) == TILE_BRIDGE_B ||
				GetTileData(x + 1, y) == TILE_BRIDGE_B)
				return false;

			if (nPower == POWER_HITBLOCK_GIANTMARIO && y > 12)
				return false;
		}
		return true;
		break;
	case TILE_PIPEHEAD_L:
	case TILE_PIPEHEAD_R:
		if (CanCrushPipe(x, y))
			return true;
		return false;
	case TILE_PIPEBODY_L:
	case TILE_PIPEBODY_R:
	case TILE_HPIPEHEAD_T:
	case TILE_HPIPEHEAD_B:
	case TILE_HPIPEBODY_T:
	case TILE_HPIPEBODY_B:
	case TILE_PIPEJUNCTION_T:
	case TILE_PIPEJUNCTION_B:
	case TILE_PIPEBOTTOM_L:
	case TILE_PIPEBOTTOM_R:
		return false;
	case TILE_ISLANDHEAD_L:
	case TILE_ISLANDHEAD_C:
	case TILE_ISLANDHEAD_R:
	case TILE_MUSHROOMHEAD_L:
	case TILE_MUSHROOMHEAD_C:
	case TILE_MUSHROOMHEAD_R:
	case TILE_BRIDGE_B:
	case TILE_MESSAGE:
		return false;
	case TILE_JUMP:
		if (nPower >= POWER_HITBLOCK_GROWING_GIANTMARIO)
			return true;
		return false;
	default:
		return true;
	}

	return false;
}

// Can press and destroy?
bool Stage::CanPress(int x, int y)
{
	int nData = GetTileData(x, y);
	int nType = GetDataType(nData);

	switch (nData)
	{
	case TILE_BRICK:
	case TILE_ITEM:
	case TILE_TAKENITEM:
	case TILE_HARDBRICK:
		if (y > 12)
			return false;

		if (GetTileData(x + 1, y - 1) == TILE_HPIPEHEAD_B)
			return false;

		return true;
	case TILE_PIPEHEAD_L:
	case TILE_PIPEHEAD_R:
		{
			int nDataUnder = GetTileData(x, y + 1);
			if (nDataUnder == TILE_PIPEBODY_L ||
				nDataUnder == TILE_PIPEBODY_R)
			{
				return true;
			}
			else
			{
				if (nDataUnder == TILE_GROUND &&
					GetTileData(x, y + 2) == TILE_GROUND &&
					y <= 12)
					return true;
			}
			return false;
		}
		break;
	default:
		return false;
	}

	return false;
}

bool Stage::CanCrushPipe(int x, int y)
{
	int nData = GetTileData(x, y);
	switch (nData)
	{
	case TILE_PIPEHEAD_L:
	case TILE_PIPEHEAD_R:
		{
			NaRect rcTile = GetTileRect(x, y);
			rcTile.bottom += 1;

			BEGIN_VECTOR_LOOP(PipeInfo*, CUR_STAGE->m_vecPipeInfo) {
				if (pObj->m_rcEnterance.IsOverlapped(rcTile))
					return false;
			} END_VECTOR_LOOP();

			BEGIN_VECTOR_LOOP(PipeExitInfo*, CUR_STAGE->m_vecPipeExitInfo) {
				if (pObj->m_rcExit.IsOverlapped(rcTile))
					return false;
			} END_VECTOR_LOOP();
		}
		break;
	default:
		return false;
		break;
	}

	return true;
}

GameObjectBase* Stage::CreateItem(int x, int y, int type)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->m_fX = x;
		pObj->m_fY = y;
		pObj->m_nType = type;
		m_vecItem.push_back(pObj);
	}
	return pObj;
}

GameObjectBase* Stage::CreateEffect(int x, int y, int type, CreateParameter *pParam)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->m_fX = x;
		pObj->m_fY = y;
		pObj->ParseParameter(pParam);
		m_vecEffect.push_back(pObj);
	}
	return pObj;
}

GameObjectBase* Stage::CreateBullet(int x, int y, int type, CreateParameter * pParam)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->m_fX = x;
		pObj->m_fY = y;
		pObj->ParseParameter(pParam);
		m_vecBullet.push_back(pObj);
	}
	return pObj;
}

GameObjectBase* Stage::CreateEventObject(int x, int y, int type, CreateParameter * pParam)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->m_fX = x;
		pObj->m_fY = y;
		pObj->ParseParameter(pParam);
		m_vecEventObject.push_back(pObj);
	}
	return pObj;
}

GameObjectBase * Stage::CreateVehicle(int x, int y, int type, CreateParameter * pParam)
{
	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->m_fX = x;
		pObj->m_fY = y;
		pObj->ParseParameter(pParam);
		m_vecVehicle.push_back(pObj);
	}
	return pObj;
}

GameObjectBase * Stage::ReserveCreateGameObject(int x, int y, int type, CreateParameter * pParam)
{
	bool bPowerUp = false;
	bool bWinged = false;

	if (type & ENEMY_MASK_POWERUP)
	{
		bPowerUp = true;
		type = type - ENEMY_MASK_POWERUP;
	}

	if (type & ENEMY_MASK_WINGED)
	{
		bWinged = true;
		type = type - ENEMY_MASK_WINGED;
	}

	GameObjectBase *pObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->x = x;
		pObj->y = y;
		pObj->m_nType = type;

		if (type >= EFFECT_BEGIN && type <= EFFECT_END)
			pObj->ParseParameter(pParam);

		if (bPowerUp)
		{
			pObj->m_bPowerUp = true;
			pObj->OnPowerUp();
		}
		if (bWinged)
			pObj->m_bWinged = true;

		m_vecReservedGameObject.push_back(pObj);
		return pObj;
	}
	return nullptr;
}

int Stage::GetObjectCount(int nType, bool bInCameraOnly)
{
	// Reference dummy vector to default.
	// Ref = Vec causes copy elements
	std::vector<GameObjectBase*> &vec = std::vector<GameObjectBase*>();
	int nNonMaskedType = nType & ~ENEMY_MASK_POWERUP & ~ENEMY_MASK_WINGED;

	if (nNonMaskedType >= EFFECT_BEGIN && nNonMaskedType <= EFFECT_END)
		vec = m_vecEffect;
	else if (nNonMaskedType >= ITEM_BEGIN && nNonMaskedType <= ITEM_END)
		vec = m_vecItem;
	else if (nNonMaskedType >= ENEMY_BEGIN && nNonMaskedType <= ENEMY_END)
		vec = m_vecEnemy;
	else if (nNonMaskedType >= EVENTOBJECT_BEGIN && nNonMaskedType <= EVENTOBJECT_END)
		vec = m_vecEventObject;
	else if (nNonMaskedType >= VEHICLE_BEGIN && nNonMaskedType <= VEHICLE_END)
		vec = m_vecVehicle;
	else if (nNonMaskedType >= BULLET_BEGIN && nNonMaskedType <= BULLET_END)
		vec = m_vecBullet;
	else
		return 0;

	int nCnt = 0;
	BEGIN_VECTOR_LOOP(GameObjectBase*, vec) {
		if (pObj->GetMaskedType() != nType)
			continue;
		if (bInCameraOnly && !pObj->IsOutOfCamera())
			nCnt++;
		else if (bInCameraOnly == false)
			nCnt++;
	} END_VECTOR_LOOP();
	
	return nCnt;
}

void Stage::TileizeMapObjects()
{
	for (int i = 0; i < 10; i++)
	{
		std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
		if (it == m_vecMapObject.end())
			break;

		for (; it != m_vecMapObject.end(); )
		{
			MapObjectBase *pObj = *it;
			bool bRet = pObj->Tileize(i);
			if (bRet)
			{
				delete pObj;
				it = m_vecMapObject.erase(it);
			}
			else
				it++;
		}
	}
}

void Stage::InitTheme()
{
	m_pGame->m_pThemeManager->LoadSettings(this);

	m_pGame->m_pThemeManager->ApplyThemeSet(this);

	if (m_pGame->m_pCurPlayer)
		m_pGame->m_pCurPlayer->ApplyTheme();

	m_pSpriteVehicle = m_pGame->m_pThemeManager->RequestSprite(L"Vehicle", m_nTheme);
}

void Stage::InitItems()
{
	// Put items on block & Create instance if not in block.
	std::map<int, int>::iterator it = m_mapItem.begin();
	for (; it != m_mapItem.end(); )
	{
		if (it->second == ITEM_DOORKEYCOIN)
			m_nMaxDoorKey++;

		if (it->second == ITEM_COIN) 
		{
			// Put coins into block
			switch (m_pTileData[it->first])
			{
			case TILE_EMPTY:
				{
					POINT pt = GetTilePosition(it->first);
					CreateItem(
						(pt.x * TILE_XS) + (TILE_XS / 2),
						(pt.y * TILE_YS) + TILE_YS,
						ITEM_COIN);
					m_mapItem.erase(it++);
					continue;
				}
				break;
			case TILE_BRICK:
				it->second = ITEM_COINS;
				break;
			case TILE_ITEM:
				it->second = ITEM_COINS1;
				break;
			default:
				if (IsBackgroundTile(m_pTileData[it->first]))
				{
					POINT pt = GetTilePosition(it->first);
					CreateItem(
						(pt.x * TILE_XS) + (TILE_XS / 2),
						(pt.y * TILE_YS) + TILE_YS,
						ITEM_COIN);
					m_mapItem.erase(it++);
					continue;
				}
				break;
			}
		}
		else if (IsBackgroundTile(m_pTileData[it->first]) ||
			m_pTileData[it->first] == TILE_EMPTY)
		{
			// Live items!
			POINT pt = GetTilePosition(it->first);
			GameObjectBase *pObj = CreateItem(
				(pt.x * TILE_XS) + (TILE_XS / 2),
				(pt.y * TILE_YS) + TILE_YS,
				it->second);
			if (pObj)
				pObj->SetState(pObj->m_nBaseState);
			else
			{
				NaDebugOut(L"Error: Cannot Create Item: %d\n", it->second);
			}

			m_mapItem.erase(it++);
			continue;
		}

		++it;
	}

	// Check Enemies, Objects on the block
	// If exists make it as item
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEnemy) {
		if (!pObj->CanBornFromBlock())
		{
			++it;
			continue;
		}

		int x = (pObj->m_fX / TILE_XS);
		int y = (pObj->m_fY - 1) / TILE_YS;
		int nData = GetTileData(x, y);

		switch (nData)
		{
		case TILE_BRICK:
		case TILE_ITEM:
		case TILE_HIDDENITEM:
		case TILE_JUMP:
			{
				EnemyObjectBase *pEnemy = (EnemyObjectBase*)pObj;
				int nType = pEnemy->GetMaskedType();

				SetItem(x, y, nType);
				it = m_vecEnemy.erase(it);
			}
			break;
		default:
			++it;
			break;
		}
	} END_VECTOR_LOOP();

	// EventObject
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEventObject) {
		if (!pObj->CanBornFromBlock())
		{
			++it;
			continue;
		}

		int x = (pObj->m_fX / TILE_XS);
		int y = (pObj->m_fY - 1) / TILE_YS;
		int nData = GetTileData(x, y);

		switch (nData)
		{
		case TILE_BRICK:
		case TILE_ITEM:
		case TILE_HIDDENITEM:
		case TILE_JUMP:
			SetItem(x, y, pObj->m_nType);
			it = m_vecEventObject.erase(it);
			break;
		default:
			++it;
			break;
		}
	} END_VECTOR_LOOP();

	// Vehicle
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecVehicle) {
		if (!pObj->CanBornFromBlock())
		{
			++it;
			continue;
		}

		int x = (pObj->m_fX / TILE_XS);
		int y = (pObj->m_fY - 1) / TILE_YS;
		int nData = GetTileData(x, y);

		switch (nData)
		{
		case TILE_BRICK:
		case TILE_ITEM:
		case TILE_HIDDENITEM:
		case TILE_JUMP:
			{
				VehicleObjectBase *pVehicle = (VehicleObjectBase*)pObj;
				int nType = pVehicle->GetMaskedType();

				SetItem(x, y, nType);
				it = m_vecVehicle.erase(it);
			}
			break;
		default:
			++it;
			break;
		}
	} END_VECTOR_LOOP();
}

void Stage::InitObjectIntoPipe()
{
	// Items
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecItem) {
		int x1 = ((pObj->m_fX) / TILE_XS);
		int x2 = ((pObj->m_fX + 9) / TILE_XS);
		int y = (pObj->m_fY + 1) / TILE_YS;

		int nDataL = GetTileData(x1, y);
		int nDataR = GetTileData(x2, y);
		if (nDataL == TILE_PIPEHEAD_L && nDataR == TILE_PIPEHEAD_R)
		{
			EnemyGenerator *pGen = (EnemyGenerator*)CreateEventObject(pObj->m_fX, pObj->m_fY, EVENTOBJECT_ENEMYGENERATOR);
			if (pGen)
			{
				pGen->m_nGeneratePosition = 3;
				pGen->m_nGenerateType = pObj->m_nType;
				pGen->m_nMaxCount = 1;
				pGen->m_nInterval = 5;
			}

			it = m_vecItem.erase(it);
		}
		else
			++it;
	} END_VECTOR_LOOP();

	// Enemies
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEnemy) {
		if (pObj->m_nType == ENEMY_PIRANHA_PLANT ||
			pObj->m_nType == ENEMY_FIREPIRANHAPLANT ||
			pObj->m_nType == ENEMY_MUNCHER)
		{
			++it;
			continue;
		}

		int x1 = ((pObj->m_fX - 1) / TILE_XS);
		int x2 = ((pObj->m_fX + 1) / TILE_XS);
		int y = (pObj->m_fY + 1) / TILE_YS;

		int nDataL = GetTileData(x1, y);
		int nDataR = GetTileData(x2, y);
		if (nDataL == TILE_PIPEHEAD_L && nDataR == TILE_PIPEHEAD_R)
		{
			EnemyGenerator *pGen = (EnemyGenerator*)CreateEventObject(pObj->m_fX - 8, pObj->m_fY, EVENTOBJECT_ENEMYGENERATOR);
			if (pGen)
			{
				pGen->m_nGeneratePosition = 3;
				pGen->m_nGenerateType = pObj->GetMaskedType();
				pGen->m_nMaxCount = 1;
				pGen->m_nInterval = 5;
			}

			it = m_vecEnemy.erase(it);
		}
		else
			++it;
	} END_VECTOR_LOOP();

	// EventObject
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEventObject) {
		if (pObj->m_nType != EVENTOBJECT_P_SWITCH)
		{
			++it;
			continue;
		}

		int x1 = ((pObj->m_fX - 1) / TILE_XS);
		int x2 = ((pObj->m_fX + 1) / TILE_XS);
		int y = (pObj->m_fY + 1) / TILE_YS;

		int nDataL = GetTileData(x1, y);
		int nDataR = GetTileData(x2, y);
		if (nDataL == TILE_PIPEHEAD_L && nDataR == TILE_PIPEHEAD_R)
		{
			EnemyGenerator *pGen = (EnemyGenerator*)ReserveCreateGameObject(pObj->m_fX - 8, pObj->m_fY, EVENTOBJECT_ENEMYGENERATOR);
			if (pGen)
			{
				pGen->m_nGeneratePosition = 3;
				pGen->m_nGenerateType = pObj->m_nType;
				pGen->m_nMaxCount = 1;
				pGen->m_nInterval = 5;
			}

			it = m_vecEventObject.erase(it);
		}
		else
			++it;
	} END_VECTOR_LOOP();

	// Vehicle
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecVehicle) {
		int x1 = ((pObj->m_fX - 1) / TILE_XS);
		int x2 = ((pObj->m_fX + 1) / TILE_XS);
		int y = (pObj->m_fY + 1) / TILE_YS;

		int nDataL = GetTileData(x1, y);
		int nDataR = GetTileData(x2, y);
		if (nDataL == TILE_PIPEHEAD_L && nDataR == TILE_PIPEHEAD_R)
		{
			EnemyGenerator *pGen = (EnemyGenerator*)CreateEventObject(pObj->m_fX - 8, pObj->m_fY, EVENTOBJECT_ENEMYGENERATOR);
			if (pGen)
			{
				pGen->m_nGeneratePosition = 3;
				pGen->m_nGenerateType = pObj->m_nType;
				pGen->m_nMaxCount = 1;
				pGen->m_nInterval = 5;
			}

			it = m_vecVehicle.erase(it);
		}
		else
			++it;
	} END_VECTOR_LOOP();
}

void Stage::BuildBackground()
{
	if (m_pTileBackgroundData)
		delete[] m_pTileBackgroundData;

	m_pTileBackgroundData = new BYTE[m_sizeTile.cx * m_sizeTile.cy];

	memset(m_pTileBackgroundData, 0, sizeof(BYTE) * m_sizeTile.cx * m_sizeTile.cy);

	BackgroundBase *pBg = BackgroundBase::CreateInstance(m_nBackgroundTheme, this);
	if (pBg)
	{
		pBg->BuildBackground();

		delete pBg;
	}

	// Removed; Auto Background Color
	m_pGame->m_pThemeManager->ApplyBackgroundColor(m_nTheme, m_nBackgroundTheme, m_lBackgroundColor);
}

void Stage::PlayBGM()
{
	SOUND_MANAGER->PlayStageBGM();
}

void Stage::PlaySound(const wchar_t * name)
{
	PLAYSOUND(name);
}

void Stage::SwapCoinAndBrick()
{
	NaDebugOut(L"Swap Coin<->Brick (%ls)\n",
		m_bSwapCoinBlock ? L"End" : L"Begin");
	m_bSwapCoinBlock = !m_bSwapCoinBlock;

	int nTileW = m_sizeTile.cx;
	int nTileH = m_sizeTile.cy;
	int nIdx = 0;
	for (int y = 0; y < nTileH; y++)
	{
		for (int x = 0; x < nTileW; x++)
		{
			int nData = m_pTileData[nIdx];
			if (nData == TILE_BRICK)
			{
				if (HasItem(x, y) == false)
					m_pTileData[nIdx] = 255;
			}
			
			nIdx++;
		}
	}

	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecItem) {
		if (pObj->m_nType == ITEM_COIN)
		{
			int x = (pObj->m_fX - (TILE_XS / 2)) / TILE_XS;
			int y = (pObj->m_fY - TILE_XS) / TILE_YS;

			delete pObj;
			it = m_vecItem.erase(it);
			SetTileData(x, y, TILE_BRICK);
		}
		else
			++it;
	} END_VECTOR_LOOP();

	nIdx = 0;
	for (int y = 0; y < nTileH; y++)
	{
		for (int x = 0; x < nTileW; x++)
		{
			int nData = m_pTileData[nIdx];
			if (nData == 255)
			{
				m_pTileData[nIdx] = TILE_EMPTY;
				CreateItem(
					(x * TILE_XS) + (TILE_XS / 2), 
					(y * TILE_YS) + TILE_YS,
					ITEM_COIN);
			}

			nIdx++;
		}
	}
}

bool Stage::CanEnterSky()
{
	if (m_strSkyStageName.GetLength() > 0)
		return true;
	return false;
}

bool Stage::CanSwim(NaRect &rc)
{
	if (m_nTheme == STAGETHEME_UNDERWATER ||
		m_nTheme == STAGETHEME_CASTLEUNDERWATER)
	{
		if (rc.bottom > 40)
			return true;
		return false;
	}

	std::vector<NaRect>::iterator it = m_vecSwimArea.begin();
	for (; it != m_vecSwimArea.end(); ++it)
	{
		if ((*it).IsOverlapped(rc))
			return true;
	}

	return false;
}

void Stage::BeginStopWatch()
{
	m_dwBeginTime = NaPlatform::GetTickCount();
	m_dwPlayTime = 0;
}

void Stage::EndStopWatch()
{
	if (m_dwPlayTime != 0)
		return;
	DWORD dwTick = NaPlatform::GetTickCount();
	m_dwPlayTime = dwTick - m_dwBeginTime;

	DWORD dwTime = m_dwPlayTime;
	NaDebugOut(L"Stage EndStopWatch: %02d:%02d:%02d.%02d (%ls)\n",
		(dwTime / 1000 / 60 / 60) % 24, // hour
		(dwTime / 1000 / 60) % 60, // min
		(dwTime / 1000) % 60, // sec
		(dwTime / 10) % 10, // milli sec
		m_strName.wstr()
	);
}

int Stage::GetTileData(int x, int y)
{
	if (x < 0 || x >= m_sizeTile.cx)
		return 0;

	if (y < 0)
		y = 0;
	if (y >= m_sizeTile.cy)
		y = m_sizeTile.cy - 1;

	CalcPageJumpedPos(x, y);

	int nIdx = GetTileIndex(x, y);
	return m_pTileData[nIdx];
}

int Stage::GetTileBackgroundData(int x, int y)
{
	if (m_pTileBackgroundData == nullptr)
		return 0;

	if (x < 0 || y < 0 || x >= m_sizeTile.cx || y >= m_sizeTile.cy)
		return 0;

	CalcPageJumpedPos(x, y);

	int nIdx = GetTileIndex(x, y);
	return m_pTileBackgroundData[nIdx];
}

void Stage::SetItem(int x, int y, int type)
{
	int nIdx = GetTileIndex(x, y);

	std::map<int, int>::iterator it = m_mapItem.find(nIdx);
	if (it != m_mapItem.end())
	{
		it->second = type;
	}
	else
	{
		m_mapItem.insert(
			std::pair<int, int>(nIdx, type)
		);
	}
}

int Stage::GetItem(int x, int y)
{
	int nIdx = GetTileIndex(x, y);

	std::map<int, int>::iterator it = m_mapItem.find(nIdx);
	if (it != m_mapItem.end())
		return it->second;

	return -1;
}

NaRect Stage::GetTileRect(int x, int y, bool bAddAnimationOffset)
{
	NaRect rc;
	rc.left = x * TILE_XS;
	rc.top = y * TILE_YS;
	rc.right = (x + 1) * TILE_XS;
	rc.bottom = (y + 1) * TILE_YS;

	if (bAddAnimationOffset)
	{
		POINT ptOffset = GetHitAnimatedTileOffset(x, y);
		rc.Offset(ptOffset.x, ptOffset.y);
	}

	return rc;
}

bool Stage::HasItem(int x, int y)
{
	int nIdx = GetTileIndex(x, y);

	//int nData = GetTileData(x, y);
	//if (nData == TILE_EMPTY)
	//	return false;

	std::map<int, int>::iterator it = m_mapItem.find(nIdx);
	if (it != m_mapItem.end())
		return true;
	return false;
}

POINT Stage::GetHitAnimatedTileOffset(int x, int y)
{
	int nIdx = GetTileIndex(x, y);
	POINT pt = { 0, 0 };

	{
		std::map<int, int>::iterator it = m_mapHitFrame.find(nIdx);
		if (it != m_mapHitFrame.end())
		{
			int nHitFrame = it->second;
			if (nHitFrame < 0)
				nHitFrame = 0;

			const int nHitAnimOffset[16] = { 0, 0, 1, -1, -3, -4, -5, -6, -7, -7, -7, -6, -6, -5, -3, -1 };
			pt.y += nHitAnimOffset[nHitFrame];

		}
	}

	{
		std::map<int, int>::iterator it = m_mapPressFrame.find(nIdx);
		if (it != m_mapPressFrame.end())
		{
			int nHitFrame = it->second;
			if (nHitFrame < 0)
				nHitFrame = 0;

			const int nPressAnimOffset[14] = 
			{ 
				0, 1, 2, 3, 4, 5, 6, 8, 8, 7, 6, 5, 3, 1,
			};
			pt.y += nPressAnimOffset[nHitFrame];
		}
	}

	return pt;
}

int Stage::GetJumpTileFrame(int x, int y)
{
	int nIdx = GetTileIndex(x, y);
	std::map<int, int>::iterator it = m_mapPressFrame.find(nIdx);
	if (it != m_mapPressFrame.end())
		return it->second;

	return -1;
}

int Stage::GetAnimatedTileIndex(int nType)
{
	switch (nType)
	{
	case TILE_ITEM:
		{
			int nVal = (m_pGame->m_nGameFrame % 48);
			if (IS_CLASSIC)
			{
				if (nVal < 24)
					return TILE_ITEM;
				else if (nVal < 32)
					return TILE_ITEM2;
				else if (nVal < 40)
					return TILE_ITEM3;
				else if (nVal < 48)
					return TILE_ITEM4;
			}
			else
			{
				if (nVal < 12)
					return TILE_ITEM1;
				else if (nVal < 24)
					return TILE_ITEM2;
				else if (nVal < 36)
					return TILE_ITEM3;
				else if (nVal < 48)
					return TILE_ITEM4;
			}
		}
		break;
	case TILE_BRICK:
		if (!IS_CLASSIC)
		{
			int nVal = (m_pGame->m_nGameFrame % 48);
			if (nVal < 12)
				return TILE_BRICK1;
			else if (nVal < 24)
				return TILE_BRICK2;
			else if (nVal < 36)
				return TILE_BRICK3;
			else if (nVal < 48)
				return TILE_BRICK4;
		}
		break;
	case TILE_SPIKE:
		{
			int nVal = (m_pGame->m_nGameFrame % 48);
			if (nVal < 36)
				return TILE_SPIKE;
			else
				return TILE_SPIKE2;
		}
		break;
	case TILE_RIVER_T:
		{
			if (!m_pGame->m_bModernStyle)
				return TILE_RIVER_T;

			// We lost original image
			int nVal = (m_pGame->m_nGameFrame % 80);
			if (nVal < 20)
				return TILE_RIVER_T2;
			else if (nVal < 40)
				return TILE_RIVER_T3;
			else if (nVal < 60)
				return TILE_RIVER_T4;
			else if (nVal < 80)
				return TILE_RIVER_T5;
		}
		break;
	case TILE_JUMP:
		if (!IS_CLASSIC)
		{
			int nVal = (m_pGame->m_nGameFrame % 48);
			if (nVal < 12)
				return TILE_JUMP1;
			else if (nVal < 24)
				return TILE_JUMP2;
			else if (nVal < 36)
				return TILE_JUMP3;
			else if (nVal < 48)
				return TILE_JUMP4;
		}
	}

	return nType;
}

int Stage::GetAnimatedBackgroundTileIndex(int nType)
{
	switch (nType)
	{
	case TILEBG_WATER_T:
		int nVal = (m_pGame->m_nGameFrame % 80);
		if (nVal < 20)
			return TILEBG_WATER_T2;
		else if (nVal < 40)
			return TILEBG_WATER_T3;
		else if (nVal < 60)
			return TILEBG_WATER_T4;
		else if (nVal < 80)
			return TILEBG_WATER_T5;
		break;
	}
	return nType;
}

int Stage::GetAutoGroundTileIndex(int x, int y)
{
	int nMatrix[9] = {
		GetTileData(x - 1, y - 1), GetTileData(x, y - 1), GetTileData(x + 1, y - 1),
		GetTileData(x - 1, y), TILE_GROUND, GetTileData(x + 1, y),
		GetTileData(x - 1, y + 1), GetTileData(x, y + 1), GetTileData(x + 1, y + 1),
	};

	bool m[9];
	for (int i=0; i<9; i++)
	{
		if (nMatrix[i] == TILE_GROUND)
			m[i] = true;
		else
			m[i] = false;
	}

	enum MatrixDirs { MAT_LT = 0, MAT_T, MAT_RT, MAT_L, MAT_C, MAT_R, MAT_LB, MAT_B, MAT_RB };

	// Pattern match
	unsigned char nPatterns[][10] = 
	{
		{ 0,0,0, 0,1,1, 0,1,1, TILE_X225 },
		{ 0,0,0, 0,1,1, 1,1,1, TILE_X225 },
		{ 0,0,0, 1,1,0, 1,1,0, TILE_X227 },
		{ 0,0,0, 1,1,0, 1,1,1, TILE_X227 },

		{ 0,0,2, 1,1,0, 1,1,2, TILE_X158 },

		{ 0,1,0, 1,1,1, 2,0,2, TILE_X192 }, 
		{ 0,1,0, 1,1,1, 0,1,0, TILE_X196 }, 
		{ 0,1,0, 1,1,1, 0,1,1, TILE_X205 },
		{ 0,1,0, 1,1,1, 1,1,0, TILE_X206 },
		{ 0,1,0, 1,1,1, 1,1,1, TILE_X209 },
		{ 0,1,0, 1,1,1, 0,1,0, TILE_X210 }, // ? top uncertainty

		{ 0,1,1, 0,1,1, 0,0,1, TILE_X231 },
		{ 0,1,1, 0,1,1, 0,1,1, TILE_X228 },
		{ 0,1,1, 1,1,1, 2,0,2, TILE_X203 }, 
		{ 0,1,1, 1,1,1, 0,1,0, TILE_X207 },
		{ 0,1,1, 1,1,1, 0,1,1, TILE_X211 },
		{ 0,1,1, 1,1,1, 1,1,0, TILE_X224 },
		{ 0,1,1, 1,1,1, 1,1,1, TILE_X234 },

		{ 0,1,2, 1,1,0, 1,1,2, TILE_X198 }, 
		{ 0,1,2, 1,1,0, 2,0,2, TILE_X188 }, 
		{ 0,1,2, 1,1,0, 0,1,2, TILE_X194 }, 

		{ 1,1,0, 1,1,0, 1,0,0, TILE_X233 },
		{ 1,1,0, 1,1,1, 2,0,2, TILE_X204 }, 
		{ 1,1,0, 1,1,1, 0,1,0, TILE_X208 },
		{ 1,1,0, 1,1,1, 1,1,0, TILE_X222 },
		{ 1,1,0, 1,1,1, 0,1,1, TILE_X223 },
		{ 1,1,0, 1,1,1, 1,1,1, TILE_X235 },

		{ 1,1,1, 0,1,1, 0,0,1, TILE_X231 },
		{ 1,1,1, 1,1,1, 0,1,0, TILE_X210 },
		{ 1,1,1, 1,1,0, 1,0,0, TILE_X233 },

		{ 1,1,1, 1,1,1, 1,1,1, TILE_X229 },
		{ 1,1,1, 1,1,1, 2,0,2, TILE_X232 },
		{ 1,1,1, 1,1,1, 0,1,1, TILE_X236 },
		{ 1,1,1, 1,1,1, 1,1,0, TILE_X237 },

		{ 1,1,2, 1,1,0, 1,1,2, TILE_X160 },
		{ 1,1,2, 1,1,0, 2,0,2, TILE_X162 },
		{ 1,1,2, 1,1,0, 0,1,2, TILE_X200 }, 
		{ 1,1,2, 1,1,0, 1,1,2, TILE_X230 },
		{ 1,1,2, 1,1,0, 2,0,2, TILE_X233 },

		{ 2,0,0, 0,1,1, 2,1,1, TILE_X157 },
		{ 2,0,2, 0,1,1, 2,1,0, TILE_X191 }, 

		{ 2,0,2, 0,1,0, 2,0,2, TILE_X181 },
		{ 2,0,2, 0,1,0, 2,1,2, TILE_X185 },

		{ 2,0,2, 0,1,1, 2,0,2, TILE_X182 },
		{ 2,0,2, 0,1,1, 2,1,1, TILE_X225 },

		{ 2,0,2, 1,1,0, 0,1,2, TILE_X190 }, 
		{ 2,0,2, 1,1,0, 2,0,2, TILE_X184 },
		{ 2,0,2, 1,1,0, 1,1,2, TILE_X227 },
		//{ 2,0,2, 1,1,1, 1,1,0, TILE_X166 },

		{ 2,0,2, 1,1,1, 0,1,0, TILE_X193 }, 
		{ 2,0,2, 1,1,1, 0,1,1, TILE_X201 }, 
		{ 2,0,2, 1,1,1, 1,1,0, TILE_X202 }, 
		{ 2,0,2, 1,1,1, 1,1,1, TILE_X226 },
		{ 2,0,2, 1,1,1, 2,0,2, TILE_X163 },
		{ 2,0,2, 1,1,1, 2,0,2, TILE_X183 },

		{ 2,1,0, 0,1,1, 2,0,2, TILE_X189 }, 
		{ 2,1,0, 0,1,1, 2,1,0, TILE_X195 }, 
		{ 2,1,0, 0,1,1, 2,1,1, TILE_X197 }, 

		{ 2,1,1, 0,1,1, 2,1,1, TILE_X159 },
		{ 2,1,1, 0,1,1, 2,0,2, TILE_X161 },
		{ 2,1,1, 0,1,1, 2,1,0, TILE_X199 }, 
		{ 2,1,1, 0,1,1, 2,1,1, TILE_X228 }, // l?
		{ 2,1,1, 0,1,1, 2,0,2, TILE_X231 },

		{ 2,1,2, 1,1,1, 0,1,0, TILE_X210 },
		{ 2,1,2, 1,1,1, 2,1,2, TILE_X169 },
		{ 2,1,2, 1,1,1, 2,0,2, TILE_X172 }, 
		//{ 2,1,2, 0,1,1, 2,1,2, TILE_X175 }, // l
		{ 2,1,2, 1,1,0, 2,1,2, TILE_X178 },
		{ 2,1,2, 0,1,0, 2,1,2, TILE_X186 },
		{ 2,1,2, 0,1,0, 2,0,2, TILE_X187 },

		{ -1,-1,-1,-1,-1,-1,-1,-1,-1,-1, },
	};

	// Per pattern
	for (int j = 0; ; j++)
	{
		if (nPatterns[j][0] == -1)
			return 0;

		bool bMismatch = false;
		
		// Per direction
		for (int i = 0; i < 9; i++)
		{
			// It's me
			if (i == 4)
				continue;

			if (m[i] && nPatterns[j][i] == 0)
			{
				bMismatch = true;
				break;
			}

			if (!m[i] && nPatterns[j][i] == 1)
			{
				bMismatch = true;
				break;
			}
		}

		if (bMismatch == false)
			return nPatterns[j][9];
	}

	return 0;
}

void Stage::SetPageJump(int nPrevPage, int nJumpPage)
{
	if (m_nNextJumpPage != -1)
		return;

	// Jump PrevPage to JumpPage
	int nCurPage = nPrevPage;

	// orginal: 0----1----2----3
	// virtual: 0----1----0----1----2 ...
	//                 ^
	//            current position 
	// curpage=1
	// nextpage=2
	// jumppage=0 (so next page of page1 is page0)

	m_nPrevJumpPage = nCurPage;
	m_nNextJumpPage = nJumpPage;

	int nNextPage = nCurPage + 1;

	// At this time:
	// Move next page's objects to temporary pocket
	// Move jump page's object to next page
#define IS_OBJ_IN_NEXTPAGE(obj) (obj->m_fX > (nNextPage) * GameDefaults::nPageWidth && obj->m_fX < (nNextPage + 2) * GameDefaults::nPageWidth)
#define IS_OBJ_IN_JUMPPAGE(obj) (obj->m_fX > (nJumpPage) * GameDefaults::nPageWidth && obj->m_fX < (nJumpPage + 2) * GameDefaults::nPageWidth)

	// Next<->Jump Page Offset
	int nOffsetX = (nJumpPage - nNextPage) * GameDefaults::nPageWidth;
	int nMoved = 0;

	// Items
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecItem)
		if (IS_OBJ_IN_NEXTPAGE(pObj))
		{
			m_vecJumpedObject.push_back(*it);
			it = vecCurrent.erase(it);
		}
		else if (IS_OBJ_IN_JUMPPAGE(pObj))
		{
			pObj->m_fX -= nOffsetX;
			nMoved++;
			++it;
		}
		else
			++it;
	END_VECTOR_LOOP();

	// Enemy
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEnemy)
		if (IS_OBJ_IN_NEXTPAGE(pObj))
		{
			m_vecJumpedObject.push_back(*it);
			it = vecCurrent.erase(it);
		}
		else if (IS_OBJ_IN_JUMPPAGE(pObj))
		{
			pObj->m_fX -= nOffsetX;
			nMoved++;
			++it;
		}
		else
			++it;
	END_VECTOR_LOOP();

	// Bullet
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecBullet)
		if (IS_OBJ_IN_NEXTPAGE(pObj))
		{
			m_vecJumpedObject.push_back(*it);
			it = vecCurrent.erase(it);
		}
		else if (IS_OBJ_IN_JUMPPAGE(pObj))
		{
			pObj->m_fX -= nOffsetX;
			nMoved++;
			++it;
		}
		else
			++it;
	END_VECTOR_LOOP();

	// EventObject
	BEGIN_VECTOR_LOOP_NOINC(GameObjectBase*, m_vecEventObject)
		if (IS_OBJ_IN_NEXTPAGE(pObj))
		{
			m_vecJumpedObject.push_back(*it);
			it = vecCurrent.erase(it);
		}
		else if (IS_OBJ_IN_JUMPPAGE(pObj))
		{
			pObj->m_fX -= nOffsetX;
			nMoved++;
			++it;
		}
		else
			++it;
	END_VECTOR_LOOP();

	NaDebugOut(L"Set PageJump: %d->%d\n", nCurPage, nJumpPage);
	NaDebugOut(L"- Jumped Objects: %d\n", m_vecJumpedObject.size());
	NaDebugOut(L"- Moved Objects from JumpPage: %d\n", nMoved);
}

void Stage::CalcPageJumpedPos(int &tx, int &ty)
{
	if (m_pGame->m_bIsDesignGame)
		return;

	if (m_nPrevJumpPage == -1 ||
		m_nNextJumpPage == -1)
		return;

	// Return Virtual Tile Coordinates
	int nCheckPointX = ((m_nPrevJumpPage + 1) * GameDefaults::nTilePerScreenWidth);
	if (tx >= nCheckPointX)
	{
		//int nInPageX = tx % GameDefaults::nTilePerScreenWidth;
		//tx = (m_nNextJumpPage * GameDefaults::nTilePerScreenWidth) + nInPageX;

		int nInPageX = tx - nCheckPointX;
		tx = (m_nNextJumpPage * GameDefaults::nTilePerScreenWidth) + nInPageX;
	}
}

// Add Item to Block or EmptyPlace
bool Stage::AddItem(int x, int y, int type)
{
	// #TODO check valid
	int nIdx = GetTileIndex(x, y);
	std::map<int, int>::iterator it = m_mapItem.find(nIdx);
	if (it == m_mapItem.end())
	{
		m_mapItem.insert(
			std::pair<int, int>(nIdx, type)
		);
	}

	return true;
}

// Add MapObject (Pipe, Flagpole, Castle, etc...)
MapObjectBase* Stage::AddMapObject(int x, int y, int type)
{
	//NaDebugOut(L"AddObject: %d, %d / Type: %d\n", x, y, type);
	MapObjectBase *pObj = MapObjectBase::CreateInstance(m_pGame, this, type);
	if (pObj)
	{
		pObj->m_nX = x * TILE_XS;
		pObj->m_nY = y * TILE_YS;

		pObj->m_nTX = x;
		pObj->m_nTY = y;
		pObj->m_nType = type;

		if (MapObjectBase::IsSingletonType(type))
		{
			std::vector<MapObjectBase*>::iterator it = m_vecMapObject.begin();
			for (; it != m_vecMapObject.end(); )
			{
				if ((*it)->m_nType == type)
				{
					NaDebugOut(L"Object already exists: %d\n", type);
					delete (*it);

					it = m_vecMapObject.erase(it);
				}
				else
					++it;
			}
		}
		m_vecMapObject.push_back(pObj);
		return pObj;
	}
	return nullptr;
}

// Add Emeny
GameObjectBase* Stage::AddEnemy(int x, int y, int type)
{
	//NaDebugOut(L"AddEnemy: %d, %d / Type: %d\n", x, y, type);
	GameObjectBase *pNewObj = GameObjectBase::CreateInstance(m_pGame, this, type);
	if (pNewObj)
	{
		pNewObj->x = x;
		pNewObj->y = y;
		pNewObj->m_nType = type;

		m_vecEnemy.push_back(pNewObj);
		return pNewObj;
	}
	return nullptr;
}

Stage * Stage::GetTopStage()
{
	Stage *pTopStage = this;
	while (true)
	{
		if (pTopStage->m_pParentStage)
			pTopStage = pTopStage->m_pParentStage;
		else
			break;
	}

	return pTopStage;
}

void Stage::AddSubStage(Stage * pStage)
{
	m_vecSubStage.push_back(pStage);
}

Stage* Stage::FindSubStage(NaString strName)
{
	BEGIN_VECTOR_LOOP(Stage*, m_vecSubStage) {
		if (pObj->m_strName.Compare(strName) == 0)
			return pObj;
	} END_VECTOR_LOOP();
	
	return nullptr;
}

int Stage::GetSubStageIndex(Stage * pStage)
{
	int nIdx = -1;
	Stage *pTopStage = CUR_STAGE->GetTopStage();

	if (pTopStage == pStage)
		return -1;

	nIdx = 0;
	BEGIN_VECTOR_LOOP(Stage*, pTopStage->m_vecSubStage) {
		if (pStage == pObj)
			return nIdx;
		
		++nIdx;
	} END_VECTOR_LOOP();

	return -1;
}

void Stage::ClearSubStages()
{
	SAFE_DELETE_OBJPTR_VECTOR(Stage*, m_vecSubStage);
}

void* Stage::CreateInstance()
{
	return (new Stage(m_pGame));
}

int Stage::GetTileIndex(int x, int y)
{
	CalcPageJumpedPos(x, y);

	int nTileW = GameDefaults::nPageTileWidth * m_nMaxPage;
	int nIdx = y * nTileW + x;

	return nIdx;
}

POINT Stage::GetTilePosition(int idx)
{
	int nTileW = GameDefaults::nPageTileWidth * m_nMaxPage;
	POINT pt;
	pt.x = idx % nTileW;
	pt.y = idx / nTileW;
	return pt;
}

void Stage::SetTileData(int x, int y, int type)
{
	if (x < 0 || y < 0 || x >= m_sizeTile.cx || y >= m_sizeTile.cy)
		return;

	int nIdx = GetTileIndex(x, y);
	
	m_pTileData[nIdx] = type;
}

void Stage::DestroyTile(int x, int y)
{
#define REPLACE_EACHOTHER(a, b) \
	case a: nReplaceData = b; break; \
	case b: nReplaceData = a; break;

	int nReplaceData = TILE_EMPTY;
	int nDataUpside = GetTileData(x, y - 1);
	switch (nDataUpside)
	{
		REPLACE_EACHOTHER(TILE_ISLANDBODY_L, TILE_SEMI_SOLID1_LBT);
		REPLACE_EACHOTHER(TILE_ISLANDBODY_C, TILE_SEMI_SOLID1_CBT);
		REPLACE_EACHOTHER(TILE_ISLANDBODY_R, TILE_SEMI_SOLID1_RBT);
	
		REPLACE_EACHOTHER(TILE_SEMI_SOLID2_LTB, TILE_SEMI_SOLID2_LBT);
		REPLACE_EACHOTHER(TILE_SEMI_SOLID2_CTB, TILE_SEMI_SOLID2_CBT);
		REPLACE_EACHOTHER(TILE_SEMI_SOLID2_RTB, TILE_SEMI_SOLID2_RBT);

		REPLACE_EACHOTHER(TILE_SEMI_SOLID3_LTB, TILE_SEMI_SOLID3_LBT);
		REPLACE_EACHOTHER(TILE_SEMI_SOLID3_CTB, TILE_SEMI_SOLID3_CBT);
		REPLACE_EACHOTHER(TILE_SEMI_SOLID3_RTB, TILE_SEMI_SOLID3_RBT);
	}

	SetTileData(x, y, nReplaceData);
}

bool Stage::IsHardTile(int x, int y, int dir)
{
	int nType = GetTileData(x, y);
	switch (nType)
	{
	case TILE_EMPTY:
	case TILE_ISLANDBODY_L:
	case TILE_ISLANDBODY_C:
	case TILE_ISLANDBODY_R:
	case TILE_BRIDGE_T:
	case TILE_CASTLE_HEAD1:
	case TILE_CASTLE_HEAD2:
	case TILE_CASTLE_WINDOW1:
	case TILE_CASTLE_BRICK:
	case TILE_CASTLE_WINDOW2:
	case TILE_CASTLE_DOOR_T:
	case TILE_CASTLE_DOOR_B:
	case TILE_RIVER_T:
	case TILE_RIVER_B:
	case TILE_HIDDENITEM:
	case TILE_VERTICAL_LINE:
		return false;
		break;
	case TILE_GROUND:
	case TILE_BRICK:
	case TILE_ITEM:
	case TILE_TAKENITEM:
	case TILE_PIPEHEAD_L:
	case TILE_PIPEHEAD_R:
	case TILE_PIPEBODY_L:
	case TILE_PIPEBODY_R:
	case TILE_HARDBRICK:
	case TILE_CANNONHEAD_T:
	case TILE_CANNONHEAD_B:
	case TILE_CANNONBODY:
	case TILE_HPIPEHEAD_T:
	case TILE_HPIPEHEAD_B:
	case TILE_HPIPEBODY_T:
	case TILE_HPIPEBODY_B:
	case TILE_PIPEJUNCTION_T:
	case TILE_PIPEJUNCTION_B:
	case TILE_ITEM2:
	case TILE_ITEM3:
	case TILE_TAKINGITEM:
	case TILE_SPIKE:
	case TILE_JUMP:
	case TILE_PIPEBOTTOM_L:
	case TILE_PIPEBOTTOM_R:
	case TILE_HPIPERIGHT_T:
	case TILE_HPIPERIGHT_B:
	case TILE_MESSAGE:
		return true;
		break;
	case TILE_CLOUD:
	case TILE_BRIDGE_B:
	case TILE_MUSHROOMHEAD_L:
	case TILE_MUSHROOMHEAD_C:
	case TILE_MUSHROOMHEAD_R:
	case TILE_MUSHROOMHEAD2_L:
	case TILE_MUSHROOMHEAD2_C:
	case TILE_MUSHROOMHEAD2_R:
	case TILE_MUSHROOMHEAD3_L:
	case TILE_MUSHROOMHEAD3_C:
	case TILE_MUSHROOMHEAD3_R:
	case TILE_ISLANDHEAD_L:
	case TILE_ISLANDHEAD_C:
	case TILE_ISLANDHEAD_R:
		{
			if (IS_CLASSIC)
				return true;
			else
			{
				if (dir == COLLISION_BOTTOM)
					return true;
				else
					return false;
			}
		}
		break;
	case TILE_SEMI_SOLID_LT:
	case TILE_SEMI_SOLID_CT:
	case TILE_SEMI_SOLID_RT:
	case TILE_SEMI_SOLID3_LT:
	case TILE_SEMI_SOLID3_CT:
	case TILE_SEMI_SOLID3_RT:
		{
			if (dir == COLLISION_BOTTOM)
				return true;
			else
				return false;
		}
		break;
	}

	return false;
}

bool Stage::IsDamageTile(int x, int y, int dir)
{
	int nType = GetTileData(x, y);
	switch (nType)
	{
	case TILE_SPIKE:
		return true;
		break;
	default:
		return false;
		break;
	}

	return false;
}

bool Stage::IsJumpTile(int x, int y, int dir)
{
	int nType = GetTileData(x, y);
	switch (nType)
	{
	case TILE_JUMP:
		return true;
		break;
	default:
		return false;
		break;
	}

	return false;
}

bool Stage::IsBackgroundTile(int type)
{
	switch (type)
	{
	case TILE_MUSHROOMBODY_T:
	case TILE_MUSHROOMBODY_B:
	case TILE_ISLANDBODY_L:
	case TILE_ISLANDBODY_C:
	case TILE_ISLANDBODY_R:
	case TILE_SEMI_SOLID2_LTB:
	case TILE_SEMI_SOLID2_CTB:
	case TILE_SEMI_SOLID2_RTB:
	case TILE_SEMI_SOLID3_LTB:
	case TILE_SEMI_SOLID3_CTB:
	case TILE_SEMI_SOLID3_RTB:

	case TILE_SEMI_SOLID1_LBT:
	case TILE_SEMI_SOLID1_CBT:
	case TILE_SEMI_SOLID1_RBT:
	case TILE_SEMI_SOLID2_LBT:
	case TILE_SEMI_SOLID2_CBT:
	case TILE_SEMI_SOLID2_RBT:
	case TILE_SEMI_SOLID3_LBT:
	case TILE_SEMI_SOLID3_CBT:
	case TILE_SEMI_SOLID3_RBT:

	case TILE_SEMI_SOLID1_LBB:
	case TILE_SEMI_SOLID1_CBB:
	case TILE_SEMI_SOLID1_RBB:
	case TILE_SEMI_SOLID2_LBB:
	case TILE_SEMI_SOLID2_CBB:
	case TILE_SEMI_SOLID2_RBB:
	case TILE_SEMI_SOLID3_LBB:
	case TILE_SEMI_SOLID3_CBB:
	case TILE_SEMI_SOLID3_RBB:
		return true;
		break;
	}

	return false;
}

int Stage::GetDataType(int type)
{
	switch (type)
	{
	case TILE_EMPTY:
	case TILE_ISLANDBODY_L:
	case TILE_ISLANDBODY_C:
	case TILE_ISLANDBODY_R:
	case TILE_MUSHROOMBODY_T:
	case TILE_MUSHROOMBODY_B:
	case TILE_BRIDGE_T:
	case TILE_CASTLE_HEAD1:
	case TILE_CASTLE_HEAD2:
	case TILE_CASTLE_WINDOW1:
	case TILE_CASTLE_BRICK:
	case TILE_CASTLE_WINDOW2:
	case TILE_CASTLE_DOOR_T:
	case TILE_CASTLE_DOOR_B:
	case TILE_RIVER_T:
	case TILE_RIVER_B:
	case TILE_VERTICAL_LINE:
	case TILE_HIDDENITEM:
		return TILETYPE_EMPTY;
		break;
	case TILE_BRICK:
		return TILETYPE_SOFTHARD;
		break;
	case TILE_TAKENITEM:
	case TILE_HARDBRICK:
	case TILE_TAKINGITEM:
		return TILETYPE_HARD;
		break;
	case TILE_GROUND:
	case TILE_PIPEHEAD_L:
	case TILE_PIPEHEAD_R:
	case TILE_PIPEBODY_L:
	case TILE_PIPEBODY_R:
	case TILE_ISLANDHEAD_L:
	case TILE_ISLANDHEAD_C:
	case TILE_ISLANDHEAD_R:
	case TILE_MUSHROOMHEAD_L:
	case TILE_MUSHROOMHEAD_C:
	case TILE_MUSHROOMHEAD_R:
	case TILE_CLOUD:
	case TILE_BRIDGE_B:
	case TILE_CANNONHEAD_T:
	case TILE_CANNONHEAD_B:
	case TILE_CANNONBODY:
	case TILE_HPIPEHEAD_T:
	case TILE_HPIPEHEAD_B:
	case TILE_HPIPEBODY_T:
	case TILE_HPIPEBODY_B:
	case TILE_PIPEJUNCTION_T:
	case TILE_PIPEJUNCTION_B:	
	case TILE_MESSAGE:
		return TILETYPE_SUPERHARD;
		break;
	case TILE_ITEM:
	case TILE_ITEM2:
	case TILE_ITEM3:
		return TILETYPE_ITEM;
		break;
	case TILE_SEMI_SOLID_LT:
	case TILE_SEMI_SOLID_CT:
	case TILE_SEMI_SOLID_RT:
		// ?
		break;
	case TILE_SPIKE:
		return TILETYPE_DAMAGE;
		break;
	case TILE_JUMP:
		return TILETYPE_JUMP;
		break;
	}

	if (IsBackgroundTile(type))
		return TILETYPE_EMPTY;

	return TILETYPE_UNKNOWN;
}
