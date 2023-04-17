#pragma once

#include "GameStateBase.h"

#include "json.h"
#include "UserProfile.h"
#include "StageMetaInfo.h"
#include "ServerTransaction.h"

class Sprite;
class UserProfile;
class UIBase;
class StagePack;
class UIList;
class UIButton;
class GameStateTitle : public GameStateBase
{
public:
	GameStateTitle(Game *pGame);
	virtual ~GameStateTitle();

	virtual long GetBackgroundColor();
	virtual void Init();
	virtual void Process();
	virtual void Render();

	void LoadProfiles();

	void InitJustNewGameUI();
	void InitProfileSelectUI();
	void InitProfileCreateUI();
	void InitGameSelectUI();
	void InitClassicGameUI();
	void InitStageSelectUI();
	void InitCustomGameSelectUI();
	void InitCustomGameUI();
	void InitCustomStageSelectUI();
	void InitWorldStageSelectUI();
	void InitWorldStageDetailUI();

	static void LoadWorldStageList(void* pParam, void* pList);
	void OnLoadWorldStageList(std::vector<StageMetaInfo*> vec, UIList* pList);
	void OnLoadWorldStageListFail(NaString strReason, UIList* pList);

	static void LoadWorldStageDetail(void* pParam);
	void OnLoadWorldStageDetail(std::vector<StageCommentInfo*> vec, UIList* pList);

	static void DownloadWorldStage(void* pParam);
	void OnDownloadWorldStage();
	
	static void LoadWorldStageRecord(void* pParam);
	void OnLoadWorldStageRecord();
	void OnPlayWorldStage();

	static void RateWorldStage(void* pParam);

	void OnSelectProfile(int nIdx);
	void OnSelectCustomGame();
	void OnSelectCustomStage();
	void OnSelectWorldStage();
	void OnSelectSomeCustomGame(int nIdx);
	void OnSelectSomeCustomStage(NaString strName);
	void OnSelectSomeWorldStage(StageMetaInfo *pMetaInfo);
	void OnSelectPlayCurrentWorldStage();
	void OnSelectContinue();
	void OnSelectStageSelect();
	void OnSelectNewGame();
	void OnCompleteUserName();
	void OnSelectStage();
	void OnCancel();
	void OnChangeWorldStageSortMethod(UIBase *pUI, int nMethod);
	void OnChangeWorldStageDifficultyFilter(UIBase *pUI);
	void OnChangeWorldStageThemeFilter(UIBase *pUI);

	int GetAvatarIndex(NaString str);

	const int m_nFirstProfileIdx;
	const int m_nProfileCnt;
	StageMetaInfo* m_pCurMetaInfo;
	ServerTransaction m_Transaction;
	int m_nCurSortMethod;
	int m_nCurDiffFilter;
	int m_nCurThemeFilter;

	std::vector<StagePack> m_vecStagePack;
	UserProfile *m_pProfile[4];
	UserProfile *m_pCurProfile;
	UIBase *m_pCurBtnProfile;

	Sprite* m_pSpriteEtc;

	enum GameStateTitleUI
	{
		UI_UNDEFINED = 0,

		// Select Game
		UI_PROFILE,
			UI_PROFILE0,
			UI_PROFILE1,
			UI_PROFILE2,
			UI_PROFILE3,
				// Create Profile
				UI_TYPENAME,
				UI_USERNAME,
				UI_CANCEL,
		UI_CUSTOMGAME,
			// CustomGames
			UI_CUSTOMGAME_000,
			UI_CUSTOMGAME_099 = UI_CUSTOMGAME_000 + 99,
			// If CustomGame == Classic
			UI_CLASSICGAME,
				UI_JUSTNEWGAME,
				UI_NEWGAME,
				UI_CONTINUE,
				UI_STAGESELECT,
					// Stages
					UI_STAGE_000,
					UI_STAGE_099 = UI_STAGE_000 + 99,
			UI_BACK,
		UI_CUSTOMSTAGE,
			// CustomStages
			UI_CUSTOMSTAGE_000,
			UI_CUSTOMSTAGE_999 = UI_CUSTOMSTAGE_000 + 999,
		UI_WORLDSTAGE,
			// DownloadableStages
			UI_WORLDSTAGE_FILTER_DIFFICULTY,
			UI_WORLDSTAGE_FILTER_GAMETHEME,
			UI_WORLDSTAGE_SORT_0,
			UI_WORLDSTAGE_SORT_1,
			UI_WORLDSTAGE_SORT_2,
			UI_WORLDSTAGE_SORT_3,
			UI_WORLDSTAGE_SORT_4,
			UI_WORLDSTAGE_SORT_5,
			UI_WORLDSTAGE_SORT_INDICATOR,

			UI_WORLDSTAGE_LIST,
			UI_WORLDSTAGE_000,
			UI_WORLDSTAGE_999 = UI_WORLDSTAGE_000 + 999,
				UI_WORLDSTAGE_CREATORAVATAR,
				UI_WORLDSTAGE_DEVELOPERRATING,
				UI_WORLDSTAGE_PLAY,
				UI_WORLDSTAGE_LIKE,
				UI_WORLDSTAGE_CREATORCOMMENT,
				UI_WORLDSTAGE_CREATORCOMMENT_CONTENT,
				UI_WORLDSTAGE_PLAYERCOMMENT,
				UI_WORLDSTAGE_PLAYERCOMMENT_CONTENT,
		UI_CREDITS,
		UI_HELP,
		UI_EXIT,
	};
};

