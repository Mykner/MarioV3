#pragma once

#include "GameStateBase.h"

#include "Princess.h"

class GameObjectBase;
class GameStateRescuePrincess : public GameStateBase
{
public:
	GameStateRescuePrincess(Game *pGame);
	virtual ~GameStateRescuePrincess();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	void ProcessThankWord(int nStateFrame);

	int m_nTimeStamp;
	Princess *m_pPrincess;
	int m_nStopPointX;
	bool m_bThankWordComplete;
	NaStrArray m_arCustomMessage;
	bool m_bUseCustomMessage;
};

