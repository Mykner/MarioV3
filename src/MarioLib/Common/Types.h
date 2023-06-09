#pragma once

enum TileDataTypes
{
	// Line1
	TILE_EMPTY = 0,
	TILE_BRICK,
	TILE_ITEM,
	TILE_HIDDENITEM,
	TILE_JUMP,
	TILE_TAKENITEM,
	TILE_HARDBRICK,
	TILE_X001, // COIN
	TILE_X002, // Treadmill
	TILE_X003, // Treadmill
	TILE_X004, // Treadmill
	TILE_HPIPEHEAD_T,
	TILE_HPIPEBODY_T,
	TILE_HPIPERIGHT_T,
	TILE_PIPEHEAD_L,
	TILE_PIPEHEAD_R,

	// Line2
	TILE_X005, // bg
	TILE_X006, // bg
	TILE_X007, // bg
	TILE_X008, // hiddenitem ....
	TILE_MESSAGE,
	TILE_TAKINGITEM,
	TILE_MUSHROOMBODY_LT, // island body lt
	TILE_MUSHROOMBODY_RT, // island body rt
	TILE_X013, // Treadmill
	TILE_X014, // Treadmill
	TILE_X015, // Treadmill
	TILE_HPIPEHEAD_B,
	TILE_HPIPEBODY_B,
	TILE_HPIPERIGHT_B,
	TILE_PIPEBODY_L,
	TILE_PIPEBODY_R,

	// Line3
	TILE_X016, // bridge lt
	TILE_BRIDGE_T,
	TILE_X017, // bridge rt
	TILE_MUSHROOMHEAD_L,
	TILE_MUSHROOMHEAD_C,
	TILE_MUSHROOMHEAD_R,
	TILE_MUSHROOMBODY_LB, // island body lb
	TILE_MUSHROOMBODY_RB, // island body rb
	TILE_X020, // Treadmill
	TILE_X021, // Treadmill
	TILE_X022, // Treadmill
	TILE_X023, // Treadmill
	TILE_X024, // Treadmill
	TILE_X025, // Treadmill
	TILE_PIPEBOTTOM_L,
	TILE_PIPEBOTTOM_R,

	// Line4
	TILE_BRIDGE_LB, // bridge lb
	TILE_BRIDGE_B,
	TILE_BRIDGE_RB, // bridge rb
	TILE_MUSHROOMHEAD2_L, // mushroom head yellow 
	TILE_MUSHROOMHEAD2_C, // mushroom head yellow 
	TILE_MUSHROOMHEAD2_R, // mushroom head yellow 
	TILE_MUSHROOMBODY_T,
	TILE_ISLANDHEAD_L,
	TILE_ISLANDHEAD_C,
	TILE_ISLANDHEAD_R,
	TILE_SEMI_SOLID_LT, // semi solid2 lt
	TILE_SEMI_SOLID_CT,
	TILE_SEMI_SOLID_RT, // semi solid2 rt
	TILE_SEMI_SOLID3_LT, // semi solid3 lt
	TILE_SEMI_SOLID3_CT, // semi solid3 ct
	TILE_SEMI_SOLID3_RT, // semi solid3 rt
	
	// Line5
	TILE_X039, // fall1
	TILE_X040, // fall2
	TILE_SPIKE,
	TILE_MUSHROOMHEAD3_L, // mushroom head green 
	TILE_MUSHROOMHEAD3_C, // mushroom head green 
	TILE_MUSHROOMHEAD3_R, // mushroom head green 
	TILE_MUSHROOMBODY_B,
	TILE_ISLANDBODY_L, // semi solid1 ltt
	TILE_ISLANDBODY_C,
	TILE_ISLANDBODY_R, // semi solid1 rtt
	TILE_SEMI_SOLID2_LTB, // semi solid2 ltt
	TILE_SEMI_SOLID2_CTB,
	TILE_SEMI_SOLID2_RTB, // semi solid2 rtt
	TILE_SEMI_SOLID3_LTB, // semi solid3 ltt
	TILE_SEMI_SOLID3_CTB, // semi solid3 ctt
	TILE_SEMI_SOLID3_RTB, // semi solid3 rtt

