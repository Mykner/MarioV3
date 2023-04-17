#pragma once

#include "GameStateBase.h"
#include "Game.h"

#include <thread>
#include "ServerTransaction.h"

class UIBase;
class ServerRegisterTicket;
class GameStateRegisterUser : public GameStateBase
{
public:
	GameStateRegisterUser(Game *pGame);
	virtual ~GameStateRegisterUser();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	NaString m_strMacAddress;
	NaString m_strComputerName;
	NaString m_strUserUniqueId;
	NaString m_strLocalKey;

	void InitCheckTicketUI();
	static void CheckTicket(void *pParam);

	void InitRegisterTicketUI();
	static void RegisterTicket(void *pParam);

	void InitRegisterTicketCodeUI(ServerRegisterTicket *pTicket);
	static void RegisterTicketCode(void *pParam);

	void InitRegisterUserUI();
	static void RegisterUser(void *pParam);

	void InitWrongPermissionUI();

	ServerTransaction m_Transaction;

	enum GameStateRegisterUserUI
	{
		UI_UNDEFINED = 0,

		UI_USERNAME,
		UI_PASSWORD,
		UI_SECUREWORD,
		UI_SUBMIT_TICKET,

		UI_REGISTERCODE,
		UI_SUBMIT_REGISTERCODE,


	};
};