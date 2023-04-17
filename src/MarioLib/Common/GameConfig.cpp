#include "GameConfig.h"

#include "NaCommon.h"
#include "JsonManager.h"
#include "Game.h"
#include "PathManager.h"

GameConfig* GameConfig::s_pGameConfig = nullptr;

BEGIN_IMPL_PROPERTYMAP(GameConfig)
	PROP_STR("Name",					VT_ETC_NASTR,	L"",	nullptr, "GameConfig"),
	PROP_STR("UniqueId",				VT_ETC_NASTR,	L"",	nullptr, "GameConfig"),
	PROP_STR("WindowRect",				VT_ETC_NASTR,	L"",	nullptr, "GameConfig"),
	PROP_STR("RenderEngine",			VT_ETC_NASTR,	L"",	nullptr, "GameConfig"),
	PROP_BOOL("SoftwareFrameSkip",		VT_BOOL,		false,	nullptr, "GameConfig"),
	PROP_STR("InputKeyCode",			VT_ETC_NASTR,	L"",	nullptr, "GameConfig"),
	PROP_INT("StageSortMethod",			VT_I4,			1,		nullptr, "GameConfig"),
	PROP_INT("StageDifficultyFilter",	VT_I4,			1,		nullptr, "GameConfig"),
	PROP_INT("StageThemeFilter",		VT_I4,			1,		nullptr, "GameConfig"),
	PROP_INT("ZoomFactor",				VT_I4,			3,		nullptr, "GameConfig"),
END_IMPL_PROPERTYMAP()

GameConfig::GameConfig(Game* pGame) :
	m_pGame(pGame)
{
	m_strName = L"";
	m_strUniqueId = L"";
	m_strWindowRect = L"";
	m_rcWindow.left = -1;
	m_rcWindow.top = -1;
	m_rcWindow.right = -1;
	m_rcWindow.bottom = -1;

	// Options
	m_strRenderEngine = L"";
	m_bSoftwareFrameSkip = false;
	m_strInputKeyCode = L"";

	int nDefaultKeyCode[6] = { 37,39,38,40,88,90 };
	for (int i = 0; i < 6; i++)
		m_nInputKeyCode[i] = nDefaultKeyCode[i];
	m_nStageSortMethod = 1;
	m_nStageDifficultyFilter = 0;
	m_nStageThemeFilter = 0;
	m_nZoomFactor = 3;
}

GameConfig::~GameConfig()
{
}

void GameConfig::Load()
{
	// C:\MyProjects\Game\MarioV3\UserData\Config.json

	NaDebugOut(L"Load Config...\n");

	NaString strName;
	strName.Format(L"%ls\\Config.json", PathManager::GetConfigPath(m_pGame));
	Json::Value *j = JsonManager::LoadFile(strName.cstr());
	if (j)
	{
		JsonManager::LoadJsonObjectProperty(this, *j);
		delete j;
		j = nullptr;
	}

	NaDebugOut(L"- Name: %ls\n", m_strName.wstr());
	NaDebugOut(L"- InputKeyCode: %ls\n", m_strInputKeyCode.wstr());

	NaStrArray ar = m_strInputKeyCode.Split(L",");
	if (ar.GetCount() == 6)
	{
		for (int i=0; i<6; i++)
		{
			m_nInputKeyCode[i] = ar[i].ToInt();
		}
	}
}

void GameConfig::Save()
{
	NaDebugOut(L"Save Config...\n");

	Json::Value j;
	if (JsonManager::SaveJsonObjectProperty(this, j))
	{
		NaString strName;
		strName.Format(L"%ls\\Config.json", PathManager::GetConfigPath(m_pGame));
		JsonManager::SaveFile(strName.cstr(), this, j);
	}
}

void GameConfig::ApplyOptions(Game *pGame)
{
	if (m_strRenderEngine.Compare(L"DirectX") == 0)
		pGame->m_Option.m_bUseDirectX = true;
	else
		pGame->m_Option.m_bUseDirectX = false;

	pGame->m_Option.m_bSoftwareFrameSkip = m_bSoftwareFrameSkip;

}

GameConfig * GameConfig::GetGameConfig(Game* pGame)
{
	// Singleton
	if (s_pGameConfig == nullptr)
	{
		s_pGameConfig = new GameConfig(pGame);
		s_pGameConfig->BuildPropertyMap();
		s_pGameConfig->Load();
	}

	return s_pGameConfig;
}

void GameConfig::Release()
{
	if (s_pGameConfig)
	{
		delete s_pGameConfig;
		s_pGameConfig = nullptr;
	}
}
