#pragma once

#include "Common.h"

#include "GameStateBase.h"
#include <vector>
#include "NaPlatformWindow.h"
#include <thread>
#include "GameOption.h"

#include "ServerRequest.h" // StageRecordInfo

class DirectXManager;
class OpenGLManager;
class RenderManager;
class FModManager;
class SpriteManager;
class Player;
class Stage;
class Camera;
class InputManager;
class ThemeManager;
class PipeInfo;
class UIManager;
class SoundManager;
class ResourceHolder;
class CreateParameter;
class Option;
class GameStateChangeParameter
{
public:
	GameStateChangeParameter()
	{
		m_pCurPipe = nullptr;
		pFromObject = nullptr;
	};

	NaString m_strOption;
	union
	{
		PipeInfo* m_pCurPipe;
		GameObjectBase *pFromObject;
	};
};

class Game
{
public:
	Game();
	virtual ~Game();

	// Main Loop
	void ProcessGameLoop();
	void ProcessGame();

	// FrameSkip (not used)
	void BeginFrameSkip();
	void EndFrameSkip();
	bool CanUpdateProcess();
	bool IsSkipFrame();

	virtual void Render();

	// Scene change effect
	void CreateSceneEffect(float fX, float fY, int nType, CreateParameter *pParam = nullptr);
	void ProcessSceneEffect();
	void ClearSceneEffect();
	void RenderSceneEffect();

	// Life cycle
	virtual void Init(NaPlatformWindowHandle hWindow, const wchar_t* szCurrentDir);
	void Release();
	void SetInitialStage(NaString strStage);
	void OnExit();
	void AttachWindow(NaPlatformWindowHandle hWindow);

	// Load resource functions
	void LoadResources();

	static void LoadSoundThead(void *pParam);
	static void LoadBGMThread(void *pParam);

	void LoadPreloadResources();
	void LoadSprites();
	void LoadBGMs();
	void LoadSounds();

	// State machine
	virtual void InitState(int nState);
	virtual void OnChangeState();
	void ChangeState(int nState, GameStateChangeParameter *pParam = nullptr);
	int GetStateFrame();

	bool IsActiveAbility(int nFlag);

	// Stage load utilities
	void LoadEntranceStage();
	void LoadNextStage();
	void LoadPipeStage();
	void LoadSkyStage();
	void LoadWarpStage(NaString strStage);
	void LoadSubStage();

	void PopStage();
	void PopAllStage();

	void SwapCoinAndBrick(bool bIsInitial = true);

	// Player, camera utilities
	void InitPlayerObject(POINT ptLastCheckPoint = { -1, -1 });
	void InitCamera();

	// for Load
	std::thread *m_pLoadSoundThread;
	std::thread *m_pLoadBGMThread;

	// Members
	NaString m_strInitialStage;
	NaString m_strGameRootDir;
	NaString m_strDocumentDir;
	NaString m_strStagePack;
	int m_nStagePackType;
	int m_nStagePackAbilityFlag;

	// Game
	GameStateBase *m_pGameState;
	std::vector<Player*> m_vecPlayers;
	Player *m_pCurPlayer;
	Stage *m_pCurStage;
	Camera *m_pCamera;
	int m_nNextState;
	GameStateChangeParameter m_paramNextState;
	int m_nGameFrame;
	bool m_bModernStyle;
	int m_nSpecialEvents;
	bool m_bIsDesignGame;
    bool m_bExit;
	std::vector<GameObjectBase*> m_vecEffect;
	GameOption m_Option;

	// #TODO Move to CacheManager?
	NaString m_strLastStage;
	StageRecordInfo m_LastStageRecord;

	// FrameSkip
	struct FrameSkipInfo {
		float m_fOldTime;
		float m_fCurTime;
		float m_fFPS;
		float m_fValue; // #TODO naming is bad
		float m_fDt;
		
		// for check current fps
		int m_nRenderCnt;
		float m_fTimer;
	};
	FrameSkipInfo m_FrameSkipInfo;

	// Window
	NaPlatformWindowHandle m_hWindow;
	int m_nWidth;
	int m_nHeight;
	virtual inline int Width() { return m_nWidth; };
	virtual inline int Height() { return m_nHeight; };

	// Managers
	RenderManager *m_pRenderManager;
	FModManager *m_pFMod;
	SpriteManager *m_pSpriteManager;
	InputManager *m_pInputManager;
	ThemeManager *m_pThemeManager;
	UIManager *m_pUIManager;
	SoundManager *m_pSoundManager;

	// Resource holder for share resources
	ResourceHolder *m_pResourceHolder;
};

