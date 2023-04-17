#pragma once

#include "GameStateBase.h"

#include "ServerTransaction.h"

class GameObjectBase;
class StageMetaInfo;
class GameStateSingleStageGameOver : public GameStateBase
{
public:
	GameStateSingleStageGameOver(Game *pGame);
	virtual ~GameStateSingleStageGameOver();

	virtual void Init();
	virtual void Process();

	static void AddFailedCount(void *pParam);
	static void RateStage(void *pParam);

	virtual void Render();

	ServerTransaction m_Transaction;
	StageMetaInfo *m_pCurMetaInfo;

	enum GameStateSingleStageGameOverUI
	{
		UI_UNDEFINED = 0,

		UI_RETRY,
		UI_RATESTAGE,
		UI_BACKTOTITLE,

		UI_BACK,
	};
};

