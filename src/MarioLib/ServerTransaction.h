#pragma once

#include "NaCurl.h"

#include <thread>
#include "NaException.h"
#include <vector>

enum JobTypes
{
	JOB_NONE = 0,

	JOB_ADD_CLEARED,
	JOB_ADD_CLEARED_END,
	JOB_LIKE,
	JOB_LIKE_END,
	JOB_ADD_FAILED,
	JOB_ADD_FAILED_END,
	JOB_LOAD_WORLDSTAGE_LIST,
	JOB_LOAD_WORLDSTAGE_LIST_END,
	JOB_LOAD_WORLDSTAGE_LIST_FAIL,
	JOB_LOAD_WORLDSTAGE_DETAIL,
	JOB_LOAD_WORLDSTAGE_DETAIL_END,
	JOB_LOAD_WORLDSTAGE_DETAIL_FAIL,
	JOB_DOWNLOAD_WORLDSTAGE,
	JOB_DOWNLOAD_WORLDSTAGE_END,
	JOB_DOWNLOAD_WORLDSTAGE_FAIL,
	JOB_RATE,
	JOB_RATE_END,
	JOB_POST_COMMENT,
	JOB_POST_COMMENT_END,
	JOB_LOAD_WORLDSTAGE_RECORD,
	JOB_LOAD_WORLDSTAGE_RECORD_END,
	JOB_LOAD_WORLDSTAGE_RECORD_FAIL,
	JOB_UPDATE_DEATHPOSITION,
	JOB_CHECK_REGISTER_TICKET,
	JOB_CHECK_REGISTER_TICKET_END,
	JOB_CHECK_REGISTER_TICKET_FAIL,
	JOB_REGISTER_TICKET,
	JOB_REGISTER_TICKET_END,
	JOB_REGISTER_TICKET_FAIL,
	JOB_REGISTER_TICKET_CODE,
	JOB_REGISTER_TICKET_CODE_END,
	JOB_REGISTER_TICKET_CODE_FAIL,
	JOB_REGISTER_USER,
	JOB_REGISTER_USER_END,
	JOB_REGISTER_USER_FAIL,
	JOB_CHECK_USER_PERMISSION,
	JOB_CHECK_USER_PERMISSION_END,
	JOB_CHECK_USER_PERMISSION_FAIL,
};

class Sprite;
class Game;
class StageMetaInfo;
class StageCommentInfo;
class StageRecordInfo;
class ServerRegisterTicket;
class ServerTransaction
{
public:
	ServerTransaction();
	virtual ~ServerTransaction();

	void Init(Game *pGame);
	bool IsWorking();
	bool IsJobFinished();

	void DeleteThread();

	void BeginJob(int nJobType);
	void ChangeJob(int nJobType);
	void EndJob();

	void Render(int x, int y, float fScale = 1.0f);

	bool m_bWorking;
	std::thread* m_pJobThread;
	int m_nJobType;
	Sprite *m_pSpriteEtc;

	class TransactionJobResponse
	{
	public:
		std::vector<StageMetaInfo*> vecMetaInfo;
		std::vector<StageCommentInfo*> vecCommentInfo;
		void *pUI;
		StageRecordInfo *pStageRecordInfo;
		ServerRegisterTicket *pServerRegisterTicket;
		NaString strReason;
	};
	TransactionJobResponse m_Response;

	Game *m_pGame;
};