
// MarioMapEditorView.cpp : CMarioMapEditorView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MarioMapEditor.h"
#endif

#include "MarioMapEditorDoc.h"
#include "MarioMapEditorView.h"

#include "Types.h"

#include "Camera.h"
#include "Stage.h"

#include "DesignStage.h"
#include "MarioMapEditor.h"
#include "MapObjectBase.h"

#include "MainFrm.h"

#include "DesignPlayGame.h"
#include "Camera.h"
#include "Player.h"
#include "InputManager.h"
#include "PipeInfo.h"
#include "Themes.h"
#include "GameConfig.h"

#include "MapObjectPipe.h"
#include "MapObjectPipeHorz.h"
#include "Bridge.h"
#include "FireBar.h"
#include "TrampolinMini.h"
#include "Yoshi.h"
#include "Princess.h"
#include "OneWayWall.h"
#include "MapObjectMushroomIsland.h"
#include "MapObjectSemiSolid.h"
#include "GoombaShoe.h"
#include "Door.h"
#include "Cannon.h"

#include "RenderManager.h"
#include "FModManager.h"
#include "SoundManager.h"

#include "TypeNames.h"

#include "CostumeDlg.h"
#include "SubStageDlg.h"
#include "SelectAbilityDlg.h"

#include "ServerRequest.h"
#include "NaException.h"
#include "CreatorCommentDlg.h"
#include "QuestionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ToolType & Type Map
int g_ControlToolTypeMap[][3] =
{
	{ ID_BTN_BLOCK_PICK,					TOOLTYPE_PICK,			-1 },

	{ ID_BTN_BLOCK_ERASER,					TOOLTYPE_ERASER,		-1 },

	{ ID_BTN_BLOCK_BRICK,					TOOLTYPE_TILE, TILE_BRICK },
	{ ID_BTN_BLOCK_ITEM,					TOOLTYPE_TILE, TILE_ITEM },
	{ ID_BTN_BLOCK_HARDBRICK,				TOOLTYPE_TILE, TILE_HARDBRICK },
	{ ID_BTN_BLOCK_GROUND,					TOOLTYPE_TILE, TILE_GROUND },
	{ ID_BTN_BLOCK_TAKENITEM,				TOOLTYPE_TILE, TILE_TAKENITEM },
	{ ID_BTN_BLOCK_HIDDENITEM,				TOOLTYPE_TILE, TILE_HIDDENITEM },
	{ ID_BTN_BLOCK_CLOUD,					TOOLTYPE_TILE, TILE_CLOUD },
	{ ID_BTN_BLOCK_VERTICAL_LINE,			TOOLTYPE_TILE, TILE_VERTICAL_LINE },
	{ ID_BTN_BLOCK_SPIKE,					TOOLTYPE_TILE, TILE_SPIKE },
	{ ID_BTN_BLOCK_JUMP,					TOOLTYPE_TILE, TILE_JUMP },
	{ ID_BTN_BLOCK_POW,						TOOLTYPE_TILE, TILE_POW },

	{ ID_BTN_OBJ_GOOMBA,					TOOLTYPE_ENEMY, ENEMY_GOOMBA },
	{ ID_BTN_OBJ_TURTLE,					TOOLTYPE_ENEMY, ENEMY_TURTLE },
	{ ID_BTN_OBJ_KILLERFLOWER,				TOOLTYPE_ENEMY, ENEMY_PIRANHA_PLANT },
	{ ID_BTN_OBJ_REDTURTLE,					TOOLTYPE_ENEMY, ENEMY_REDTURTLE },
	{ ID_BTN_OBJ_REDTURTLE_WINGED_PATROL,	TOOLTYPE_ENEMY, ENEMY_REDTURTLE_WINGED_PATROL },
	{ ID_BTN_OBJ_FIREBAR,					TOOLTYPE_ENEMY, ENEMY_FIREBAR },
	{ ID_BTN_OBJ_PODOBOO,					TOOLTYPE_ENEMY, ENEMY_PODOBOO },
	{ ID_BTN_OBJ_BOWSER,					TOOLTYPE_ENEMY, ENEMY_BOWSER },
	{ ID_BTN_OBJ_TURTLE_WINGED,				TOOLTYPE_ENEMY, ENEMY_TURTLE_WINGED },
	{ ID_BTN_OBJ_BLOOBER,					TOOLTYPE_ENEMY, ENEMY_BLOOBER },
	{ ID_BTN_OBJ_CHEEPCHEEP,				TOOLTYPE_ENEMY, ENEMY_CHEEPCHEEP },
	{ ID_BTN_OBJ_REDCHEEPCHEEP,				TOOLTYPE_ENEMY, ENEMY_REDCHEEPCHEEP },
	{ ID_BTN_OBJ_CHEEPCHEEP_FLY,			TOOLTYPE_ENEMY, ENEMY_CHEEPCHEEP_FLY },
	{ ID_BTN_OBJ_REDCHEEPCHEEP_FLY,			TOOLTYPE_ENEMY, ENEMY_REDCHEEPCHEEP_FLY },
	{ ID_BTN_OBJ_HAMMERBROTHERS,			TOOLTYPE_ENEMY, ENEMY_HAMMERBROTHERS },
	{ ID_BTN_OBJ_LAKITU,					TOOLTYPE_ENEMY, ENEMY_LAKITU },
	{ ID_BTN_OBJ_SPINY,						TOOLTYPE_ENEMY, ENEMY_SPINY },
	{ ID_BTN_OBJ_BUZZYBEETLE,				TOOLTYPE_ENEMY, ENEMY_BUZZYBEETLE },
 	{ ID_BTN_OBJ_MUNCHER,					TOOLTYPE_ENEMY, ENEMY_MUNCHER },
 	{ ID_BTN_OBJ_CHAINCHOMP,				TOOLTYPE_ENEMY, ENEMY_CHAINCHOMP },
 	{ ID_BTN_OBJ_BOBOMB,					TOOLTYPE_ENEMY, ENEMY_BOBOMB },
 	{ ID_BTN_OBJ_SLEDGEBROTHERS,			TOOLTYPE_ENEMY, ENEMY_SLEDGEBROTHERS },
 	{ ID_BTN_OBJ_THWOMP,					TOOLTYPE_ENEMY, ENEMY_THWOMP },
 	{ ID_BTN_OBJ_DRYBONE,					TOOLTYPE_ENEMY, ENEMY_DRYBONE },
 	{ ID_BTN_OBJ_FIREPIRANHAPLANT,			TOOLTYPE_ENEMY, ENEMY_FIREPIRANHAPLANT },
 	{ ID_BTN_OBJ_GRINDER,					TOOLTYPE_ENEMY, ENEMY_GRINDER },
	{ ID_BTN_OBJ_BOOMERANGBROTHERS,			TOOLTYPE_ENEMY, ENEMY_BOOMERANGBROTHERS },
	{ ID_BTN_OBJ_FIREBROTHERS,				TOOLTYPE_ENEMY, ENEMY_FIREBROTHERS },
	{ ID_BTN_OBJ_BOWSERJUNIOR,				TOOLTYPE_ENEMY, ENEMY_BOWSERJUNIOR },
	{ ID_BTN_OBJ_BOO,						TOOLTYPE_ENEMY, ENEMY_BOO },
	{ ID_BTN_OBJ_ROCKYWRENCH,				TOOLTYPE_ENEMY, ENEMY_ROCKYWRENCH },
	{ ID_BTN_OBJ_BOOMBOOM,					TOOLTYPE_ENEMY, ENEMY_BOOMBOOM },

	{ ID_BTN_OBJ_COIN,						TOOLTYPE_ITEM, ITEM_COIN },
	{ ID_BTN_OBJ_MUSHROOM,					TOOLTYPE_ITEM, ITEM_MUSHROOM },
	{ ID_BTN_OBJ_FIREFLOWER,				TOOLTYPE_ITEM, ITEM_FIREFLOWER },
	{ ID_BTN_OBJ_STAR,						TOOLTYPE_ITEM, ITEM_SUPERSTAR },
	{ ID_BTN_OBJ_1UP,						TOOLTYPE_ITEM, ITEM_1UP },
	{ ID_BTN_OBJ_VINE,						TOOLTYPE_ITEM, ITEM_VINE },
	{ ID_BTN_OBJ_GIANTMUSHROOM,				TOOLTYPE_ITEM, ITEM_GIANTMUSHROOM },
	{ ID_BTN_OBJ_YOSHIEGG,					TOOLTYPE_ITEM, ITEM_YOSHIEGG },
	{ ID_BTN_OBJ_CLOUDITEM,					TOOLTYPE_ITEM, ITEM_CLOUD },
	{ ID_BTN_OBJ_DOORKEYCOIN,				TOOLTYPE_ITEM, ITEM_DOORKEYCOIN },
	{ ID_BTN_OBJ_DOORKEY,					TOOLTYPE_ITEM, ITEM_DOORKEY },
	{ ID_BTN_OBJ_TURTLESHELL,				TOOLTYPE_ITEM, ITEM_TURTLESHELL },
	{ ID_BTN_OBJ_MYSTERYMUSHROOM,			TOOLTYPE_ITEM, ITEM_MYSTERYMUSHROOM },

	{ ID_BTN_OBJ_PIPE,						TOOLTYPE_MAPOBJECT, MAPOBJECT_PIPE },
	{ ID_BTN_OBJ_PIPEHORZ,					TOOLTYPE_MAPOBJECT, MAPOBJECT_PIPEHORZ },
	{ ID_BTN_OBJ_PIPEBODY,					TOOLTYPE_MAPOBJECT, MAPOBJECT_PIPEBODY },
	{ ID_BTN_OBJ_ISLAND,					TOOLTYPE_MAPOBJECT, MAPOBJECT_ISLAND },
	{ ID_BTN_OBJ_STARTPOINT,				TOOLTYPE_MAPOBJECT, MAPOBJECT_STARTPOINT },
	{ ID_BTN_OBJ_PIPEEXIT,					TOOLTYPE_MAPOBJECT, MAPOBJECT_PIPEEXIT },
	{ ID_BTN_OBJ_CHECKPOINT,				TOOLTYPE_MAPOBJECT, MAPOBJECT_CHECKPOINT },
	{ ID_BTN_OBJ_CAMERASTOP,				TOOLTYPE_MAPOBJECT, MAPOBJECT_CAMERASTOP },
	{ ID_BTN_OBJ_RIVER,						TOOLTYPE_MAPOBJECT, MAPOBJECT_RIVER },
	{ ID_BTN_OBJ_SKYEXIT,					TOOLTYPE_MAPOBJECT, MAPOBJECT_SKYEXIT },
	{ ID_BTN_OBJ_RIVERBRIDGE,				TOOLTYPE_MAPOBJECT, MAPOBJECT_RIVERBRIDGE },
	{ ID_BTN_OBJ_ENEMYSTOP,					TOOLTYPE_MAPOBJECT, MAPOBJECT_ENEMYSTOP },
	{ ID_BTN_OBJ_BILLBLASTER,				TOOLTYPE_MAPOBJECT, MAPOBJECT_BILLBLASTER },
	{ ID_BTN_OBJ_MUSHROOMISLAND,			TOOLTYPE_MAPOBJECT, MAPOBJECT_MUSHROOMISLAND },
	{ ID_BTN_OBJ_WALL,						TOOLTYPE_MAPOBJECT, MAPOBJECT_WALL },
	{ ID_BTN_OBJ_SEMI_SOLID,				TOOLTYPE_MAPOBJECT, MAPOBJECT_SEMISOLID },
	{ ID_BTN_OBJ_MESSAGEBLOCK,				TOOLTYPE_MAPOBJECT, MAPOBJECT_MESSAGEBLOCK },

	{ ID_BTN_OBJ_FLAGPOLE,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_FLAGPOLE },
	{ ID_BTN_OBJ_GOALPOINT,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_GOALPOINT },
	{ ID_BTN_OBJ_BRIDGE,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_BRIDGE },
	{ ID_BTN_OBJ_BOWSERFIRE,				TOOLTYPE_EVENTOBJECT, EVENTOBJECT_BOWSERFIRESHOOTER },
	{ ID_BTN_OBJ_BOSSBRIDGE,				TOOLTYPE_EVENTOBJECT, EVENTOBJECT_BOSSBRIDGE },
	{ ID_BTN_OBJ_AXE,						TOOLTYPE_EVENTOBJECT, EVENTOBJECT_BOSSBRIDGE_AXE },
	{ ID_BTN_OBJ_PRINCESS,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_PRINCESS },
	{ ID_BTN_OBJ_TRAMPOLIN,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_TRAMPOLIN },
	{ ID_BTN_OBJ_WARPZONETEXT,				TOOLTYPE_EVENTOBJECT, EVENTOBJECT_WARPZONETEXT },
	{ ID_BTN_OBJ_SKYBRIDGE,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_SKYBRIDGE },
	{ ID_BTN_OBJ_ENEMYGENERATOR,			TOOLTYPE_EVENTOBJECT, EVENTOBJECT_ENEMYGENERATOR },
	{ ID_BTN_OBJ_LOOPHANDLER,				TOOLTYPE_EVENTOBJECT, EVENTOBJECT_LOOPHANDLER },
	{ ID_BTN_OBJ_LOOPFLAG,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_LOOPFLAG },
	{ ID_BTN_OBJ_WARPZONEFLAG,				TOOLTYPE_EVENTOBJECT, EVENTOBJECT_WARPZONEFLAG },
	{ ID_BTN_OBJ_TRAMPOLIN_MINI,			TOOLTYPE_EVENTOBJECT, EVENTOBJECT_TRAMPOLINMINI },
 	{ ID_BTN_OBJ_CHECKPOINTFLAG,			TOOLTYPE_EVENTOBJECT, EVENTOBJECT_CHECKPOINT_FLAG },
 	{ ID_BTN_OBJ_VINES,						TOOLTYPE_EVENTOBJECT, EVENTOBJECT_VINELADDER },
 	{ ID_BTN_OBJ_PSWITCH,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_P_SWITCH },
 	{ ID_BTN_OBJ_STARTPOINT_SIGNARROW,		TOOLTYPE_EVENTOBJECT, EVENTOBJECT_STARTPOINT_SIGN },
 	{ ID_BTN_OBJ_SIGNARROW,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_SIGN },
 	{ ID_BTN_OBJ_DOOR,						TOOLTYPE_EVENTOBJECT, EVENTOBJECT_DOOR },
	{ ID_BTN_OBJ_CANNON,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_CANNON },
	{ ID_BTN_OBJ_ONEWAYWALL,				TOOLTYPE_EVENTOBJECT, EVENTOBJECT_ONEWAYWALL },
	{ ID_BTN_OBJ_POWBLOCK,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_POWBLOCK },
	{ ID_BTN_OBJ_GOALBOX,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_GOALBOX },
	{ ID_BTN_OBJ_GOALPOST,					TOOLTYPE_EVENTOBJECT, EVENTOBJECT_GOALPOST },

	{ ID_BTN_OBJ_YOSHI,						TOOLTYPE_VEHICLE, VEHICLE_YOSHI },
	{ ID_BTN_OBJ_CLOUD,						TOOLTYPE_VEHICLE, VEHICLE_CLOUD },
	{ ID_BTN_OBJ_CLOWNCAR,					TOOLTYPE_VEHICLE, VEHICLE_CLOWNCAR },
	{ ID_BTN_OBJ_GOOMBASHOE,				TOOLTYPE_VEHICLE, VEHICLE_GOOMBASHOE },

	{ -1, -1, -1 },
};

