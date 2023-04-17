#include "EventObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"

#include "FlagPole.h"
#include "GoalPoint.h"
#include "Bridge.h"
#include "BowserFireShooter.h"
#include "BossBridge.h"
#include "BossBridgeAxe.h"
#include "Princess.h"
#include "Trampolin.h"
#include "WarpZoneText.h"
#include "SkyBridge.h"
#include "Vortex.h"
#include "EnemyGenerator.h"
#include "LoopHandler.h"
#include "BillBlaster.h"
#include "LoopFlag.h"
#include "WarpZoneFlag.h"
#include "TrampolinMini.h"
#include "PSwitch.h"
#include "StartPointSignArrow.h"
#include "SignArrow.h"
#include "Door.h"
#include "VineLadder.h"
#include "CheckPointFlag.h"
#include "Cannon.h"
#include "OneWayWall.h"
#include "PowBlock.h"
#include "ShoeSmoke.h"
#include "GoalBox.h"
#include "GoalPost.h"
#include "ObjectGenerator.h"

EventObjectBase::EventObjectBase(Game *pGame, Stage *pStage) :
	GameObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_NONE;
	
	RequestSprites(pStage);

	m_bCanCollide = false;
	m_bCanLand = false;
	m_bEventDead = false;
}

EventObjectBase::~EventObjectBase()
{
	
}

bool EventObjectBase::CanLand()
{
	return m_bCanLand;
}

bool EventObjectBase::CanCollide(int nDir)
{
	return m_bCanCollide;
}

bool EventObjectBase::CanBornFromBlock()
{
	return false;
}

bool EventObjectBase::CanBurnt(int nType)
{
	return false;
}

int EventObjectBase::GetSpriteIndex()
{
	if ((m_nState == STATE_ITEMPOP || m_nState == STATE_ITEMPOPDOWN) &&
		m_nStateFrame < 30)
		return -1;

	return 0;
}

int EventObjectBase::GetMaskedType()
{
	int nType = m_nType;
	if (m_bPowerUp)
		nType |= ENEMY_MASK_POWERUP;
	if (m_bWinged)
		nType |= ENEMY_MASK_WINGED;

	return nType;
}

GameObjectBase * EventObjectBase::CreateInstance(Game * pGame, Stage * pStage, int nType)
{
	GameObjectBase *pObj = nullptr;
	switch (nType)
	{
	case EVENTOBJECT_FLAGPOLE:			pObj = new FlagPole(pGame, pStage);				break;
	case EVENTOBJECT_GOALPOINT:			pObj = new GoalPoint(pGame, pStage);			break;
	case EVENTOBJECT_BRIDGE:			pObj = new Bridge(pGame, pStage);				break;
	case EVENTOBJECT_BOWSERFIRESHOOTER: pObj = new BowserFireShooter(pGame, pStage);	break;
	case EVENTOBJECT_BOSSBRIDGE:		pObj = new BossBridge(pGame, pStage);			break;
	case EVENTOBJECT_BOSSBRIDGE_AXE:	pObj = new BossBridgeAxe(pGame, pStage);		break;
	case EVENTOBJECT_PRINCESS:			pObj = new Princess(pGame, pStage);				break;
	case EVENTOBJECT_TRAMPOLIN:			pObj = new Trampolin(pGame, pStage);			break;
	case EVENTOBJECT_WARPZONETEXT:		pObj = new WarpZoneText(pGame, pStage);			break;
	case EVENTOBJECT_SKYBRIDGE:			pObj = new SkyBridge(pGame, pStage);			break;
	case EVENTOBJECT_VORTEX:			pObj = new Vortex(pGame, pStage);				break;
	case EVENTOBJECT_ENEMYGENERATOR:	pObj = new EnemyGenerator(pGame, pStage);		break;
	case EVENTOBJECT_LOOPHANDLER:		pObj = new LoopHandler(pGame, pStage);			break;
	case EVENTOBJECT_BILLBLASTER:		pObj = new BillBlaster(pGame, pStage);			break;
	case EVENTOBJECT_LOOPFLAG:			pObj = new LoopFlag(pGame, pStage);				break;
	case EVENTOBJECT_WARPZONEFLAG:		pObj = new WarpZoneFlag(pGame, pStage);			break;
	case EVENTOBJECT_TRAMPOLINMINI:		pObj = new TrampolinMini(pGame, pStage);		break;
	case EVENTOBJECT_P_SWITCH:			pObj = new PSwitch(pGame, pStage);				break;
	case EVENTOBJECT_STARTPOINT_SIGN:	pObj = new StartPointSignArrow(pGame, pStage);	break;
	case EVENTOBJECT_SIGN:				pObj = new SignArrow(pGame, pStage);			break;
	case EVENTOBJECT_DOOR:				pObj = new Door(pGame, pStage);					break;
	case EVENTOBJECT_CHECKPOINT_FLAG:	pObj = new CheckPointFlag(pGame, pStage);		break;
	case EVENTOBJECT_VINELADDER:		pObj = new VineLadder(pGame, pStage);			break;
	case EVENTOBJECT_CANNON:			pObj = new Cannon(pGame, pStage);				break;
	case EVENTOBJECT_ONEWAYWALL:		pObj = new OneWayWall(pGame, pStage);			break;
	case EVENTOBJECT_POWBLOCK:			pObj = new PowBlock(pGame, pStage);				break;
	case EVENTOBJECT_SHOESMOKE:			pObj = new ShoeSmoke(pGame, pStage);			break;
	case EVENTOBJECT_GOALBOX:			pObj = new GoalBox(pGame, pStage);				break;
	case EVENTOBJECT_GOALPOST:			pObj = new GoalPost(pGame, pStage);				break;
    case EVENTOBJECT_OBJECTGENERATOR:   pObj = new ObjectGenerator(pGame, pStage);      break;
	}

	return pObj;
}
