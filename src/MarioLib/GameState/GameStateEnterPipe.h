#pragma once

#include "GameStateBase.h"

class PipeInfo;
class GameObjectBase;
class NaRect;
class GameStateEnterPipe : public GameStateBase
{
public:
	GameStateEnterPipe(Game *pGame);
	virtual ~GameStateEnterPipe();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	void ProcessOnAfterEnterStage();
	void ProcessOnAfterBonusStage();

	void ProcessExitBonusStage(PipeInfo *pPipeInfo);
	void ProcessEnterBonusStage(PipeInfo *pPipeInfo);
	void ProcessWarpToCurrentStage(PipeInfo *pPipeInfo);
	void ProcessWarpZone(PipeInfo *pPipeInfo);
	void ProcessWarpToSubZone(PipeInfo *pPipeInfo);
};
