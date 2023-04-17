#pragma once

#include "NaCurl.h"

#include "StageMetaInfo.h"
#include <vector>
#include "PropertyMap.h"
#include "json.h"
#include "Updater.h"
#include <functional>

class ServerInfo : public PropertyObjectBase
{
public:
	ServerInfo();

	int m_nVersion;
	int m_nStatus;
	int m_nCanPlayGame;
	int m_nCanUploadStage;
	int m_nCanRegisterUser;
	int m_nCanPlayStage;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Version", &m_nVersion)
		DECL_PROP("Status", &m_nStatus)
		DECL_PROP("CanPlayGame", &m_nCanPlayGame)
		DECL_PROP("CanPlayStage", &m_nCanPlayStage)
		DECL_PROP("CanRegisterUser", &m_nCanRegisterUser)
		DECL_PROP("CanUploadStage", &m_nCanUploadStage)
	END_DECLARE_PROPERTYMAP()
};

class ServerUserInfo : public PropertyObjectBase
{
public:
	ServerUserInfo();

	NaString m_strUniqueId;

	NaString m_strName;
	int m_nPermission;
	NaString m_strSecureText;
	NaString m_strLastPlayed;
	NaString m_strLastUploaded;
	NaString m_strAvatar;
	
	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Name", &m_strName)
		DECL_PROP("Permission", &m_nPermission)
		DECL_PROP("SecureText", &m_strSecureText)
		DECL_PROP("LastPlayed", &m_strLastPlayed)
		DECL_PROP("LastUploaded", &m_strLastUploaded)
		DECL_PROP("Avatar", &m_strAvatar)
	END_DECLARE_PROPERTYMAP()
};

class ServerRegisterTicket : public PropertyObjectBase
{
public:
	ServerRegisterTicket();

	NaString m_strTicketHash;

	int m_nTicketNumber;
	NaString m_strName;
	NaString m_strPassword;
	NaString m_strSecurityText;
	NaString m_strUserUniqueId;
	NaString m_strMacAddress;
	NaString m_strPcName;
	NaString m_strWrongCodeReference;
	int m_nStatus;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("TicketNumber", &m_nTicketNumber)
		DECL_PROP("Name", &m_strName)
		DECL_PROP("Password", &m_strPassword)
		DECL_PROP("SecurityText", &m_strSecurityText)
		DECL_PROP("UserUniqueId", &m_strUserUniqueId)
		DECL_PROP("MacAddress", &m_strMacAddress)
		DECL_PROP("PcName", &m_strPcName)
		DECL_PROP("WrongCodeReference", &m_strWrongCodeReference)
		DECL_PROP("Status", &m_nStatus)
	END_DECLARE_PROPERTYMAP()
};

class StageCommentInfo : public PropertyObjectBase
{
public:
	StageCommentInfo();

	NaString m_strName;
	NaString m_strUniqueId; // Player
	NaString m_strAvatar;
	NaString m_strComment;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Name", &m_strName)
		DECL_PROP("UniqueId", &m_strUniqueId)
		DECL_PROP("Comment", &m_strComment)
		DECL_PROP("Avatar", &m_strAvatar)
	END_DECLARE_PROPERTYMAP()
};

class StageRecordInfo : public PropertyObjectBase
{
public:
	StageRecordInfo();

	NaString m_strDeathPositions;
	int m_nBestTime;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("DeathPositions", &m_strDeathPositions)
		DECL_PROP("BestTime", &m_nBestTime)
	END_DECLARE_PROPERTYMAP()
};

enum UpdateMetaInfoOptions
{
	UPDATE_METAINFO_CLEARED = 1,
	UPDATE_METAINFO_FAILED,
	UPDATE_METAINFO_LIKE,
	UPDATE_METAINFO_CREATORCOMMENT,
	UPDATE_METAINFO_DEVELOPERRATING,

	UPDATE_METAINFO_GAMETHEME,
};

enum UpdateUserInfoOptions
{
	UPDATE_USERINFO_LASTUPLOADED = 1,
	UPDATE_USERINFO_SECURETEXT,
	UPDATE_USERINFO_PERMISSION,
};

class Stage;
class ServerRequest
{
public:
	ServerRequest();
	virtual ~ServerRequest();

	// Upload Stage
	bool CanUpdate(Stage *pStage, NaString strUserUniqueId);
	void UploadStage(Stage *pStage);
	void UpdateStageUniqueId(Stage *pStage);
	void UpdateStage(Stage *pStage);
	StageMetaInfo* UpdateMetaInfo(Stage *pStage, NaString strCreatorUniqueId);

	// Download Stage
	std::vector<StageMetaInfo*> GetMetaInfoList();
	bool DownloadStage(NaString strUniqueId, NaString strFilePath);

	// Register User
	ServerRegisterTicket* GetRegisterTicket(NaString strTicketHash);
	int GetLastRegisterTicketNumber();
	void SubmitRegisterInfo(NaString strTicketHash, ServerRegisterTicket *pTicket);
	void SubmitRegisterTicketCode(NaString strTicketHash, NaString strCode, NaString strRefer);

	// User Profile
	ServerUserInfo* GetUserInfo(NaString strUniqueId);
	ServerUserInfo* RegisterUser(NaString strName);

	// Record Handling
	StageRecordInfo* GetStageRecordInfo(NaString strUniqueId);
	void UpdateStageRecordInfo(NaString strUniqueId, NaString strDeathPositions);
	void UpdateStageRecordInfo(NaString strUniqueId, int nBestTime);

	// Interaction
	ServerInfo* GetServerInfo();
	StageMetaInfo* GetMetaInfo(NaString strUniqueId);
	void UpdateMetaInfo(StageMetaInfo *pMeta);
	void UpdateMetaInfo(StageMetaInfo *pMeta, int nOption);
	void UpdateUserInfo(ServerUserInfo *pUserInfo, int nOption);
	std::vector<StageCommentInfo*> GetComments(NaString strUniqueId);
	void UpdateComment(NaString strStageUniqueId, NaString strPlayerUniqueId, StageCommentInfo *pComment);

	// Update
	bool CheckUpdate(Json::Value &jUpdateInfo);
	void DownloadUpdateFiles(UpdateInfo &info, std::function<void(long lSize)> fnCallback);

	// Util
	NaString GetStageString(Stage *pStage);

	static int s_nCurrentVersion;
	static NaString s_strBaseUrl;
	static NaString s_strUpdateUrl;
};

