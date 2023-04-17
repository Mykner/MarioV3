#include "Updater.h"

#include "Game.h"

#include "json.h"
#include "NaPlatformAPI.h"
#include "NaString.h"
#include "aes/md5.h"
#include "JsonManager.h"
#include "PathManager.h"

#include <Windows.h>
#pragma comment(lib,"version.lib")

// Update SecureKey #1
NaString Updater::s_strSecureKeyA = L"SFMB";

void UpdateInfo::Add(NaString strFileInfo)
{
	auto arValue = strFileInfo.Split(L":");
	UpdateFileItem item(arValue[0], arValue[1], arValue[2].ToInt());

	m_map.insert(
		std::pair<NaString, UpdateFileItem>(arValue[0], item)
	);
}

void UpdateInfo::Add(UpdateFileItem item)
{
	m_map.insert(
		std::pair<NaString, UpdateFileItem>(item.m_strFile, item)
	);
}

UpdateFileItem::UpdateFileItem(NaString strFile, NaString strHash, long lFileSize)
{
	m_strFile = strFile;
	m_strHash = strHash;
	m_lFileSize = lFileSize;
}

Updater::Updater(Game *pGame)
{
	m_pGame = pGame;
}

Updater::~Updater()
{
}

UpdateInfo Updater::GetUpdateInfo()
{
	UpdateInfo u;

	m_strRootDir = L"";
	if (m_strRootDir.GetLength() == 0)
		m_strRootDir = PathManager::GetGameRootPath(m_pGame);

	NaStrArray ar;
	CollectFileList(m_strRootDir, m_strRootDir, ar);

	for (int i = 0; i < ar.GetCount(); i++)
	{
		u.Add(ar[i]);
	}

	NaString strExe;
	strExe.Format(L"%ls\\Mario.exe", m_strRootDir.wstr());

	NaString strVersion = GetFileVersion(strExe);
	u.m_strVersion = strVersion.cstr();

	return u;
}

void Updater::MakeUpdateInfo(NaString strRootDir, NaString strSecureKey)
{
	m_strRootDir = strRootDir;
	if (m_strRootDir.GetLength() == 0)
		m_strRootDir = PathManager::GetGameRootPath(m_pGame);

	NaStrArray ar;
	CollectFileList(m_strRootDir, m_strRootDir, ar);

	Json::Value j;
	j["Files"] = Json::Value(Json::arrayValue);

	for (int i = 0; i < ar.GetCount(); i++)
	{
		j["Files"].append(ar[i].cstr());
	}

	NaString strExe;
	strExe.Format(L"%ls\\Mario.exe", m_strRootDir.wstr());

	NaString strVersion = GetFileVersion(strExe);
	j["Version"] = strVersion.cstr();

	if (strSecureKey.GetLength() > 0)
	{
		j["SecureKey"] = strSecureKey.cstr();
	}

	auto a1 = j.toStyledString();
	auto a2 = a1.c_str();
	auto a3 = NaString(a2);
	auto a4 = a3.wstr();

	NaString strOutput = m_strRootDir;
	strOutput += L"\\update.json";
	JsonManager::SaveFile(strOutput.cstr(), j);
}

UpdateInfo Updater::GetUpdatedFileList(UpdateInfo &ServerInfo, UpdateInfo &LocalInfo)
{
	UpdateInfo ret;

	auto it = ServerInfo.m_map.begin();
	for (; it != ServerInfo.m_map.end(); ++it)
	{
		bool bUpdate = false;
		NaString strLocalHash = L"<not found>";

		auto it2 = LocalInfo.m_map.find(it->second.m_strFile);
		if (it2 != LocalInfo.m_map.end())
		{
			// Compare Hash
			strLocalHash = it2->second.m_strHash;
			if (it->second.m_strHash.Compare(strLocalHash) != 0)
			{
				bUpdate = true;
			}
		}
		else
			bUpdate = true;

		if (bUpdate)
		{
			/*
			NaDebugOut(L"Diff: %ls (S:%ls <-> L:%ls)\n", 
				it->first.wstr(),
				it->second.m_strHash.wstr(),
				strLocalHash.wstr()
				);
				*/

			ret.Add(it->second);
		}
	}

	return ret;
}