// CMarioMapEditorView

IMPLEMENT_DYNCREATE(CMarioMapEditorView, CScrollView)

BEGIN_MESSAGE_MAP(CMarioMapEditorView, CScrollView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMarioMapEditorView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_CLEAR_GROUND_BLOCKS, &CMarioMapEditorView::OnClearGroundBlocks)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_AUTOBUILDING_UNDERGROUNDBONUSSTAGE, &CMarioMapEditorView::OnAutobuildingUndergroundbonusstage)
	ON_COMMAND(ID_CREATE_BALANCED_LIFT, &CMarioMapEditorView::OnCreateBalancedLift)
	ON_COMMAND(ID_CREATE_PIRANHAPLANT_ALLPIPE, &CMarioMapEditorView::OnCreatePiranhaplantAllpipe)
	ON_COMMAND(ID_INSERT_EMPTYPAGE, &CMarioMapEditorView::OnInsertEmptypage)
	ON_WM_SIZE()
	ON_COMMAND(ID_CHECK_VALID, &CMarioMapEditorView::OnCheckValid)
	ON_COMMAND(ID_TOGGLE_MODERNSTYLE, &CMarioMapEditorView::OnToggleModernstyle)
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_USER_LOADDIALOGCOMPLETE, OnLoadDialogComplete)
	ON_WM_VSCROLL()
	ON_COMMAND(ID_SUBSTAGES_ADDSUBSTAGE, &CMarioMapEditorView::OnSubstagesAddsubstage)
	ON_COMMAND(ID_SUBSTAGES_MAINSTAGE, &CMarioMapEditorView::OnSubstagesMainstage)
	ON_COMMAND(ID_SUBSTAGES_SUBSTAGE1, &CMarioMapEditorView::OnSubstagesSubstage1)
	ON_COMMAND(ID_SUBSTAGES_SUBSTAGE2, &CMarioMapEditorView::OnSubstagesSubstage2)
	ON_COMMAND(ID_SUBSTAGES_SUBSTAGE3, &CMarioMapEditorView::OnSubstagesSubstage3)
	ON_COMMAND(ID_SUBSTAGES_SUBSTAGE4, &CMarioMapEditorView::OnSubstagesSubstage4)
	ON_COMMAND(ID_QUICK_CHANGE_PROPERTY, &CMarioMapEditorView::OnQuickChangeProperty)
	ON_COMMAND(ID_QUICK_POWER_UP, &CMarioMapEditorView::OnQuickPowerUp)
	ON_COMMAND(ID_QUICK_TOGGLE_WING, &CMarioMapEditorView::OnQuickToggleWing)
	ON_COMMAND(ID_SELECT_ABILITY_FLAGS, &CMarioMapEditorView::OnSelectAbilityFlags)
	ON_COMMAND(ID_FILE_UPLOAD, &CMarioMapEditorView::OnFileUpload)
	ON_COMMAND(ID_EDIT_COPY, &CMarioMapEditorView::OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, &CMarioMapEditorView::OnEditPaste)
END_MESSAGE_MAP()

// CMarioMapEditorView 생성/소멸

CMarioMapEditorView::CMarioMapEditorView()
{
	m_pDesignPlayGame = nullptr;
}

CMarioMapEditorView::~CMarioMapEditorView()
{
	if (IsPlaying())
	{
		if (GetMainFrame()->m_pDesignPlayThread)
			WaitForSingleObject(GetMainFrame()->m_pDesignPlayThread->m_hThread, INFINITE);
	}
}

BOOL CMarioMapEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CMarioMapEditorView 그리기

void CMarioMapEditorView::OnDraw(CDC* /*pDC*/)
{
	if (IsPlaying() && m_pDesignPlayGame)
	{
		//m_pDesignPlayGame->Render();
		return;
	}

	CMarioMapEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	Game *pGame = pDoc->m_pGame;
	if (pGame && IsActiveView())
		pGame->Render();
}


// CMarioMapEditorView 인쇄


CPoint CMarioMapEditorView::ClientToGame(CPoint & pt)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	Game *pGame = GetGame();
	float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);
	float fScaleY = (float)rcClient.Height() / (float)(pGame->m_nHeight);

	CPoint ptNew(pt.x / fScaleX, pt.y / fScaleY);
	ptNew.Offset(pGame->m_pCamera->m_ptPos.x, pGame->m_pCamera->m_ptPos.y);
	return ptNew;
}

CPoint CMarioMapEditorView::GameToTile(CPoint & pt)
{
	CPoint ptNew(pt.x / TILE_XS, pt.y / TILE_YS);
	return ptNew;
}

CPoint CMarioMapEditorView::ClientToTile(CPoint & pt)
{
	CRect rcClient;
	GetClientRect(&rcClient);

	Game *pGame = GetGame();
	float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);
	float fScaleY = (float)rcClient.Height() / (float)(pGame->m_nHeight);

	CPoint ptNew((pt.x / fScaleX) / TILE_XS, (pt.y / fScaleY) / TILE_YS);
	return ptNew;
}

void CMarioMapEditorView::InvalidateGame()
{
	if (IsPlaying())
		return;

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	pGame->Render();
}

void CMarioMapEditorView::SetEditMode(int nMode)
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	m_nMode = nMode;
	pStage->SetEditMode(nMode);
	
	// StatusBar
	wchar_t* szModes[] = { L"Select", L"Selected", L"DragMove", L"Draw" };
	NaString str;
	str.Format(L"%s\n", szModes[m_nMode]);

	GetMainFrame()->SetStatusText(ID_INDICATOR_EDITMODE, str.wstr());
}

// Toolbar Handler
void CMarioMapEditorView::SetCurTool(int nId)
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	int nToolType = 0;
	int nType = -1;

	for (int i = 0; ; i++)
	{
		if (g_ControlToolTypeMap[i][0] == -1)
			break;

		if (g_ControlToolTypeMap[i][0] == nId)
		{
			nToolType = g_ControlToolTypeMap[i][1];
			nType = g_ControlToolTypeMap[i][2];
			break;
		}
	}

	SetEditMode(EDIT_DRAW);
	pStage->ChangeTool(nToolType, nType);

	UpdateStatusBar(ID_INDICATOR_TYPENAME);	

	if (!IsPlaying())
		InvalidateGame();
}

int CMarioMapEditorView::GetCurToolID()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	int nToolType = pStage->m_nCurToolType;
	int nType = pStage->m_nCurType;

	for (int i = 0; ; i++)
	{
		if (g_ControlToolTypeMap[i][0] == -1)
			break;

		if (nToolType == g_ControlToolTypeMap[i][1] &&
			nType == g_ControlToolTypeMap[i][2])
			return g_ControlToolTypeMap[i][0];
	}

	return -1;
}

void CMarioMapEditorView::CancelTool()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	pStage->m_bDrag = false;

	pStage->m_pVehicleObject = nullptr;
	pStage->m_pEventObject = nullptr;
	pStage->m_pMapObject = nullptr;
	pStage->m_pEnemyObject = nullptr;

	SetEditMode(EDIT_SELECT);
	pStage->m_nCurToolType = TOOLTYPE_PICK;
	pStage->m_nCurType = -1;

	UpdateStatusBar(ID_INDICATOR_TYPENAME);
	InvalidateGame();

	RefreshProperties();
}