	// Line6
	TILE_X053, // debris1
	TILE_X054, // debris2
	TILE_X055, // debris3
	TILE_X056, // debris4
	TILE_SPIKE2,
	TILE_RIVER_T,
	TILE_X059, // pink jump
	TILE_SEMI_SOLID1_LBT, // semi solid1 lbt
	TILE_SEMI_SOLID1_CBT, // semi solid1 cbt
	TILE_SEMI_SOLID1_RBT, // semi solid1 rbt
	TILE_SEMI_SOLID2_LBT, // semi solid2 lbt
	TILE_SEMI_SOLID2_CBT, // semi solid2 cbt
	TILE_SEMI_SOLID2_RBT, // semi solid2 rbt
	TILE_SEMI_SOLID3_LBT, // semi solid3 lbt
	TILE_SEMI_SOLID3_CBT, // semi solid3 cbt
	TILE_SEMI_SOLID3_RBT, // semi solid3 rbt

	// Line7
	TILE_X069, // line
	TILE_X070, // line
	TILE_X071, // line
	TILE_X072, // line
	TILE_X073, // line
	TILE_RIVER_B,
	TILE_CLOUD,
	TILE_SEMI_SOLID1_LBB, // semi solid1 lbb
	TILE_SEMI_SOLID1_CBB, // semi solid1 cbb
	TILE_SEMI_SOLID1_RBB, // semi solid1 rbb
	TILE_SEMI_SOLID2_LBB, // semi solid2 lbb
	TILE_SEMI_SOLID2_CBB, // semi solid2 cbb
	TILE_SEMI_SOLID2_RBB, // semi solid2 rbb
	TILE_SEMI_SOLID3_LBB, // semi solid3 lbb
	TILE_SEMI_SOLID3_CBB, // semi solid3 cbb
	TILE_SEMI_SOLID3_RBB, // semi solid3 rbb

	// Line8
	TILE_CASTLE_HEAD2,
	TILE_CASTLE_HEAD1,
	TILE_CASTLE_BRICK,
	TILE_RIVER_T2,
	TILE_RIVER_T3,
	TILE_RIVER_T4,
	TILE_RIVER_T5,
	TILE_CANNONHEAD_T,
	TILE_X093, // ice
	TILE_GROUND, // TILE_X094, // normal ct
	TILE_X095, // normal rt
	TILE_X096, // normal lt
	TILE_X097, // normal ct
	TILE_X098, // vine b
	TILE_X099, // vine c
	TILE_X100, // vine t

	// Line9
	TILE_CASTLE_DOOR_T,
	TILE_CASTLE_WINDOW1,
	TILE_CASTLE_WINDOW2,
	TILE_X104, // line
	TILE_X105, // line
	TILE_X106, // line
	TILE_X107, // line
	TILE_CANNONHEAD_B,
	TILE_X109, // ice debris
	TILE_X110, // normal cb
	TILE_X111, // normal rb
	TILE_X112, // normal lb
	TILE_X113, // normal cb
	TILE_X114, // bush
	TILE_X115, // bush
	TILE_PIPEJUNCTION_T,

	// Line10
	TILE_CASTLE_DOOR_B,
	TILE_VERTICAL_LINE,
	TILE_X119, // line
	TILE_X120, // line
	TILE_X121, // line
	TILE_X122, // line
	TILE_X123, // line
	TILE_CANNONBODY,
	TILE_X125, // line
	TILE_X126, // line
	TILE_X127, // line
	TILE_X128, // line
	TILE_X129, // line
	TILE_X130, // line
	TILE_X131, // line
	TILE_PIPEJUNCTION_B,

	// Line11
	TILE_ITEM1, // item ani1
	TILE_ITEM2, // item ani2
	TILE_ITEM3, // item ani3
	TILE_ITEM4, // item ani4
	TILE_JUMP1, // jump ani1
	TILE_JUMP2, // jump ani2
	TILE_JUMP3, // jump ani3
	TILE_JUMP4, // jump ani4
	TILE_SUPERJUMP1, // pink jump ani1
	TILE_SUPERJUMP2, // pink jump ani2
	TILE_SUPERJUMP3, // pink jump ani3
	TILE_SUPERJUMP4, // pink jump ani4
	TILE_BRICK1, // brick ani1
	TILE_BRICK2, // brick ani2
	TILE_BRICK3, // brick ani3
	TILE_BRICK4, // brick ani4

