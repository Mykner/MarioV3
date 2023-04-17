#pragma once

#include "ControllerBase.h"
#include "Common.h"

#include "GameObjectBase.h"
#include <vector>

class Game;
class UserProfile;
class Player : public ControllerBase
{
public:
	Player(Game *pGame);
	virtual ~Player();

	virtual bool IsInputHold(int nInputType, bool bApplyInputLock = true);

	void OnBeforeProcess();
	void Process();
	void OnAfterProcess();
	void ProcessTime();
	void ProcessPSwitchTime();

	void RenderControlObj();
	void RenderShapeChange();
	void RenderStatus();
	void RenderClassicStatus();
	void RenderCustomStatus();
	void RenderDesignStatus();

	void SetControl(GameObjectBase *pObj);
	GameObjectBase* GetControl();
	GameObjectBase* GetLastControlObject();

	virtual void AddScore(int nScore);
	virtual void AddScoreWithEffect(int nScore, GameObjectBase* pFromObject);
	virtual void AddCoin();
	virtual void AddLife();

	void OnLostLife();

	virtual void AddDoorKeyCoin();
	virtual void AddDoorKey(GameObjectBase *pKey);
	virtual bool HasDoorKey();
	virtual void UseDoorKey();
	virtual void ClearDoorKey();
	
	void ApplyUserProfile(UserProfile *pProfile);
	void ApplyUserProfile(NaString strName);
	void LoadUserProfile(NaString strName);
	void SaveUserProfile();
	void Reset();

	void ApplyTheme();

	int m_nJumpHold;
	std::vector<GameObjectBase*> m_vecPressedEnemy;
	
	UserProfile* m_pProfile;
	GameObjectBase *m_pControlObj;

	// Property
	int m_nScore;
	int m_nLife;
	int m_nCoin;
	int m_nTime;
	int m_nDoorKeyCoin;
	std::vector<GameObjectBase*> m_vecDoorKey;

	int m_nPSwitchTime;
};