void CMarioMapEditorView::RemoveSelected()
{
	if (IsPlaying())
		return;

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	pStage->m_bDrag = false;
	pStage->RemoveSelected();

	SetEditMode(EDIT_SELECT);
	pStage->m_nCurToolType = TOOLTYPE_PICK;
	pStage->m_nCurType = -1;

	UpdateStatusBar(ID_INDICATOR_TYPENAME);
	InvalidateGame();
	
	RefreshProperties();

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::MakeDuplicate(int nCount)
{
	if (IsPlaying())
		return;

	DesignStage *pStage = GetStage();
	switch (pStage->m_nCurToolType)
	{
	case TOOLTYPE_ENEMY:
		{
			int x = pStage->m_pEnemyObject->m_fX;
			int y = pStage->m_pEnemyObject->m_fY;
			int type = pStage->m_pEnemyObject->m_nType;
			for (int i = 1; i < nCount; i++)
			{
				x += 16;
				x += 8;
				if (pStage->HitTest(x, y, pStage->m_nCurToolType) == false)
					pStage->AddEnemy(x, y, type);
			}
			GetDocument()->SetModifiedFlag(TRUE);
		}
		break;
	case TOOLTYPE_ITEM:
		{
			int x = pStage->m_ptTileSelected.x;
			int y = pStage->m_ptTileSelected.y;
			int type = pStage->m_nCurType;
			for (int i = 1; i < nCount; i++)
			{
				x += 1;
				pStage->AddItem(x, y, type);
			}
			GetDocument()->SetModifiedFlag(TRUE);
		}
		break;
	}

	InvalidateGame();
}

void CMarioMapEditorView::QuickChangeProperty()
{
	DesignStage *pStage = GetStage();
	bool bProcessed = true;

#define TOGGLE_BOOL(b)	(b) = !(b);
#define CYCLE_VALUE(_class, _var, _member, _min, _max) \
	((_class*)_var)->_member++; \
	if (((_class*)_var)->_member > _max) \
		((_class*)_var)->_member = _min;

	switch (pStage->m_nCurType)
	{
	case ENEMY_FIREBAR:
		{
			((FireBar*)pStage->m_pEnemyObject)->m_fSpeed *= -1.0f;
		}
		break;
	case EVENTOBJECT_TRAMPOLINMINI:
		{
			((TrampolinMini*)pStage->m_pEventObject)->m_bIsHorizontal =
				!((TrampolinMini*)pStage->m_pEventObject)->m_bIsHorizontal;
		}
		break;
	case MAPOBJECT_PIPE:
		{
			MapObjectPipe *pPipe = ((MapObjectPipe*)pStage->m_pMapObject);
			if (pPipe->m_bUseNewStyle)
			{
				if (pPipe->m_nDirection == PIPEENTRANCE_UPTODOWN)
					pPipe->m_nDirection = PIPEENTRANCE_LEFTTORIGHT;
				else if (pPipe->m_nDirection == PIPEENTRANCE_LEFTTORIGHT)
					pPipe->m_nDirection = PIPEENTRANCE_DOWNTOUP;
				else if (pPipe->m_nDirection == PIPEENTRANCE_DOWNTOUP)
					pPipe->m_nDirection = PIPEENTRANCE_RIGHTTOLEFT;
				else if (pPipe->m_nDirection == PIPEENTRANCE_RIGHTTOLEFT)
					pPipe->m_nDirection = PIPEENTRANCE_UPTODOWN;
			}
			else
			{
				pPipe->m_bUpToDown = !pPipe->m_bUpToDown;
			}
		}
		break;
	case MAPOBJECT_PIPEHORZ:
		{
			((MapObjectPipeHorz*)pStage->m_pMapObject)->m_bLeftToRight =
				!((MapObjectPipeHorz*)pStage->m_pMapObject)->m_bLeftToRight;
		}
		break;
	case MAPOBJECT_MUSHROOMISLAND:
		{
			CYCLE_VALUE(MapObjectMushroomIsland, pStage->m_pMapObject, m_nSubType, 0, 2);
		}
		break;
	case MAPOBJECT_SEMISOLID:
		{
			CYCLE_VALUE(MapObjectSemiSolid, pStage->m_pMapObject, m_nSubType, 0, 2);
		}
		break;
	case VEHICLE_YOSHI:
		{
			TOGGLE_BOOL(((Yoshi*)pStage->m_pVehicleObject)->m_bHatched);
		}
		break;
	case EVENTOBJECT_DOOR:
		{
			TOGGLE_BOOL(((Door*)pStage->m_pEventObject)->m_bLocked);
		}
		break;
	case EVENTOBJECT_PRINCESS:
		{
			TOGGLE_BOOL(((Princess*)pStage->m_pEventObject)->m_bIsReal);
		}
		break;
	case EVENTOBJECT_ONEWAYWALL:
		{
			OneWayWall *pWall = ((OneWayWall*)pStage->m_pEventObject);
			if (++pWall->m_nDirection >= 4)
				pWall->m_nDirection = 0;
		}
		break;
	case EVENTOBJECT_CANNON:
		{
			Cannon *pObj = ((Cannon*)pStage->m_pEventObject);
			if (++pObj->m_nDirection >= 8)
				pObj->m_nDirection = 0;
		}
		break;
	case VEHICLE_GOOMBASHOE:
		{
			CYCLE_VALUE(GoombaShoe, pStage->m_pVehicleObject, m_nShoeType, 0, 1);
		}
		break;
	default:
		bProcessed = false;
		break;
	}

	if (pStage->m_nCurType >= ITEM_MYSTERYMUSHROOM &&
		pStage->m_nCurType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
	{
		Game *pGame = GetGame();

		CostumeDlg dlg(this);
		dlg.m_pGame = pGame;
		dlg.m_pSpriteItem = pGame->m_pSpriteManager->RequestThemed(L"Item", L"Overworld", false);
		dlg.m_nType = pStage->m_nCurType;

		if (dlg.DoModal() == IDOK)
		{
			pStage->SetSelectedCostume(dlg.m_nType);
			bProcessed = true;
		}
	}

	if (bProcessed)
	{
		GetDocument()->SetModifiedFlag(TRUE);

		InvalidateGame();
		RefreshProperties();
	}
}

void CMarioMapEditorView::QuickPowerUp()
{
	DesignStage *pStage = GetStage();

	if (pStage->QuickPowerUp() == false)
		return;

	RefreshProperties();
	GetDocument()->SetModifiedFlag(TRUE);
	
	InvalidateGame();
}

void CMarioMapEditorView::QuickToggleWing()
{
	DesignStage *pStage = GetStage();
	
	if (pStage->QuickToggleWing() == false)
		return;

	RefreshProperties();
	GetDocument()->SetModifiedFlag(TRUE);
	
	InvalidateGame();
}

bool CMarioMapEditorView::OnDragMoveComplete()
{
	bool bRet = false;

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = GetStage();
	if (pStage->m_nCurToolType == TOOLTYPE_PLAYBOT)
	{
		MapObjectBase *pMapObj = (MapObjectBase*)pStage->HitTest(
			pStage->m_pPlayBotObject->m_fX,
			pStage->m_pPlayBotObject->m_fY,
			TOOLTYPE_MAPOBJECT
		);

		if (pMapObj)
		{
			if (pMapObj->m_nType == MAPOBJECT_PIPE || pMapObj->m_nType == MAPOBJECT_PIPEHORZ)
			{
				NaDebugOut(L"PlayBot moved on pipe!\n");

				MapObjectPipe *pPipe = (MapObjectPipe*)pMapObj;
				switch (pPipe->m_nPipeType)
				{
				case PIPE_NONE:
					// Recommended -> Make SubZone
					{
						// Move current playbot to top of pipe
						NaRect rcPipe = pPipe->GetRect();
						pStage->m_pPlayBotObject->SetPosition(rcPipe.Center(), rcPipe.top - 1);
						int nCurIdx = pStage->GetSubStageIndex(pStage);

						// Create or select sub stage.
						SubStageDlg dlg(this);
						dlg.m_nStage = nCurIdx + 1;
						dlg.m_nSubStageCnt = pStage->GetSubStageCount();
						if (dlg.DoModal() == IDOK)
						{
							DesignStage *pSubStage = nullptr;
							int nNewIdx = -1;
							if (nCurIdx + 1 == dlg.m_nStage)
							{
								// Warp in current stage
								pPipe->m_nPipeType = PIPE_WARP_TO_CURRENTSTAGE;
								pSubStage = pStage;
							}
							else if (nCurIdx != -1)
							{
								// Sub to Main
								pPipe->m_nPipeType = PIPE_WARP_TO_SUBZONE;
								pSubStage = (DesignStage*)pStage->GetTopStage();
							}
							else if (dlg.m_nStage == dlg.m_nSubStageCnt + 1)
							{
								// Create & warp to SubStage
								pPipe->m_nPipeType = PIPE_WARP_TO_SUBZONE;
								pSubStage = (DesignStage *)pStage->CreateSubStage();
							}
							else if (nCurIdx == -1)
							{
								// Use exist SubStage
								pPipe->m_nPipeType = PIPE_WARP_TO_SUBZONE;
								pSubStage = (DesignStage*)pStage->m_vecSubStage[dlg.m_nStage - 1];
							}

							nNewIdx = pStage->GetSubStageIndex(pSubStage);

							if (pPipe->m_nPipeType == PIPE_WARP_TO_SUBZONE)
							{
								if (nCurIdx != -1)
									pPipe->m_strStageName.Format(L"MainStage");
								else
									pPipe->m_strStageName.Format(L"Sub%d", nNewIdx + 1);
							}

							ActivateSubStage(nNewIdx, false);

							// 'Make ExitPipe' Mode
							SetCurTool(ID_BTN_OBJ_PIPE);
							pSubStage->m_ptCursor = pStage->m_ptCursor;
							pStage->MoveSelected(pSubStage->m_ptCursor.x, pSubStage->m_ptCursor.y);

							MapObjectPipe *pExitPipe = (MapObjectPipe*)(pSubStage->m_pMapObject);
							int nExitIndex = pSubStage->MakeNextPipeExitIndex();
							pPipe->m_nExitIndex = nExitIndex;
							pExitPipe->m_nUseAsExitIndex = nExitIndex;

							bRet = true;

							ActivateSubStage(nNewIdx, true);
						}
					}
					break;
				case PIPE_WARP_TO_BONUSZONE:
					{
						POSITION pos = theApp.GetFirstDocTemplatePosition();
						CDocTemplate* pTemplate = theApp.GetNextDocTemplate(pos);
						
						CString str = GetDocument()->GetPathName();
						int nIdx = str.ReverseFind(L'\\');
						if (nIdx > 0)
						{
							str = str.Left(nIdx);
							str += L"\\";
							str += pPipe->m_strStageName.wstr();
							str += L".map";

							// Check already open?
							pos = pTemplate->GetFirstDocPosition();
							while (pos != NULL)
							{
								CDocument *pDoc = pTemplate->GetNextDoc(pos);
								CString strEnum = pDoc->GetPathName();
								if (str.CompareNoCase(strEnum) == 0)
								{
									pStage->MovePlayBotToPipeExit(pPipe);
									pos = pDoc->GetFirstViewPosition();
									CView *pView = pDoc->GetNextView(pos);

									// View
									CMDIFrameWndEx *pMDIFrame = ((CMDIFrameWndEx*)(theApp.GetMainWnd()));
									pMDIFrame->SetActiveView(pView);
									
									// Tab
									const CObList &tabs = pMDIFrame->GetMDITabGroups();
									for (POSITION pos = tabs.GetHeadPosition(); pos != 0;)
									{
										CMFCTabCtrl* pNextWnd = DYNAMIC_DOWNCAST(CMFCTabCtrl, tabs.GetNext(pos));
										int nTabCnt = pNextWnd->GetTabsNum();
										for (int i=0; i<nTabCnt; i++)
										{
											CString strTabLabel;
											pNextWnd->GetTabLabel(i, strTabLabel);

											if (strTabLabel.Right(2).Compare(L" *") == 0)
											{
												strTabLabel = strTabLabel.Left(strTabLabel.GetLength() - 2);
											}

											CString strComp = strEnum.Right(strTabLabel.GetLength());
											if (strComp.CompareNoCase(strTabLabel) == 0)
											{
												pNextWnd->ActivateMDITab(i);
												break;
											}
										}
										
										break;
									}

									pStage->MovePlayBotToSafePlace();
									return bRet;
								}
							}

							pTemplate->OpenDocumentFile(str);
						}

						pStage->MovePlayBotToSafePlace();
					}
					break;
				case PIPE_WARP_TO_CURRENTSTAGE:
					{
						pStage->MovePlayBotToPipeExit(pPipe);

						NaRect rcView = pGame->m_pCamera->GetViewport();
						rcView.left += 16;
						rcView.right -= 16;

						// Move Scroll if Bot moved out of camera
						if (rcView.ptInRect(pStage->m_pPlayBotObject->m_fX, pStage->m_pPlayBotObject->m_fY) == false)
						{
							CRect rcClient;
							GetClientRect(&rcClient);
							float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);

							pGame->m_pCamera->m_ptPos.x = max(0, pStage->m_pPlayBotObject->m_fX - (pGame->m_nWidth / 2));

							SetScrollPos(
								SB_HORZ,
								pGame->m_pCamera->m_ptPos.x * fScaleX,
								1);
						}
					}
					break;
				case PIPE_WARPZONE:
					break;
				case PIPE_WARP_TO_SUBZONE:
					{
						NaRect rcPipe = pPipe->GetRect();
						pStage->m_pPlayBotObject->SetPosition(rcPipe.Center(), rcPipe.top - 1);
						int nCurIdx = pStage->GetSubStageIndex(pStage);
						int nNewIdx = -1;

						DesignStage *pSubStage = nullptr;
						if (nCurIdx == -1)
						{
							// Main to Sub
							int i = 0;
							Stage *pTopStage = pStage->GetTopStage();
							BEGIN_VECTOR_LOOP(Stage*, pTopStage->m_vecSubStage) {
								if (pObj->m_strName.Compare(pPipe->m_strStageName) == 0)
								{
									nNewIdx = i;
									break;
								}
								i++;
							} END_VECTOR_LOOP();

							if (nNewIdx == -1)
							{
								// External sub stage?
								MessageBox(L"Cannot find sub stage! External?");
								return bRet;
							}

							pSubStage = (DesignStage*)pTopStage->m_vecSubStage[nNewIdx];
						}
						else
						{
							// Sub to Main
							pSubStage = (DesignStage*)pStage->GetTopStage();
							nNewIdx = -1;
						}

						ActivateSubStage(nNewIdx, false);

						MapObjectBase *pExit = pSubStage->FindPipeExit(pPipe->m_nExitIndex);
						if (pExit)
						{
							NaRect rc;
							if (pExit->m_nType == MAPOBJECT_PIPEEXIT)
							{
								rc = pExit->GetRect();
							}
							else
							{
								MapObjectPipe *pExitPipe = (MapObjectPipe *)pExit;
								rc = pExitPipe->GetEntranceRect();
							}

							pSubStage->m_pPlayBotObject->SetPosition(
								rc.Center(), rc.Middle()
							);
						}
						else
						{
							// Error?
							pSubStage->MovePlayBotToSafePlace();
						}
					}
					break;
				}
			}
		}
	}

	return bRet;
}

DesignStage * CMarioMapEditorView::GetStage()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	return pStage;
}