	// Line12
	TILE_POW, // POW block (old=line)
	TILE_X150, // line
	TILE_X151, // line
	TILE_X152, // line
	TILE_X153, // line
	TILE_X154, // line
	TILE_X155, // line
	TILE_X156, // line
	TILE_X157, // ground lt
	TILE_X158, // ground rt // 185
	TILE_X159, // ground l
	TILE_X160, // ground r
	TILE_X161, // ground lb
	TILE_X162, // ground rb
	TILE_X163, // ground tb // 190
	TILE_X164, // ground tb

	// Line13
	TILE_X165, // ground tb
	TILE_X166, // ground t
	TILE_X167, // ground t
	TILE_X168, // ground t
	TILE_X169, // ground c
	TILE_X170, // ground c
	TILE_X171, // ground c
	TILE_X172, // ground b
	TILE_X173, // ground b // 200
	TILE_X174, // ground b
	TILE_X175, // ground l
	TILE_X176, // ground l
	TILE_X177, // ground l
	TILE_X178, // ground r
	TILE_X179, // ground r
	TILE_X180, // ground r

	// Line14
	TILE_X181, // ground ltrb /
	TILE_X182, // ground ltb /
	TILE_X183, // ground tb
	TILE_X184, // ground rtb
	TILE_X185, // ground ltr
	TILE_X186, // ground lr
	TILE_X187, // ground lrb
	TILE_X188, // ground rb  ��
	TILE_X189, // ground lb ��
	TILE_X190, // ground rt
	TILE_X191, // ground lr
	TILE_X192, // ground b ��
	TILE_X193, // ground t
	TILE_X194, // ground r ��
	TILE_X195, // ground l ��
	TILE_X196, // ground c ��

	// Line15
	TILE_X197, // ground l ��
	TILE_X198, // ground r ��
	TILE_X199, // ground l
	TILE_X200, // ground r
	TILE_X201, // ground t
	TILE_X202, // ground t

	TILE_X203, // ground b xoo
				//          ooo

	TILE_X204, // ground b oox
				//          ooo

	TILE_X205, // ground c xox
				//          ooo

	TILE_X206, // ground c xox
				//          ooo

	TILE_X207, // ground c xoo
				//          ooo

	TILE_X208, // ground c oox
				//          ooo

	TILE_X209, // ground c xox
				//          ooo

	TILE_X210, // ground c

	TILE_X211, // ground c xoo
				//          ooo

	TILE_X222, // ground c oox
				//          ooo

	// Line16
	TILE_X223, // ground ?
	TILE_X224, // ground ?
	TILE_X225, // ground lt
	TILE_X226, // ground t
	TILE_X227, // ground rt
	TILE_X228, // ground l
	TILE_X229, // ground c
	TILE_X230, // ground r
	TILE_X231, // ground lb
	TILE_X232, // ground b
	TILE_X233, // ground rb
	TILE_X234, // ground ?
	TILE_X235, // ground ?
	TILE_X236, // ground ?
	TILE_X237, // ground ?

	TILE_X238, // boss bridge
};

