#include "ServerRequest.h"
//#include "ServerRequest_.h"

#include "Stage.h"
#include "NaException.h"
#include "StageMetaInfo.h"
#include "JsonManager.h"
#include "NaPlatformAPI.h"

#include "aes/md5.h"
#include "PathManager.h"

NaString ServerRequest::s_strUpdateUrl = L"http://[storage server]/update";
NaString ServerRequest::s_strBaseUrl = L"https://[rest server]/";

// Using a7
int ServerRequest::s_nCurrentVersion = 11;

#if (false)
	#define METAINFOS_URL	L"metainfos"
	#define MAPS_URL		L"maps"
	#define COMMENTS_URL	L"comments"
#else
 	#define METAINFOS_URL	L"a8_metainfos"
 	#define MAPS_URL		L"a8_maps"
	#define COMMENTS_URL	L"a8_comments"
	#define RECORDS_URL		L"a8_records"

// 	#define METAINFOS_URL	L"dev_metainfos"
// 	#define MAPS_URL		L"dev_maps"
#endif

#define USERS_URL			L"users"
#define REGISTER_URL		L"register"
#define SERVERINFO_URL		L"serverinfo"

// ServerInfo class
BEGIN_IMPL_PROPERTYMAP(ServerInfo)
	PROP_INT("Version", VT_I4, 0, 0, "ServerInfo"),
	PROP_INT("Status", VT_I4, 0, 0, "ServerInfo"),
	PROP_INT("CanPlayGame", VT_I4, 0, 0, "ServerInfo"),
	PROP_INT("CanRegisterUser", VT_I4, 0, 0, "ServerInfo"),
	PROP_INT("CanUpdateStage", VT_I4, 0, 0, "ServerInfo"),
	PROP_INT("CanPlayStage", VT_I4, 0, 0, "ServerInfo"),
END_IMPL_PROPERTYMAP()

ServerInfo::ServerInfo()
{
	m_nVersion = -1;
	m_nStatus = -1;
	m_nCanPlayGame = -1;
	m_nCanUploadStage = -1;
	m_nCanRegisterUser = -1;
	m_nCanPlayStage = -1;
}

// ServerUserInfo class
BEGIN_IMPL_PROPERTYMAP(ServerUserInfo)
	PROP_STR("Name",			VT_ETC_NASTR,	L"",	0, "ServerUserInfo"),
	PROP_INT("Permission",		VT_I4,			0,		0, "ServerUserInfo"),
	PROP_STR("SecureText",		VT_ETC_NASTR,	L"",	0, "ServerUserInfo"),
	PROP_STR("LastPlayed",		VT_ETC_NASTR,	L"",	0, "ServerUserInfo"),
	PROP_STR("LastUploaded",	VT_ETC_NASTR,	L"",	0, "ServerUserInfo"),
	PROP_STR("Avatar",			VT_ETC_NASTR,	L"",	0, "ServerUserInfo"),
END_IMPL_PROPERTYMAP()

ServerUserInfo::ServerUserInfo()
{
	m_strUniqueId = L"";

	m_strName = L"";
	m_nPermission = 0;
	m_strSecureText = L"";
	m_strLastPlayed = L"";
	m_strLastUploaded = L"";
	m_strAvatar = L"";
}

// ServerRegisterTicket class
BEGIN_IMPL_PROPERTYMAP(ServerRegisterTicket)
	PROP_INT("TicketNumber",	VT_I4,			0,		0, "ServerRegisterTicket"),
	PROP_STR("Name",			VT_ETC_NASTR,	L"",	0, "ServerRegisterTicket"),
	PROP_STR("Password",		VT_ETC_NASTR,	L"",	0, "ServerRegisterTicket"),
	PROP_STR("SecurityText",	VT_ETC_NASTR,	L"",	0, "ServerRegisterTicket"),
	PROP_STR("UserUniqueId",	VT_ETC_NASTR,	L"",	0, "ServerRegisterTicket"),
	PROP_STR("MacAddress",		VT_ETC_NASTR,	L"",	0, "ServerRegisterTicket"),
	PROP_STR("PcName",			VT_ETC_NASTR,	L"",	0, "ServerRegisterTicket"),
	PROP_STR("WrongCodeReference", VT_ETC_NASTR, L"",	0, "ServerRegisterTicket"),
	PROP_INT("Status",			VT_I4,			0,		0, "ServerRegisterTicket"),
