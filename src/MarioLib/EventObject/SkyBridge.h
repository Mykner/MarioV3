#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

enum SkyBridgeTypes
{
	SkyBridgeTYPE_NORMAL,
	SkyBridgeTYPE_CYCLEUP,
	SkyBridgeTYPE_CYCLEDOWN,
	SkyBridgeTYPE_DOWNUP,
	SkyBridgeTYPE_RIGHTLEFT,
	SkyBridgeTYPE_UPDOWN,
	SkyBridgeTYPE_LEFTRIGHT,
};

class SkyBridge : public EventObjectBase
{
public:
	SkyBridge(Game *pGame, Stage *pStage);
	virtual ~SkyBridge();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderSkyBridge(int x, int y, int nColor = 0xffffffff);

	virtual void OnIdle();
	void OnMove();
	
	virtual void AddPassenger(GameObjectBase *pObj);

	virtual SIZE GetSize();

	std::vector<GameObjectBase*> m_vecPassenger;
	int m_nLength;
	int m_nInitialX;
	int m_nInitialY;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Length", &m_nLength },
	END_DECLARE_PROPERTYMAP()
};