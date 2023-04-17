#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class VineLadder : public EventObjectBase
{
public:
	VineLadder(Game *pGame, Stage *pStage);
	virtual ~VineLadder();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual NaRect GetRect();
	virtual NaRectT<float> GetRectFloat();
	virtual SIZE GetSize();

	int m_nLength;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Length", &m_nLength },
	END_DECLARE_PROPERTYMAP()
};