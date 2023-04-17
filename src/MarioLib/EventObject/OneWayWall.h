#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class OneWayWall : public EventObjectBase
{
public:
	OneWayWall(Game *pGame, Stage *pStage);
	virtual ~OneWayWall();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual bool CanPushedUp();

	virtual bool CanCollide(int nDir);
	virtual int GetSpriteIndex();
	virtual SIZE GetSize();

	std::vector<GameObjectBase*> m_vecPassenger;
	int m_nDirection;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Direction", &m_nDirection },
	END_DECLARE_PROPERTYMAP()
};