// TileDataTypes v0.9 - for migration
enum TileDataTypesV0_9
{
	TILE_V0_9_EMPTY = 0,
	TILE_V0_9_GROUND,
	TILE_V0_9_BRICK,
	TILE_V0_9_ITEM,
	TILE_V0_9_TAKENITEM,
	TILE_V0_9_PIPEHEAD_L,
	TILE_V0_9_PIPEHEAD_R,
	TILE_V0_9_PIPEBODY_L,
	TILE_V0_9_PIPEBODY_R,
	TILE_V0_9_HARDBRICK,
	TILE_V0_9_ISLANDHEAD_L,
	TILE_V0_9_ISLANDHEAD_C,
	TILE_V0_9_ISLANDHEAD_R,
	TILE_V0_9_ISLANDBODY,
	TILE_V0_9_MUSHROOMHEAD_L,
	TILE_V0_9_MUSHROOMHEAD_C,
	TILE_V0_9_MUSHROOMHEAD_R,
	TILE_V0_9_MUSHROOMBODY_T,
	TILE_V0_9_MUSHROOMBODY_B,
	TILE_V0_9_CLOUD,
	TILE_V0_9_BRIDGE_T,
	TILE_V0_9_BRIDGE_B,
	TILE_V0_9_CANNONHEAD_T,
	TILE_V0_9_CANNONHEAD_B,
	TILE_V0_9_CANNONBODY,
	TILE_V0_9_HPIPEHEAD_T,
	TILE_V0_9_HPIPEHEAD_B,
	TILE_V0_9_HPIPEBODY_T,
	TILE_V0_9_HPIPEBODY_B,
	TILE_V0_9_PIPEJUNCTION_T,
	TILE_V0_9_PIPEJUNCTION_B,
	TILE_V0_9_ITEM2,
	TILE_V0_9_ITEM3,
	TILE_V0_9_CASTLE_HEAD1,
	TILE_V0_9_CASTLE_HEAD2,
	TILE_V0_9_CASTLE_WINDOW1,
	TILE_V0_9_CASTLE_BRICK,
	TILE_V0_9_CASTLE_WINDOW2,
	TILE_V0_9_CASTLE_DOOR_T,
	TILE_V0_9_CASTLE_DOOR_B,
	TILE_V0_9_RIVER_T,
	TILE_V0_9_RIVER_B,
	TILE_V0_9_HIDDENITEM,
	TILE_V0_9_TAKINGITEM,
	TILE_V0_9_VERTICAL_LINE,
	TILE_V0_9_SPIKE,
	TILE_V0_9_JUMP,
	TILE_V0_9_SEMI_SOLID_T,
	TILE_V0_9_SEMI_SOLID_B,
	TILE_V0_9_SPIKE2,
	TILE_V0_9_RIVER_T2,
	TILE_V0_9_RIVER_T3,
	TILE_V0_9_RIVER_T4,
	TILE_V0_9_RIVER_T5,
	TILE_V0_9_PIPEBOTTOM_L,
	TILE_V0_9_PIPEBOTTOM_R,
	TILE_V0_9_HPIPERIGHT_T,
	TILE_V0_9_HPIPERIGHT_B,
	TILE_V0_9_MESSAGE,
};

enum TileTypes
{
	TILETYPE_UNKNOWN = -1,
	TILETYPE_EMPTY = 0,
	TILETYPE_SOFTHARD = 1,	// Brick
	TILETYPE_HARD,			// Giant enemies can destroy
	TILETYPE_SUPERHARD,		// Cannot destroy (Only Giant Mario can destroy)
	TILETYPE_ITEM,

	TILETYPE_DAMAGE,
	TILETYPE_JUMP,
};

enum TileBackgroundTypes
{
	TILEBG_EMPTY = 0,
	TILEBG_BUSH_L,
	TILEBG_BUSH_C,
	TILEBG_BUSH_R,
	TILEBG_MOUNTAIN_L,
	TILEBG_MOUNTAIN_C,
	TILEBG_MOUNTAIN_TREE,
	TILEBG_MOUNTAIN_R,
	TILEBG_MOUNTAIN_T,
	TILEBG_CLOUD_LT,
	TILEBG_CLOUD_T,
	TILEBG_CLOUD_RT,
	TILEBG_CLOUD_LB,
	TILEBG_CLOUD_B,
	TILEBG_CLOUD_RB,
	TILEBG_WATER_SKY,
	TILEBG_WATER_T,
	TILEBG_TREE_T,
	TILEBG_TREE_C,
	TILEBG_TREE_B,
	TILEBG_FENCE,
	TILEBG_TREE_T1,
	TILEBG_SNOWTREE_T,
	TILEBG_SNOWTREE_C,
	TILEBG_SNOWTREE_T1,
	TILEBG_REDCLOUD_LT,
	TILEBG_REDCLOUD_T,
	TILEBG_REDCLOUD_RT,
	TILEBG_REDCLOUD_LB,
	TILEBG_REDCLOUD_B,
	TILEBG_REDCLOUD_RB,
	TILEBG_WATER_T2,
	TILEBG_WATER_T3,
	TILEBG_WATER_T4,
	TILEBG_WATER_T5,
	TILEBG_DEADMUSHROOM,
	TILEBG_DEADBUSH_R,
	TILEBG_DEADBUSH_LR,
	TILEBG_DEADBUSH_BODY,
	TILEBG_DEADBONE_L,
	TILEBG_DEADBONE_C,
	TILEBG_DEADBONE_R,
	TILEBG_BOWSERJR_STATUE_T,
	TILEBG_BOWSERJR_STATUE_B,
	TILEBG_BOWSER_STATUE_T,
	TILEBG_BOWSER_STATUE_B,
	TILEBG_BOWSER_STATUE_BODY,
	TILEBG_CASTLE_FENCE_L,
	TILEBG_CASTLE_FENCE_C,
	TILEBG_CASTLE_FENCE_R,

