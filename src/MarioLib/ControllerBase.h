#pragma once

#include "Common.h"
#include "GameObjectBase.h"

class Game;
class ControllerBase
{
public:
	ControllerBase(Game *pGame);
	virtual ~ControllerBase();

	virtual void BindInputs(int, int, int, int, int, int);
	virtual void BindInputs(int[6]);
	virtual bool IsInputHold(int nInputType, bool bApplyInputLock = true);
	virtual bool IsInputPressed(int nInputType, bool bApplyInputLock = true);

	virtual void LockInput();
	virtual void UnlockInput();
	virtual void HoldInput(int nInputType);
	virtual void UnholdInput(int nInputType);
	virtual void UnholdInputAll();

	virtual void AddScore(int nScore) {};
	virtual void AddScoreWithEffect(int nScore, GameObjectBase* pFromObject) {};
	virtual void AddCoin() {};
	virtual void AddLife() {};

	virtual void AddDoorKeyCoin() {};
	virtual void AddDoorKey(GameObjectBase *pKey) {};
	virtual bool HasDoorKey() { return false; };
	virtual void UseDoorKey() {};
	virtual void ClearDoorKey() {};

	int m_nKeys[6];
	int m_nForceInput[6];
	bool m_bInputLock;
	Game *m_pGame;
};

