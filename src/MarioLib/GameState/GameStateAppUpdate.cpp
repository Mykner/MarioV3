#include "GameStateAppUpdate.h"

#include "Player.h"
#include "Stage.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UIText.h"
#include "UIEdit.h"
#include "UIBase.h"

#include "ServerRequest.h"
#include "json.h"
#include "Updater.h"
#include "PathManager.h"
#include "NaPlatformAPI.h"

GameStateAppUpdate::GameStateAppUpdate(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_APPUPDATE;

	m_pCheckUpdate = nullptr;
	m_strSecondSecureKey = L"";
	m_nUpdateProgress = UPDATE_UNDEFINED;

	m_lDownloadSize = 0;
	m_lDownloadedSize = 0;

	m_nUpdatedFlag = 0;
}

GameStateAppUpdate::~GameStateAppUpdate()
{
}

long GameStateAppUpdate::GetBackgroundColor()
{
	return 0;
}

void GameStateAppUpdate::Init()
{
	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlayBGM(L"100MarioMap");

	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	InitCheckUpdateUI();
}

void GameStateAppUpdate::Process()
{
	if (m_nUpdateProgress == UPDATE_CHECKED && m_nUpdatedFlag == 1)
	{
		m_nUpdateProgress = UPDATE_CHECKSECUREKEY;
		InitFirstSecureKeyUI();
	}

	if (m_nUpdateProgress == UPDATE_DONE)
	{
		m_nUpdateProgress = UPDATE_UNDEFINED;
		::PostQuitMessage(0);
	}

	if (m_nUpdateProgress == UPDATE_DENY)
	{
		if (m_pUpdateThread)
		{
			m_pUpdateThread->join();
			delete m_pUpdateThread;
			m_pUpdateThread = nullptr;
		}

		InitDenyUI();
	}

	if (m_nUpdateProgress == UPDATE_LATESTVERSION)
	{
		if (m_pUpdateThread)
		{
			m_pUpdateThread->join();
			delete m_pUpdateThread;
			m_pUpdateThread = nullptr;
		}

		m_pGame->ChangeState(GAMESTATE_TITLE);
	}

	UI_MANAGER->Process();

	if (KE(VK_F9) && KS(VK_SHIFT))
	{
		Updater updater(m_pGame);
		auto LocalInfo = updater.GetUpdateInfo();

		// Check Diff
		auto UpdatedInfo = updater.GetUpdatedFileList(m_ServerInfo, LocalInfo);
		if (UpdatedInfo.m_map.size() > 0)
		{
			NaString str = L"";
			auto it = UpdatedInfo.m_map.begin();
			for (; it != UpdatedInfo.m_map.end(); ++it)
			{
				NaString strDownloadPath;
				strDownloadPath.Format(L"%ls/UserData/Update/%ls",
					PathManager::GetGameRootPath(nullptr),
					it->first.wstr());
				strDownloadPath.ReplaceAll(L"/", L"\\");

				NaString strLocalPath = strDownloadPath;
				strLocalPath.ReplaceAll(L"UserData\\Update\\", L"");

				str += strLocalPath;
				str += L"\n";
			}

			::MessageBox(nullptr, str, L"Debug", MB_OK);
		}
	}

	if (UI_MANAGER->m_pClicked)
	{
		int nIDUI = UI_MANAGER->m_pClicked->m_nID;
		switch (nIDUI)
		{
		case UI_FIRST_SECUREKEY:
			{
				bool bValidKey = false;
				UIEdit *pUI = (UIEdit*)UI_MANAGER->GetFocus();
				if (pUI->m_strText.GetLength() > 0)
				{
					if (pUI->m_strText.CompareNoCase(Updater::s_strSecureKeyA) == 0)
						bValidKey = true;
				}

				if (bValidKey)
				{
					UI_MANAGER->PopPanel();

					InitSecondSecureKeyUI();
					SOUND_MANAGER->PlaySoundEffect(L"1UP");
				}
				else
				{
					SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");
					InitDenyUI();					
				}
			}
			break;
		case UI_SECOND_SECUREKEY:
			{
				bool bValidKey = false;
				UIEdit *pUI = (UIEdit*)UI_MANAGER->GetFocus();
				if (pUI->m_strText.GetLength() > 0)
				{
					m_strSecondSecureKey = pUI->m_strText;
				
					// Check secureKey #2
					if (m_strSecondSecureKey.CompareNoCase(m_strServerSecureKey) == 0)
						bValidKey = true;
				}

				if (bValidKey)
				{
					UI_MANAGER->PopPanel();
					InitUpdateUI();
				}
				else
				{
					SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");
					InitDenyUI();
				}
			}
			break;
		}
	}
}