	TILEBG_GHOSTHOUSE_DEADMUSHROOM,
	TILEBG_GHOSTHOUSE_LAMP_T,
	TILEBG_GHOSTHOUSE_LAMP_B,
	TILEBG_GHOSTHOUSE_CLOCK_T,
	TILEBG_GHOSTHOUSE_CLOCK_C,
	TILEBG_GHOSTHOUSE_CLOCK_B,
	TILEBG_AIRSHIP_MINIBOLT,
	TILEBG_AIRSHIP_MINIPIPE_T,
	TILEBG_AIRSHIP_MINIPIPE_B,
	TILEBG_AIRSHIP_MINIFLAG_T,
	TILEBG_AIRSHIP_MINIFLAG_C,
	TILEBG_AIRSHIP_MINIFLAG_B,

	TILEBG_DARKAREA_SEAM,
	TILEBG_DARKAREA_MOUNTAIN_LT,
	TILEBG_DARKAREA_MOUNTAIN_RT,
	TILEBG_DARKAREA_MOUNTAIN_L,
	TILEBG_DARKAREA_MOUNTAIN_R,
	TILEBG_DARKAREA_MOUNTAIN_LB,
	TILEBG_DARKAREA_MOUNTAIN_LBB,
	TILEBG_DARKAREA_MOUNTAIN_RB,
	TILEBG_DARKAREA_MOUNTAIN_RBB,
};

enum MapObjectTypes
{
	MAPOBJECT_NONE = 1000,
	MAPOBJECT_BEGIN = 1000,

	MAPOBJECT_PIPE = 1001,
	MAPOBJECT_GOAL,
	MAPOBJECT_STARTPOINT,
	MAPOBJECT_REMOVED_OBJ_1,
	MAPOBJECT_REMOVED_OBJ_2,
	MAPOBJECT_PIPEEXIT,
	MAPOBJECT_REMOVED_OBJ_3,
	MAPOBJECT_CHECKPOINT,
	MAPOBJECT_PIPEHORZ,
	MAPOBJECT_PIPEBODY,
	MAPOBJECT_CAMERASTOP,
	MAPOBJECT_REMOVED_OBJ_4,
	MAPOBJECT_ISLAND,
	MAPOBJECT_RIVER,
	MAPOBJECT_SKYEXIT,
	MAPOBJECT_RIVERBRIDGE,
	MAPOBJECT_ENEMYSTOP,
	MAPOBJECT_BILLBLASTER,
	MAPOBJECT_MUSHROOMISLAND,
	MAPOBJECT_WALL,
	MAPOBJECT_SEMISOLID,
	MAPOBJECT_MESSAGEBLOCK,

	MAPOBJECT_END = 1999,
};

enum ItemTypes
{
	ITEM_NONE = 2000,
	ITEM_BEGIN = 2000,

