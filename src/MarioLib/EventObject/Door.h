#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class Door : public EventObjectBase
{
public:
	Door(Game *pGame, Stage *pStage);
	virtual ~Door();

	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);
	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
    virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual int GetSpriteIndex();
	virtual void GetScale(float &fScaleX, float &fScaleY);
	virtual SIZE GetSize();

	bool m_bLocked;
	int m_nSubType;
	int m_nPairIndex;
    bool m_bResetStage;
    bool m_bPSwitchDoor;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"PairIndex", &m_nPairIndex },
        { L"Locked", &m_bLocked },
        { L"ResetStage", &m_bResetStage },
        { L"PSwitchDoor", &m_bPSwitchDoor },
	END_DECLARE_PROPERTYMAP()
};