END_IMPL_PROPERTYMAP()

ServerRegisterTicket::ServerRegisterTicket()
{
	m_strTicketHash = L"";
	m_nTicketNumber = -1;
	m_strName = L"";
	m_strPassword = L"";
	m_strSecurityText = L"";
	m_strUserUniqueId = L"";
	m_strMacAddress = L"";
	m_strPcName = L"";
	m_strWrongCodeReference = L"";
	m_nStatus = 0;
}

// StageCommentInfo class
BEGIN_IMPL_PROPERTYMAP(StageCommentInfo)
	PROP_STR("Name",	VT_ETC_NASTR, L"", 0, "StageCommentInfo"),
	PROP_STR("UniqueId", VT_ETC_NASTR, L"", 0, "StageCommentInfo"),
	PROP_STR("Avatar",	VT_ETC_NASTR, L"", 0, "StageCommentInfo"),
	PROP_STR("Comment", VT_ETC_NASTR, L"", 0, "StageCommentInfo"),
END_IMPL_PROPERTYMAP()

StageCommentInfo::StageCommentInfo()
{
	m_strName = L"";
	m_strUniqueId = L"";
	m_strAvatar = L"";
	m_strComment = L"";
}

// StageRecordInfo class
BEGIN_IMPL_PROPERTYMAP(StageRecordInfo)
	PROP_STR("DeathPositions",	VT_ETC_NASTR,	L"",	0, "StageRecordInfo"),
	PROP_INT("BestTime",		VT_I4,			-1,		0, "StageRecordInfo"),
END_IMPL_PROPERTYMAP()

StageRecordInfo::StageRecordInfo()
{
	m_strDeathPositions = L"";
	m_nBestTime = -1;
}

// ServerRequest class
ServerRequest::ServerRequest()
{
}

ServerRequest::~ServerRequest()
{
}

bool ServerRequest::CanUpdate(Stage * pStage, NaString strUserUniqueId)
{
	if (pStage == nullptr)
		return false;

	bool bUpdate = (pStage->m_strUniqueId.GetLength() > 0);
	
	NaDebugOut(L"// Check MetaInfo List...\n");

	NaCurl curl;
	NaString strUrlMetaInfos = s_strBaseUrl;
	strUrlMetaInfos += METAINFOS_URL;
	strUrlMetaInfos += L".json";

	NaString strRet = curl.Post(strUrlMetaInfos);

	Json::Value jMeta;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"MetaInfo list broken.");
	}

	// Check server error
	Json::Value jError = jMeta.get("error", Json::nullValue);
	if (!jError.isNull())
	{
		throw NaException(__FUNCTIONW__, __LINE__, 
			L"Server error: %ls",
			NaString(jError.asString().c_str()).wstr());
	}

	if (bUpdate)
	{
		Json::Value jValue = jMeta.get(pStage->m_strUniqueId.cstr(), Json::nullValue);
		if (jValue.isNull())
			bUpdate = false;
		else
		{
			// It's a registered stage!
			StageMetaInfo meta;
			for (auto jIt = jMeta.begin(); jIt != jMeta.end(); jIt++)
			{
				StageMetaInfo temp;
				temp.BuildPropertyMap();
				temp.m_strUniqueId = jIt.key().toStyledString().c_str();
				temp.m_strUniqueId = temp.m_strUniqueId.Mid(1);
				temp.m_strUniqueId = temp.m_strUniqueId.Left(temp.m_strUniqueId.GetLength() - 2);

				JsonManager::LoadJsonObjectProperty(&temp, *jIt);

				if (temp.m_strUniqueId.Compare(pStage->m_strUniqueId) == 0)
				{
					meta = temp;
					break;
				}
			}

			if (meta.m_strCreatorUniqueId.GetLength() == 0)
			{
				throw NaException(__FUNCTIONW__, __LINE__, L"Metainfo doesn't have creator's information.");
			}

			// Check UserUniqueId
			if (meta.m_strCreatorUniqueId.CompareNoCase(strUserUniqueId) != 0)
			{
				throw NaException(__FUNCTIONW__, __LINE__, L"Cannot modify other's stage.");
			}
		}	
	}
	
	return bUpdate;
}

