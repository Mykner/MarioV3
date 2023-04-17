#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

enum BridgeTypes
{
	BRIDGETYPE_NORMAL,
	BRIDGETYPE_CYCLEUP,
	BRIDGETYPE_CYCLEDOWN,
	BRIDGETYPE_DOWNUP,
	BRIDGETYPE_RIGHTLEFT,
	BRIDGETYPE_UPDOWN,
	BRIDGETYPE_LEFTRIGHT,
	BRIDGETYPE_BALANCE_LEFT,
	BRIDGETYPE_BALANCE_RIGHT,
	BRIDGETYPE_FALL,
};

class Bridge : public EventObjectBase
{
public:
	Bridge(Game *pGame, Stage *pStage);
	virtual ~Bridge();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderBridge(float x, float y, int nColor = 0xffffffff);
	void RenderLineAndAnchor(int nColor = 0xffffffff);
	
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual void OnIdle();
	virtual void OnFall();

	void OnMove();
	void OnMoveCycleUp();
	void OnMoveCycleDown();
	void OnMoveDownUp();
	void OnMoveUpDown();
	void OnMoveRightLeft();
	void OnMoveLeftRight();
	
	void FindLinkedBridge();
	void InitAnchorPositionY();
	virtual void AddPassenger(GameObjectBase *pObj);

	virtual SIZE GetSize();

	std::vector<GameObjectBase*> m_vecPassenger;
	bool m_bIsShort;
	int m_nLength;
	int m_nBridgeType;
	int m_nDistance;
	
	int m_nAnchorHeight;
	int m_nAnchorPositionY;
	int m_nLinkedPositionX;

	int m_nInitialX;
	int m_nInitialY;
	Bridge* m_pLinkedBridge;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"IsShortSize", &m_bIsShort },
		{ L"Length", &m_nLength },
		{ L"BridgeType", &m_nBridgeType },
		{ L"MoveDistance", &m_nDistance },
		{ L"AnchorHeight", &m_nAnchorHeight },
	END_DECLARE_PROPERTYMAP()
};