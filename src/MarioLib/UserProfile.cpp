#include "UserProfile.h"

#include "Player.h"
#include "Stage.h"
#include "Game.h"
#include "NaPlatformAPI.h"
#include "StagePack.h"
#include "JsonManager.h"
#include "PathManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(UserProfile)
	PROP_STR("Name", VT_ETC_NASTR, L"", 0, "UserProfile"),
	PROP_STR("SpecialCodes", VT_ETC_NASTR, L"", 0, "UserProfile"),
	PROP_STR("ClearedSingleStages", VT_ETC_NASTR, L"", 0, "UserProfile"),
	PROP_STR("LikedStages", VT_ETC_NASTR, L"", 0, "UserProfile"),
END_IMPL_PROPERTYMAP()

BEGIN_IMPL_PROPERTYMAP(StagePackRecord)
	PROP_STR("Name", VT_ETC_NASTR, L"", 0, "StagePackRecord"),
	PROP_STR("LastStage", VT_ETC_NASTR, L"", 0, "StagePackRecord"),
	PROP_STR("SelectableStages", VT_ETC_NASTR, L"1-1", 0, "StagePackRecord"),
	PROP_INT("Life", VT_I4, 3, 0, "StagePackRecord"),
	PROP_INT("Score", VT_I4, 0, 0, "StagePackRecord"),
	PROP_INT("Coin", VT_I4, 0, 0, "StagePackRecord"),
	PROP_INT("Shape", VT_I4, 0, 0, "StagePackRecord"),
END_IMPL_PROPERTYMAP()

BEGIN_IMPL_PROPERTYMAP(SingleStageRecord)
	PROP_STR("Name", VT_ETC_NASTR, L"", 0, "SingleStageRecord"),
	PROP_INT("BestTime", VT_I4, 0, 0, "SingleStageRecord"),
	PROP_INT("BestScore", VT_I4, 0, 0, "SingleStageRecord"),
END_IMPL_PROPERTYMAP()

UserProfile::UserProfile(Game *pGame)
{
    m_pGame = pGame;
    
	m_strName = L"";
	m_strSpecialCodes = L"ModernStyle";
	m_strClearedSingleStages = L"";
	m_strLikedStages = L"";
}

UserProfile::~UserProfile()
{
	{
		std::map<NaString, StagePackRecord*>::iterator it = m_mapStagePackRecord.begin();
		for (; it != m_mapStagePackRecord.end(); )
		{
			StagePackRecord *pRecord = it->second;
			delete pRecord;

			m_mapStagePackRecord.erase(it++);
		}
	}

	{
		std::map<NaString, SingleStageRecord*>::iterator it = m_mapSingleStageRecord.begin();
		for (; it != m_mapSingleStageRecord.end(); )
		{
			SingleStageRecord *pRecord = it->second;
			delete pRecord;

			m_mapSingleStageRecord.erase(it++);
		}
	}
}

void UserProfile::Load()
{
	if (m_pGame->m_Option.m_bTestMode)
		return;

	NaString strName;
	strName.Format(L"%ls\\%ls.json", PathManager::GetProfilePath(m_pGame), m_strFileName.wstr());
	NaDebugOut(L"Load Profile: %ls\n", m_strFileName.wstr());

	FILE *fp;
	NaPlatform::FileOpen(&fp, strName.cstr(), "rb");

	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		long lFileLen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char *szBuf = new char[lFileLen + 1];
		fread(szBuf, sizeof(char), lFileLen, fp);
		szBuf[lFileLen] = '\0';

		Json::Value j;
		Json::Reader jReader;
		bool bParseJson = jReader.parse(szBuf, j);
		if (bParseJson)
		{
			JsonManager::LoadJsonObjectProperty(this, j);

			// Load StagePack Record
			Json::Value jGroup = j["StagePackRecord"];

			// #TODO Leak!
			m_mapStagePackRecord.clear();
			for (int i = 0; i < jGroup.size(); i++)
			{
				Json::Value jObj = jGroup[i];
				StagePackRecord *pRecord = new StagePackRecord;
				pRecord->BuildPropertyMap();

				JsonManager::LoadJsonObjectProperty(pRecord, jObj);

				m_mapStagePackRecord.insert(
					std::pair<NaString, StagePackRecord*>(pRecord->m_strName, pRecord)
				);
			}

			// Load SingleStage Record
			jGroup = j["SingleStageRecord"];

			// #TODO Leak!
			m_mapSingleStageRecord.clear();
			for (int i = 0; i < jGroup.size(); i++)
			{
				Json::Value jObj = jGroup[i];
				SingleStageRecord *pRecord = new SingleStageRecord;
				pRecord->BuildPropertyMap();

				JsonManager::LoadJsonObjectProperty(pRecord, jObj);

				m_mapSingleStageRecord.insert(
					std::pair<NaString, SingleStageRecord*>(pRecord->m_strName, pRecord)
				);
			}
		}
		fclose(fp);
		delete[] szBuf;
	}
}