void ServerRequest::UploadStage(Stage * pStage)
{
	// Upload New Map
	NaDebugOut(L"// Upload stage file...\n");

	// Then, Upload Map First, Return value is UniqueId
	NaCurl curl;
	NaString strUrlMaps = s_strBaseUrl;
	strUrlMaps += MAPS_URL;
	strUrlMaps += L".json";

	// ex) {"name":"-KgyMs7YQMtjGRGpeWkx"}
	NaString strMap = GetStageString(pStage);
	NaString strRet = curl.Post(strUrlMaps, strMap);
	Json::Value jRet;

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jRet))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Upload stage failed");
	}

	// Check server error
	Json::Value jError = jRet.get("error", Json::nullValue);
	if (!jError.isNull())
	{
		throw NaException(__FUNCTIONW__, __LINE__,
			L"Server error: %ls",
			NaString(jError.asString().c_str()).wstr());
	}

	Json::Value jUniqueId = jRet.get("name", Json::nullValue);
	if (jUniqueId.isNull())
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Get UniqueId failed");
	}

	pStage->m_strUniqueId = jUniqueId.asString().c_str();

	UpdateStageUniqueId(pStage);
}

void ServerRequest::UpdateStageUniqueId(Stage * pStage)
{
	// Update Current Map
	NaDebugOut(L"// Update stage unique-id...\n");

	// Return value is MapJson itself
	NaCurl curl;
	NaString strUrlMaps = s_strBaseUrl;
	strUrlMaps += MAPS_URL;
	strUrlMaps += L"/";
	strUrlMaps += pStage->m_strUniqueId;
	strUrlMaps += L"/UniqueId";
	strUrlMaps += L".json";

	NaString strValue;
	strValue.Format(L"\"%ls\"", pStage->m_strUniqueId.wstr());

	NaString strRet = curl.Put(strUrlMaps, strValue);
	Json::Value jRet;

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jRet))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update unique id of stage failed");
	}
}

void ServerRequest::UpdateStage(Stage * pStage)
{
	// Update Current Map
	NaDebugOut(L"// Update Map File...\n");

	// Return value is MapJson itself
	NaCurl curl;
	NaString strUrlMaps = s_strBaseUrl;
	strUrlMaps += MAPS_URL;
	strUrlMaps += L"/";
	strUrlMaps += pStage->m_strUniqueId;
	strUrlMaps += L".json";

	pStage->m_nRevision += 1;

	NaString strMap = GetStageString(pStage);
	NaString strRet = curl.Put(strUrlMaps, strMap);
	Json::Value jRet;

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jRet))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update stage failed");
	}
}

StageMetaInfo* ServerRequest::UpdateMetaInfo(Stage * pStage, NaString strCreatorUniqueId)
{
	NaDebugOut(L"// Update MetaInfo...\n");

	if (strCreatorUniqueId.GetLength() == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update metainfo failed (CreatorUniqueId is empty)");
	}

	NaCurl curl;
	NaString strUrlMetaInfos = s_strBaseUrl;
	strUrlMetaInfos += METAINFOS_URL;
	strUrlMetaInfos += L"/";
	strUrlMetaInfos += pStage->m_strUniqueId;
	strUrlMetaInfos += L".json";

	// #TODO When upload first-time, meta is null.
	auto pMeta = GetMetaInfo(pStage->m_strUniqueId);
	pMeta->m_strTitle = pStage->m_strTitle;
	pMeta->m_nRevision = pStage->m_nRevision;
	pMeta->m_strCreator = pStage->m_strCreator;
	pMeta->m_strGameTheme = pStage->m_strGameTheme;
	if (pMeta->m_strCreatorUniqueId.GetLength() == 0)
		pMeta->m_strCreatorUniqueId = strCreatorUniqueId;

	Json::Value jMeta;
	JsonManager::SaveJsonObjectProperty(pMeta, jMeta);
	NaString strMeta = jMeta.toStyledString().c_str();
	NaString strRet = curl.Put(strUrlMetaInfos, strMeta);

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update metainfo failed");
	}

	return pMeta;
}