void CMarioMapEditorView::RefreshProperties()
{
	((CMainFrame*)theApp.m_pMainWnd)->m_wndProperties.RefreshPropValues(this);
}

void CMarioMapEditorView::OnPropertyChange(NaString strName)
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;
	
	if (IsPlaying())
		StopPlaying();

	if (strName.Compare(L"MaxPage") == 0)
	{
		if (pStage->m_nMaxPage < 1)
			pStage->m_nMaxPage = 1;
		
		int nCurMaxPage = pStage->m_sizeTile.cx / GameDefaults::nPageTileWidth;
		if (pStage->m_nMaxPage < nCurMaxPage)
		{
			if (IDNO == MessageBox(L"Some data will be lost! Proceed?", L"Warning", MB_YESNO))
			{
				pStage->m_nMaxPage = nCurMaxPage;
				return;
			}
		}
		pStage->OnMaxPageChanged();

		RecalcScrollSize();
	}
	else if (strName.Compare(L"MaxVerticalPage") == 0)
	{
		if (pStage->m_nMaxVertPage < 1)
			pStage->m_nMaxVertPage = 1;

		int nCurMaxPage = pStage->m_sizeTile.cy / GameDefaults::nPageTileHeight;
		if (pStage->m_nMaxVertPage < nCurMaxPage)
		{
			if (IDNO == MessageBox(L"Some data will be lost! Proceed?", L"Warning", MB_YESNO))
			{
				pStage->m_nMaxVertPage = nCurMaxPage;
				return;
			}
		}
		pStage->OnMaxVertPageChanged();

		RecalcScrollSize();
	}
	else if (strName.Compare(L"CustomThemeName") == 0)
	{
		pStage->InitTheme();
		RefreshProperties();
	}
	else if (strName.Compare(L"Theme") == 0)
	{
		pStage->InitTheme();
	}
	else if (strName.Compare(L"BackgroundTheme") == 0)
	{
		pStage->BuildBackground();
	}
	else if (strName.Compare(L"PowerUp") == 0)
	{
		if (pStage->m_nCurToolType == TOOLTYPE_ENEMY)
			pStage->m_pEnemyObject->OnPowerUp();
		if (pStage->m_nCurToolType == TOOLTYPE_VEHICLE)
			pStage->m_pVehicleObject->OnPowerUp();
	}
	else if (strName.Compare(L"ThemeSet") == 0)
	{
		pStage->InitTheme();
		pStage->BuildBackground();
		RefreshProperties();
	}
	else if (strName.Compare(L"Gravity") == 0)
	{
		if (pStage->m_fGravity <= 0)
			pStage->m_fGravity = 0.0001f;

		RefreshProperties();
	}
	else if (strName.Compare(L"Time") == 0)
	{
		if (pStage->m_nTime >= 0 && pStage->m_nTime < 10)
		{
			pStage->m_nTime = 10;
			RefreshProperties();
		}
	}
	else if (strName.Compare(L"GameTheme") == 0)
	{
		pStage->InitTheme();
		pStage->BuildBackground();
	}

	InvalidateGame();

	GetDocument()->SetModifiedFlag(TRUE);
}

BOOL CMarioMapEditorView::IsPlaying()
{
	if (m_pDesignPlayGame != nullptr)
		return TRUE;

	if (GetMainFrame()->m_pDesignPlayThread)
		return TRUE;

	return FALSE;
}

void CMarioMapEditorView::StopPlaying()
{
	if (IsPlaying())
	{
		// Just Toggle Flag
		m_pDesignPlayGame->m_bExitFlag = true;
	}
}

void CMarioMapEditorView::DoPlaying(int nX, int nY)
{
	if (IsPlaying())
		return;

	DesignGame *pDesignGame = (DesignGame*)GetGame();
	DesignStage *pDesignStage = (DesignStage*)pDesignGame->m_pCurStage;

	auto pDoc = GetDocument();
	const CString strTitle = pDoc->GetTitle();

	Stage *pTopStage = pDesignGame->m_pCurStage->GetTopStage();
	pTopStage->m_strName = strTitle;
	NaString &strName = pTopStage->m_strName;
	if (strName.Right(4).CompareNoCase(L".map") == 0)
	{
		strName = strName.Left(strName.GetLength() - 4);
	}

	DesignPlayGame *pGame = m_pDesignPlayGame = new DesignPlayGame();
	pGame->m_pResourceHolder = pDesignGame->m_pResourceHolder;
	pGame->InitFromGame(pDesignGame);

	// Parse StagePack Path
	NaString strPathName = pDoc->GetPathName();
	{
		wchar_t *szMapBase = L"Resources\\Map\\";
		NaString strMapBase = szMapBase;
		int nMapIdx = strPathName.Find(szMapBase);
		if (nMapIdx > 0)
		{
			NaString strTemp = strPathName.Mid(nMapIdx + strMapBase.GetLength());
			if (strTemp.Find(L"\\") > 0)
			{
				strTemp = strTemp.Left(strTemp.Find(L"\\"));

				if (strTemp.CompareNoCase(L"Classic") == 0)
				{
					pGame->m_strStagePack = L"Classic";
					pGame->m_nStagePackType = STAGEPACK_CLASSIC;
				}
				else
				{
					pGame->m_strStagePack = strTemp;
					pGame->m_nStagePackType = STAGEPACK_CUSTOM;
				}
			}
			else
			{
				pGame->m_strStagePack = L"";
				pGame->m_nStagePackType = STAGEPACK_SINGLE;
			}
		}
	}

	GameObjectBase *pPlayerObj = pGame->m_pCurPlayer->m_pControlObj;
	if (nX == -1 && nY == -1)
	{
		// Auto
		NaRect rcView = pDesignGame->m_pCamera->GetViewport();

		// Find StartPoint in current view
		MapObjectBase *pStartPointInViewport = pDesignStage->GetStartPointInViewport();
		if (pStartPointInViewport)
		{
			nX = pPlayerObj->m_fX = pStartPointInViewport->m_nTX * TILE_XS + TILE_XS / 2;
			nY = pPlayerObj->m_fY = pStartPointInViewport->m_nTY * TILE_YS + TILE_YS;
		}

		// 
		if (nX == -1)
		{
			pDesignStage->MovePlayBotToSafePlace();

			pPlayerObj->m_fX = pDesignStage->m_pPlayBotObject->m_fX;
			pPlayerObj->m_fY = pDesignStage->m_pPlayBotObject->m_fY + 1;
			pPlayerObj->m_nState = STATE_STAND;
			pPlayerObj->m_nNextState = -1;
#if !defined(NDEBUG)
			pPlayerObj->m_nDamageCooltime = 24;
#else
			pPlayerObj->m_nDamageCooltime = 24 * 5;
#endif
		}
	}
	else
	{
		pPlayerObj->m_fX = nX;
		pPlayerObj->m_fY = nY;
	}

	SetFocus();
	GetMainFrame()->m_pDesignPlayThread = AfxBeginThread(PlayGameThreadProc, this);

	InvalidateGame();
}

UINT CMarioMapEditorView::PlayGameThreadProc(LPVOID pArg)
{
	CMarioMapEditorView *pThis = (CMarioMapEditorView*)pArg;
	DesignPlayGame *pGame = pThis->m_pDesignPlayGame;

	MSG msg = {};
	while (pGame->m_bExitFlag != true && msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		pGame->ProcessGameLoop();
	}

	NaDebugOut(L"DesignPlayGame: Out Of MainLoop\n");
	pGame->RestoreGame();
	pGame->Release();
	delete pGame;

	DesignGame *pDesignGame = (DesignGame*)pThis->GetGame();

	// Restore ScrollBar
	if (!pThis->GetMainFrame()->m_bOnClosing)
	{
		pThis->SyncScrollPos();
	}

	if (pDesignGame->m_pResourceHolder)
	{
		pDesignGame->m_pResourceHolder->AttachResources(pDesignGame);
		pDesignGame->m_pRenderManager->Init();
		pDesignGame->m_pSpriteManager->m_vecRenderQueue.clear();
	}

	pThis->m_pDesignPlayGame = nullptr;
	pThis->GetMainFrame()->m_pDesignPlayThread = nullptr;
	NaDebugOut(L"DesignPlayGame: Current RenderQueue Size: %d\n",
		pThis->GetGame()->m_pSpriteManager->m_vecRenderQueue.size()
	);

	if (pThis->IsActiveView())
	{
		//pThis->InvalidateGame();
		pThis->Invalidate();
	}

	NaDebugOut(L"DesignPlayGame: delete Game\n");

	return 0;
}

void CMarioMapEditorView::OnActivateView()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	
	if (pGame->m_pResourceHolder)
	{
		pGame->m_pResourceHolder->AttachResources(pGame);
		InvalidateGame();
	}

	RefreshProperties();
}

bool CMarioMapEditorView::IsActiveView()
{
	if (((CMainFrame*)theApp.GetMainWnd())->m_pActiveView == this)
	{
		return true;
	}
	return false;
}

void CMarioMapEditorView::ActivateSubStage(int nIdx, bool bRedraw)
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = GetStage();

	pStage = (DesignStage*)pStage->GetTopStage();
	if (nIdx == -1)
	{
		// Main Stage
		pGame->m_pCurStage = pStage;
	}
	else if (pStage->m_vecSubStage.size() > nIdx)
	{
		pGame->m_pCurStage = pStage->m_vecSubStage[nIdx];
	}
	else
	{
		MessageBox(L"No Sub Stage exists.");
		return;
	}

	if (bRedraw)
	{
		GameObjectBase *pPlayBot = ((DesignStage*)(pGame->m_pCurStage))->m_pPlayBotObject;
		pGame->m_pCamera->LookAt(pPlayBot);
		pGame->m_pCamera->SetPosition(
			pPlayBot->m_fX - (pGame->Width() / 2),
			pPlayBot->m_fY - (pGame->Height() / 2));

		RecalcScrollSize();
		SyncScrollPos();

		Invalidate();
		InvalidateGame();
	}

	RefreshProperties();
}

