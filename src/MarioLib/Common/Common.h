#pragma once

#include "NaCommon.h"
#include "NaString.h"
#include "NaRect.h"
#include "NaPoint.h"

#include <stdlib.h>
#if defined(WIN32)
#define CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include <vector>
#include <math.h>

#include "Macros.h"

#pragma warning(disable:4244)

namespace GameWindowDefaults
{
	//const int nInnerWidth = 256;
	const int nInnerWidth = 240 / 9 * 16;
	const int nInnerHeight = 240;
#if !defined(NDEBUG)
	const int nZoomFactor = 2;
#else
	const int nZoomFactor = 3;
#endif
};

enum DamageGroups
{
	DAMAGEGROUP_PLAYER = 0,
	DAMAGEGROUP_ENEMY,
	DAMAGEGROUP_NEUTRAL,
};

// Not used
enum DamageTypes
{
	DAMAGE_UNKNOWN = 0,
	DAMAGE_PRESS,
	DAMAGE_FIREBALL,
};

enum PowerType
{
	POWERTYPE_HITBLOCK = 1,
	POWERTYPE_PRESS,
};

enum HitBlockPowerTypes
{
	POWER_HITBLOCK_UNKNOWN = 0,
	POWER_HITBLOCK_TINY,				// Can hit ItemBlock only
	POWER_HITBLOCK_NORMAL,				// Can hit SoftBlock but not destroyed
	POWER_HITBLOCK_BIGMARIO,			// Can destroy SoftBlock
	POWER_HITBLOCK_GIANTSHELL,			// Can destroy HardBlock
	POWER_HITBLOCK_GIANTMARIO,			// Can destroy SuperHardBlock
	POWER_HITBLOCK_GROWING_GIANTMARIO,	// Can destroy SuperHardBlock + JumpBlock
};

enum PressPowerTypes
{
	POWER_PRESS_UNKNOWN = 0,
	POWER_PRESS_TINY,
	POWER_PRESS_NORMAL,
	POWER_PRESS_GOOMBASHOES,
	POWER_PRESS_YOSHI,
	POWER_PRESS_THWOMP,
	POWER_PRESS_GIANTMARIO,
};

enum BurnTypes
{
	BURN_UNKNOWN = 0,
	BURN_FIREBALL,
	BURN_YOSHIFIRE,
	BURN_TURTLESHELL,
	BURN_BOWSERFIRE,
	BURN_BOBOMB,
	BURN_THWOMPSMASH,
	BURN_STARMAN,
	BURN_POWBLOCK,
};

enum EarthQuakePowerTypes
{
	POWER_EARTHQUAKE_UNKNOWN = 0,
	POWER_EARTHQUAKE_NORMAL,
	POWER_EARTHQUAKE_GIANTMARIOSTEP,
	POWER_EARTHQUAKE_GIANTMARIOJUMP,
	POWER_EARTHQUAKE_POWERBLOCK,
	POWER_EARTHQUAKE_SLEDGEBROTHERS,
};

enum PipeEntranceTypes
{
	PIPEENTRANCE_UNKNOWN = 0,
	PIPEENTRANCE_UPTODOWN = 1,
	PIPEENTRANCE_LEFTTORIGHT,
	PIPEENTRANCE_EXITONLY,
	PIPEENTRANCE_DOWNTOUP,
	PIPEENTRANCE_RIGHTTOLEFT,
};

enum PipeTypes
{
	PIPE_NONE = 0, // Cannot enter
	
	// Classic Types
	PIPE_WARP_TO_BONUSZONE,
	PIPE_WARP_TO_CURRENTSTAGE,
	PIPE_WARPZONE,

	// New
	PIPE_WARP_TO_SUBZONE,
};

enum StageEntranceTypes
{
	STAGEENTRANCE_NONE = 0,
	STAGEENTRANCE_UNDERGROUND,
	STAGEENTRANCE_FROMPIPE,
	STAGEENTRANCE_GROUNDTOSKY,
};

enum AutoScrollTypes
{
	AUTOSCROLL_NONE = 0,
	AUTOSCROLL_HORZ_SLOW,
	AUTOSCROLL_HORZ_NORMAL,
	AUTOSCROLL_HORZ_FAST,
	AUTOSCROLL_HORZ_GOD,
	AUTOSCROLL_VERT_SLOW,
	AUTOSCROLL_VERT_NORMAL,
	AUTOSCROLL_VERT_FAST,
	AUTOSCROLL_VERT_GOD,
};

enum SpecialEventTypes
{
	SPECIALEVENT_NONE = 0,
	SPECIALEVENT_GIANTMUSHROOM = 0x0001,
	SPECIALEVENT_GIANTPREVENTHOLE = 0x0002,
};

enum AbilityMasks
{
	ABILITY_NONE = 0,
	ABILITY_CARRYOBJECT = 0x0001,
	ABILITY_SPINJUMP	= 0x0002,
	ABILITY_AUTOACCEL	= 0x0004,
	ABILITY_AUTORUN		= 0x0008,
};

enum TextActionTypes
{
	TEXTACTION_NONE = 0,
	TEXTACTION_BLINK,
	TEXTACTION_MOVEUP,
};