void Updater::CollectFileList(NaString strCurrentDir, NaString &strRootDir, NaStrArray &ar)
{
	NaString strCurrentDir2 = strCurrentDir;
	strCurrentDir2 += L"\\";

	NaStrArray arFile = NaPlatform::GetFileList(strCurrentDir);
	NaString strIgnoreFile = L".git;.vahashtags;.mdj;.sln;.VC;.dmp;.bat;.lnk;MakeUpdateInfo.exe;update.json;Profile1.json";
	NaStrArray arIgnoreFile = strIgnoreFile.Split(L";");
	for (int i = 0; i < arFile.GetCount(); i++)
	{
		bool bIgnore = false;
		for (int j=0; j<arIgnoreFile.GetCount(); j++)
		{
			if (arFile[i].Find(const_cast<wchar_t*>(arIgnoreFile[j].wstr())) >= 0)
			{
				bIgnore = true;
				break;
			}
		}

		if (bIgnore)
			continue;

		NaString strFullPath;
		strFullPath.Format(L"%ls\\%ls", strCurrentDir.wstr(), arFile[i].wstr());

		NaString strHash = L"";
		long lFileSize = 0;
		if (GetFileHash(strFullPath, strHash, lFileSize))
		{
			/*
			NaDebugOut(L"file=%s\\%ls / %ls\n",
				strCurrentDir.Mid(strRootDir.GetLength() + 1).wstr(), arFile[i].wstr(), strHash.wstr()
			);
			///*/

			NaString strValue;
			strValue.Format(L"%ls%ls:%ls:%d",
				strCurrentDir2.Mid(strRootDir.GetLength() + 1).wstr(),
				arFile[i].wstr(),
				strHash.wstr(),
				lFileSize);

			ar.Add(strValue);
		}
		else
		{
			NaDebugOut(L"file=%s\\%ls\n",
				strCurrentDir.Mid(strRootDir.GetLength() + 1).wstr(), arFile[i].wstr());
		}
	}

	NaStrArray arDir = NaPlatform::GetSubDirectoryList(strCurrentDir);
	NaString strIgnoreDir = L".vs;.git;ipch;Output;References;src;UserData;Download;Profile";
	NaStrArray arIgnoreDir = strIgnoreDir.Split(L";");
	
	/*
	for (int i = 0; i < arDir.GetCount(); i++)
	{
		if (arIgnoreDir.Find(arDir[i]) != -1)
			continue;

		NaDebugOut(L"dir=%ls\\%ls\n",
			strCurrentDir.Mid(strRootDir.GetLength() + 1).wstr(), arDir[i].wstr());
	}
	*/

	for (int i = 0; i < arDir.GetCount(); i++)
	{
		if (arIgnoreDir.Find(arDir[i]) != -1)
			continue;

		NaString strNewDir;
		strNewDir.Format(L"%ls\\%ls", strCurrentDir.wstr(), arDir[i].wstr());
		CollectFileList(strNewDir, strRootDir, ar);
	}
}

NaString Updater::GetFileVersion(NaString strPath)
{
	NaString strVersion = L"";
	
	DWORD dwSize = GetFileVersionInfoSize(strPath.wstr(), 0);
	char *buffer = new char[dwSize];
	if (::GetFileVersionInfo(strPath.wstr(), 0, dwSize, buffer) != 0)
	{
		VS_FIXEDFILEINFO* pFineInfo = NULL;
		UINT bufLen = 0;

		if (VerQueryValue(buffer, L"\\", (LPVOID*)&pFineInfo, &bufLen) != 0)
		{
			WORD majorVer, minorVer, buildNum, revisionNum;
			majorVer = HIWORD(pFineInfo->dwFileVersionMS);
			minorVer = LOWORD(pFineInfo->dwFileVersionMS);
			buildNum = HIWORD(pFineInfo->dwFileVersionLS);
			revisionNum = LOWORD(pFineInfo->dwFileVersionLS);

			strVersion.Format(L"%d.%d.%d.%d", majorVer, minorVer, buildNum, revisionNum);
		}
	}
	delete[] buffer;

	return strVersion;
}

bool Updater::GetFileHash(NaString strPath, NaString &strHash, long &lFileSize)
{
	FILE *fp;
	NaPlatform::FileOpen(&fp, strPath.cstr(), "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		lFileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char *szBuf = new char[lFileSize + 1];
		fread(szBuf, sizeof(char), lFileSize, fp);
		szBuf[lFileSize] = '\0';

		// Make Hash
		unsigned char md5_value[MD5_SIZE];
		MD5_CTX md5;
		MD5Init(&md5);
		MD5Update(&md5, (unsigned char*)szBuf, lFileSize);
		MD5Final(&md5, md5_value);

		fclose(fp);
		delete[] szBuf;

		char szMD5[MD5_STR_LEN + 1];
		for (int j = 0; j < MD5_SIZE; j++)
		{
			snprintf(szMD5 + j * 2, 2 + 1, "%02x", md5_value[j]);
		}

		strHash = szMD5;

		return true;
	}

	return false;
}