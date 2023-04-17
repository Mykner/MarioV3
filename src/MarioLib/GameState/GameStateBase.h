#pragma once

#include "Common.h"
#include "GameStates.h"

class Game;
class PipeInfo;
class GameObjectBase;
class PipeExitInfo;
class GameStateBase
{
public:
	GameStateBase(Game *pGame);
	virtual ~GameStateBase();

	virtual long GetBackgroundColor();
	virtual void Init();
	virtual void Process();
	virtual void Render();

	virtual void DeleteOldState();
	virtual void PopState();

	void MoveCameraToPlayer();
	void MoveCameraToExit(PipeExitInfo *pPipeExitInfo);

	void MovePlayerObjectToExit(PipeExitInfo *pPipeExitInfo);
	void MovePlayerObjectToSkyExit();
	void MovePlayerObjectToPairDoor(GameObjectBase *pEntrance);

	PipeExitInfo* FindFirstExitOnlyPipe();
	PipeExitInfo* FindPipeExit(int nIndex);
	GameObjectBase* FindPairDoor(GameObjectBase *pEntrance);
	GameObjectBase* FindPiranhaPlantAtExit(NaRect rcExit);

	void ClearEnemyInViewport();
	void ChangeEnemyInViewportToCoin();
	void ChangeVehicleInViewportToCoin();

	void SetEnemyInViewportState(int nState);
	
	void InitCustomStageSelectUI(int nFirstStageIDUI, int nBackIDUI);

	static GameStateBase* CreateInstance(Game *pGame, int nType);

	int m_nStateFrame;
	int m_nType;
	Game *m_pGame;
	GameStateBase *m_pOldState;
};
