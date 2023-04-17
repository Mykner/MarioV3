#pragma once

#include "PropertyMap.h"
#include <map>

class Player;
class Game;
class StagePackRecord;
class SingleStageRecord;
class UserProfile : public PropertyObjectBase
{
public:
	UserProfile(Game *pGame);
	virtual ~UserProfile();

	void Load();
	void Save();

	StagePackRecord* GetStagePackRecord(NaString strName);
	SingleStageRecord* GetSingleStageRecord(NaString strName);
	
	int GetSingleStageBestScore(NaString strName);
	int GetSingleStageBestTime(NaString strName);

	NaString GetLastStage();
	NaString GetSelectableStages();

	void SetLastStage(NaString strName);
	void AddSelectableStage(NaString strName);
	void AddClearedSingleStage(NaString strName, DWORD &dwTime, int &nScore);
	void AddLikedStage(NaString strUniqueId);

	void Update(Player *pPlayer);

    Game *m_pGame;
	NaString m_strFileName;

	// Property Members
	NaString m_strName;
	NaString m_strSpecialCodes;
	NaString m_strClearedSingleStages;
	NaString m_strLikedStages;
	std::map<NaString, StagePackRecord*> m_mapStagePackRecord;
	std::map<NaString, SingleStageRecord*> m_mapSingleStageRecord;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Name", &m_strName },
		{ L"SpecialCodes", &m_strSpecialCodes },
		{ L"ClearedSingleStages", &m_strClearedSingleStages },
		DECL_PROP("LikedStages", &m_strLikedStages)
	END_DECLARE_PROPERTYMAP()
};

class StagePackRecord : public PropertyObjectBase
{
public:
	StagePackRecord();
	virtual ~StagePackRecord();

	NaString m_strName;
	NaString m_strLastStage;
	NaString m_strSelectableStages;
	int m_nLife;
	int m_nScore;
	int m_nCoin;
	int m_nShape;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Name", &m_strName },
		{ L"LastStage", &m_strLastStage },
		{ L"SelectableStages", &m_strSelectableStages },
		{ L"Life", &m_nLife },
		{ L"Score", &m_nScore },
		{ L"Coin", &m_nCoin },
		{ L"Shape", &m_nShape },
	END_DECLARE_PROPERTYMAP()
};

class SingleStageRecord : public PropertyObjectBase
{
public:
	SingleStageRecord();
	virtual ~SingleStageRecord();

	NaString m_strName;
	int m_nBestTime;
	int m_nBestScore;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"Name", &m_strName },
		{ L"BestScore", &m_nBestScore },
		{ L"BestTime", &m_nBestTime },
	END_DECLARE_PROPERTYMAP()
};