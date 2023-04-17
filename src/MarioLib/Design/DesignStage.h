#pragma once

#include "Common.h"

#include <map>
#include <vector>

#include "Stage.h"

class Game;
class Sprite;
class GameObjectBase;

enum ToolTypes
{
	TOOLTYPE_PICK = 0,
	TOOLTYPE_TILE = 1,
	TOOLTYPE_MAPOBJECT,
	TOOLTYPE_ENEMY,
	TOOLTYPE_ITEM,
	TOOLTYPE_EVENTOBJECT,
	TOOLTYPE_ERASER,
	TOOLTYPE_PLAYBOT,
	TOOLTYPE_VEHICLE,
};

enum EditModes
{
	EDIT_SELECT = 0,
	EDIT_SELECTED,
	EDIT_DRAGMOVE,
	EDIT_DRAW,
};

class DesignStage : public Stage
{
public:
	DesignStage(Game *pGame);
	virtual ~DesignStage();

	virtual bool LoadFile(const char* lpszPathName, bool bDecryptIfEncrypted = true);

	virtual void Render();
	virtual void Process();
	virtual void RenderTile(int x, int y, int nType, long lColor = 0xffffffff, int nZOrder = Z_MAP);

	void RenderBrush();
	void RenderBrushTile();
	void RenderBrushItem();

	// Render Current Objects
	void RenderItem(int x, int y, int nType, int nColor = 0xffffffff);
	void RenderPage();
	void RenderSelectionIndicator();

	// Tool
	void FillTileData(int x1, int y1, int x2, int y2, int type);
	void FillItemData(int x1, int y1, int x2, int y2, int type);
	void* HitTest(int x, int y, int nToolType = -1);
	bool Pick(int x, int y);
	void ChangeTool(int nToolType, int nType);

	bool Select(int x, int y);
	void MoveSelected(int x, int y);
	void RemoveSelected();

	GameObjectBase *GetSelectedObject();
	
	void SetSelectedCostume(int nType);

	bool Draw();
	void DragMove(int x, int y);
	void OnDragMove();

	POINT GetDefaultHandOffset(int nType);
	MapObjectBase* GetStartPointInViewport();
	void MovePlayBot(float x, float y);
	void MovePlayBotToPipeExit(MapObjectBase *pPipeObj);
	void MovePlayBotToSafePlace();

	MapObjectBase* FindPipeExit(int nIndex);

	void ClearGroundBlocks();
	void CheckValid();
	bool QuickPowerUp();
	bool QuickToggleWing();

	Stage* CreateSubStage();
	int GetSubStageCount();
	int MakeNextPipeExitIndex();
	virtual void* CreateInstance();

	virtual void OnMaxVertPageChanged();

	void SetEditMode(int nMode);

	POINT m_ptCursor;
	int m_nCurToolType;
	int m_nCurType;
	int m_nCurMode;
	bool m_bIsCurToolNotPlaced;
	POINT m_ptDragStart;
	POINT m_ptDragEnd;
	POINT m_ptTileSelected;
	bool m_bDrag;
	POINT m_ptHandOffset;
	std::vector<NaPointT<float>> m_vecPlayedShadow;
	bool m_bShowPlayedShadow;

	NaString m_strTransaction;

	GameObjectBase *m_pPlayBotObject;
	MapObjectBase *m_pMapObject;
	GameObjectBase *m_pEnemyObject;
	GameObjectBase *m_pEventObject;
	GameObjectBase *m_pVehicleObject;
	Sprite *m_pSpriteDesign;
};