std::vector<StageMetaInfo*> ServerRequest::GetMetaInfoList()
{
	NaDebugOut(L"// Check MetaInfo List...\n");

	std::vector<StageMetaInfo*> vec;

	NaCurl curl;
	NaString strUrlMetaInfos = s_strBaseUrl;
	strUrlMetaInfos += METAINFOS_URL;
	strUrlMetaInfos += L".json";

	NaString strRet = curl.Post(strUrlMetaInfos);

	Json::Value jMeta;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"MetaInfo list broken.");
	}

	NaDebugOut(L"MetaInfo Count: %d\n", jMeta.size());
	int nIndex = 0;
	for (auto jIt = jMeta.begin(); jIt != jMeta.end(); jIt++)
	{
		StageMetaInfo *pMetaInfo = new StageMetaInfo;
		pMetaInfo->BuildPropertyMap();
		pMetaInfo->m_strUniqueId = jIt.key().toStyledString().c_str();

		// Remove quote
		pMetaInfo->m_strUniqueId = pMetaInfo->m_strUniqueId.Mid(1);
		pMetaInfo->m_strUniqueId = pMetaInfo->m_strUniqueId.Left(pMetaInfo->m_strUniqueId.GetLength() - 2);

		if (pMetaInfo->m_strUniqueId.CompareNoCase(L"error") == 0)
		{
			throw NaException(__FUNCTIONW__, __LINE__, L"Internal server error");
		}

		try
		{
			JsonManager::LoadJsonObjectProperty(pMetaInfo, *jIt);
		}
		catch(Json::LogicError &e)
		{
			NaDebugOut(L"Error: Bad Metainfo found: %ls\n", pMetaInfo->m_strUniqueId.wstr());
			delete pMetaInfo;
			continue;
		}
		
		pMetaInfo->m_nIndex = nIndex;
		nIndex++;

		vec.push_back(pMetaInfo);
	}

	return vec;
}

bool ServerRequest::DownloadStage(NaString strUniqueId, NaString strFilePath)
{
	NaDebugOut(L"// Download Stage...\n");

	NaCurl curl;
	NaString strUrlMetaInfos;
	strUrlMetaInfos.Format(L"%ls%ls/%ls.json",
		s_strBaseUrl.wstr(),
		MAPS_URL,
		strUniqueId.wstr());

	NaString strRet = curl.Post(strUrlMetaInfos);

	Json::Value jMap;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMap))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Download stage failed.");
	}

	JsonManager::SaveToEncryptFile(strFilePath.cstr(), jMap);

	return true;
}

ServerRegisterTicket * ServerRequest::GetRegisterTicket(NaString strTicketHash)
{
	NaDebugOut(L"// Get RegisterTicket...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += REGISTER_URL;
	strUrl += L"/";
	strUrl += strTicketHash;
	strUrl += L".json";

	NaString strRet = curl.Post(strUrl);
	if (strRet.Compare(L"null") == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"RegisterTicket not found: %ls", strTicketHash.wstr());
	}

	Json::Value jObj;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jObj))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"RegisterTicket broken: %ls", strTicketHash.wstr());
	}

	// Check server error
	Json::Value jError = jObj.get("error", Json::nullValue);
	if (!jError.isNull())
	{
		throw NaException(__FUNCTIONW__, __LINE__,
			L"Server error: %ls",
			NaString(jError.asString().c_str()).wstr());
	}

	ServerRegisterTicket *pTicket = new ServerRegisterTicket;
	pTicket->m_strTicketHash = strTicketHash;
	pTicket->BuildPropertyMap();
	JsonManager::LoadJsonObjectProperty(pTicket, jObj);

	return pTicket;
}

int ServerRequest::GetLastRegisterTicketNumber()
{
	NaDebugOut(L"// Get LastRegisterTicket Number...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += REGISTER_URL;
	strUrl += L"/LastTicketNumber.json";

	NaString strRet = curl.Post(strUrl);
	if (strRet.Compare(L"null") == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"LastTicketNumber is null");
	}

	// Increase
	int nCur = strRet.ToInt();
	nCur += 1;

	NaString strBody;
	strBody.Format(L"%d", nCur);
	strRet = curl.Put(strUrl, strBody);
	if (strRet.Compare(L"null") == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Get NewTicketNumber failed");
	}

	return nCur;
}

void ServerRequest::SubmitRegisterInfo(NaString strTicketHash, ServerRegisterTicket *pTicket)
{
	NaDebugOut(L"// Submit RegisterInfo...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += REGISTER_URL;
	strUrl += L"/";
	strUrl += strTicketHash;
	strUrl += L".json";

	// Make new ticket
	pTicket->m_nTicketNumber = GetLastRegisterTicketNumber();

	Json::Value jObj;
	JsonManager::SaveJsonObjectProperty(pTicket, jObj);
	delete pTicket;

	// Submit
	NaString strTicket = jObj.toStyledString().c_str();
	NaString strRet = curl.Put(strUrl, strTicket);
	Json::Value jRet;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jRet))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Submit RegisterInfo failed");
	}

	// Check server error
	Json::Value jError = jObj.get("error", Json::nullValue);
	if (!jError.isNull())
	{
		throw NaException(__FUNCTIONW__, __LINE__,
			L"Server error: %ls",
			NaString(jError.asString().c_str()).wstr());
	}
}