void CMarioMapEditorView::UpdateStatusBar(int nID)
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	switch (nID)
	{
	case ID_INDICATOR_TYPENAME:
		{
			int nType = pStage->m_nCurType;
			NaString str = L" ";
			switch (pStage->m_nCurToolType)
			{
			case TOOLTYPE_PICK:	str = L"Pick"; break;
			case TOOLTYPE_TILE: 
				{
					if (nType == TILE_GROUND)
					{
						int nIdx = pStage->GetAutoGroundTileIndex(pStage->m_ptTileSelected.x, pStage->m_ptTileSelected.y);
						str.Format(L"Tile (%d)", nIdx);
					}
					else
						str = L"Tile"; 
					break;
				}
			case TOOLTYPE_ITEM: 
				if (nType >= ITEM_MYSTERYMUSHROOM &&
					nType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
				{
					str = L"MysteryMushroom";
				}
				else
					str = g_szItemTypes[nType - ITEM_BEGIN]; break;
			case TOOLTYPE_ENEMY: str = g_szEnemyTypes[nType - ENEMY_BEGIN]; break;
			case TOOLTYPE_MAPOBJECT: str = g_szMapObjectTypes[nType - MAPOBJECT_BEGIN]; break;
			case TOOLTYPE_EVENTOBJECT: str = g_szEventObjectTypes[nType - EVENTOBJECT_BEGIN]; break;
			case TOOLTYPE_VEHICLE: str = g_szVehicleObjectTypes[nType - VEHICLE_BEGIN]; break;
			case TOOLTYPE_ERASER: str = L"Eraser"; break;
			case TOOLTYPE_PLAYBOT: str = L"PlayBot"; break;
			}

			GetMainFrame()->SetStatusText(ID_INDICATOR_TYPENAME, str.wstr());
		}
		break;
	default:
		break;
	}
}

CMainFrame * CMarioMapEditorView::GetMainFrame()
{
	return (CMainFrame*)(theApp.GetMainWnd());
}

void CMarioMapEditorView::ShowResourceList()
{
	NaString str = L"== Sprites ==\n";
	auto g = GetGame();
	BEGIN_VECTOR_LOOP(Sprite*, g->m_pSpriteManager->m_vecSprites) {
		NaString strTemp;
		strTemp.Format(L"    %ls, (%s)\n",
			pObj->m_strName.wstr(), 
			pObj->m_strGameTheme.GetLength() > 0 ? pObj->m_strGameTheme.wstr() : L"ORIGINAL"
			);

		str += strTemp;
	} END_VECTOR_LOOP()

	MessageBox(str.wstr(), L"Debug", MB_OK);
}

void CMarioMapEditorView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMarioMapEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMarioMapEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMarioMapEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMarioMapEditorView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	if (IsPlaying())
		return;

	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMarioMapEditorView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	bool bQuickChangeProp = false;
	bool bQuickPowerUp = false;
	bool bQuickToggleWing = false;
	bool bMysteryMushroom = false;
	CString strQuickProp = L"Quick Property";

	DesignStage *pStage = GetStage();
	switch (pStage->m_nCurToolType)
	{
	case TOOLTYPE_ENEMY:
		bQuickPowerUp = true;
		bQuickToggleWing = true;
		break;
	}

	switch(pStage->m_nCurType)
	{
	case ENEMY_FIREBAR:
		strQuickProp = L"Toggle Direction (+ / -)";
		bQuickChangeProp = true;
		break;
	case EVENTOBJECT_TRAMPOLINMINI:
		strQuickProp = L"Toggle Horz<->Vert";
		bQuickChangeProp = true;
		break;
	case MAPOBJECT_PIPE:
		strQuickProp = L"Change Pipe.Direction";
		bQuickChangeProp = true;
		break;
	case MAPOBJECT_PIPEHORZ:
		strQuickProp = L"Toggle Pipe.Direction";
		bQuickChangeProp = true;
		break;
	}

	if (pStage->m_nCurType >= ITEM_MYSTERYMUSHROOM &&
		pStage->m_nCurType <= ITEM_MYSTERYMUSHROOM_RESERVED_LAST)
	{
		strQuickProp = L"Change Costume";
		bQuickChangeProp = true;
		bMysteryMushroom = true;
	}

	if (!bQuickChangeProp && !bQuickPowerUp && !bQuickToggleWing)
	{
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
	}
	else
	{
		CMenu *pMenu = new CMenu;
		pMenu->CreatePopupMenu();

		if (bQuickChangeProp)
			pMenu->AppendMenu(MF_STRING, ID_QUICK_CHANGE_PROPERTY, strQuickProp);

		if (bQuickPowerUp)
			pMenu->AppendMenu(MF_STRING, ID_QUICK_POWER_UP, L"Toggle PowerUp");
		
		if (bQuickToggleWing)
			pMenu->AppendMenu(MF_STRING, ID_QUICK_TOGGLE_WING, L"Toggle Wing");

		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		delete pMenu;
	}
#endif
}


// CMarioMapEditorView 진단

#ifdef _DEBUG
void CMarioMapEditorView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMarioMapEditorView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMarioMapEditorDoc* CMarioMapEditorView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMarioMapEditorDoc)));
	return (CMarioMapEditorDoc*)m_pDocument;
}
#endif //_DEBUG


// CMarioMapEditorView 메시지 처리기

Game* CMarioMapEditorView::GetGame()
{
	return GetDocument()->m_pGame;
}

void CMarioMapEditorView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	DesignGame *pGame = (DesignGame*)GetGame();

	// Just Attach window.
	pGame->Init(m_hWnd, nullptr);
	if (GetDocument()->m_bCreated)
	{
		NaString strConfigName = GameConfig::GetGameConfig(pGame)->m_strName;
		if (strConfigName.GetLength() > 0)
		{
			pGame->m_pCurStage->m_strCreator = strConfigName;
		}
	}

	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;
	//pGame->AttachWindow(m_hWnd);

	CMarioMapEditorDoc *pDoc = GetDocument();
	if (pDoc->m_strPathName.GetLength() > 0)
	{
		pStage->LoadFile(pDoc->m_strPathName.cstr());
		if (pStage->m_strTitle.GetLength() == 0)
		{
			auto ar = pDoc->m_strPathName.Split(L"\\");

			// Cut filename only
			pStage->m_strTitle = ar[ar.GetCount() - 1];

			// Cut .map
			pStage->m_strTitle = pStage->m_strTitle.Left(pStage->m_strTitle.GetLength() - 4);
		}
	}

	pStage->InitTheme();
	pStage->BuildBackground();

	// Recursive - Sub Stage
	BEGIN_VECTOR_LOOP(Stage*, pStage->m_vecSubStage) {
		pObj->InitTheme();
		pObj->BuildBackground();
	} END_VECTOR_LOOP();

	// for Scroll to StartPoint
	PostMessage(WM_USER_LOADDIALOGCOMPLETE);

	SetEditMode(EDIT_SELECT);
	m_bLButtonDown = FALSE;

	RefreshProperties();
}

LRESULT CMarioMapEditorView::OnLoadDialogComplete(WPARAM wParam, LPARAM lParam)
{
	NaDebugOut(L"OnLoadDialogComplete\n");

	RecalcScrollSize();
	
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	// Find StartPoint in current view
	MapObjectBase *pStartPoint = pStage->GetStartPointInViewport();
	BEGIN_VECTOR_LOOP(MapObjectBase*, pStage->m_vecMapObject) {
		if (pObj->m_nType == MAPOBJECT_STARTPOINT)
		{
			pStartPoint = pObj;
			break;
		}
	} END_VECTOR_LOOP();

	if (pStartPoint)
	{
		pGame->m_pCamera->SetPosition(
			(pStartPoint->m_nTX * TILE_XS) +
			(TILE_XS / 2) -
			(pGame->m_nWidth / 2), 
			(pStartPoint->m_nTY / GameDefaults::nPageTileHeight) *
			pGame->m_nHeight
			);
	}
	else
		pGame->m_pCamera->SetPosition(0, 9999); // LeftBottom corner
	
	pStage->MovePlayBotToSafePlace();

	SyncScrollPos();

	return 0;
}

void CMarioMapEditorView::RecalcScrollSize()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;
	if (pStage == nullptr)
		return;

	CRect rcClient;
	GetClientRect(&rcClient);
	float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);
	float fScaleY = (float)rcClient.Height() / (float)(pGame->m_nHeight);

	CSize size(
		GameDefaults::nPageWidth * pStage->m_nMaxPage * fScaleX,
		GameDefaults::nPageHeight * pStage->m_nMaxVertPage * fScaleY);

	SetScrollSizes(MM_TEXT, size);
}

void CMarioMapEditorView::SyncScrollPos()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = GetStage();
	
	CRect rcClient;
	GetClientRect(&rcClient);

	float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);
	//if (pGame->m_nWidth * fScaleX > rcClient.Width())
	if (pStage->m_sizeTile.cx * TILE_XS > pGame->m_nWidth)
	{
		float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);
		SetScrollPos(SB_HORZ, pGame->m_pCamera->m_ptPos.x * fScaleX, 1);
	}
	
	if (pStage->m_sizeTile.cy > 16)
	{
		float fScaleY = (float)rcClient.Height() / (float)(pGame->m_nHeight);
		SetScrollPos(SB_VERT, pGame->m_pCamera->m_ptPos.y * fScaleY, 1);
	}
}

void CMarioMapEditorView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (IsPlaying())
		return;

	//NaDebugOut(L"%s()\n-> Code: %d\n-> Pos: %d\n", __FUNCTIONW__, nSBCode, nPos);

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	CRect rcClient;
	GetClientRect(&rcClient);
	float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);

	float &fPos = pGame->m_pCamera->m_ptPos.x;

	float fCur = fPos;

	switch (nSBCode)
	{
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			pGame->m_pCamera->m_ptPos.x = (nPos / fScaleX);
		}
		break;
	}
	
	SetRedraw(FALSE);
	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	CPoint pt = CScrollView::GetScrollPosition();

	switch (nSBCode)
	{
	case SB_LINEUP: // wheel, "<" button
		fPos = (pt.x / fScaleX);
		break;
	case SB_LINEDOWN: // wheel, ">" button
		fPos = (pt.x / fScaleX);
		break;
	}

	float fOffset = fPos - fCur;
	pStage->MovePlayBot(fOffset, 0);

	SetRedraw(TRUE);
	InvalidateGame();
	Invalidate();
}

void CMarioMapEditorView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (IsPlaying())
		return;

	//NaDebugOut(L"%s()\n-> Code: %d\n-> Pos: %d\n", __FUNCTIONW__, nSBCode, nPos);

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	CRect rcClient;
	GetClientRect(&rcClient);
	float fScaleY = (float)rcClient.Height() / (float)(pGame->m_nHeight);

	float &fPos = pGame->m_pCamera->m_ptPos.y;
	float fCur = fPos;

	switch (nSBCode)
	{
	case SB_PAGEUP:
	case SB_PAGEDOWN:
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		{
			pGame->m_pCamera->m_ptPos.y = (nPos / fScaleY);
		}
		break;
	}

	SetRedraw(FALSE);

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);

	CPoint pt = CScrollView::GetScrollPosition();
	switch (nSBCode)
	{
	case SB_LINEUP: // wheel, "<" button
		fPos = (pt.y / fScaleY);
		break;
	case SB_LINEDOWN: // wheel, ">" button
		fPos = (pt.y / fScaleY);
		break;
	}

	float fOffset = fPos - fCur;
	pStage->MovePlayBot(0, fOffset);

	SetRedraw(TRUE);
	InvalidateGame();
	Invalidate();
}

