#pragma once

#include "GameStateBase.h"

#include <vector>

class BossBridge;
class BossBridgeAxe;
class GameObjectBase;
class GameStateCutBridge : public GameStateBase
{
public:
	GameStateCutBridge(Game *pGame);
	virtual ~GameStateCutBridge();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	int m_nTimeStamp;
	std::vector<GameObjectBase*> m_vecOverBridge;
	BossBridge *m_pBridge;
	BossBridgeAxe *m_pAxe;
	int m_nFallTime;
};

