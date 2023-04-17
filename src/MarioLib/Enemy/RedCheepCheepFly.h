#pragma once

#include "Common.h"

#include "CheepCheepFly.h"

class RedCheepCheepFly : public CheepCheepFly
{
public:
	RedCheepCheepFly(Game *pGame, Stage *pStage);
	virtual ~RedCheepCheepFly();
	
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual int GetSpriteIndex();
};