void ServerRequest::SubmitRegisterTicketCode(NaString strTicketHash, NaString strCode, NaString strRefer)
{
	NaDebugOut(L"// Submit RegisterTicketCode...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += REGISTER_URL;
	strUrl += L"/";
	strUrl += strTicketHash;
	strUrl += L".json";

	NaString strRightCode = L"";
	int nTemp = 0;
	for (int i=0; i<strTicketHash.GetLength(); i++)
	{
		NaDebugOut(L"%c / %d\n", strTicketHash[i], strTicketHash[i]);
		if (i % 5 == 0)
			nTemp += (strTicketHash[i]);
		if (i % 5 == 1)
			nTemp += (strTicketHash[i]) / 2;
		if (i % 5 == 2)
			nTemp += (strTicketHash[i]) / 4;
		if (i % 5 == 3)
			nTemp += (strTicketHash[i]) / 5;
		if (i % 5 == 4)
		{
			nTemp += (strTicketHash[i]) / 3;
			if (nTemp > 255)
				nTemp = nTemp % 255;

			NaString strTemp;
			strTemp.Format(L"%X", nTemp);
			strRightCode += strTemp;

			nTemp = 0;
		}
	}

	NaDebugOut(L"RightCode: %ls\n", strRightCode.wstr());
	NaDebugOut(L"EnteredCode: %ls\n", strCode.wstr());
	NaString strRet;
	if (strRightCode.CompareNoCase(strCode) != 0)
	{
		// Record Wrong code References 
		if (strCode.GetLength() == 12)
		{
			strUrl = s_strBaseUrl;
			strUrl += REGISTER_URL;
			strUrl += L"/";
			strUrl += strTicketHash;
			strUrl += L"/";
			strUrl += L"WrongCodeReference.json";
			strRet = curl.Post(strUrl);

			if (strRet.Compare(L"null") == 0)
				strRet.Format(L"\"%ls\"", strCode.wstr());
			else
			{
				strRet.ReplaceAll(L"\"", L"");
				if (strRet.GetLength() == 0)
					strRet.Format(L"\"%ls\"", strCode.wstr());
				else
					strRet.Format(L"\"%ls,%ls\"", strRet.wstr(), strCode.wstr());
			}

			curl.Put(strUrl, strRet);
		}

		// Change status
		strUrl = s_strBaseUrl;
		strUrl += REGISTER_URL;
		strUrl += L"/";
		strUrl += strTicketHash;
		strUrl += L"/";
		strUrl += L"Status.json";
		curl.Put(strUrl, L"-1");

		throw NaException(__FUNCTIONW__, __LINE__, L"Wrong RegisterTicketcode");
	}

	// Ok. Code is correct...

	// Change status
	strUrl = s_strBaseUrl;
	strUrl += REGISTER_URL;
	strUrl += L"/";
	strUrl += strTicketHash;
	strUrl += L"/";
	strUrl += L"Status.json";
	curl.Put(strUrl, L"1");
}

ServerUserInfo* ServerRequest::GetUserInfo(NaString strUniqueId)
{
	NaDebugOut(L"// Get UserInfo...\n");

	ServerUserInfo *pInfo = new ServerUserInfo;

	NaCurl curl;
	NaString strUrlUsers = s_strBaseUrl;
	strUrlUsers += USERS_URL;
	strUrlUsers += L"/";
	strUrlUsers += strUniqueId;
	strUrlUsers += L".json";

	NaString strRet = curl.Post(strUrlUsers);

	Json::Value jMeta;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"UserInfo broken: %ls", strUniqueId);
	}

	pInfo->m_strUniqueId = strUniqueId;
	pInfo->BuildPropertyMap();
	JsonManager::LoadJsonObjectProperty(pInfo, jMeta);
	
	return pInfo;
}

