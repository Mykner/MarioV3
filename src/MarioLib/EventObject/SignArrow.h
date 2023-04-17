#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

enum SignDirections
{
	SIGNDIR_R = 0,
	SIGNDIR_RB,
	SIGNDIR_B, 
	SIGNDIR_LB,
	SIGNDIR_L,
	SIGNDIR_LT,
	SIGNDIR_T,
	SIGNDIR_RT,
};

class SignArrow : public EventObjectBase
{
public:
	SignArrow(Game *pGame, Stage *pStage);
	virtual ~SignArrow();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	
	virtual SIZE GetSize();
	int m_nSignDirection;
	
	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"SignDirection", &m_nSignDirection },
	END_DECLARE_PROPERTYMAP()
};