BOOL CMarioMapEditorView::PreTranslateMessage(MSG* pMsg)
{
	POINT ptScroll = { 0, 0 };
	bool bHandled = true;

	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			if (IsPlaying())
			{
				switch (pMsg->wParam)
				{
#if defined(DEBUG)
				case VK_F1:
					ShowResourceList();
					break;
#endif
				case VK_F5:
					if (GetAsyncKeyState(VK_SHIFT))
						StopPlaying();
					break;
				case VK_SPACE:
					StopPlaying();
					break;
				default:
					bHandled = false;
					break;
				}
			}
			else
			{
				switch (pMsg->wParam)
				{
				case VK_ESCAPE:
					CancelTool();
					return TRUE;
					break;
				case VK_DELETE:
					RemoveSelected();
					break;
#if defined(DEBUG)
				case VK_F1:
					ShowResourceList();
					break;
#endif
				case VK_F5:
				case VK_SPACE:
					DoPlaying(-1, -1);
					break;
				case VK_OEM_3: // `~
					ActivateSubStage(-1);
					break;
				case VK_1:
					ActivateSubStage(0);
					break;
				case VK_2:
					if (GetAsyncKeyState(VK_SHIFT))
						MakeDuplicate(2);
					else
						ActivateSubStage(1);
					break;
				case VK_3:
					if (GetAsyncKeyState(VK_SHIFT))
						MakeDuplicate(3);
					else
						ActivateSubStage(2);
					break;
				case VK_4:
					if (GetAsyncKeyState(VK_SHIFT))
						MakeDuplicate(4);
					else
						ActivateSubStage(3);
					break;
				case VK_E:
					SetCurTool(ID_BTN_BLOCK_ERASER);
					break;
				case VK_SUBTRACT:
				case VK_O:
					QuickChangeProperty();
					return TRUE;
					break;
				case VK_ADD:
				case VK_P:
					QuickPowerUp();
					break;
				case VK_MULTIPLY:
				case VK_W:
					QuickToggleWing();
					break;
				case VK_LEFT:
					ptScroll.x = -1;
					break;
				case VK_RIGHT:
					ptScroll.x = 1;
					break;
				case VK_UP:
					ptScroll.y = -1;
					break;
				case VK_DOWN:
					ptScroll.y = 1;
					break;
				default:
					bHandled = false;
					break;
				}
			}
		}
		break;
	}

	if (ptScroll.x != 0 || ptScroll.y != 0)
	{
		int nScrollSize = 16;
		DesignGame *pGame = (DesignGame*)GetGame();
		DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;
		auto ptPos = pGame->m_pCamera->GetPosition();
		pGame->m_pCamera->SetPosition(
			ptPos.x + (ptScroll.x * nScrollSize),
			ptPos.y + (ptScroll.y * nScrollSize));

		pStage->MovePlayBot(
			(ptScroll.x * nScrollSize),
			(ptScroll.y * nScrollSize)
		);
		SyncScrollPos();

		InvalidateGame();
	}
	
	if (bHandled && pMsg->hwnd != GetSafeHwnd())
		return 0;

	return CScrollView::PreTranslateMessage(pMsg);
}

BOOL CMarioMapEditorView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (IsPlaying())
		return TRUE;
	
	UINT nMessage = WM_HSCROLL;
	bool bHScroll = true;
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;
	if (pStage->m_sizeTile.cy > 16)
	{
		bHScroll = false;
		nMessage = WM_VSCROLL;
	}

	UINT WheelScrollLines;
	::SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &WheelScrollLines, 0);

	if (WheelScrollLines == WHEEL_PAGESCROLL)
	{
		SendMessage(nMessage, MAKEWPARAM((zDelta > 0) ? SB_PAGEUP : SB_PAGEDOWN, 0), 0);
	}
	else
	{
		int nLines = (abs(zDelta) * WheelScrollLines) / WHEEL_DELTA;
		while (nLines--)
		{
			SendMessage(nMessage, MAKEWPARAM((zDelta > 0) ? SB_LINEUP : SB_LINEDOWN, 0), 0);
		}
	}

	return TRUE;
}


BOOL CMarioMapEditorView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	//return CScrollView::OnEraseBkgnd(pDC);

	return TRUE;
}

void CMarioMapEditorView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (IsPlaying())
		return;

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	CPoint pt = ClientToGame(point);
	pStage->m_ptDragStart = pt;
	pStage->m_bDrag = true;

	m_bLButtonDown = TRUE;

	switch (m_nMode)
	{
	case EDIT_SELECT:
	case EDIT_SELECTED:
		pStage->Select(pt.x, pt.y);
		if (pStage->m_nCurToolType != TOOLTYPE_PICK)
		{
			SetEditMode(EDIT_SELECTED);
			pStage->m_bIsCurToolNotPlaced = false;
		}
		else
		{
			SetEditMode(EDIT_SELECT);
			SetCurTool(TOOLTYPE_PICK);
			pStage->m_bIsCurToolNotPlaced = true;
		}

		UpdateStatusBar(ID_INDICATOR_TYPENAME);
		break;
	case EDIT_DRAW:
		{
			if (pStage->m_nCurToolType == TOOLTYPE_TILE ||
				pStage->m_nCurToolType == TOOLTYPE_ITEM ||
				pStage->m_nCurToolType == TOOLTYPE_ERASER)
			{
				pStage->Draw();
			}
			else
			{
				SetEditMode(EDIT_SELECTED);
			}

			GetDocument()->SetModifiedFlag(TRUE);
			pStage->m_bIsCurToolNotPlaced = false;
		}
		break;
	}

	InvalidateGame();

	CScrollView::OnLButtonDown(nFlags, point);
}

void CMarioMapEditorView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (IsPlaying())
		return;

	// Convert to Game Coord
	CPoint pt = ClientToGame(point);
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	pStage->m_ptCursor = { pt.x, pt.y };

	//NaDebugOut(L"PlayBot: %.2f, %.2f\n", pStage->m_pPlayBotObject->m_fX, pStage->m_pPlayBotObject->m_fY);

	if (m_bLButtonDown)
	{
		switch (m_nMode)
		{
		case EDIT_SELECT:
		case EDIT_SELECTED:
			if (pStage->m_ptDragStart.x != pt.x ||
				pStage->m_ptDragStart.y != pt.y)
			{
				if (pStage->Pick(pt.x, pt.y))
				{
					pStage->m_bDrag = false;
					SetEditMode(EDIT_DRAGMOVE);
				}
			}
			break;
		case EDIT_DRAW:
			{
				if (pStage->m_nCurToolType == TOOLTYPE_TILE ||
					pStage->m_nCurToolType == TOOLTYPE_ITEM ||
					pStage->m_nCurToolType == TOOLTYPE_ERASER
					)
				{
					pStage->Draw();

					GetDocument()->SetModifiedFlag(TRUE);
				}
			}
			break;
		case EDIT_DRAGMOVE:
			{
				pStage->m_bDrag = false;
				pStage->MoveSelected(pt.x, pt.y);

				GetDocument()->SetModifiedFlag(TRUE);
			}
			break;
		}
	}
	else
	{
		switch (m_nMode)
		{
		case EDIT_DRAW:
			pStage->m_bDrag = false;
			pStage->MoveSelected(pt.x, pt.y);
			break;
		}
	}

	// StatusBar
	NaString str;
	str.Format(L"Game: %4d,%4d / Tile: %3d,%3d",
		pt.x,
		pt.y,
		pt.x / TILE_XS,
		pt.y / TILE_YS
	);
	GetMainFrame()->SetStatusText(ID_INDICATOR_CURSOR, str.wstr());

	InvalidateGame();

	CScrollView::OnMouseMove(nFlags, point);
}

void CMarioMapEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (IsPlaying())
		return;

	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	CPoint pt = ClientToGame(point);
	CPoint ptTile = GameToTile(pt);
	pStage->m_ptDragEnd = pt;
	pStage->m_bDrag = false;

	m_bLButtonDown = FALSE;

	switch (m_nMode)
	{
	case EDIT_SELECT:
		pStage->Select(pt.x, pt.y);
		break;
	case EDIT_DRAGMOVE:
		pStage->DragMove(pt.x, pt.y);
		if (OnDragMoveComplete() == false)
			SetEditMode(EDIT_SELECT);

		GetDocument()->SetModifiedFlag(TRUE);
		break;
	case EDIT_DRAW:
		pStage->Draw();

		if (pStage->m_nCurToolType != TOOLTYPE_TILE &&
			pStage->m_nCurToolType != TOOLTYPE_ITEM &&
			pStage->m_nCurToolType != TOOLTYPE_ERASER)
		SetEditMode(EDIT_SELECT);

		GetDocument()->SetModifiedFlag(TRUE);
		break;
	}

	InvalidateGame();
	RefreshProperties();

	CScrollView::OnLButtonUp(nFlags, point);
}

void CMarioMapEditorView::OnClearGroundBlocks()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = (DesignStage*)pGame->m_pCurStage;

	pStage->ClearGroundBlocks();
	InvalidateGame();

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CPoint ptClicked = ClientToGame(point);

	DesignGame *pDGame = (DesignGame*)GetGame();
	DesignStage *pDStage = (DesignStage*)pDGame->m_pCurStage;

	if (IsPlaying())
	{
		m_pDesignPlayGame->m_bExitFlag = true;
		return;
	}

	DoPlaying(ptClicked.x, ptClicked.y);
}


