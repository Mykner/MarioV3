#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class WarpZoneText : public EventObjectBase
{
public:
	WarpZoneText(Game *pGame, Stage *pStage);
	virtual ~WarpZoneText();

	virtual void CheckCollisionPlayer();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual bool CanPushedUp();

	virtual SIZE GetSize();

	bool m_bVisible;
	NaString m_strText;
	bool m_bIsFront;
	bool m_bShadow;
	int m_nActionType;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Text", &m_strText)
		DECL_PROP("IsFront", &m_bIsFront)
		DECL_PROP("Shadow", &m_bShadow)
		DECL_PROP("ActionType", &m_nActionType)
	END_DECLARE_PROPERTYMAP()
};

