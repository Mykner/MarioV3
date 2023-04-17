#pragma once

#include "GameStateBase.h"

#include "ServerTransaction.h"

class GameObjectBase;
class StageMetaInfo;
class Sprite;
class GameStateSingleStageClear : public GameStateBase
{
public:
	GameStateSingleStageClear(Game *pGame);
	virtual ~GameStateSingleStageClear();

	virtual void Init();
	virtual void Process();
	void InitCustomStageSelectUI();
	void InitPostCommentUI();
	
	static void LikeStage(void *pParam);
	static void AddClearedCount(void *pParam);
	static void RateStage(void *pParam);
	static void PostComment(void *pParam);

	virtual void Render();

	bool m_bGoalByCastle;
	bool m_bBestTime;
	bool m_bBestScore;
	NaString m_strTime;
	NaString m_strBestTime;
	NaString m_strBestScore;

	ServerTransaction m_Transaction;
	StageMetaInfo *m_pCurMetaInfo;

	enum GameStateSingleStageClearUI
	{
		UI_UNDEFINED = 0,

		UI_RETRY,
		UI_SELECTSTAGE,
		UI_RATESTAGE, // Developer only
		UI_COMMENTSTAGE,
			UI_POSTCOMMENT,

		UI_LIKESTAGE,
		UI_BACKTOTITLE,

		UI_CUSTOMSTAGE_000,
		UI_CUSTOMSTAGE_099 = UI_CUSTOMSTAGE_000 + 99,

		UI_BACK,
	};
};

