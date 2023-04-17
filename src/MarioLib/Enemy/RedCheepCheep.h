#pragma once

#include "Common.h"

#include "CheepCheep.h"

class RedCheepCheep : public CheepCheep
{
public:
	RedCheepCheep(Game *pGame, Stage *pStage);
	virtual ~RedCheepCheep();
	
	virtual void ChangeState(int nState, ChangeStateInfo *pInfo = nullptr);

	virtual int GetSpriteIndex();
};

