#pragma once

#include "Common.h"
#include "Types.h"
#include "GameObjectStates.h"
#include "SpriteIndex.h"

#include <vector>
#include "PropertyMap.h"
#include "JsonManager.h"

class Game;
class Sprite;
class Player;
class GameObjectBase;
class PipeInfo;
class Stage;
class VehicleObjectBase;
class ControllerBase;

class CreateParameter
{
public:
	CreateParameter() : 
		nType(-1), 
		nScore(0), 
		pFromPlayer(nullptr), 
		pFromObject(nullptr), 
		fXS(0), 
		fYS(0),
		fZS(0),
		fScaleX(0),
		fScaleY(0),
		nDuration(0),
		nStartDelay(0),
		nFlip(-1),
		pSprite(nullptr)
	{};

	int nType;
	int nScore;
	ControllerBase *pFromPlayer;
	GameObjectBase *pFromObject;
	float fXS, fYS, fZS;
	float fScaleX, fScaleY;
	int nDuration;
	int nStartDelay;
	int nFlip;
	Sprite *pSprite;
};

class ChangeStateInfo
{
public:
	ChangeStateInfo() : 
		pFromObject(nullptr), 
		nNextState(0), // STATE_IDLE
		nPower(0)
	{};
	GameObjectBase *pFromObject;
	int nNextState;
	int nPower;
};

class GameObjectBase : public PropertyObjectBase
{
public:
	GameObjectBase(Game *pGame, Stage *pStage);
	virtual ~GameObjectBase();
	
	virtual void RequestSprites(Stage *pStage = nullptr);
	virtual void ParseParameter(CreateParameter *pParam) {};

	bool HitTest(int x, int y);
	virtual void CheckCollision();
	virtual void CheckCollisionPlayer();
	virtual void CheckCollisionEnemy();
	virtual void CheckCollisionVehicle();
	virtual bool IsOverlapped(GameObjectBase *pOther);
	virtual bool CanCollisionWith(GameObjectBase *pOther);

	virtual POINT FindCollidableOffset(int nCollisionDir, int nMax = 32);

	virtual bool CheckHitBlock(int nPower = POWER_HITBLOCK_NORMAL, GameObjectBase *pWho = nullptr);
	virtual bool CheckReachWall();
	virtual bool CheckPressEnemy(int nPower = -1);
	virtual bool CheckRideVehicle();
	virtual bool CheckFalling();
	virtual bool CheckMoveWithMe(GameObjectBase *pObj);

	virtual void PressJumpBlock();

	virtual bool IsInputHold(int nInputType);
	virtual bool IsInputPressed(int nInputType);
	virtual void Process();
	virtual void ProcessBase();
	virtual void ProcessShapeChange(int nStateFrame) {};
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
    virtual void ProcessState();