namespace GameDefaults
{
	const float fFPS = 60.0f;
	const int nTileWidth = 16;
	const int nTileHeight = 16;
	const int nTilePerScreenWidth = 16;
	const int nTilePerScreenHeight = 13 + 3; // 3=invisible
	const int nMaxStagePage = 16;
	const float fDefaultGravity = 0.33f;
	const bool bFrameSkip = false;
	
	const int nWorldTileWidth = nTilePerScreenWidth * nMaxStagePage;
	const int nDefaultStopPointX = nTilePerScreenWidth * nMaxStagePage * nTileWidth;

	const int nPageTileWidth = 16;
	const int nPageTileHeight = 16; // last one is invisible

	const int nPageWidth = nTilePerScreenWidth * nTileWidth;
	const int nPageHeight = nTilePerScreenHeight * nTileHeight;

	const int nWorldWidth = nWorldTileWidth * nTileWidth;
	const int nMaxCameraPosX = nWorldWidth - nPageWidth;

	const POINT ptDefaultCheckPoint = { -1, -1 };

	const int nGiantTime = 24 * (50);
	const int nInvinsibleTime = 24 * (24 + 8);
	const int nDamageCooltime = 24 * (10);

	const int nBlockJumpFreeze = 7;

	const int nJsonEncryptKey[8] = { 19, 82, 3, 15, 18, 44, 66, 77 };
};

#define SZ_JSON_ENCRYPT_HEADER		"Neoarc's Json Encrypted"

namespace SpriteDefaults
{
	const DWORD dwColorKey = 0xff48ff00;
	const DWORD dwColorShadow = 0x70000000;
	
	const DWORD dwColorSkipFilter = 0x50ffffff;
};

enum CollisionSpot
{
	COLLISION_LEFT = 0,
	COLLISION_TOP,
	COLLISION_RIGHT,
	COLLISION_BOTTOM,
};

enum MarioShapes
{
	// From SMB1
	SHAPE_MINI = 1,
	SHAPE_NOTUSED1, // Not Used
	SHAPE_BIG,
	SHAPE_FIRE,

	// From SMM
	SHAPE_COSTUME,

	// From SMB3
	SHAPE_RACCOON,
	SHAPE_FROG,
	SHAPE_HAMMER,
	SHAPE_TANOOKI,

	// From SMW
	SHAPE_CAPE,

	// From NSMB
	SHAPE_SHELL,
	SHAPE_PENGUIN,
	SHAPE_GIANT,
};

enum InputTypes
{
	INPUT_L = 0,
	INPUT_R,
	INPUT_U,
	INPUT_D,
	INPUT_J,
	INPUT_F,
};

enum SpriteRenderZOrders
{
	Z_ZERO = 0,

	Z_FAR_BACKGROUND,

	Z_BACKGROUND,
	Z_DESIGN_EMPTYTILE,

	Z_MAP_SHADOW,

	Z_SEMISOLID_BACKGROUND,

	Z_IN_PIPE_VEHICLE_BEHIND_OBJ,
	Z_IN_PIPE_VEHICLE_BEHIND_OBJ_HEAD,
	Z_IN_PIPE_OBJ,
	Z_IN_PIPE_VEHICLE_ABOVE_OBJ,

	Z_POPPING_ITEM,
	Z_BEHIND_MAP,

	Z_MAP,
	Z_MAP_ANIMATING,
	Z_AFTER_MAP,

	Z_CASTLE_FLAG,
	Z_CASTLE,

	Z_DOOR,
	Z_FOLLOWING_DOORKEY,

	Z_VEHICLE_BEHIND_OBJ,
	Z_VEHICLE_BEHIND_OBJ_HEAD,

	Z_COMMON_OBJ,
	Z_ITEM,
	Z_BULLET_BEHIND_ENEMY,
	Z_ENEMY,

	Z_HELMET,
	Z_BULLET,
	Z_VEHICLE,

	Z_MAPOBJECT,
	Z_MAPOBJECT_TOP,

	Z_WARPZONE_TEXT,
	Z_EFFECT,

	Z_SPEECHBUBBLE_SHADOW,
	Z_SPEECHBUBBLE,
	Z_SPEECHBUBBLE_CONTENT_SHADOW,
	Z_SPEECHBUBBLE_CONTENT,

	Z_UI_TITLEBOX,

	Z_STATUS_SHADOW,
	Z_STATUS,

	Z_DESIGN_PLAYBOT,
	Z_DESIGN_INDICATOR,
	Z_DESIGN_ANCHOR,

	Z_UI_BOX_SHADOW,
	Z_UI_BOX,
	Z_UI_TEXT_SHADOW,
	Z_UI_TEXT,

	Z_UI_PANEL,
	Z_UI_LIST,
	Z_UI_BUTTON_BACKGROUND,
	Z_UI_BUTTON_TEXT,

	Z_UI_INDICATOR,

	Z_TEXT_SHADOW,
	Z_TEXT,

	Z_SCENE_EFFECT_SHADOW,
	Z_SCENE_EFFECT,
};

enum StagePackTypes
{
	STAGEPACK_CLASSIC = 0,
	STAGEPACK_CUSTOM,
	STAGEPACK_SINGLE,
};
