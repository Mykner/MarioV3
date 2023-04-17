#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class Princess : public EventObjectBase
{
public:
	Princess(Game *pGame, Stage *pStage);
	virtual ~Princess();

	virtual void CheckCollisionPlayer();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual bool CanApplied();

	virtual SIZE GetSize();

	bool m_bIsReal;
	NaString m_strThankWord;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"IsReal", &m_bIsReal },
		{ L"ThankWord", &m_strThankWord },
	END_DECLARE_PROPERTYMAP()
};