void CMarioMapEditorView::OnAutobuildingUndergroundbonusstage()
{
	DesignStage *pStage = GetStage();

	pStage->m_nTheme = STAGETHEME_UNDERGROUND;
	pStage->m_nBackgroundTheme = STAGEBGTHEME_NONE;

	for (int i = 2; i <= 12; i++)
	{
		pStage->SetTileData(0, i, TILE_BRICK);
	}

	pStage->AddMapObject(2, 2, MAPOBJECT_STARTPOINT);
	pStage->AddMapObject(16, 12, MAPOBJECT_CAMERASTOP);
	pStage->AddMapObject(15, 2, MAPOBJECT_PIPEBODY);

	MapObjectPipeHorz *pObj = (MapObjectPipeHorz*)pStage->AddMapObject(13, 11, MAPOBJECT_PIPEHORZ);
	pObj->m_strStageName = L"Exit";
	pObj->m_nPipeType = PIPE_WARP_TO_BONUSZONE;

	pStage->InitTheme();
	pStage->BuildBackground();
	InvalidateGame();

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::OnCreateBalancedLift()
{
	DesignStage *pStage = GetStage();

	int x = pStage->m_ptCursor.x;
	int y = pStage->m_ptCursor.y;

	Bridge *pBridge = (Bridge*)pStage->CreateEventObject(x, y, EVENTOBJECT_BRIDGE);
	pBridge->m_nBridgeType = BRIDGETYPE_BALANCE_LEFT;

	pBridge = (Bridge*)pStage->CreateEventObject(x + (16*4), y, EVENTOBJECT_BRIDGE);
	pBridge->m_nBridgeType = BRIDGETYPE_BALANCE_RIGHT;

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::OnCreatePiranhaplantAllpipe()
{
	DesignStage *pStage = GetStage();

	BEGIN_VECTOR_LOOP(MapObjectBase*, pStage->m_vecMapObject) {
		if (pObj->m_nType == MAPOBJECT_PIPE)
		{
			NaRect rc = pObj->GetRect();
			if (pStage->HitTest(rc.Center(), rc.top - 1, TOOLTYPE_ENEMY) == false)
			{
				pStage->AddEnemy(rc.Center(), rc.top, ENEMY_PIRANHA_PLANT);
			}
		}
	} END_VECTOR_LOOP();

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::OnInsertEmptypage()
{
	DesignStage *pStage = GetStage();

	int nCurPage = pStage->m_ptCursor.x / GameDefaults::nPageWidth;
	int nCurPageTileX = nCurPage * GameDefaults::nPageTileWidth;
	int nNextPageTileX = (nCurPage + 1) * GameDefaults::nPageTileWidth;

	int nTileSize =
		pStage->m_nMaxPage *
		GameDefaults::nPageTileWidth *
		GameDefaults::nPageTileHeight;

	BYTE *pNewTileData = new BYTE[nTileSize];
	ZeroMemory(pNewTileData, sizeof(BYTE) * nTileSize);

	// 0...15 / 16...31 / 32...47 / 48...
	// 0...15 / NewPage / 16...31 / 32...47
	//             ^
	//          inserted here

	for (int i = 0; i < GameDefaults::nPageTileHeight; i++)
	{
		int nRowSize;
		nRowSize = (nCurPage) * GameDefaults::nPageTileWidth;
		if (nRowSize > 0)
		{
			// Left Pages
			memcpy(
				pNewTileData + (i * (pStage->m_nMaxPage * GameDefaults::nPageTileWidth)),
				pStage->m_pTileData + (i * (pStage->m_nMaxPage * GameDefaults::nPageTileWidth)),
				nRowSize * sizeof(BYTE)
			);
		}

		// Right Pages
		nRowSize = (pStage->m_nMaxPage - nCurPage - 1) * GameDefaults::nPageTileWidth;
		memcpy(
			pNewTileData + nNextPageTileX + (i * (pStage->m_nMaxPage * GameDefaults::nPageTileWidth)),
			pStage->m_pTileData + nCurPageTileX + (i * (pStage->m_nMaxPage * GameDefaults::nPageTileWidth)),
			nRowSize * sizeof(BYTE)
		);
	}

	delete[] pStage->m_pTileData;

	pStage->m_pTileData = pNewTileData;

	// Move Items
	{
		int nCurCX = pStage->m_nMaxPage * GameDefaults::nPageTileWidth;
		std::map<int, int> mapItemNew;

		// Copy
		std::map<int, int>::iterator it = pStage->m_mapItem.begin();
		for (; it != pStage->m_mapItem.end(); ++it)
		{
			int nCurIdx = it->first;
			int nCurX = nCurIdx % nCurCX;
			int nCurY = nCurIdx / nCurCX;
			if (nCurX > nCurPageTileX)
				nCurX += GameDefaults::nPageTileWidth;

			int nNewIdx = pStage->GetTileIndex(nCurX, nCurY);
			mapItemNew.insert(
				std::pair<int, int>(nNewIdx, it->second)
			);
		}

		pStage->m_mapItem.clear();

		// Paste
		it = mapItemNew.begin();
		for (; it != mapItemNew.end(); ++it)
		{
			pStage->m_mapItem.insert(
				std::pair<int, int>(it->first, it->second)
			);
		}
	}

	// Move Objects
	{
		BEGIN_VECTOR_LOOP(MapObjectBase*, pStage->m_vecMapObject) {
			if (pObj->m_nX >= nCurPageTileX)
				pObj->m_nX += GameDefaults::nPageTileWidth;
		} END_VECTOR_LOOP();

		BEGIN_VECTOR_LOOP(GameObjectBase*, pStage->m_vecEnemy) {
			if (pObj->m_fX >= nCurPageTileX * TILE_XS)
				pObj->m_fX += GameDefaults::nPageWidth;
		} END_VECTOR_LOOP();

		BEGIN_VECTOR_LOOP(GameObjectBase*, pStage->m_vecEventObject) {
			if (pObj->m_fX >= nCurPageTileX * TILE_XS)
				pObj->m_fX += GameDefaults::nPageWidth;
		} END_VECTOR_LOOP();
	}

	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);

	RecalcScrollSize();

	Game *pGame = GetGame();
	if (pGame && pGame->m_pRenderManager)
	{
		pGame->m_pRenderManager->OnSize(cx, cy);

		CRect rcClient;
		GetClientRect(&rcClient);
		float fScaleX = (float)rcClient.Width() / (float)(pGame->m_nWidth);
		float fScaleY = (float)rcClient.Height() / (float)(pGame->m_nHeight);

		if (fScaleX == 0.f || fScaleY == 0.f)
			return;

		NaDebugOut(L"MapEditor's GameView Scale: %.2f, %.2f\n", fScaleX, fScaleY);
	}
}

void CMarioMapEditorView::OnCheckValid()
{
	DesignStage *pStage = GetStage();

	pStage->CheckValid();
	
	GetDocument()->SetModifiedFlag(TRUE);
}

void CMarioMapEditorView::OnToggleModernstyle()
{
	Game *pGame = GetGame();
	pGame->m_bModernStyle = !pGame->m_bModernStyle;
}

BOOL CMarioMapEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	DesignStage *pStage = GetStage();

	switch (pStage->m_nCurToolType)
	{
	case TOOLTYPE_ERASER:
		::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ERASER));
		return TRUE;
		break;
	default:
		if (IsPlaying())
		{
			// #TODO Movie slate cursor
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MARIO_HAND));
		}
		else
		{
			::SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MARIO_HAND));
		}
		return TRUE;
		break;
	}

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

void CMarioMapEditorView::OnSubstagesAddsubstage()
{
	DesignGame *pGame = (DesignGame*)GetGame();
	DesignStage *pStage = GetStage();

	pStage = (DesignStage*)pStage->GetTopStage();
	if (pStage->m_vecSubStage.size() > 4)
	{
		MessageBox(L"Already has 4 SubStages.");
		return;
	}

	pGame->m_pCurStage = pStage->CreateSubStage();

	Invalidate();
	InvalidateGame();
}

void CMarioMapEditorView::OnSubstagesMainstage()
{
	ActivateSubStage(-1);
}

void CMarioMapEditorView::OnSubstagesSubstage1()
{
	ActivateSubStage(0);
}

void CMarioMapEditorView::OnSubstagesSubstage2()
{
	ActivateSubStage(1);
}

void CMarioMapEditorView::OnSubstagesSubstage3()
{
	ActivateSubStage(2);
}

void CMarioMapEditorView::OnSubstagesSubstage4()
{
	ActivateSubStage(3);
}

void CMarioMapEditorView::OnQuickChangeProperty()
{
	QuickChangeProperty();
}

void CMarioMapEditorView::OnQuickPowerUp()
{
	QuickPowerUp();
}

void CMarioMapEditorView::OnQuickToggleWing()
{
	QuickToggleWing();
}

void CMarioMapEditorView::OnSelectAbilityFlags()
{
	DesignStage *pStage = GetStage();
	
	SelectAbilityDlg dlg;
	dlg.m_nAbilityFlag = pStage->m_nAbilityFlag;
	if (dlg.DoModal() == IDOK)
	{
		pStage->m_nAbilityFlag = dlg.m_nAbilityFlag;
	}

	RefreshProperties();
}


void CMarioMapEditorView::OnFileUpload()
{
	NaDebugOut(L"CMarioMapEditorView::OnFileUpload\n");

	auto pDoc = GetDocument();
	if (pDoc->IsModified() || pDoc->m_strPathName.GetLength() == 0)
	{
		if (MessageBox(L"You must save before upload. proceed?", L"Confirm", MB_OKCANCEL) == IDCANCEL)
			return;

		if (pDoc->m_strPathName.GetLength() == 0)
			pDoc->DoSave(NULL);
		else
			pDoc->DoFileSave();
	}

	if (IsPlaying())
		StopPlaying();

	auto pGame = GetGame();
	DesignStage *pStage = (DesignStage*)(GetStage()->GetTopStage());
	CWaitCursor WaitCursor;
	ServerRequest req;

	// Add logs to stage overlay
	NaString str;
	CMarioMapEditorView *pThis = this;
	auto AddLog = [&pThis, &pStage](const wchar_t* str)
	{
		pStage->m_strTransaction += str; 
		pThis->InvalidateGame();
	};

	auto ClearLog = [&pThis, &pStage]()
	{
		pStage->m_strTransaction = L"";
		pThis->InvalidateGame();
	};

	// Check permission
	AddLog(L"Check user information...\n");

	auto pConfig = GameConfig::GetGameConfig(pGame);
	if (pConfig->m_strName.GetLength() == 0)
	{
		MessageBox(L"You didn't set your name.", L"Cannot upload stage", MB_OK | MB_ICONERROR);
		ClearLog();
		return;
	}

	AddLog(L"Check stage permission...\n");

	if (pConfig->m_strName.CompareNoCase(pStage->m_strCreator) != 0)
	{
		MessageBox(L"You don't seem to be the creator of this stage.", L"Cannot upload stage", MB_OK | MB_ICONERROR);
		ClearLog();
		return;
	}

	// Check server status
	ServerInfo *pServerInfo = nullptr;
	try
	{
		pServerInfo = req.GetServerInfo();
		if (pServerInfo == nullptr)
			throw NaException(__FUNCTIONW__, __LINE__, "Cannot get server information");

#if defined(NDEBUG)
		if (pServerInfo->m_nStatus != 1)
			throw NaException(__FUNCTIONW__, __LINE__, "The server is under maintenance break");

		if (pServerInfo->m_nVersion < ServerRequest::s_nCurrentVersion)
			throw NaException(__FUNCTIONW__, __LINE__, "The server is being updated");

		if (pServerInfo->m_nVersion > ServerRequest::s_nCurrentVersion)
			throw NaException(__FUNCTIONW__, __LINE__, "Game update required");

		if (pServerInfo->m_nCanUploadStage == 0)
			throw NaException(__FUNCTIONW__, __LINE__, "Upload stage does not allowed now");
#endif
	}
	catch (NaException &e)
	{
		SAFE_DELETE(pServerInfo);
		MessageBox(NaString(e.m_szWhat).wstr(), L"Error :(", MB_OK | MB_ICONERROR);
		ClearLog();
		return;
	}

	// Register user
	ServerUserInfo *pUserInfo = nullptr;
	if (pConfig->m_strUniqueId.GetLength() == 0)
	{
		MessageBox(L"Unregistered user.\nRun the game first and proceed with the registration process.", L"Error :(", MB_OK | MB_ICONERROR);
		ClearLog();
		return;

		/*
		try
		{
			str.Format(L"Registering user: %ls ...\n", pConfig->m_strName.wstr());
			AddLog(str.wstr());

			if (pServerInfo->m_nCanRegisterUser == 0)
				throw NaException(__FUNCTIONW__, __LINE__, "Registering user does not allowed now");

			pUserInfo = req.RegisterUser(pConfig->m_strName);
			auto strUniqueId = pUserInfo->m_strUniqueId;
			if (strUniqueId.GetLength() == 0)
			{
				SAFE_DELETE(pUserInfo);
				throw NaException(__FUNCTIONW__, __LINE__, L"Registering failed");
			}

			pConfig->m_strUniqueId = strUniqueId;
			pConfig->Save();
		}
		catch (NaException &e)
		{
			SAFE_DELETE(pUserInfo);
			MessageBox(L"Registering user failed", L"Error :(", MB_OK | MB_ICONERROR);
			ClearLog();
			return;
		}
		*/
	}
	else
	{
		try
		{
			AddLog(L"Check upload permission...\n");

			pUserInfo = req.GetUserInfo(pConfig->m_strUniqueId);
			if (pUserInfo->m_nPermission < 1)
			{
				SAFE_DELETE(pUserInfo);
				MessageBox(L"You don't have permission to upload", L"Error :(", MB_OK | MB_ICONERROR);
				ClearLog();
				return;
			}
		}
		catch (NaException &e)
		{
			SAFE_DELETE(pUserInfo);
			MessageBox(L"Cannot confirm user information", L"Error :(", MB_OK | MB_ICONERROR);
			ClearLog();
			return;
		}
	}

	if (pUserInfo->m_strSecureText.GetLength() == 0)
	{
		QuestionDlg dlg;
		dlg.m_strQuestion = 
			L"You seem to be a new user, and you'll have editing rights "
			L"for the stage you uploaded in the future. However, if you "
			L"delete the UserData folder, you can not modify the stage "
			L"you uploaded.\n\n"
			L"In case you have a problem with your account, you need to ENTER THE SECURITY TEXT. (ex: Your real name in English)\n\n"
			L"This is NOT A PASSWORD but that's what you need when you lose your account."
			;

		if (dlg.DoModal() == IDOK)
		{
			pUserInfo->m_strSecureText = dlg.m_strAnswer;

			try
			{
				req.UpdateUserInfo(pUserInfo, UPDATE_USERINFO_SECURETEXT);
			}
			catch (NaException &e)
			{
				SAFE_DELETE(pUserInfo);
				MessageBox(NaString(e.m_szWhat).wstr(), L"Error :(", MB_OK | MB_ICONERROR);
				ClearLog();
				return;
			}
		}
	}

	NaDebugOut(L"> StageUID: %ls\n", pStage->m_strUniqueId);
	NaDebugOut(L"> StageREV: %d\n", pStage->m_nRevision);
	NaDebugOut(L"> CreatorUID: %ls\n", pConfig->m_strUniqueId);

	if (pStage->m_strTitle.GetLength() == 0)
	{
		AddLog(L"Use filename as stage title...\n");

		auto ar = pDoc->m_strPathName.Split(L"\\");
		pStage->m_strTitle = ar[ar.GetCount() - 1];
		pStage->m_strTitle = pStage->m_strTitle.Left(pStage->m_strTitle.GetLength() - 4);
	}

	try
	{
		AddLog(L"Checking if an update is required...\n");

		bool bUpdate = req.CanUpdate(pStage, pConfig->m_strUniqueId);
		if (!bUpdate)
		{
			AddLog(L"Upload stage...\n");
			req.UploadStage(pStage);
		}
		else
		{
			str.Format(L"Update stage (Version: %d -> %d)...\n",
				pStage->m_nRevision, pStage->m_nRevision + 1);
			AddLog(str.wstr());

			req.UpdateStage(pStage);
		}

		AddLog(L"Update stage metainfo...\n");

		auto pMeta = req.UpdateMetaInfo(pStage, pConfig->m_strUniqueId);

		CreatorCommentDlg dlg;
		dlg.m_strComment = pMeta->m_strCreatorComment.cstr();
		if (dlg.DoModal() == IDOK)
		{
			if (dlg.m_strComment.Compare(pMeta->m_strCreatorComment.wstr()) != 0)
			{
				AddLog(L"Update creator's comment...\n");

				pMeta->m_strCreatorComment = dlg.m_strComment;
				req.UpdateMetaInfo(pMeta, UPDATE_METAINFO_CREATORCOMMENT);
			}
			else
			{
				AddLog(L"Pass update creator's comment (no change)...\n");
			}
		}
		else
		{
			AddLog(L"Pass update creator's comment (canceled)...\n");
		}

		delete pMeta;
		pMeta = nullptr;
	}
	catch (NaException &e)
	{
		SAFE_DELETE(pUserInfo);
		MessageBox(NaString(e.m_szWhat).wstr(), L"Error :(", MB_OK | MB_ICONERROR);
		ClearLog();
		return;
	}

	try
	{
		NaString strDataTime;
		CTime t = CTime::GetCurrentTime();
		strDataTime.Format(L"%04d%02d%02d_%02d%02d%02d",
			t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond()
		);

		pUserInfo->m_strLastUploaded = strDataTime;
		req.UpdateUserInfo(pUserInfo, UPDATE_USERINFO_LASTUPLOADED);
	}
	catch (NaException &e)
	{
		SAFE_DELETE(pUserInfo);
		MessageBox(NaString(e.m_szWhat).wstr(), L"Error :(", MB_OK | MB_ICONERROR);
		ClearLog();
		return;
	}

	SAFE_DELETE(pUserInfo);

	// Auto save (uuid, revision)
	AddLog(L"Update local stage file...\n");

	pDoc->DoFileSave();
	AddLog(L"Done.\n");

	RefreshProperties();
	MessageBox(L"Stage upload completed!", L"Congratulations", MB_OK);

	ClearLog();
}


