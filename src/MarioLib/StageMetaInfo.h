#pragma once

#include "NaString.h"
#include "PropertyMap.h"

enum StageMetaInfoDifficultyFilters
{
	STAGEMETAINFO_DIFFICULTY_FILTER_ALL = 0,
	STAGEMETAINFO_DIFFICULTY_FILTER_EASY,
	STAGEMETAINFO_DIFFICULTY_FILTER_NORMAL,
	STAGEMETAINFO_DIFFICULTY_FILTER_HARD,
	STAGEMETAINFO_DIFFICULTY_FILTER_HELL,
};

enum StageMetaInfoThemeFilters
{
	STAGEMETAINFO_THEME_FILTER_ALL = 0,
	STAGEMETAINFO_THEME_FILTER_SMB1,
	STAGEMETAINFO_THEME_FILTER_SMB3,
	STAGEMETAINFO_THEME_FILTER_SMBW,
	STAGEMETAINFO_THEME_FILTER_CUSTOM,
};

class Game;
class StageMetaInfo : public PropertyObjectBase
{
public:
	StageMetaInfo();
	virtual ~StageMetaInfo();
	
	void CheckDownloaded(Game *pGame);
	float CalcClearRate() const;
	int CalcDifficulty() const;
	int CalcGameTheme();

	int m_nIndex;
	NaString m_strUniqueId;
	NaString m_strFilePath;
	int m_nLocalRevision;

	NaString m_strTitle;
	int m_nRevision;
	int m_nDeveloperRating;
	int m_nLike;
	int m_nDownload;
	NaString m_strCreator;
	NaString m_strCreatorUniqueId;
	NaString m_strCreatorComment;
	int m_nCleared;
	int m_nFailed;
	NaString m_strGameTheme;

	enum StageMataInfoSortOptions 
	{
		STAGEMETAINFO_SORT_INDEX = 1,
		STAGEMETAINFO_SORT_RATING,
		STAGEMETAINFO_SORT_CLEARRATE,
		STAGEMETAINFO_SORT_DOWNLOAD,
		STAGEMETAINFO_SORT_DEVLOPERRATING,
	};
	static int s_nStageMataInfoSortOption;
	static int s_nStageMataInfoDifficultyFilter;
	static int s_nStageMataInfoThemeFilter;

	static bool Compare(const StageMetaInfo *t1, const StageMetaInfo *t2) 
	{
		if (s_nStageMataInfoDifficultyFilter != 0)
		{
			int c1 = t1->CalcDifficulty();
			int c2 = t2->CalcDifficulty();
			if (c1 != c2)
			{
				if (c1 == s_nStageMataInfoDifficultyFilter)
					return true;
				else if (c2 == s_nStageMataInfoDifficultyFilter)
					return false;
			}
		}

		if (s_nStageMataInfoThemeFilter != 0)
		{
			int c1 = const_cast<StageMetaInfo*>(t1)->CalcGameTheme();
			int c2 = const_cast<StageMetaInfo*>(t2)->CalcGameTheme();
			if (c1 != c2)
			{
				if (c1 == s_nStageMataInfoThemeFilter)
					return true;
				else if (c2 == s_nStageMataInfoThemeFilter)
					return false;
			}
		}

		switch (s_nStageMataInfoSortOption)
		{
		case STAGEMETAINFO_SORT_INDEX:
		default:
			return (t1->m_nIndex > t2->m_nIndex);
			break;
		case -STAGEMETAINFO_SORT_INDEX:
			return (t1->m_nIndex < t2->m_nIndex);
			break;
		case STAGEMETAINFO_SORT_RATING:
			return (t1->m_nLike > t2->m_nLike);
			break;
		case -STAGEMETAINFO_SORT_RATING:
			return (t1->m_nLike < t2->m_nLike);
			break;
		case STAGEMETAINFO_SORT_CLEARRATE:
		case -STAGEMETAINFO_SORT_CLEARRATE:
			{
				float f1, f2;
				if (t1->m_nCleared == 0 && t1->m_nFailed == 0)
					f1 = -1;
				else
					f1 = (t1->m_nCleared * 100.0f) / (t1->m_nCleared + t1->m_nFailed);

				if (t2->m_nCleared == 0 && t2->m_nFailed == 0)
					f2 = -1;
				else
					f2 = (t2->m_nCleared * 100.0f) / (t2->m_nCleared + t2->m_nFailed);

				if (s_nStageMataInfoSortOption == STAGEMETAINFO_SORT_CLEARRATE)
					return (f1 > f2);
				else
					return (f1 < f2);
			}
			break;
		case STAGEMETAINFO_SORT_DOWNLOAD:
			return (t1->m_nDownload > t2->m_nDownload);
			break;
		case -STAGEMETAINFO_SORT_DOWNLOAD:
			return (t1->m_nDownload < t2->m_nDownload);
			break;
		case STAGEMETAINFO_SORT_DEVLOPERRATING:
			return (t1->m_nDeveloperRating > t2->m_nDeveloperRating);
			break;
		case -STAGEMETAINFO_SORT_DEVLOPERRATING:
			return (t1->m_nDeveloperRating < t2->m_nDeveloperRating);
			break;
		}

		return false;
	};

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Title", &m_strTitle)
		DECL_PROP("Revision", &m_nRevision)
		DECL_PROP("DeveloperRating", &m_nDeveloperRating)
		DECL_PROP("Like", &m_nLike)
		DECL_PROP("Download", &m_nDownload)
		DECL_PROP("Creator", &m_strCreator)
		DECL_PROP("CreatorUniqueId", &m_strCreatorUniqueId)
		DECL_PROP("CreatorComment", &m_strCreatorComment)
		DECL_PROP("Cleared", &m_nCleared)
		DECL_PROP("Failed", &m_nFailed)
		DECL_PROP("GameTheme", &m_strGameTheme)
	END_DECLARE_PROPERTYMAP()
};
