#include "GameStateRegisterUser.h"

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
#include "aes/md5.h"
#include "GameConfig.h"

GameStateRegisterUser::GameStateRegisterUser(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_REGISTERUSER;

	m_Transaction.Init(pGame);
}

GameStateRegisterUser::~GameStateRegisterUser()
{
}

long GameStateRegisterUser::GetBackgroundColor()
{
	return 0;
}

void GameStateRegisterUser::Init()
{
	SOUND_MANAGER->StopBGM();
	SOUND_MANAGER->PlayBGM(L"10MarioMap");

	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	// Collect user's info
	m_strMacAddress = NaPlatform::GetMacAddress();
	m_strComputerName = NaPlatform::GetComputerName();
	NaDebugOut(L"MacAddress: %ls\n", m_strMacAddress.wstr());
	NaDebugOut(L"Computer name: %ls\n", m_strComputerName.wstr());

	NaString str;
	str.Format(L"%ls%ls", m_strMacAddress.wstr(), m_strComputerName.wstr());

	char szMD5[MD5_STR_LEN + 1];
	Compute_string_md5((unsigned char*)str.wstr(), str.GetBufferSize(), szMD5);
	m_strLocalKey = szMD5;
	
	auto pConfig = GameConfig::GetGameConfig(m_pGame);
	if (pConfig->m_strUniqueId.GetLength() > 0)
		m_strUserUniqueId = pConfig->m_strUniqueId;
	else
		m_strUserUniqueId = L"";

	InitCheckTicketUI();
}

void GameStateRegisterUser::Process()
{
	UI_MANAGER->Process();

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();

		switch (m_Transaction.m_nJobType)
		{
		case JOB_CHECK_REGISTER_TICKET_END:
			InitRegisterTicketCodeUI(m_Transaction.m_Response.pServerRegisterTicket);
			break;
		case JOB_CHECK_REGISTER_TICKET_FAIL:
			InitRegisterTicketUI();
			break;
		case JOB_REGISTER_TICKET_END:
			InitCheckTicketUI();
			break;
		case JOB_REGISTER_TICKET_CODE_END:
			InitRegisterUserUI();
			break;
		case JOB_REGISTER_TICKET_FAIL:
		case JOB_REGISTER_TICKET_CODE_FAIL:
			InitWrongPermissionUI();
			break;
		case JOB_REGISTER_USER_END:
			SOUND_MANAGER->PlaySoundEffect(L"MarioLetsGo");
			m_pGame->ChangeState(GAMESTATE_LOGO);
			break;
		}

		// #TODO Handling download fail

		m_Transaction.ChangeJob(JOB_NONE);
	}

	if (!m_Transaction.IsWorking() && UI_MANAGER->m_pClicked)
	{
		int nIDUI = UI_MANAGER->m_pClicked->m_nID;
		switch (nIDUI)
		{
		case UI_REGISTERCODE:
			{
				m_Transaction.m_pJobThread = new std::thread(RegisterTicketCode, this);
			}
			break;
		case UI_SUBMIT_TICKET:
			{
				auto pUI = UI_MANAGER->GetControl(UI_USERNAME, UI_MANAGER->m_pCurPanel);
				if (pUI)
				{
					UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
					if (pEdit->m_strText.GetLength() == 0)
					{
						SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");
						return;
					}
				}

				pUI = UI_MANAGER->GetControl(UI_PASSWORD, UI_MANAGER->m_pCurPanel);
				if (pUI)
				{
					UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
					if (pEdit->m_strText.GetLength() == 0)
					{
						SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");
						return;
					}
				}

				pUI = UI_MANAGER->GetControl(UI_SECUREWORD, UI_MANAGER->m_pCurPanel);
				if (pUI)
				{
					UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
					if (pEdit->m_strText.GetLength() == 0)
					{
						SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");
						return;
					}
				}

				m_Transaction.m_pJobThread = new std::thread(RegisterTicket, this);
			}
			break;
		}
	}
}

void GameStateRegisterUser::Render()
{
	UI_MANAGER->Render();

	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth - 25, m_pGame->m_nHeight - 25, 0.5f);
	}

	UIBase* pFocusedUI = UI_MANAGER->GetFocus();
	if (pFocusedUI)
	{
		bool bVisible = true;
		
		NaRect rc = pFocusedUI->GetScreenRect();
		DRAWTEXT(
			rc.left - 12,
			rc.top + rc.Height() / 2 - 4,
			L"\x08");
	}
}

void GameStateRegisterUser::InitCheckTicketUI()
{
	m_Transaction.m_pJobThread = new std::thread(CheckTicket, this);
}