void UserProfile::Save()
{
	if (m_pGame->m_Option.m_bTestMode)
		return;

	NaString strName;
	strName.Format(L"%ls\\%ls.json", PathManager::GetProfilePath(m_pGame), m_strFileName.wstr());
	NaDebugOut(L"Save Profile: %ls (%ls)\n", m_strFileName.wstr(), m_strName.wstr());

	Json::Value j;
	JsonManager::SaveJsonObjectProperty(this, j);

	// StagePack Record
	{
		j["StagePackRecord"] = Json::Value();

		std::map<NaString, StagePackRecord*>::iterator it = m_mapStagePackRecord.begin();
		for (; it != m_mapStagePackRecord.end(); ++it)
		{
			StagePackRecord *pRecord = it->second;

			Json::Value jObj;
			JsonManager::SaveJsonObjectProperty(pRecord, jObj);

			j["StagePackRecord"].append(jObj);
		}
	}

	// SingleStage Record
	{
		j["SingleStageRecord"] = Json::Value();

		std::map<NaString, SingleStageRecord*>::iterator it = m_mapSingleStageRecord.begin();
		for (; it != m_mapSingleStageRecord.end(); ++it)
		{
			SingleStageRecord *pRecord = it->second;

			Json::Value jObj;
			JsonManager::SaveJsonObjectProperty(pRecord, jObj);

			j["SingleStageRecord"].append(jObj);
		}
	}

	FILE *fp;
    NaPlatform::FileOpen(&fp, strName.cstr(), "wb");

	if (fp)
	{
		std::string stdstr = j.toStyledString();
		fwrite(stdstr.c_str(), stdstr.length(), 1, fp);

		fclose(fp);
	}
}

StagePackRecord* UserProfile::GetStagePackRecord(NaString strName)
{
	if (m_mapStagePackRecord.find(strName) == m_mapStagePackRecord.end())
	{
		StagePackRecord *pRecord = new StagePackRecord;
		pRecord->BuildPropertyMap();
		pRecord->m_strName = strName;

		m_mapStagePackRecord.insert(
			std::pair<NaString, StagePackRecord*>(pRecord->m_strName, pRecord)
		);
	}

	StagePackRecord *pRecord = nullptr;
	std::map<NaString, StagePackRecord*>::iterator it = m_mapStagePackRecord.find(strName);
	if (it != m_mapStagePackRecord.end())
	{
		pRecord = it->second;
	}
	return pRecord;
}

SingleStageRecord * UserProfile::GetSingleStageRecord(NaString strName)
{
	if (m_mapSingleStageRecord.find(strName) == m_mapSingleStageRecord.end())
	{
		SingleStageRecord *pRecord = new SingleStageRecord;
		pRecord->BuildPropertyMap();
		pRecord->m_strName = strName;

		m_mapSingleStageRecord.insert(
			std::pair<NaString, SingleStageRecord*>(pRecord->m_strName, pRecord)
		);
	}

	SingleStageRecord *pRecord = nullptr;
	std::map<NaString, SingleStageRecord*>::iterator it = m_mapSingleStageRecord.find(strName);
	if (it != m_mapSingleStageRecord.end())
	{
		pRecord = it->second;
	}
	return pRecord;
}

int UserProfile::GetSingleStageBestScore(NaString strName)
{
	SingleStageRecord *pRecord = GetSingleStageRecord(strName);

	return pRecord->m_nBestScore;
}

int UserProfile::GetSingleStageBestTime(NaString strName)
{
	SingleStageRecord *pRecord = GetSingleStageRecord(strName);

	return pRecord->m_nBestTime;
}

NaString UserProfile::GetLastStage()
{
	StagePackRecord *pStatus = GetStagePackRecord(m_pGame->m_strStagePack);
	
	return pStatus->m_strLastStage;
}

