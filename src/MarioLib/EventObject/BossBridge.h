#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class BossBridge : public EventObjectBase
{
public:
	BossBridge(Game *pGame, Stage *pStage);
	virtual ~BossBridge();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnDie();

	virtual void AddPassenger(GameObjectBase *pObj);

	virtual NaRect GetRect();
	virtual NaRectT<float> GetRectFloat();
	virtual SIZE GetSize();

	std::vector<GameObjectBase*> m_vecPassenger;
	int m_nLength;
	int m_bAnchor;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Length", &m_nLength },
	END_DECLARE_PROPERTYMAP()
};