ServerUserInfo* ServerRequest::RegisterUser(NaString strName)
{
	NaDebugOut(L"// Register User...\n");

	NaCurl curl;
	NaString strUrlUsers = s_strBaseUrl;
	strUrlUsers += USERS_URL;
	strUrlUsers += L".json";

	// ex) {"name":"-KgyMs7YQMtjGRGpeWkx"}
	ServerUserInfo *pInfo = new ServerUserInfo;
	pInfo->BuildPropertyMap();
	pInfo->m_strName = strName;
	pInfo->m_nPermission = 0;
	
	Json::Value jInfo;
	JsonManager::SaveJsonObjectProperty(pInfo, jInfo);

	NaString strUserInfo = jInfo.toStyledString().c_str();
	NaString strRet = curl.Post(strUrlUsers, strUserInfo);
	Json::Value jRet;

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jRet))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Register user failed");
	}

	Json::Value jUniqueId = jRet.get("name", Json::nullValue);
	if (jUniqueId.isNull())
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Get User's UniqueId failed");
	}

	pInfo->m_strUniqueId = jUniqueId.asString().c_str();
	
	return pInfo;
}

StageRecordInfo * ServerRequest::GetStageRecordInfo(NaString strUniqueId)
{
	NaDebugOut(L"// Get StageRecordInfo...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += RECORDS_URL;
	strUrl += L"/";
	strUrl += strUniqueId;
	strUrl += L".json";

	NaString strRet = curl.Post(strUrl);

	Json::Value jInfo;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jInfo))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Get StageRecordInfo failed.");
	}

	StageRecordInfo *pInfo = new StageRecordInfo;
	pInfo->BuildPropertyMap();

	JsonManager::LoadJsonObjectProperty(pInfo, jInfo);
	return pInfo;
}

void ServerRequest::UpdateStageRecordInfo(NaString strUniqueId, NaString strDeathPositions)
{
	NaDebugOut(L"// Update StageRecordInfo (Uid: %ls)\n", strUniqueId.wstr());

	if (strUniqueId.GetLength() == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"StageRecordInfo doesn't have unique id. Please report to developer with screenshot");
	}

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += RECORDS_URL;
	strUrl += L"/";
	strUrl += strUniqueId;
	strUrl += L"/DeathPositions.json";

	NaString strData;
	strData.Format(L"\"%ls\"", strDeathPositions.wstr());
	NaString strRet = curl.Put(strUrl, strData);

	Json::Value jMeta;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update StageRecordInfo failed");
	}
}

void ServerRequest::UpdateStageRecordInfo(NaString strUniqueId, int nBestTime)
{
	throw NaException(__FUNCTIONW__, __LINE__, L"Update StageRecordInfo failed");
}

ServerInfo* ServerRequest::GetServerInfo()
{
	NaDebugOut(L"// Get ServerInfo...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += SERVERINFO_URL;
	strUrl += L".json";

	NaString strRet = curl.Post(strUrl);

	Json::Value jInfo;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jInfo))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Get ServerInfo failed.");
	}

	ServerInfo *pInfo = new ServerInfo;
	pInfo->BuildPropertyMap();

	JsonManager::LoadJsonObjectProperty(pInfo, jInfo);
	return pInfo;
}

StageMetaInfo * ServerRequest::GetMetaInfo(NaString strUniqueId)
{
	NaDebugOut(L"// Get MetaInfo: (UID: %ls)...\n", strUniqueId.wstr());

	NaCurl curl;
	NaString strUrlMetaInfos = s_strBaseUrl;
	strUrlMetaInfos += METAINFOS_URL;
	strUrlMetaInfos += L"/";
	strUrlMetaInfos += strUniqueId;
	strUrlMetaInfos += L".json";

	NaString strRet = curl.Post(strUrlMetaInfos);

	Json::Value jMeta;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Get metainfo failed.");
	}

	StageMetaInfo *pMetaInfo = new StageMetaInfo;
	pMetaInfo->BuildPropertyMap();
	pMetaInfo->m_strUniqueId = strUniqueId;
	
	JsonManager::LoadJsonObjectProperty(pMetaInfo, jMeta);
	return pMetaInfo;
}

