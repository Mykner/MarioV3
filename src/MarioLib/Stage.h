#pragma once

#include "Common.h"

#include <map>
#include <vector>
#include <algorithm>

#include "json.h"
#include "PropertyMap.h"
#include "ThemeSetting.h"

class Game;
class Sprite;
class GameObjectBase;
class MapObjectBase;
class CreateParameter;
class PipeInfo;
class PipeExitInfo;
class BackgroundObjectBase;
class Stage : public PropertyObjectBase
{
public:
	Stage(Game *pGame);
	virtual ~Stage();

	void Clear();

    int CreateTileData();
    void DestroyTileData();

	// Load/Save Map File
	virtual bool Load(const wchar_t* name);
	virtual bool LoadFile(const char* lpszPathName, bool bDecryptIfEncrypted = true);
	void LoadJsonMap(Json::Value &j);
	void LoadBinaryMap(FILE *fp);
	void OnLoad();
	void SaveJsonMap(Json::Value &j);

	// Load/Save Map Helper
	static NaString ReadString(FILE *fp);
	void MigrateTileData(BYTE &nTileData, double dVersionFrom);

	// Preset Stage
	void CreatePresetStage(NaString strName);
	void CreateDefaultGround();

	virtual void OnMaxPageChanged();
	virtual void OnMaxVertPageChanged();
	void InitEnemyStack();
	void MakeEarthquake(int nPower = POWER_EARTHQUAKE_NORMAL);

	virtual void RenderBackground();
	virtual void Render();
	virtual void OnBeforeProcess();
	virtual void Process();
	virtual void ProcessBase();
	virtual void OnAfterProcess();
	void ProcessEffects();
	virtual void RenderTile(int x, int y, int nType, long lColor = 0xffffffff, int nZOrder = Z_MAP);
	void RenderFarBackground();
	void RenderItems();
	void RenderEnemies();
	void RenderEffects();
	void RenderBullets();
	void RenderEventObjects();
	void RenderVehicles();

	long GetBackgroundColor();
	float GetGravity() { return m_fGravity; }

    void CreateCrushBlockEffect(int type, int x, int y, GameObjectBase *pWho = nullptr, bool bHit = false);

	void HitBlock(int x, int y, int nPower, GameObjectBase *pWho = nullptr, GameObjectBase *pOwner = nullptr);
	void CrushBlock(int x, int y, GameObjectBase *pWho = nullptr);
	void PressBlock(int x, int y, int nPower, GameObjectBase *pWho = nullptr);
	void PopItemUp(int x, int y, GameObjectBase *pWho = nullptr, int nDir = -1);
	bool PressJumpBlock(int x, int y, GameObjectBase *pWho = nullptr);

	bool CanCrush(int x, int y, int nPower);
	bool CanPress(int x, int y);
	bool CanCrushPipe(int x, int y);

	GameObjectBase* CreateItem(int x, int y, int type);
	GameObjectBase* CreateEffect(int x, int y, int type, CreateParameter *pParam = nullptr);
	GameObjectBase* CreateBullet(int x, int y, int type, CreateParameter *pParam = nullptr);
	GameObjectBase* CreateEventObject(int x, int y, int type, CreateParameter *pParam = nullptr);
	GameObjectBase* CreateVehicle(int x, int y, int type, CreateParameter *pParam = nullptr);

	GameObjectBase* ReserveCreateGameObject(int x, int y, int type, CreateParameter *pParam = nullptr);

	int GetObjectCount(int nType, bool bInCameraOnly);

	void TileizeMapObjects();

	void InitTheme();
	void InitItems();
	void InitObjectIntoPipe();
	void BuildBackground();

	void PlayBGM();
	void PlaySound(const wchar_t *name);

	void SwapCoinAndBrick();

	bool CanEnterSky();
	bool CanSwim(NaRect &rc);
    bool CanHurt(NaRect &rc);
    void AddSwimArea(NaRect &rc);
    void AddHurtArea(NaRect &rc);

	void BeginStopWatch();
	void EndStopWatch();

	// Hit/Press Animation Counter
	std::map<int, int> m_mapHitFrame;
	std::map<int, int> m_mapPressFrame;

	// Live Objects
	std::vector<GameObjectBase*> m_vecEnemy;
	std::vector<GameObjectBase*> m_vecItem;
	std::vector<GameObjectBase*> m_vecEffect;
	std::vector<GameObjectBase*> m_vecBullet;
	std::vector<GameObjectBase*> m_vecEventObject;
	std::vector<GameObjectBase*> m_vecVehicle;

	// Live Object Queue
	std::vector<GameObjectBase*> m_vecReservedGameObject;

	// Tile Handling
	int GetTileIndex(int x, int y);
    int GetTileDataIndex(int x, int y);
	POINT GetTilePosition(int idx);
	void SetTileData(int x, int y, int type);
	void DestroyTile(int x, int y);
	int GetTileData(int x, int y);
	int GetDataType(int type);
	void SetItem(int x, int y, int type);
	int GetItem(int x, int y);
	NaRect GetTileRect(int x, int y, bool bAddAnimationOffset = true);
	bool IsHardTile(int x, int y, int dir = -1);
	bool IsDamageTile(int x, int y, int dir = -1);
	bool IsJumpTile(int x, int y, int dir = -1);
	bool IsBackgroundTile(int type);
    bool IsGroundTile(int type);
    bool IsSomethingTile(int type);
	bool HasItem(int x, int y);

