#include "TypeNames.h"

#include "Common.h"
#include "Types.h"

// TypeNames for Editor
wchar_t *g_szStageThemeSet[] =
{
	L"Custom",
	L"OverWorld",
	L"UnderGround",
	L"Castle",
	L"UnderWater",
	L"GhostHouse",
	L"AirShip",
	L"Snow",
	nullptr,
};

wchar_t *g_szStageTheme[] =
{
	L"OverWorld",
	L"UnderGround",
	L"Castle",
	L"UnderWater",
	L"CastleUnderWater",
	L"ClassicSnow",
	L"Mushroom",
	L"Gray",
	L"GhostHouse",
	L"AirShip",
	L"Snow",
	nullptr,
};

wchar_t *g_szStageBackgroundTheme[] =
{
	L"Mountain",
	L"Cloud",
	L"None",
	L"UnderWater",
	L"Fence",
	L"Snow",
	L"Night",
	L"SnowNight",
	L"RedCloud",
	L"Underground",
	L"Castle",
	nullptr,
}; 

wchar_t *g_szStageEntranceType[] =
{
	L"None",
	L"ToUnderGround",
	L"OutFromPipe",
	L"Ground->Sky",
	nullptr,
};

wchar_t *g_szAutoScrollType[] =
{
	L"None",
	L"Horz Slow",
	L"Horz Normal",
	L"Horz Fast",
	L"Horz God",
	L"Vert Slow",
	L"Vert Normal",
	L"Vert Fast",
	L"Vert God",
	nullptr,
};

wchar_t *g_szPipeType[] =
{
	L"None",
	L"WarpToBonusZone",
	L"WarpInCurrentStage",
	L"WarpZone(World)",
	L"WarpToSubZone",
	nullptr,
};

wchar_t *g_szPipeDir[] =
{
	L"Unknown",
	L"Up To Down (ก้)",
	L"Left To Right (กๆ)",
	L"ExitOnly (DoNotUse)",
	L"Down To Up (ก่)",
	L"Right To Left (ก็)",
	nullptr,
};

wchar_t *g_szOneWayWallDir[] =
{
	L"Right",
	L"Bottom",
	L"Left",
	L"Top",
	nullptr,
};

wchar_t *g_szBridgeTypes[] =
{
	L"Static",
	L"CycleUp",
	L"CycleDown",
	L"Down->Up",
	L"Right->Left",
	L"Up->Down",
	L"Left->Right",
	L"Balanced (Left)",
	L"Balanced (Right)",
	L"Fall (when Stand On)",
	nullptr,
};

wchar_t *g_szTextActionTypes[] =
{
	L"None",
	L"Blink",
	L"MoveUp",
	//
	// #TODO More text actions.
	//
	nullptr,
};

wchar_t *g_szItemTypes[] =
{
	L"None",
	L"Coin",
	L"Mushroom",
	L"Mushroom1UP",
	L"FireFlower",
	L"SuperStar",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Coins",
	L"Vine",
	L"GiantMushroom",
	L"YoshiEgg",
	L"Cloud",
	L"TurtleShell",
	L"Leaf",
	L"ThrownCoin",
	L"DoorKeyCoin",
	L"DoorKey",
	nullptr,
};

wchar_t *g_szEnemyTypes[] =
{
	L"None",
	L"Goomba",
	L"Turtle",
	L"PiranhaPlant",
	L"RedTurtle",
	L"RedTurtleWingedPatrol",
	L"FireBar",
	L"Podoboo",
	L"Bowser",
	L"TurtleWinged",
	L"Bloober",
	L"CheepCheep",
	L"RedCheepCheep",
	L"CheepCheepFly",
	L"RedCheepCheepFly",
	L"HammerBros.",
	L"Lakitu",
	L"BuzzyBeetle",
	L"Spiny",
	L"BulletBill",
	L"Muncher",
	L"ChainChomp",
	L"Bob-omb",
	L"SledgeBrothers",
	L"Thwomp",
	L"DryBone",
	L"FirePiranhaPlant",
	L"Grinder",
	L"ChainChompStake",
	L"BoomerangBros.",
	L"FireBros.",
	L"ChargingChuck",
	L"BowserJunior",
	L"Boo",
	L"DryFish",
	L"MontyMale",
	L"RockyWrench",
	L"CannonBall",
	L"BlooberBaby",
	L"Wiggler",
	L"Anger",
	L"MagiKoopa",
	L"BoomBoom",
	nullptr,
};

