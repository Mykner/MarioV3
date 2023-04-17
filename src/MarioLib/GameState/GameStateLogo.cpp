#include "GameStateLogo.h"

#include "Player.h"
#include "Stage.h"
#include "SoundManager.h"
#include "InputManager.h"
#include "RenderManager.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UIText.h"
#include "PathManager.h"
#include "GameConfig.h"

#include "NaPlatformAPI.h"
#include <aes/md5.h>

GameStateLogo::GameStateLogo(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_LOGO;

	m_Transaction.Init(pGame);
	m_bHavePermission = false;
}

GameStateLogo::~GameStateLogo()
{
}

long GameStateLogo::GetBackgroundColor()
{
	return 0;
}

void GameStateLogo::Init()
{
	SOUND_MANAGER->StopBGM();

	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;

	// Delete Old version files
	NaString strOldMarioExe;
	strOldMarioExe.Format(L"%ls\\Mario.delete", PathManager::GetGameRootPath(m_pGame));
	::DeleteFile(strOldMarioExe.wstr());

	strOldMarioExe.Format(L"%ls\\MarioMapEditor.delete", PathManager::GetGameRootPath(m_pGame));
	::DeleteFile(strOldMarioExe.wstr());

	strOldMarioExe.Format(L"%ls\\MarioConfig.delete", PathManager::GetGameRootPath(m_pGame));
	::DeleteFile(strOldMarioExe.wstr());

	// Collect user's info
	auto strMacAddress = NaPlatform::GetMacAddress();
	auto strComputerName = NaPlatform::GetComputerName();
	NaDebugOut(L"MacAddress: %ls\n", strMacAddress.wstr());
	NaDebugOut(L"Computer name: %ls\n", strComputerName.wstr());

	NaString str;
	str.Format(L"%ls%ls", strMacAddress.wstr(), strComputerName.wstr());

	char szMD5[MD5_STR_LEN + 1];
	Compute_string_md5((unsigned char*)str.wstr(), str.GetBufferSize(), szMD5);
	m_strLocalKey = szMD5;
}

void GameStateLogo::Process()
{
	if (m_bHavePermission &&
		m_nStateFrame > 240 && m_nStateFrame < 590 && KE(VK_SPACE))
		m_nStateFrame = 590;

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();

		switch (m_Transaction.m_nJobType)
		{
		case JOB_CHECK_USER_PERMISSION_END:
			m_bHavePermission = true;
			break;
		case JOB_CHECK_USER_PERMISSION_FAIL:
			m_pGame->ChangeState(GAMESTATE_REGISTERUSER);
			break;
		}
		
		m_Transaction.ChangeJob(JOB_NONE);
	}

	switch(m_nStateFrame)
	{
	case 1:
		{
			auto pConfig = GameConfig::GetGameConfig(m_pGame);
			if (pConfig->m_strName.GetLength() == 0 ||
				pConfig->m_strUniqueId.GetLength() == 0)
			{
				m_pGame->ChangeState(GAMESTATE_REGISTERUSER);
			}
			else
			{
				m_Transaction.m_pJobThread = new std::thread(CheckPermissions, this); 
			}
		}
		break;
	case 60:
		{
			UI_MANAGER->CreateText(0, 0, 0, L"Nintendo");
			SOUND_MANAGER->PlaySoundEffect(L"Coin");
		}
		break;
	case 180:
		{
			UI_MANAGER->Clear();
		}
		break;
	case 240:
		{
			UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);

			int _x = 12;
			int _y = 32;
			UI_MANAGER->CreateText(0, _x, _y, L"This game is a non-profit project developed by");	_y += 16;
			UI_MANAGER->CreateText(0, _x, _y, L"a programmer who likes Super Mario series");		_y += 16;
			UI_MANAGER->CreateText(0, _x, _y, L"based on Nintendo's Super Mario Bros. All the ");	_y += 16;
			UI_MANAGER->CreateText(0, _x, _y, L"characters, music and sound used in this game ");	_y += 16;
			UI_MANAGER->CreateText(0, _x, _y, L"are copyrighted by Nintendo.");						_y += 32;
			UI_MANAGER->CreateText(0, _x, _y, L"Please purchase a Nintendo's game machine to");		_y += 16;
			UI_MANAGER->CreateText(0, _x, _y, L"enjoy a better game. :)");							_y += 16;
		}
		break;
	case 600:
		{
			CreateParameter param;
			param.nStartDelay = 0;
			param.nDuration = 120;
			m_pGame->CreateSceneEffect(
				m_pGame->Width() / 2,
				m_pGame->Height() / 2,
				EFFECT_SCENE_CIRCLEOUT, &param);
		}
		break;
	case 720:
		UI_MANAGER->Clear();
		break;
	}

	if (m_nStateFrame >= 800 && m_bHavePermission)
	{
		/*if (KS(VK_DOWN))
			m_pGame->ChangeState(GAMESTATE_TITLE);
		else
			m_pGame->ChangeState(GAMESTATE_APPUPDATE);
			*/

#if defined(NDEBUG)
		m_pGame->ChangeState(GAMESTATE_APPUPDATE);
#else
		m_pGame->ChangeState(GAMESTATE_TITLE);
#endif
	}
}

void GameStateLogo::Render()
{
	UI_MANAGER->Render();

	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth - 25, m_pGame->m_nHeight - 25, 0.5f);
	}
}

void GameStateLogo::CheckPermissions(void * pParam)
{
	GameStateLogo *pThis = (GameStateLogo*)pParam;

	pThis->m_Transaction.BeginJob(JOB_CHECK_USER_PERMISSION);

	ServerRequest req;
	try
	{
		Game *m_pGame = pThis->m_pGame;

		auto pTicket = req.GetRegisterTicket(pThis->m_strLocalKey);
		if (pTicket->m_nStatus == 1)
		{
			auto pConfig = GameConfig::GetGameConfig(m_pGame);
			auto pUserInfo = req.GetUserInfo(pConfig->m_strUniqueId);
			if (pUserInfo->m_nPermission == 1)
			{
				delete pUserInfo;

				pThis->m_Transaction.ChangeJob(JOB_CHECK_USER_PERMISSION_END);
				pThis->m_Transaction.EndJob();
				return;
			}
		}

		pThis->m_Transaction.ChangeJob(JOB_CHECK_USER_PERMISSION_FAIL);
	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());

		pThis->m_Transaction.ChangeJob(JOB_CHECK_USER_PERMISSION_FAIL);
	}

	pThis->m_Transaction.EndJob();
}