void GameStateAppUpdate::Render()
{
	UI_MANAGER->Render();

	if (m_lDownloadSize != 0 && m_lDownloadedSize != 0)
	{
		NaRect rc;
		rc.left = 170;
		rc.right = m_pGame->m_nWidth - 170;
		rc.top = m_pGame->m_nHeight / 2 + 8;
		rc.bottom = rc.top + 8;

		UIBase u(m_pGame);
		u.RenderBox(rc, 0xff777777);

		float fMax = rc.Width();
		float fPer = ((float)m_lDownloadedSize / (float)m_lDownloadSize);
		float fCur = fMax * fPer;
		rc.right = rc.left + fCur;
		u.RenderBox(rc, 0xffffffff);
	}
}

// SelfUpdater
void GameStateAppUpdate::CheckUpdateThread(void * pParam)
{
	GameStateAppUpdate *pThis = reinterpret_cast<GameStateAppUpdate *>(pParam);
	Game *m_pGame = pThis->m_pGame;
	UIText *pUI = reinterpret_cast<UIText*>(pThis->m_pCheckUpdate);

	// Get server info
	UpdateInfo ServerInfo;
	ServerRequest req;
	Json::Value jUpdateInfo;

	if (req.CheckUpdate(jUpdateInfo))
	{
		ServerInfo.m_strVersion = jUpdateInfo["Version"].asString().c_str();
		auto date = jUpdateInfo["Date"].asString();
		auto files = jUpdateInfo["Files"];
		pThis->m_strServerSecureKey = jUpdateInfo["SecureKey"].asString().c_str();

		for (int i = 0; i < files.size(); i++)
		{
			Json::Value jValue = files[i];
			NaString strValue = jValue.asString().c_str();

			ServerInfo.Add(strValue);
		}
	}
	else
	{
		NaDebugOut(L"Failed to access update server.\n");
		SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");

		pUI->m_strText = L"Failed " WSZ_BAD;
		::Sleep(5000);
		pThis->m_nUpdateProgress = UPDATE_DENY;
		return;
	}

	// Get local info
	Updater updater(m_pGame);
	auto LocalInfo = updater.GetUpdateInfo();

	// Check Diff
	auto UpdatedInfo = updater.GetUpdatedFileList(ServerInfo, LocalInfo);
	if (UpdatedInfo.m_map.size() > 0)
	{
		pThis->m_ServerInfo = ServerInfo;
		pThis->m_nUpdatedFlag = 1;
		pThis->m_nUpdateProgress = UPDATE_CHECKED;
	}
	else
	{
		// Nothing changed
		pThis->m_nUpdatedFlag = -1;
		pThis->m_nUpdateProgress = UPDATE_LATESTVERSION;
	}
}