void CMarioMapEditorView::OnEditCopy()
{
	DesignStage *pStage = GetStage();
	
	if (pStage->m_nCurMode != EDIT_SELECTED)
		return;

	bool bCopy = false;
	Json::Value jObj;
	switch (pStage->m_nCurToolType)
	{
	case TOOLTYPE_PICK:
	case TOOLTYPE_ERASER:
	case TOOLTYPE_PLAYBOT:
		break;
	case TOOLTYPE_TILE:
		{
			int nIdx = pStage->GetTileIndex(pStage->m_ptTileSelected.x, pStage->m_ptTileSelected.y);
			int nData = pStage->GetTileData(pStage->m_ptTileSelected.x, pStage->m_ptTileSelected.y);
			if (nData != TILE_EMPTY)
			{
				jObj["X"] = pStage->m_ptTileSelected.x;
				jObj["Y"] = pStage->m_ptTileSelected.y;
				jObj["Type"] = nData;
				bCopy = true;
			}
		}
		break;
	case TOOLTYPE_ITEM:
		{
			int nIdx = pStage->GetTileIndex(pStage->m_ptTileSelected.x, pStage->m_ptTileSelected.y);
			std::map<int, int>::iterator it = pStage->m_mapItem.find(nIdx);
			if (it != pStage->m_mapItem.end())
			{
				jObj["X"] = pStage->m_ptTileSelected.x;
				jObj["Y"] = pStage->m_ptTileSelected.y;
				jObj["Type"] = pStage->m_nCurType;
				//JsonManager::SaveJsonObjectProperty(pObj, jObj);
				bCopy = true;
			}
		}
		break;
	case TOOLTYPE_MAPOBJECT:
		{
			auto pObj = pStage->m_pMapObject;

			jObj["X"] = pObj->m_nX;
			jObj["Y"] = pObj->m_nY;
			jObj["Type"] = pObj->m_nType;
			JsonManager::SaveJsonObjectProperty(pObj, jObj);
			bCopy = true;
		}
		break;
	case TOOLTYPE_ENEMY:
	case TOOLTYPE_EVENTOBJECT:
	case TOOLTYPE_VEHICLE:
		{
			auto pObj = pStage->GetSelectedObject();

			jObj["X"] = pObj->m_fX;
			jObj["Y"] = pObj->m_fY;
			jObj["Type"] = pObj->m_nType;
			JsonManager::SaveJsonObjectProperty(pObj, jObj);
			bCopy = true;
		}
		break;
	}

	if (bCopy)
	{
		std::string stdstr = jObj.toStyledString();

		NaString str = stdstr.c_str();
		if (!OpenClipboard())
			return;

		EmptyClipboard();
		NaDebugOut(L"Copy Clipboard:\n%ls\n", str.wstr());

		HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(wchar_t*) * str.GetLength());
		wchar_t *pchData = (wchar_t*)GlobalLock(hClipboardData);
		memcpy(pchData, str.wstr(), sizeof(wchar_t*) * str.GetLength());

		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_UNICODETEXT, hClipboardData);
		CloseClipboard();

		//GetGame()->m_pFMod->PlaySound(L"Coin");
	}
}


void CMarioMapEditorView::OnEditPaste()
{
	if (!OpenClipboard())
		return;

	HANDLE hData = GetClipboardData(CF_UNICODETEXT);
	if (hData == nullptr)
		return;

	// Lock the handle to get the actual text pointer
	wchar_t* pszText = static_cast<wchar_t*>(GlobalLock(hData));
	if (pszText == nullptr)
		return;

	// Save text in a string class instance
	NaString str = pszText;

	// Release the lock
	GlobalUnlock(hData);

	// Release the clipboard
	CloseClipboard();

	NaDebugOut(L"Paste Clipboard:\n%ls\n", str.wstr());

	Json::Value *jObj = new Json::Value;
	Json::Reader jReader;
	bool bParseJson = jReader.parse(str.cstr(), *jObj);
	while (bParseJson)
	{
		DesignStage *pStage = GetStage();
		int nType = (*jObj)["Type"].asInt();
		if (nType >= 0 && nType <= 255)
		{
			int nX = (*jObj)["X"].asInt();
			int nY = (*jObj)["Y"].asInt();

			// Offset position & Create
			for (; ; )
			{
				int nTileIdx = pStage->GetTileIndex(nX, nY);
				if (pStage->GetTileData(nX, nY) == nType)
					nX++;
				else
				{
					pStage->SetTileData(nX, nY, nType);
					break;
				}
			}

			// Select
			pStage->m_nCurToolType = TOOLTYPE_TILE;
			pStage->m_nCurType = nType;
			pStage->m_ptTileSelected = { nX, nY };
		}
		else if (nType >= ITEM_BEGIN && nType <= ITEM_END)
		{
			int nX = (*jObj)["X"].asInt();
			int nY = (*jObj)["Y"].asInt();

			// Offset position & Create
			for (; ; )
			{
				int nTileIdx = pStage->GetTileIndex(nX, nY);
				if (pStage->GetItem(nX, nY) == nType)
					nX++;
				else
				{
					pStage->SetItem(nX, nY, nType);
					break;
				}
			}

			// Select
			pStage->m_nCurToolType = TOOLTYPE_ITEM;
			pStage->m_nCurType = nType;
			pStage->m_ptTileSelected = { nX, nY };
		}
		else if (nType >= MAPOBJECT_BEGIN && nType <= MAPOBJECT_END)
		{
			// Create & Load Property
			auto pObj = pStage->AddMapObject(
				(*jObj)["X"].asInt() / TILE_XS,
				(*jObj)["Y"].asInt() / TILE_YS,
				nType);
			if (pObj == nullptr)
				break;

			JsonManager::LoadJsonObjectProperty(pObj, *jObj);

			// Offset position
			NaRect rc = pObj->GetRect();
			int nX = rc.Center();
			int nY = rc.Middle();
			for (; ; )
			{
				if (pStage->HitTest(nX, nY, TOOLTYPE_MAPOBJECT) == nullptr)
					break;
				nX += TILE_XS;
			}

			pObj->m_nX = nX;
			pObj->m_nTX = nX / TILE_XS;

			pStage->m_pMapObject = pObj;
		}
		else if ((nType >= ENEMY_BEGIN && nType <= ENEMY_END) ||
			(nType >= EVENTOBJECT_BEGIN && nType <= EVENTOBJECT_END) ||
			(nType >= VEHICLE_BEGIN && nType <= VEHICLE_END))
		{
			// Create & Load Property
			auto pObj = pStage->ReserveCreateGameObject(
				(*jObj)["X"].asInt(),
				(*jObj)["Y"].asInt(),
				nType);
			if (pObj == nullptr)
				break;
			pObj->LoadProperty(*jObj);

			// Offset position
			for (; ; )
			{
				auto rc = pObj->GetRect();
				if (pStage->HitTest(rc.Center(), rc.Middle()) == nullptr)
					break;
				pObj->m_fX += TILE_XS;
			}

			// Move to its group
			std::vector<GameObjectBase*>::iterator it;
			it = pStage->m_vecReservedGameObject.begin();
			for (; it != pStage->m_vecReservedGameObject.end(); )
			{
				GameObjectBase *pObj = *it;
				if (pObj->m_nType >= ITEM_BEGIN && pObj->m_nType <= ITEM_END)
					pStage->m_vecItem.push_back(pObj);
				else if (pObj->m_nType >= EFFECT_BEGIN && pObj->m_nType <= EFFECT_END)
					pStage->m_vecEffect.push_back(pObj);
				else if (pObj->m_nType >= ENEMY_BEGIN && pObj->m_nType <= ENEMY_END)
					pStage->m_vecEnemy.push_back(pObj);
				else if (pObj->m_nType >= EVENTOBJECT_BEGIN && pObj->m_nType <= EVENTOBJECT_END)
					pStage->m_vecEventObject.push_back(pObj);
				else if (pObj->m_nType >= VEHICLE_BEGIN && pObj->m_nType <= VEHICLE_END)
					pStage->m_vecVehicle.push_back(pObj);

				it = pStage->m_vecReservedGameObject.erase(it);
			}

			// Select
			if (IS_ENEMY(pObj))
				pStage->m_pEnemyObject = pObj;
			else if (IS_EVENTOBJECT(pObj))
				pStage->m_pEventObject = pObj;
			else if (IS_VEHICLE(pObj))
				pStage->m_pVehicleObject = pObj;
		}

		Invalidate();
		RefreshProperties();

		//GetGame()->m_pFMod->PlaySound(L"1UP");
		break;
	}

	if (jObj)
		delete jObj;
}