	ITEM_COIN = 2001,
	ITEM_MUSHROOM,
	ITEM_1UP,
	ITEM_FIREFLOWER,
	ITEM_SUPERSTAR,
	ITEM_COINS1,
	ITEM_COINS2,
	ITEM_COINS3,
	ITEM_COINS4,
	ITEM_COINS5,
	ITEM_COINS6,
	ITEM_COINS7,
	ITEM_COINS8,
	ITEM_COINS9,
	ITEM_COINS10,
	ITEM_COINS11,
	ITEM_COINS = ITEM_COINS11,
	ITEM_VINE,

	// New Items
	ITEM_GIANTMUSHROOM,
	ITEM_YOSHIEGG,
	ITEM_CLOUD,
	ITEM_TURTLESHELL,
	ITEM_LEAF,

	ITEM_THROWNCOIN,	// Created by YoshiFire, etc.
	
	ITEM_DOORKEYCOIN,
	ITEM_DOORKEY,

	// For costume (x160)
	ITEM_MYSTERYMUSHROOM = 2800,
	ITEM_MYSTERYMUSHROOM_RESERVED_LAST = 2960,

	ITEM_END = 2999,
};

enum EnemyTypes
{
	ENEMY_NONE = 3000,
	ENEMY_BEGIN = 3000,

	ENEMY_GOOMBA = 3001,
	ENEMY_TURTLE, // Koopa Troopa
	ENEMY_PIRANHA_PLANT, // Piranha Plant
	ENEMY_REDTURTLE,
	ENEMY_REDTURTLE_WINGED_PATROL,
	ENEMY_FIREBAR,
	ENEMY_PODOBOO, // Jumping Magma
	ENEMY_BOWSER,
	ENEMY_TURTLE_WINGED,
	ENEMY_BLOOBER,
	ENEMY_CHEEPCHEEP,
	ENEMY_REDCHEEPCHEEP,
	ENEMY_CHEEPCHEEP_FLY,
	ENEMY_REDCHEEPCHEEP_FLY,
	ENEMY_HAMMERBROTHERS,
	ENEMY_LAKITU,
	ENEMY_BUZZYBEETLE,
	ENEMY_SPINY,
	ENEMY_BULLETBILL,
	ENEMY_MUNCHER,
	ENEMY_CHAINCHOMP,
	ENEMY_BOBOMB,
	ENEMY_SLEDGEBROTHERS,
	ENEMY_THWOMP,
	ENEMY_DRYBONE,
	ENEMY_FIREPIRANHAPLANT,
	ENEMY_GRINDER,
	ENEMY_CHAINCHOMPSTAKE,

	ENEMY_BOOMERANGBROTHERS,
	ENEMY_FIREBROTHERS,
	ENEMY_CHARGINGCHUCK,
	ENEMY_BOWSERJUNIOR,

	ENEMY_BOO,
	ENEMY_DRYFISH,
	ENEMY_MONTYMALE,
	ENEMY_ROCKYWRENCH,
	ENEMY_CANNONBALL,
	ENEMY_BLOOBERBABY,
	ENEMY_WIGGLER,
	ENEMY_ANGER,
	ENEMY_MAGIKOOPA,

	ENEMY_BOOMBOOM,

    ENEMY_DECOMP_UNK1, // Mykner> Unknown value found in decomp
    ENEMY_DECOMP_UNK2, // Mykner> Unknown value found in decomp
    ENEMY_SKEWER, // Mykner> TODO

	ENEMY_END = 3999,

	ENEMY_MASK_POWERUP = 0x10000,
	ENEMY_MASK_WINGED = 0x20000,
    ENEMY_MASK_UNKNOWN1 = 0x30000, // Mykner> Unknown value found in decomp
    ENEMY_MASK_UNKNOWN2 = 0x40000,  // Mykner> Unknown value found in decomp
};

enum BulletTypes
{
	BULLET_NONE = 4000,
	BULLET_BEGIN = 4000,

	BULLET_FIREBALL = 4001,
	BULLET_BOWSERFIRE,
	BULLET_THROWINGHAMMER,
	BULLET_YOSHITONGUE,
	BULLET_YOSHIFIRE,
	BULLET_SPINYSPIKE,
	BULLET_UNUSED___,
	BULLET_WRENCH,
	BULLET_BOOMERANG,

