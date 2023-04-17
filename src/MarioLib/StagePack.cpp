#include "StagePack.h"

#include "Game.h"
#include "NaPlatformAPI.h"
#include "Stage.h"
#include "JsonManager.h"
#include "PathManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(StagePack)
	PROP_STR("Name", VT_ETC_NASTR, L"", 0, "StagePack"),
	PROP_STR("Creator", VT_ETC_NASTR, L"", 0, "StagePack"),
	PROP_STR("InitialStage", VT_ETC_NASTR, L"", 0, "StagePack"),
END_IMPL_PROPERTYMAP()

StagePack::StagePack(Game *pGame)
{
	m_pGame = pGame;

	m_bValid = false;
	m_strName = L"";
	m_strFileName = L"";
	m_strInitialStage = L"";
	m_strCreator = L"";
}

StagePack::~StagePack()
{
}

void StagePack::Load(NaString strName)
{
	m_strFileName = strName;

	// Load StagePack.json
	NaString strMapPath;
	strMapPath.Format(L"%ls\\%ls\\StagePack.json", 
		PathManager::GetMapPath(m_pGame),
		m_strFileName.wstr()
		);

	FILE *fp;
	NaPlatform::FileOpen(&fp, strMapPath.cstr(), "rb");
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
		}
		fclose(fp);
		delete[] szBuf;
	}
}

bool StagePack::IsValid()
{
	if (m_strInitialStage.GetLength() > 0)
		return true;

	return false;
}