	virtual void ChangeShape(int nShape) {};
	virtual void OnChangeShape(int nShape) {};

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1) {};
	virtual void RenderWing(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1) { Render(nColor, nZOrder); };
	virtual void RenderShapeChange();
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void OnChangeState(int nState);
	virtual void SetState(int nState);

	virtual void OnIdle();
	virtual void OnStand();
	virtual void OnSit();
	virtual void OnWalk();
	virtual void OnReachWall(bool bIsLeftWall);
	virtual void OnReachFriend();
	virtual void OnBreak();
	virtual void OnJump();
	virtual void OnFall();
	virtual void OnApplyGravity(float fMagnification, float fMaxSpeed = 4.0f);
	virtual void OnSwim();
	virtual void OnSwimJump();
	virtual void OnSpinJump() {};
	virtual void OnGroundPound() {};
	virtual void OnPushedUp();
	virtual void OnPressJump();
	virtual void OnGotOffVehicle();
	virtual void OnTrampolinPress();
	virtual void OnTrampolinJump();
	virtual void OnItemPop();
	virtual void OnItemPopDown();
	virtual void OnAfterItemPop(int nNextStatePopUp = STATE_WALK, int nNextStatePopDown = STATE_FALL);
	virtual void OnPipeUp();
	virtual void OnPipeDown();
	virtual void OnPipeLeft();
	virtual void OnPipeRight();
	virtual void OnPressed() {};
	virtual void OnThrown();
	virtual void OnThrownFly() {};
	virtual void OnBurnt() {};
	virtual void OnSpitted() {};
	virtual void OnSpittedSafe() {};
	virtual void OnDie() {};
	virtual void OnSpring() {};
	virtual void OnShellIdle() {};
	virtual void OnShellRun() {};
	virtual void OnShellAwake() {};
	virtual void OnMove() {};
	virtual void OnMoveUp() {};
	virtual void OnMoveDown() {};
	virtual void OnThrowFire() {};
	virtual void OnRide() {};
	virtual void OnFly() {};
	virtual void OnFlyByAI() {};
	virtual void OnHatch() {};
	virtual void OnStacked() {};
	virtual void OnBlockJump();
	virtual void OnLanding();
	virtual void OnStomped(bool bCauseDead = true, GameObjectBase *pWhoPressed = nullptr);
	virtual void OnFollowPrepare() {};
	virtual void OnFollow() {};
	virtual void OnJumpInto() {};
	virtual void OnParalyze() {};
	virtual void OnSpecialPose() {};
	virtual void OnVictoryPose() {};
	virtual void OnPowerUp() {};
	virtual void OnLostWing();
	virtual void OnRetreat() {};
	virtual void OnEventDead() {};
	virtual void OnCarryByHand() {};
	virtual void OnKicked();
	virtual void OnPlacedByHand();
	virtual void OnKickedUp();
	virtual void OnExplode() {};
	virtual void OnPowed();

	virtual void OnBounceTrampolinMiniHorz(GameObjectBase *pTrampolin);
	virtual void OnApplyItem(GameObjectBase *pItemObject) {};
	virtual void OnApplyItem(int m_nType) {};
	virtual void OnApplyEvent(GameObjectBase *pEventObject) {};
	
	virtual void Carry(GameObjectBase* pObj);
	virtual void MoveCarryObject() {};
	virtual void Kick(GameObjectBase* pObj, bool bIsCarried = false);

	virtual void ReleaseWing();
	
	virtual bool IsAlive();
	virtual bool IsOutOfStage();
	virtual bool IsOutOfCamera();
	virtual bool HasMushroom();

	virtual bool IsInvinsible();
	virtual bool IsOpponent(GameObjectBase *pObj);
	virtual bool CanLand();
	virtual bool CanCollide(int nDir);
	virtual bool CanKicked();
	virtual bool CanPressed(int nPower = POWER_PRESS_NORMAL);
	virtual bool CanPushedUp();
	virtual bool CanRided(GameObjectBase *pByWho);
	virtual bool CanBurnt(int nType = BURN_UNKNOWN);
	virtual bool CanPowed();
	virtual bool CanCarry();
	virtual bool CanCarried();
	virtual bool CanEat();
	virtual bool CanSpit();
	virtual bool CanDamageFriend();
	virtual bool CanDamageEnemy();
	virtual bool CanPressEnemy();
	virtual bool CanSwim();
	virtual bool CanBounceTrampolinMiniHorz();
	virtual bool CanBornFromBlock();
	virtual bool CanStandOnSpike();

	virtual void AddPassenger(GameObjectBase *pObj) {};

	virtual void SetPosition(float fX, float fY);
	virtual void SetSpeed(float fX, float fY);
	virtual void ResetSpeed();

	virtual int GetPower(int nPowerForWhat);

	virtual int GetSpriteIndex() { return 0; };
	virtual int GetZOrder();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual std::vector<POINT> GetWingPosition();
	virtual int GetWingSpriteIndex();
	virtual NaRect GetTileRect();
	virtual NaRect GetRect();
	virtual NaRectT<float> GetRectFloat();
	virtual std::vector<NaRect> GetRectList();
	virtual SIZE GetSize();

	virtual int GetMaskedType();
	virtual void GetTypeSpriteWithIndex(Sprite &refSprite, int &nIndex) {};
	virtual SIZE GetSpriteSize();

	PipeInfo* FindPipeNearMe(int nCollisionType);
	virtual void EnterPipe(PipeInfo *pPipeInfo) {};
	virtual void EnterSky(PipeInfo *pPipeInfo) {};

	GameObjectBase* FindDoorNearMe();
	virtual void EnterDoor(GameObjectBase *pObj) {};

	void AddCombo();
	void StopCombo();
	int CalcScoreFrom(int nBeginScore);

	virtual const wchar_t* GetTypeName();
	static GameObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	void LoadProperty(Json::Value &jObj);
	NaString GetStateName(int nState = -1);

	short m_nDamageGroup;
	int m_nType;
	int m_nState;
	int m_nNextState;
	int m_nBaseState;
	ChangeStateInfo m_ChangeStateInfo;

	// Collision Infos
	bool m_bCollision[4];
	NaPointT<float> m_ptCollision[4];
	bool m_bCollisionPlayer;
	std::vector<GameObjectBase*> m_vecPressedEnemy;
	std::vector<GameObjectBase*> m_vecOverlappedEnemy;
	std::vector<GameObjectBase*> m_vecRidableVehicle;
	bool m_bReachFriend;
	bool m_bDamagedByMap;
	bool m_bStandOnJumpBlock;

	int m_nKickCooltime;
	int m_nDamageCooltime;
	int m_nInvinsibleTime;
	int m_nGiantTime;
	int m_nImmuneTime;

	int m_nCombo;

	bool m_bPowerUp;
	bool m_bWinged;

	union {
		float x;
		float m_fX;
	};
	union {
		float y;
		float m_fY;
	};
	float m_fXS;
	float m_fYS;

	int m_nShape;
	int m_nNextShape;

	Game *m_pGame;
	Stage *m_pStage;
	Sprite *m_pSprite;
	Sprite **m_ppSprite;
	int m_nStateFrame;
	int m_nSpriteIndex;
	bool m_bFlip; // true: look left / false: look right
	bool m_bFlipV;
	float m_fScaleX;
	float m_fScaleY;
	ControllerBase *m_pController;
	GameObjectBase *m_pParent;
	VehicleObjectBase *m_pVehicle;
	GameObjectBase *m_pCarry;
	GameObjectBase *m_pKickedBy;

	// Property Interface
	virtual void BuildPropertyMap() {};
};

