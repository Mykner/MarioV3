#pragma once

#include "GameStateBase.h"
#include "Game.h"

#include <thread>

class UIBase;
class GameStateAppUpdate : public GameStateBase
{
public:
	GameStateAppUpdate(Game *pGame);
	virtual ~GameStateAppUpdate();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();
	
	static void CheckUpdateThread(void *pParam);
	static void UpdateThread(void *pParam);

	void InitCheckUpdateUI();
	void InitFirstSecureKeyUI();
	void InitSecondSecureKeyUI();
	void InitUpdateUI();
	void InitDenyUI();

	std::thread *m_pUpdateThread;
	UIBase *m_pCheckUpdate;
	NaString m_strSecondSecureKey;
	int m_nUpdateProgress;
	long m_lDownloadSize;
	long m_lDownloadedSize;

	int m_nUpdatedFlag;
	UpdateInfo m_ServerInfo;
	NaString m_strServerSecureKey;

	enum GameStateAppUpdateUI
	{
		UI_UNDEFINED = 0,

		UI_FIRST_SECUREKEY,
		UI_SECOND_SECUREKEY,
		UI_CHECK_UPDATE
	};

	enum GameUpdateProgress
	{
		UPDATE_UNDEFINED = 0,

		UPDATE_CHECKED,
		UPDATE_CHECKSECUREKEY,

		UPDATE_DENY,
		UPDATE_DOWNLOAD,
		UPDATE_PATCH,
		UPDATE_DONE,
		UPDATE_LATESTVERSION,
	};
};