void GameStateAppUpdate::UpdateThread(void * pParam)
{
	GameStateAppUpdate *pThis = reinterpret_cast<GameStateAppUpdate *>(pParam);
	Game *m_pGame = pThis->m_pGame;
	UIText *pUI = reinterpret_cast<UIText*>(pThis->m_pCheckUpdate);

	// Get server info
	UpdateInfo ServerInfo = pThis->m_ServerInfo;
	ServerRequest req;
	Json::Value jUpdateInfo;

	NaString strVersion = ServerInfo.m_strVersion;
	
	// Get local info
	Updater updater(m_pGame);
	auto LocalInfo = updater.GetUpdateInfo();

	// Check Diff
	auto UpdatedInfo = updater.GetUpdatedFileList(ServerInfo, LocalInfo);
	if (UpdatedInfo.m_map.size() > 0)
	{
		// Size calculation
		auto it = UpdatedInfo.m_map.begin();
		long lTotalSize = 0;
		for (; it != UpdatedInfo.m_map.end(); ++it)
		{
			lTotalSize += it->second.m_lFileSize;
		}

		pThis->m_nUpdateProgress = UPDATE_DOWNLOAD;
		pThis->m_lDownloadSize = lTotalSize;
		pUI->m_strText.Format(L"Downloading %ls...", strVersion.wstr());
		reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcBoxSize();
		reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcAlignPos();

		int nDownloaded = 0;
		req.DownloadUpdateFiles(UpdatedInfo, [&](long lSize)
		{
			nDownloaded++;

			pUI->m_strText.Format(L"Downloading %ls (%d/%d)...",
				strVersion.wstr(),
				nDownloaded, UpdatedInfo.m_map.size());
			reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcBoxSize();
			reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcAlignPos();

			pThis->m_lDownloadedSize += lSize;

			wchar_t* szSounds[] =
			{
				L"1UP",
				L"ItemPop",
				L"JumpSmall",
				L"ClapLong",
				L"Coin",
				L"ThrowFireball",
				L"Cannon",
				L"PowerUp",
				L"PowerDown",
			};

			int nSoundLen = sizeof(szSounds) / sizeof(wchar_t*);
			int nRandom = rand() % (nSoundLen);
			SOUND_MANAGER->PlaySoundEffect(szSounds[nRandom]);

			Sleep(1);
		});

		pThis->m_lDownloadedSize = pThis->m_lDownloadSize;
		Sleep(1);

		// Update (Copy files)
		pThis->m_nUpdateProgress = UPDATE_PATCH;
		pUI->m_strText = L"Patching...";
		reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcBoxSize();
		reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcAlignPos();

		it = UpdatedInfo.m_map.begin();
		for (; it != UpdatedInfo.m_map.end(); ++it)
		{
			NaString strDownloadPath;
			strDownloadPath.Format(L"%ls/UserData/Update/%ls",
				PathManager::GetGameRootPath(nullptr),
				it->first.wstr());
			strDownloadPath.ReplaceAll(L"/", L"\\");

			NaString strLocalPath = strDownloadPath;
			strLocalPath.ReplaceAll(L"UserData\\Update\\", L"");

			if (strLocalPath.Right(4).CompareNoCase(L".exe") == 0)
			{
				NaString strDeletePath = strLocalPath;
				strDeletePath.ReplaceAll(L".exe", L".delete");
				::MoveFile(strLocalPath.wstr(), strDeletePath.wstr());
			}

			// Make Path
			NaStrArray arLocalPath = strLocalPath.Split(L"\\");
			arLocalPath.Pop();
			NaPlatform::CheckAndCreatePath(arLocalPath.Join(L"\\"));

			NaDebugOut(L"CopyTo: %ls\n", strLocalPath.wstr());
			::CopyFile(strDownloadPath.wstr(), strLocalPath.wstr(), 0);
			Sleep(1);
		}

		// Clear Update(download) folder
		it = UpdatedInfo.m_map.begin();
		for (; it != UpdatedInfo.m_map.end(); ++it)
		{
			NaString strDownloadPath;
			strDownloadPath.Format(L"%ls/UserData/Update/%ls",
				PathManager::GetGameRootPath(nullptr),
				it->first.wstr());
			strDownloadPath.ReplaceAll(L"/", L"\\");

			NaDebugOut(L"Delete: %ls\n", strDownloadPath.wstr());
			::DeleteFile(strDownloadPath.wstr());
		}

		pUI->m_strText = L"Update Complete!";
		reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcBoxSize();
		reinterpret_cast<UIPanel*>(pUI->m_pParent)->RecalcAlignPos();

		SOUND_MANAGER->PlaySoundEffect(L"1UP");
		Sleep(5000);

		// Re-launch
		NaString strMarioExe = PathManager::GetGameRootPath(m_pGame);
		strMarioExe += L"\\Mario.exe";

		ShellExecute(
			nullptr,
			L"open",
			strMarioExe.wstr(),
			nullptr,
			nullptr,
			SW_SHOW
		);

		// Exit myself
		pThis->m_nUpdateProgress = UPDATE_DONE;
		return;
	}
}

void GameStateAppUpdate::InitCheckUpdateUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	m_pCheckUpdate = UI_MANAGER->CreateText(UI_UNDEFINED, 0, 0, L"Checking update...");
	m_pUpdateThread = new std::thread(CheckUpdateThread, this);
}

void GameStateAppUpdate::InitFirstSecureKeyUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	UI_MANAGER->CreateText(UI_UNDEFINED, 0, 0, L"Type SecureKey #1");
	auto pUI = UI_MANAGER->CreateEdit(UI_FIRST_SECUREKEY, 0, 12, L"", L"-> ");
	UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
	pEdit->m_strEmptyText = L"";
	pEdit->m_bIsPassword = true;
}

void GameStateAppUpdate::InitSecondSecureKeyUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	UI_MANAGER->CreateText(UI_UNDEFINED, 0, 0, L"Type SecureKey #2");
	auto pUI = UI_MANAGER->CreateEdit(UI_SECOND_SECUREKEY, 0, 12, L"", L"-> ");
	UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
	pEdit->m_strEmptyText = L"";
	pEdit->m_bIsPassword = true;
}

void GameStateAppUpdate::InitUpdateUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	m_pCheckUpdate = UI_MANAGER->CreateText(UI_CHECK_UPDATE, 0, 0, L"Updating...");
	m_pUpdateThread = new std::thread(UpdateThread, this);
}

void GameStateAppUpdate::InitDenyUI()
{
	UI_MANAGER->PopPanel();

	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	UI_MANAGER->CreateText(UI_UNDEFINED, 0, 0, L"YOU DON'T HAVE UPDATE PERMISSION");
}
