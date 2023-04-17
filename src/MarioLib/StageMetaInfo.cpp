#include "StageMetaInfo.h"

#include "Game.h"
#include "NaPlatformAPI.h"
#include "Stage.h"
#include "JsonManager.h"
#include "PathManager.h"
#include "Stage.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(StageMetaInfo)
	PROP_STR("Title", VT_ETC_NASTR, L"", 0, "StageMetaInfo"),
	PROP_INT("Revision", VT_I4, 1, 0, "StageMetaInfo"),
	PROP_INT("DeveloperRating", VT_I4, 0, 0, "StageMetaInfo"),
	PROP_INT("Like", VT_I4, 0, 0, "StageMetaInfo"),
	PROP_INT("Download", VT_I4, 0, 0, "StageMetaInfo"),
	PROP_STR("Creator", VT_ETC_NASTR, L"", 0, "StageMetaInfo"),
	PROP_STR("CreatorUniqueId", VT_ETC_NASTR, L"", 0, "StageMetaInfo"),
	PROP_STR("CreatorComment", VT_ETC_NASTR, L"", 0, "StageMetaInfo"),
	PROP_INT("Cleared", VT_I4, 0, 0, "StageMetaInfo"),
	PROP_INT("Failed", VT_I4, 0, 0, "StageMetaInfo"),
	PROP_STR("GameTheme", VT_ETC_NASTR, L"", 0, "StageMetaInfo"),
END_IMPL_PROPERTYMAP()

int StageMetaInfo::s_nStageMataInfoSortOption = StageMetaInfo::STAGEMETAINFO_SORT_INDEX;
int StageMetaInfo::s_nStageMataInfoDifficultyFilter = 0;
int StageMetaInfo::s_nStageMataInfoThemeFilter = 0;

StageMetaInfo::StageMetaInfo()
{
	m_nIndex = 0;
	m_strUniqueId = L"";
	m_strFilePath = L"";
	m_nLocalRevision = -1;

	m_strTitle = L"";
	m_nRevision = 1;
	m_nDeveloperRating = -1;
	m_nLike = 0;
	m_nDownload = 0;
	
	m_strCreator = L"";
	m_strCreatorUniqueId = L"";
	m_strCreatorComment = L"";

	m_nCleared = 0;
	m_nFailed = 0;
	m_strGameTheme = L"?";
}

StageMetaInfo::~StageMetaInfo()
{
}

void StageMetaInfo::CheckDownloaded(Game *pGame)
{
	// Check already downloaded
	NaString strMapDownloadPath = PathManager::GetMapDownloadPath(pGame);
	m_strFilePath.Format(L"%ls\\%ls.map", strMapDownloadPath.wstr(), m_strUniqueId.wstr());

	Json::Value *j = JsonManager::LoadFile(m_strFilePath.cstr(), true);
	if (j)
	{
		Stage *pStage = new Stage(pGame);
		pStage->BuildPropertyMap();

		JsonManager::LoadJsonObjectProperty(pStage, *j);
		delete j;

		m_nLocalRevision = pStage->m_nRevision;
	}
	else
	{
		m_nLocalRevision = -1;
	}
}

float StageMetaInfo::CalcClearRate() const
{
	if (m_nCleared == 0)
		return -1;

	return (m_nCleared * 100.0f) / (m_nCleared + m_nFailed);
}

int StageMetaInfo::CalcDifficulty() const
{
	float fClearRate = CalcClearRate();
	if (fClearRate == 0)
		return 5;
	else if (fClearRate > 0 && fClearRate <= 10)
		return 4;
	else if (fClearRate > 10 && fClearRate <= 40)
		return 3;
	else if (fClearRate > 40 && fClearRate <= 90)
		return 2;
	else if (fClearRate > 90)
		return 1;

	return 5;
}

int StageMetaInfo::CalcGameTheme()
{
	if (m_strGameTheme.CompareNoCase(L"SMB") == 0 ||
		m_strGameTheme.CompareNoCase(L"SMB1") == 0 ||
		m_strGameTheme.GetLength() == 0)
		return 1;
	else if (m_strGameTheme.CompareNoCase(L"SMB3") == 0)
		return 2;
	else if (m_strGameTheme.CompareNoCase(L"SMW") == 0)
		return 3;
	else if (m_strGameTheme.GetLength() > 0)
		return 4;

	return 0;
}
