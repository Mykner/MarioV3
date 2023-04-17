#pragma once

#include "Common.h"

#include "EventObjectBase.h"

class GoalPoint : public EventObjectBase
{
public:
	GoalPoint(Game *pGame, Stage *pStage);
	virtual ~GoalPoint();
	
	virtual void RequestSprites(Stage *pStage = nullptr);

	virtual void CheckCollisionPlayer();

	virtual void Process();
	virtual void OnBeforeProcess();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	virtual bool CanApplied();
	virtual bool CanPushedUp();

	unsigned char GetTileData(int nIdx);
	virtual SIZE GetSize();

	int m_nFlagHeight;
	bool m_bBigCastle;
	Sprite **m_ppSpriteTile;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"IsBigCastle", &m_bBigCastle },
	END_DECLARE_PROPERTYMAP()
};