void ServerRequest::UpdateMetaInfo(StageMetaInfo *pMeta)
{
	NaDebugOut(L"// Update MetaInfo (Uid: %ls)\n", pMeta->m_strUniqueId.wstr());

	if (pMeta->m_strUniqueId.GetLength() == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Metainfo doesn't have unique id. Please report to developer with screenshot");
	}

	NaCurl curl;
	NaString strUrlMetaInfos = s_strBaseUrl;
	strUrlMetaInfos += METAINFOS_URL;
	strUrlMetaInfos += L"/";
	strUrlMetaInfos += pMeta->m_strUniqueId;
	strUrlMetaInfos += L".json";

	Json::Value jMeta;
	JsonManager::SaveJsonObjectProperty(pMeta, jMeta);
	NaString strMeta = jMeta.toStyledString().c_str();
	NaString strRet = curl.Put(strUrlMetaInfos, strMeta);

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update metainfo failed");
	}
}

void ServerRequest::UpdateMetaInfo(StageMetaInfo * pMeta, int nOption)
{
	NaDebugOut(L"// Update MetaInfo (Uid: %ls) (op: %d)\n", pMeta->m_strUniqueId.wstr(), nOption);

	if (pMeta->m_strUniqueId.GetLength() == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Metainfo doesn't have unique id. Please report to developer with screenshot");
	}

	NaCurl curl;
	NaString strUrlMetaInfos = s_strBaseUrl;
	strUrlMetaInfos += METAINFOS_URL;
	strUrlMetaInfos += L"/";
	strUrlMetaInfos += pMeta->m_strUniqueId;
	strUrlMetaInfos += L"/";

	NaString strData;
	switch (nOption)
	{
	case UPDATE_METAINFO_CLEARED:
		strUrlMetaInfos += L"Cleared";
		strData.Format(L"%d", pMeta->m_nCleared);
		break;
	case UPDATE_METAINFO_FAILED:
		strUrlMetaInfos += L"Failed";
		strData.Format(L"%d", pMeta->m_nFailed);
		break;
	case UPDATE_METAINFO_LIKE:
		strUrlMetaInfos += L"Like";
		strData.Format(L"%d", pMeta->m_nLike);
		break;
	case UPDATE_METAINFO_CREATORCOMMENT:
		strUrlMetaInfos += L"CreatorComment";
		strData.Format(L"\"%ls\"", pMeta->m_strCreatorComment.wstr());
		break;
	case UPDATE_METAINFO_DEVELOPERRATING:
		strUrlMetaInfos += L"DeveloperRating";
		strData.Format(L"%d", pMeta->m_nDeveloperRating);
		break;
	case UPDATE_METAINFO_GAMETHEME:
		// For Migration
		strUrlMetaInfos += L"GameTheme";
		strData.Format(L"\"%ls\"", pMeta->m_strGameTheme.wstr());
		break;
	default:
		throw NaException(__FUNCTIONW__, __LINE__, L"Called UpdateMetaInfo with wrong option");
		break;
	}

	strUrlMetaInfos += L".json";

	NaString strRet = curl.Put(strUrlMetaInfos, strData);

	Json::Value jMeta;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jMeta))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update metainfo failed");
	}
}

void ServerRequest::UpdateUserInfo(ServerUserInfo * pUserInfo, int nOption)
{
	NaDebugOut(L"// Update UserInfo (Uid: %ls) (op: %d)\n", pUserInfo->m_strUniqueId.wstr(), nOption);

	if (pUserInfo->m_strUniqueId.GetLength() == 0)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Userinfo doesn't have unique id. Please report to developer with screenshot");
	}

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += USERS_URL;
	strUrl += L"/";
	strUrl += pUserInfo->m_strUniqueId;
	strUrl += L"/";

	NaString strData;
	switch (nOption)
	{
	case UPDATE_USERINFO_LASTUPLOADED:
		strUrl += L"LastUploaded";
		strData.Format(L"\"%ls\"", pUserInfo->m_strLastUploaded.wstr());
		break;
	case UPDATE_USERINFO_SECURETEXT:
		strUrl += L"SecureText";
		strData.Format(L"\"%ls\"", pUserInfo->m_strSecureText.wstr());
		break;
	case UPDATE_USERINFO_PERMISSION:
		strUrl += L"Permission";
		strData.Format(L"%d", pUserInfo->m_nPermission);
		break;
	default:
		throw NaException(__FUNCTIONW__, __LINE__, L"Called Userinfo with wrong option");
		break;
	}

	strUrl += L".json";

	NaString strRet = curl.Put(strUrl, strData);

	Json::Value jUser;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jUser))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update Userinfo failed");
	}
}