NaString UserProfile::GetSelectableStages()
{
	StagePackRecord *pStatus = GetStagePackRecord(m_pGame->m_strStagePack);

	return pStatus->m_strSelectableStages;
}

void UserProfile::SetLastStage(NaString strName)
{
	StagePackRecord *pStatus = GetStagePackRecord(m_pGame->m_strStagePack);

	pStatus->m_strLastStage = strName;
}

void UserProfile::AddSelectableStage(NaString strName)
{
	if (strName.GetLength() == 0)
		return;
	StagePackRecord *pStatus = GetStagePackRecord(m_pGame->m_strStagePack);

	if (pStatus->m_strSelectableStages.GetLength() == 0)
		pStatus->m_strSelectableStages = strName;
	else
	{
		NaStrArray ar = pStatus->m_strSelectableStages.Split(L",");
		if (ar.Find(strName) < 0)
			ar.Add(strName);

		pStatus->m_strSelectableStages = ar.Join(L",");
	}
}

void UserProfile::AddClearedSingleStage(NaString strName, DWORD &dwTime, int &nScore)
{
	if (strName.GetLength() == 0)
		return;

	// Add Cleared List
	if (m_strClearedSingleStages.GetLength() == 0)
		m_strClearedSingleStages = strName;
	else
	{
		NaStrArray ar = m_strClearedSingleStages.Split(L",");
		if (ar.Find(strName) < 0)
			ar.Add(strName);

		m_strClearedSingleStages = ar.Join(L",");
	}

	// Add SingleStage Record
	SingleStageRecord *pRecord = GetSingleStageRecord(strName);
	if (dwTime < pRecord->m_nBestTime || pRecord->m_nBestTime == 0)
	{
		pRecord->m_nBestTime = dwTime;
	}

	if (nScore > pRecord->m_nBestScore)
		pRecord->m_nBestScore = nScore;

	dwTime = pRecord->m_nBestTime;
	nScore = pRecord->m_nBestScore;
}

void UserProfile::AddLikedStage(NaString strUniqueId)
{
	if (m_strLikedStages.GetLength() == 0)
		m_strLikedStages = strUniqueId;
	else
	{
		NaStrArray ar = m_strLikedStages.Split(L",");
		if (ar.Find(strUniqueId) < 0)
			ar.Add(strUniqueId);

		m_strLikedStages = ar.Join(L",");
	}
}

void UserProfile::Update(Player * pPlayer)
{
	if (m_pGame->m_nStagePackType != STAGEPACK_SINGLE)
	{
		// Save Current State to StagePackRecord
		StagePackRecord *pStatus = GetStagePackRecord(m_pGame->m_strStagePack);

		pStatus->m_nScore = pPlayer->m_nScore;
		pStatus->m_nLife = pPlayer->m_nLife;
		pStatus->m_nCoin = pPlayer->m_nCoin;

		if (pPlayer->m_pControlObj)
			pStatus->m_nShape = pPlayer->m_pControlObj->m_nShape;
	}

	m_strSpecialCodes = L"";

	NaStrArray ar;
	if (pPlayer->m_pGame->m_bModernStyle)
		ar.Add(L"ModernStyle");
	if (pPlayer->m_pGame->m_nSpecialEvents != 0)
	{
		int nEvents = pPlayer->m_pGame->m_nSpecialEvents;
		if (nEvents & SPECIALEVENT_GIANTMUSHROOM)
			ar.Add(L"SpecialEventGiantMushroom");
		if (nEvents & SPECIALEVENT_GIANTPREVENTHOLE)
			ar.Add(L"SpecialEventGiantPreventHole");
	}

	if (ar.GetCount() > 0)
		m_strSpecialCodes = ar.Join(L",");
}

//
// StagePackRecord class
//
StagePackRecord::StagePackRecord()
{
	m_strName = L"";
	m_strLastStage = L"";
	m_strSelectableStages = L"";
	m_nLife = 3;
	m_nScore = 0;
	m_nCoin = 0;
	m_nShape = SHAPE_MINI;
}

StagePackRecord::~StagePackRecord()
{

}

//
// SingleStageRecord class
//
SingleStageRecord::SingleStageRecord()
{
	m_strName = L"";
	m_nBestScore = 0;
	m_nBestTime = 0;
}

SingleStageRecord::~SingleStageRecord()
{
}
