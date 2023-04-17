#include "ObjectGenerator.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"
#include "SoundManager.h"

#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(ObjectGenerator)
	PROP_INT("GenerateType",	VT_I4,	-1,		g_szGeneratableTypes,			"ObjectGenerator"),
	PROP_INT("Interval",		VT_I4,	1,		0,								"ObjectGenerator"),
	PROP_INT("Chance",			VT_I4,	100,	0,								"ObjectGenerator"),
	PROP_INT("MaxCount",		VT_I4,	3,		0,								"ObjectGenerator"),
	PROP_INT("Range",			VT_I4,	0,		g_szEnemyGeneratorRangeTypes,	"ObjectGenerator"),
	PROP_INT("GeneratePosition", VT_I4, 0,		g_szEnemyGeneratorPositions,	"ObjectGenerator"),
END_IMPL_PROPERTYMAP()

ObjectGenerator::ObjectGenerator(Game *pGame, Stage *pStage) :
    EnemyGenerator(pGame, pStage)
{
    m_nType = EVENTOBJECT_OBJECTGENERATOR;
}

ObjectGenerator::~ObjectGenerator()
{
}