std::vector<StageCommentInfo*> ServerRequest::GetComments(NaString strUniqueId)
{
	NaDebugOut(L"// Get Comments...\n");

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += COMMENTS_URL;
	strUrl += L"/";
	strUrl += strUniqueId;
	strUrl += L".json";

	NaString strRet = curl.Post(strUrl);

	Json::Value jComments;
	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jComments))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Comments broken: %ls", strUniqueId);
	}

	NaDebugOut(L"Comments Count: %d\n", jComments.size());
	int nIndex = 0;
	std::vector<StageCommentInfo*> vec;
	for (auto jIt = jComments.begin(); jIt != jComments.end(); jIt++)
	{
		StageCommentInfo *pComment = new StageCommentInfo;
		pComment->BuildPropertyMap();
		
		try
		{
			JsonManager::LoadJsonObjectProperty(pComment, *jIt);
		}
		catch (Json::LogicError &e)
		{
			NaDebugOut(L"Error: Bad Comments found: %ls\n", pComment->m_strUniqueId.wstr());
			delete pComment;
			continue;
		}

		//pComment->m_nIndex = nIndex;
		nIndex++;

		vec.push_back(pComment);
	}

	return vec;
}

void ServerRequest::UpdateComment(NaString strStageUniqueId, NaString strPlayerUniqueId, StageCommentInfo *pComment)
{
	NaDebugOut(L"// Update Comment...\n");

	if (strStageUniqueId.GetLength() == 0 ||
		strPlayerUniqueId.GetLength() == 0 ||
		pComment == nullptr)
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update comment failed (Something is empty)");
	}

	NaCurl curl;
	NaString strUrl = s_strBaseUrl;
	strUrl += COMMENTS_URL;
	strUrl += L"/";
	strUrl += strStageUniqueId;
	strUrl += L"/";
	strUrl += strPlayerUniqueId;
	strUrl += L".json";

	Json::Value jComment;
	JsonManager::SaveJsonObjectProperty(pComment, jComment);
	NaString strComment = jComment.toStyledString().c_str();
	NaString strRet = curl.Put(strUrl, strComment);

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jComment))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Update comment failed");
	}
}

bool ServerRequest::CheckUpdate(Json::Value &jUpdateInfo)
{
	NaDebugOut(L"// Check Update...\n");

	NaCurl curl;
	NaString strUrl;
	strUrl.Format(L"%ls/update.json", s_strUpdateUrl.wstr());

	NaString strRet = curl.Post(strUrl);

	Json::Reader jReader;
	if (!jReader.parse(strRet.cstr(), jUpdateInfo))
	{
		throw NaException(__FUNCTIONW__, __LINE__, L"Check update failed.");
	}
	
	return true;
}

void ServerRequest::DownloadUpdateFiles(UpdateInfo &info, std::function<void(long lSize)> fnCallback)
{
	NaDebugOut(L"// Download UpdateFiles...\n");

	auto it = info.m_map.begin();
	for (; it != info.m_map.end(); ++it)
	{
		NaString strUrl;
		strUrl.Format(L"%ls/%ls", s_strUpdateUrl.wstr(), it->first.wstr());
		strUrl.ReplaceAll(L"\\", L"/");

		NaDebugOut(L"Download: %ls \n", it->first.wstr());

		char *buf = new char[it->second.m_lFileSize + 1];
		NaCurl curl;
		if (curl.Get(strUrl, buf, it->second.m_lFileSize))
		{
			buf[it->second.m_lFileSize] = 0;

			NaString strDownloadPath;
			strDownloadPath.Format(L"%ls/UserData/Update/%ls", 
				PathManager::GetGameRootPath(nullptr),
				it->first.wstr());
			strDownloadPath.ReplaceAll(L"/", L"\\");

			NaDebugOut(L"- Download: %ls\n", strDownloadPath.wstr());

			FILE *fp;
			NaPlatform::FileOpen(&fp, strDownloadPath.cstr(), "wb");
			fwrite(buf, it->second.m_lFileSize, 1, fp);
			fclose(fp);

			// UI Callback
			fnCallback(it->second.m_lFileSize);
		}
		else
		{
			NaDebugOut(L"(Error) Download Failed.\n");
		}

		delete[] buf;
	}
}

NaString ServerRequest::GetStageString(Stage * pStage)
{
	Json::Value jMap;
	pStage->SaveJsonMap(jMap);

	std::string stdstr = jMap.toStyledString();
	NaString strRet = stdstr.c_str();
	return strRet;
}
