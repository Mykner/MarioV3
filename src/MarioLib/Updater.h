#pragma once

#include "NaString.h"
#include "PropertyMap.h"

class UpdateFileItem;
class UpdateInfo
{
public:
	void Add(NaString strFileInfo);
	void Add(UpdateFileItem item);

	NaString m_strVersion;
	std::map<NaString, UpdateFileItem> m_map;
};

class UpdateFileItem
{
public:
	UpdateFileItem(NaString stRfile, NaString strHash, long lFileSize);
	~UpdateFileItem() {};

	NaString m_strFile;
	NaString m_strHash;
	long m_lFileSize;
};

class Game;
class Updater
{
public:
	Updater(Game *pGame);
	virtual ~Updater();

	UpdateInfo GetUpdateInfo();
	void MakeUpdateInfo(NaString strRootDir, NaString strSecureKey = L"");

	// Diff
	UpdateInfo GetUpdatedFileList(UpdateInfo &ServerInfo, UpdateInfo &LocalInfo);

	// Utils
	void CollectFileList(NaString strCurrentDir, NaString & strRootDir, NaStrArray & ar);
	NaString GetFileVersion(NaString strPath);
	bool GetFileHash(NaString strPath, NaString & strHash, long & lFileSize);

	Game *m_pGame;
	NaString m_strRootDir;

	static NaString s_strSecureKeyA;
};