wchar_t *g_szMapObjectTypes[] =
{
	L"None",
	L"Pipe",
	L"GoalPoint",
	L"StartPoint",
	L"",
	L"",
	L"PipeExit",
	L"",
	L"CheckPoint",
	L"Pipe(Horz)",
	L"Pipe(Body)",
	L"CameraStop",
	L"",
	L"Island",
	L"River",
	L"SkyExit",
	L"RiverBridge",
	L"EnemyStop",
	L"BillBlaster",
	L"MushroomIsland",
	L"Wall",
	L"SemiSolid",
	L"MessageBlock",
	nullptr,
};

wchar_t *g_szBulletTypes[] =
{
	L"None",
	L"FireBall",
	L"BowserFire",
	L"ThrowingHammer",
	L"YoshiTongue",
	nullptr,
};

wchar_t *g_szEffectTypes[] =
{
	L"None",
	L"Score",
	L"Coin",
	L"BrickDebris",
	L"Explosion",
	L"FireWorks",
	L"AirBubble",
	L"EggDebris",
	L"Pop",
	L"Dust",
	L"SmallDust",
	L"Hit",
	L"StarDust",
	L"BombExplosion",
	nullptr,
};

wchar_t *g_szEventObjectTypes[] =
{
	L"None",
	L"FlagPole",
	L"GoalPoint",
	L"Bridge",
	L"BowserFireShooter",
	L"BossBridge",
	L"BossBridgeAxe",
	L"Princess",
	L"Trampolin",
	L"WarpZoneText",
	L"SkyBridge",
	L"Vortex",
	L"EnemyGenerator",
	L"BillBlaster",
	L"LoopHandler",
	L"LoopFlag",
	L"WaprZoneFlag",
	L"TrampolinMini",
	L"CheckPointFlag",
	L"Vine",
	L"P-Switch",
	L"StartPointSign",
	L"Sign",
	L"Door",
	L"Cannon",
	L"OneWayWall",
	L"PowBlock",
	L"ShoeSmoke",
	nullptr,
};

wchar_t *g_szPlayerObjectTypes[] =
{
	L"None",
	L"Mario",
	L"NewMario",
	nullptr,
};

wchar_t *g_szVehicleObjectTypes[] =
{
	L"None",
	L"Yoshi",
	L"Cloud",
	L"ClownCar",
	L"GoombaShoe",
	nullptr,
};

wchar_t *g_szShoeTypes[] =
{
	L"Goomba's Shoe",
	L"Stiletto",
	nullptr,
};

wchar_t *g_szEnemyGeneratorRangeTypes[] =
{
	L"JustThisPage",
	L"WholeStage",
	nullptr,
};

wchar_t *g_szEnemyGeneratorPositions[] =
{
	L"GeneratorPosition",
	L"BottomOfScreen",
	L"RightOfScreen",
	nullptr,
};

wchar_t *g_szGameStates[] =
{
	L"<NotUsed>",

	L"Design",
	L"DesignPlay",

	L"Init",
	L"Credits",

	L"Logo",
	L"Title",
	L"AppUpdate",
	L"RegisterUser",
	L"DemoPlay",
	L"StageIntro",

	L"Play",
	L"Test",

	L"ShapeChange",
	L"Die",

	L"MessageBlock",

	L"EnterStage",
	L"EnterPipe",
	L"ExitPipe",
	L"EnterSky",
	L"SkyIntro",
	L"EnterDoor",
	L"ExitDoor",

	L"FlagDown",
	L"Goal",
	L"CutBridge",
	L"RescuePrincess",
	L"HitGoalBox",
	L"HitGoalPost",

	L"TimeUp",
	L"GameOver",

	L"SingleStageClear",
	L"SingleStageGameOver",

	L"Unknown",
	nullptr,
};