void GameStateRegisterUser::CheckTicket(void * pParam)
{
	GameStateRegisterUser *pThis = (GameStateRegisterUser*)pParam;

	pThis->m_Transaction.BeginJob(JOB_CHECK_REGISTER_TICKET);

	ServerRequest req;
	try
	{
		auto pTicket = req.GetRegisterTicket(pThis->m_strLocalKey);
		pThis->m_Transaction.m_Response.pServerRegisterTicket = pTicket;

		pThis->m_Transaction.ChangeJob(JOB_CHECK_REGISTER_TICKET_END);
	}
	catch (NaException &e)
	{
		NaString strErrorMsg = NaString(e.m_szWhat);
		NaDebugOut(L"Error: %ls\n", strErrorMsg.wstr());

		if (strErrorMsg.Find(L"RegisterTicket broken") >= 0)
			pThis->m_Transaction.ChangeJob(JOB_REGISTER_TICKET_CODE_FAIL);
		else
			pThis->m_Transaction.ChangeJob(JOB_CHECK_REGISTER_TICKET_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateRegisterUser::InitRegisterTicketUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	int y = 0;
	UI_MANAGER->CreateText(UI_UNDEFINED, 0, y, L"-- FILL YOUR INFORMATION --");
	y += 12;

	auto pConfig = GameConfig::GetGameConfig(m_pGame);
	auto pUI = UI_MANAGER->CreateEdit(UI_USERNAME, 8, y, pConfig->m_strName, L"Name: ");
	UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
	pEdit->m_bAllowAll = true;
	pEdit->m_strEmptyText = L"";
	y += 12;

	pUI = UI_MANAGER->CreateEdit(UI_PASSWORD, 8, y, L"", L"Password: ");
	pEdit = reinterpret_cast<UIEdit*>(pUI);
	pEdit->m_bAllowAll = true;
	pEdit->m_strEmptyText = L"";
	pEdit->m_bIsPassword = true;
	y += 12;

	pUI = UI_MANAGER->CreateEdit(UI_SECUREWORD, 8, y, L"", L"Security text: ");
	pEdit = reinterpret_cast<UIEdit*>(pUI);
	pEdit->m_bAllowAll = true;
	pEdit->m_strEmptyText = L"";
	y += 12;

	pUI = UI_MANAGER->CreateText(UI_UNDEFINED, 8, y, L"(NOT A PASSWORD)");
	y += 12;

	y += 8;
	pUI = UI_MANAGER->CreateButton(UI_SUBMIT_TICKET, 8, y, L"SUBMIT");
	y += 12;
}

void GameStateRegisterUser::RegisterTicket(void * pParam)
{
	GameStateRegisterUser *pThis = (GameStateRegisterUser*)pParam;

	pThis->m_Transaction.BeginJob(JOB_REGISTER_TICKET);

	ServerRequest req;
	try
	{
		Game *m_pGame = pThis->m_pGame;
		ServerRegisterTicket *pTicket = new ServerRegisterTicket;
		pTicket->BuildPropertyMap();

		NaString strName = L"";
		NaString strPassword = L"";
		NaString strSecurityText = L"";

		auto pUI = UI_MANAGER->GetControl(UI_USERNAME, UI_MANAGER->m_pCurPanel);
		if (pUI)
		{
			UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
			pTicket->m_strName = pEdit->m_strText;
		}

		pUI = UI_MANAGER->GetControl(UI_PASSWORD, UI_MANAGER->m_pCurPanel);
		if (pUI)
		{
			UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
			pTicket->m_strPassword = pEdit->m_strText;
		}

		pUI = UI_MANAGER->GetControl(UI_SECUREWORD, UI_MANAGER->m_pCurPanel);
		if (pUI)
		{
			UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
			pTicket->m_strSecurityText = pEdit->m_strText;
		}

		pTicket->m_strPcName = pThis->m_strComputerName;
		pTicket->m_strMacAddress = pThis->m_strMacAddress;
		pTicket->m_strUserUniqueId = pThis->m_strUserUniqueId;

		req.SubmitRegisterInfo(pThis->m_strLocalKey, pTicket);

		pThis->m_Transaction.ChangeJob(JOB_REGISTER_TICKET_END);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());

		pThis->m_Transaction.ChangeJob(JOB_REGISTER_TICKET_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateRegisterUser::InitRegisterTicketCodeUI(ServerRegisterTicket * pTicket)
{
	if (pTicket->m_nStatus == -1)
	{
		delete pTicket;
		InitWrongPermissionUI();
		return;
	}
	
	if (pTicket->m_nStatus == 1)
	{
		// Already confirmed
		delete pTicket;
		InitRegisterUserUI();
		return;
	}

	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	int y = 0;
	UI_MANAGER->CreateText(UI_UNDEFINED, 0, y, L"-- YOUR REGISTRATION INFO --");
	y += 12;

	NaString str;
	str.Format(L"Ticket #%05d", pTicket->m_nTicketNumber);
	UI_MANAGER->CreateText(UI_UNDEFINED, 8, y, str);
	y += 12;

	UI_MANAGER->CreateText(UI_UNDEFINED, 8, y, L"Enter activation code");
	y += 12;

	auto pUI = UI_MANAGER->CreateEdit(UI_REGISTERCODE, 8, y, L"", L"-> ");
	UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
	pEdit->m_bAllowAll = true;
	pEdit->m_strEmptyText = L"";
	y += 12;	

	y += 12;
	UI_MANAGER->CreateText(UI_UNDEFINED, 0, y, L"Capture this and send to Developer");
	y += 12;

	UI_MANAGER->CreateText(UI_UNDEFINED, 0, y, L"Then you can get activation code");
	y += 12;

	delete pTicket;
}

void GameStateRegisterUser::RegisterTicketCode(void * pParam)
{
	GameStateRegisterUser *pThis = (GameStateRegisterUser*)pParam;

	pThis->m_Transaction.BeginJob(JOB_REGISTER_TICKET_CODE);

	ServerRequest req;
	try
	{
		Game *m_pGame = pThis->m_pGame;

		auto pUI = UI_MANAGER->GetControl(UI_REGISTERCODE, UI_MANAGER->m_pCurPanel);
		if (pUI)
		{
			UIEdit *pEdit = reinterpret_cast<UIEdit*>(pUI);
			auto strCode = pEdit->m_strText;

			NaString strRefer;
			strRefer.Format(L"%ls,%ls,%ls",
				pThis->m_strMacAddress.wstr(),
				pThis->m_strComputerName.wstr(),
				pThis->m_strLocalKey.wstr()
			);
			req.SubmitRegisterTicketCode(pThis->m_strLocalKey, strCode, strRefer);

			pThis->m_Transaction.ChangeJob(JOB_REGISTER_TICKET_CODE_END);
			pThis->m_Transaction.EndJob();
			return;
		}

		pThis->m_Transaction.ChangeJob(JOB_REGISTER_TICKET_CODE_FAIL);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());

		pThis->m_Transaction.ChangeJob(JOB_REGISTER_TICKET_CODE_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateRegisterUser::InitRegisterUserUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	int y = 0;
	UI_MANAGER->CreateText(UI_UNDEFINED, 0, y, L"Updating your permission...");
	y += 12;

	m_Transaction.m_pJobThread = new std::thread(RegisterUser, this);
}

void GameStateRegisterUser::RegisterUser(void * pParam)
{
	GameStateRegisterUser *pThis = (GameStateRegisterUser*)pParam;

	pThis->m_Transaction.BeginJob(JOB_REGISTER_USER);

	ServerRequest req;
	try
	{
		Game *m_pGame = pThis->m_pGame;		
		auto pTicket = req.GetRegisterTicket(pThis->m_strLocalKey);
		if (pThis->m_strUserUniqueId.GetLength() > 0)
		{
			auto pUserInfo = req.GetUserInfo(pThis->m_strUserUniqueId);
			pUserInfo->m_nPermission = 1;
			
			req.UpdateUserInfo(pUserInfo, UPDATE_USERINFO_PERMISSION);
			delete pUserInfo;

			pThis->m_Transaction.ChangeJob(JOB_REGISTER_USER_END);
			pThis->m_Transaction.EndJob();
			return;
		}
		else if (pTicket->m_strUserUniqueId.GetLength() > 0)
		{
			auto pUserInfo = req.GetUserInfo(pTicket->m_strUserUniqueId);
			pUserInfo->m_nPermission = 1;

			req.UpdateUserInfo(pUserInfo, UPDATE_USERINFO_PERMISSION);
			delete pUserInfo;

			auto pConfig = GameConfig::GetGameConfig(m_pGame);
			pConfig->m_strUniqueId = pTicket->m_strUserUniqueId;
			pConfig->m_strName = pTicket->m_strName;
			pConfig->Save();

			pThis->m_Transaction.ChangeJob(JOB_REGISTER_USER_END);
			pThis->m_Transaction.EndJob();
			return;
		}
		else
		{
			auto pTicket = req.GetRegisterTicket(pThis->m_strLocalKey);
			auto pUserInfo = req.RegisterUser(pTicket->m_strName);
			delete pTicket;

			pUserInfo->m_nPermission = 1;
			
			auto pConfig = GameConfig::GetGameConfig(m_pGame);
			pConfig->m_strName = pUserInfo->m_strName;
			pConfig->m_strUniqueId = pUserInfo->m_strUniqueId;
			pConfig->Save();

			req.UpdateUserInfo(pUserInfo, UPDATE_USERINFO_PERMISSION);
			delete pUserInfo;

			pThis->m_Transaction.ChangeJob(JOB_REGISTER_USER_END);
			pThis->m_Transaction.EndJob();
			return;
		}

		pThis->m_Transaction.ChangeJob(JOB_REGISTER_USER_FAIL);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());

		pThis->m_Transaction.ChangeJob(JOB_REGISTER_USER_FAIL);
	}

	pThis->m_Transaction.EndJob();
}

void GameStateRegisterUser::InitWrongPermissionUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	int y = 0;
	UI_MANAGER->CreateText(UI_UNDEFINED, 0, y, L"YOU DON'T HAVE PERMISSION TO ACCESS GAME");
	y += 12;

	SOUND_MANAGER->PlaySoundEffect(L"NotCorrect");
}