	BULLET_END = 4999,
};

enum EffectTypes
{
	EFFECT_NONE = 5000,
	EFFECT_BEGIN = 5000,

	EFFECT_SCORE = 5001,
	EFFECT_COIN,
	EFFECT_BRICKDEBRIS,
	EFFECT_EXPLOSION,
	EFFECT_FIREWORKS,
	EFFECT_AIRBUBBLE,

	EFFECT_EGGDEBRIS,
	EFFECT_POP,
	EFFECT_DUST,
	EFFECT_SMALLDUST,
	EFFECT_HIT,
	EFFECT_STARDUST,
	EFFECT_BOMBEXPLOSION,
	EFFECT_DRYBONEDEBRIS,
	EFFECT_TWINKLE,
	EFFECT_PINKTWINKLE,
	EFFECT_GOLDTWINKLE,

	EFFECT_SCENE_FADE,
	EFFECT_SCENE_FADEIN,
	EFFECT_SCENE_FADEOUT,
	EFFECT_SCENE_CIRCLE,
	EFFECT_SCENE_CIRCLEIN,
	EFFECT_SCENE_CIRCLEOUT,
	EFFECT_SCENE_TILE,
	EFFECT_SCENE_TILEIN,
	EFFECT_SCENE_TILEOUT,

	EFFECT_WINGDEBRIS,
	EFFECT_PARALYZE,
	EFFECT_CHAINDEBRIS,
	EFFECT_DEBRISBASE,
	EFFECT_SPIN,
	EFFECT_SPLASH,
	EFFECT_DEATHPOSITION,

	EFFECT_END = 5999,
};

enum EventObjectTypes
{
	EVENTOBJECT_NONE = 6000,
	EVENTOBJECT_BEGIN = 6000,

	EVENTOBJECT_FLAGPOLE = 6001,
	EVENTOBJECT_GOALPOINT,
	EVENTOBJECT_BRIDGE,
	EVENTOBJECT_BOWSERFIRESHOOTER,
	EVENTOBJECT_BOSSBRIDGE,
	EVENTOBJECT_BOSSBRIDGE_AXE,
	EVENTOBJECT_PRINCESS,
	EVENTOBJECT_TRAMPOLIN,
	EVENTOBJECT_WARPZONETEXT,
	EVENTOBJECT_SKYBRIDGE,
	EVENTOBJECT_VORTEX,
	EVENTOBJECT_ENEMYGENERATOR,
	EVENTOBJECT_BILLBLASTER,
	EVENTOBJECT_LOOPHANDLER,
	EVENTOBJECT_LOOPFLAG,
	EVENTOBJECT_WARPZONEFLAG,
	EVENTOBJECT_TRAMPOLINMINI,
	EVENTOBJECT_CHECKPOINT_FLAG,
	EVENTOBJECT_VINELADDER,
	EVENTOBJECT_P_SWITCH,
	EVENTOBJECT_STARTPOINT_SIGN,
	EVENTOBJECT_SIGN,
	EVENTOBJECT_DOOR,
	EVENTOBJECT_CANNON,
	EVENTOBJECT_ONEWAYWALL,
	EVENTOBJECT_POWBLOCK,
	EVENTOBJECT_SHOESMOKE,
	EVENTOBJECT_GOALBOX,
	EVENTOBJECT_GOALPOST,

    EVENTOBJECT_OBJECTGENERATOR, // Mykner> 6035 in decomp

	EVENTOBJECT_END = 6999,
};

enum PlayerObjectTypes
{
	PLAYEROBJECT_NONE = 7000,
	PLAYEROBJECT_BEGIN = 7000,

	PLAYEROBJECT_MARIO = 7001,
	PLAYEROBJECT_NEWMARIO,

	PLAYEROBJECT_END = 7999,
};

enum VehicleObjectTypes
{
	VEHICLE_NONE = 8000,
	VEHICLE_BEGIN = 8000,

	VEHICLE_YOSHI,
	VEHICLE_CLOUD,
	VEHICLE_CLOWNCAR,
	VEHICLE_GOOMBASHOE,

	VEHICLE_END = 8999,
};