int g_nThrowableTypes[] =
{
	-1,

	ITEM_MUSHROOM,
	ITEM_1UP,
	ITEM_FIREFLOWER,
	ITEM_SUPERSTAR,
	ITEM_GIANTMUSHROOM,
	ITEM_THROWNCOIN,

	ENEMY_GOOMBA,
	ENEMY_TURTLE,
	ENEMY_REDTURTLE,
	ENEMY_TURTLE_WINGED,
	ENEMY_BUZZYBEETLE,
	ENEMY_SPINY,
	ENEMY_BULLETBILL,

	// Do not change order!

	0,
};

wchar_t *g_szThrowableTypes[] =
{
	L"None",

	L"Mushroom",
	L"Mushroom1UP",
	L"FireFlower",
	L"SuperStar",
	L"GiantMushroom",
	L"ThrownCoin",

	L"Goomba",
	L"Turtle",
	L"RedTurtle",
	L"TurtleWinged",
	L"BuzzyBeetle",
	L"Spiny",
	L"BulletBill",
	
	// Do not change order!

	nullptr,
};

int g_nGeneratableTypes[] =
{
	-1,

	ITEM_MUSHROOM,
	ITEM_1UP,
	ITEM_FIREFLOWER,
	ITEM_SUPERSTAR,
	ITEM_GIANTMUSHROOM,
	ITEM_YOSHIEGG,
	ITEM_CLOUD,
	ITEM_TURTLESHELL,
	ITEM_THROWNCOIN,

	ENEMY_GOOMBA,
	ENEMY_TURTLE,
	ENEMY_REDTURTLE,
	ENEMY_TURTLE_WINGED,
	ENEMY_BUZZYBEETLE,
	ENEMY_SPINY,
	ENEMY_PODOBOO,
	ENEMY_BOWSER,
	ENEMY_BLOOBER,
	ENEMY_CHEEPCHEEP,
	ENEMY_REDCHEEPCHEEP,
	ENEMY_HAMMERBROTHERS,
	ENEMY_CHAINCHOMP, 
	ENEMY_BOBOMB,
	ENEMY_SLEDGEBROTHERS,
	ENEMY_DRYBONE,

	EVENTOBJECT_P_SWITCH,

	// Do not change order!

	0,
};

wchar_t *g_szGeneratableTypes[] =
{
	L"None",

	L"Mushroom",
	L"Mushroom1UP",
	L"FireFlower",
	L"SuperStar",
	L"GiantMushroom",
	L"YoshiEgg",
	L"Cloud",
	L"TurtleShell",
	L"ThrownCoin",

	L"Goomba",
	L"Turtle",
	L"RedTurtle",
	L"TurtleWinged",
	L"BuzzyBeetle",
	L"Spiny",
	L"Podoboo",
	L"Bowser",
	L"Bloober",
	L"CheepCheep",
	L"RedCheepCheep",
	L"HammerBros.",
	L"ChainChomp",
	L"Bob-omb",
	L"SledgeBrothers",
	L"DryBone",

	L"PSwitch",

	// Do not change order!

	nullptr,
};


int g_nCannonBulletTypes[] =
{
	-1,

	ENEMY_CANNONBALL,
	ITEM_THROWNCOIN,

	// Do not change order!

	0,
};

wchar_t *g_szCannonBulletTypes[] =
{
	L"None",

	L"CannonBall",
	L"ThrownCoin",

	// Do not change order!

	nullptr,
};
