#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class LoopHandler : public EventObjectBase
{
public:
	LoopHandler(Game *pGame, Stage *pStage);
	virtual ~LoopHandler();

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Process();
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual SIZE GetSize();

	int m_nSleep;
	std::vector<GameObjectBase*> m_vecCheckedFlag;
	void SetCheck(GameObjectBase* pLoopFlag);

	int m_nNextPage;
	int m_nImmunityFlag;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"NextPage", &m_nNextPage },
		{ L"ImmunityFlagCount", &m_nImmunityFlag },
	END_DECLARE_PROPERTYMAP()
};