	POINT GetHitAnimatedTileOffset(int x, int y, int* hitOffset = nullptr, int* pressOffset = nullptr);
	int GetJumpTileFrame(int x, int y);
	int GetAnimatedTileIndex(int nType);
	int GetAnimatedBackgroundTileIndex(int nType);
	int GetAutoGroundTileIndex(int x, int y);
    int GetGroundTileDecoration(int x, int y, int nType);
    void SetAutoGroundTiles();
    void UpdateAutoGroundTileRegion(int x, int y);

	void SetPageJump(int nPrevPage, int nJumpPage);
	void CalcPageJumpedPos(int &tx, int &ty);

	// For Load & Design
	bool AddItem(int x, int y, int type);
	MapObjectBase* AddMapObject(int x, int y, int type);
    BackgroundObjectBase* AddBackgroundObject(int x, int y, int type);
	GameObjectBase* AddEnemy(int x, int y, int type);

	// SubStage
	Stage* GetTopStage();
	void AddSubStage(Stage* pStage);
	Stage* FindSubStage(NaString strName);
	int GetSubStageIndex(Stage *pStage);
	void ClearSubStages();

	virtual void* CreateInstance();

	// Map Data
	int m_nMaxPage;
	int m_nMaxVertPage;
	BYTE* m_pTileData;

	POINT m_ptStartPoint;
	POINT m_ptStopPoint;
	POINT m_ptEnemyStop;
	POINT m_ptSkyExit;
	NaString m_strLastCheckPoint;
	POINT m_ptLastCheckPoint;
	int m_nAutoScroll;

	NaString m_strGameTheme;
	int m_nThemeSet;
	int m_nTheme;
	int m_nBackgroundTheme;
	NaString m_strCustomThemeName;
	ThemeSetting m_ThemeSetting;
	
	float m_fGravity;
	long m_lBackgroundColor;
	
	bool m_bCanGoBack;
	int m_nEntranceType;
	NaString m_strNextStageName;
	NaString m_strSkyStageName;

	int m_nAbilityFlag;
	int m_nInitialShape;
	int m_nStateFrame;

	// New Advanced Properties
	int m_nTime;
	NaString m_strCreator;
	int m_nMaxDoorKey;
	bool m_bSwapCoinBlock;
	int m_nReqSwapCoinBlock;

	NaString m_strUniqueId;
	
	std::map<int, int> m_mapItem; // Item that hide in Block (not live object)
	std::map<int, NaString> m_mapMessage;
	std::vector<MapObjectBase*> m_vecMapObject;
    std::vector<BackgroundObjectBase*> m_vecBackgroundObject;
	std::vector<PipeInfo*> m_vecPipeInfo;
	std::vector<PipeExitInfo*> m_vecPipeExitInfo;

	PipeInfo* m_pCurPipe;
	std::vector<NaRect> m_vecSwimArea;
    std::vector<NaRect> m_vecHurtArea;
	SIZE m_sizeTile;
	bool m_bIsDesignStage;

	int m_nPrevJumpPage;
	int m_nNextJumpPage;
	std::vector<GameObjectBase*> m_vecJumpedObject;

	bool m_bIsSubStage;
	std::vector<Stage*> m_vecSubStage;

	DWORD m_dwBeginTime;
	DWORD m_dwPlayTime;
	int m_nTryCount;

	// ...
	NaString m_strName;
	NaString m_strTitle;
	int m_nRevision;

	Game *m_pGame;
	Stage *m_pParentStage;
	Sprite *m_pSprite;
	Sprite *m_pSpritePiece;
	Sprite *m_pSpriteBackground;
	Sprite *m_pSpriteFarBackground;
	Sprite *m_pSpriteItem;
	Sprite *m_pSpriteEnemy;
	Sprite *m_pSpriteEffect;
	Sprite *m_pSpriteMapObject;
	Sprite *m_pSpriteVehicle;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("MaxPage", &m_nMaxPage)
		DECL_PROP("MaxVerticalPage", &m_nMaxVertPage)
		DECL_PROP("Theme", &m_nTheme)
		DECL_PROP("BackgroundTheme", &m_nBackgroundTheme)
		DECL_PROP("Gravity", &m_fGravity)
		DECL_PROP("BackgroundColor", &m_lBackgroundColor)
		DECL_PROP("CanGoBack", &m_bCanGoBack)
		DECL_PROP("EntranceType", &m_nEntranceType)
		DECL_PROP("NextStageName", &m_strNextStageName)
		DECL_PROP("SkyStageName", &m_strSkyStageName)
		DECL_PROP("Time", &m_nTime)
		DECL_PROP("Creator", &m_strCreator)
		DECL_PROP("ThemeSet", &m_nThemeSet)
		DECL_PROP("AutoScroll", &m_nAutoScroll)
		DECL_PROP("AbilityFlag", &m_nAbilityFlag)
#if !defined(NDEBUG)
		DECL_PROP("InitialShape", &m_nInitialShape)
#endif
		DECL_PROP("CustomThemeName", &m_strCustomThemeName)
		DECL_PROP("GameTheme", &m_strGameTheme)
		DECL_PROP("UniqueId", &m_strUniqueId)
		DECL_PROP("Revision", &m_nRevision)
		DECL_PROP("Title", &m_strTitle)
	END_DECLARE_PROPERTYMAP()
};
