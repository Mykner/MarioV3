#include "GameStateBase.h"

#include "Game.h"
#include "SpriteManager.h"
#include "Player.h"
#include "Stage.h"
#include "StagePack.h"
#include "Camera.h"
#include "SoundManager.h"

#include "GameStateTest.h"
#include "GameStatePlay.h"
#include "GameStateShapeChange.h"
#include "GameStateDie.h"
#include "GameStateEnterPipe.h"
#include "GameStateExitPipe.h"
#include "GameStateFlagDown.h"
#include "GameStateGoal.h"
#include "GameStateTitle.h"
#include "GameStateStageIntro.h"
#include "GameStateEnterStage.h"
#include "GameStateGameOver.h"
#include "GameStateTimeUp.h"
#include "GameStateCutBridge.h"
#include "GameStateRescuePrincess.h"
#include "PipeInfo.h"
#include "PiranhaPlant.h"
#include "GameStateDesignPlay.h"
#include "GameStateEnterSky.h"
#include "GameStateSkyIntro.h"
#include "GameStateSingleStageClear.h"
#include "GameStateSingleStageGameOver.h"
#include "GameStateEnterDoor.h"
#include "GameStateExitDoor.h"
#include "Door.h"
#include "GameStateMessageBlock.h"
#include "GameStateLogo.h"
#include "GameStateCredits.h"
#include "GameStateHitGoalBox.h"
#include "GameStateHitGoalPost.h"
#include "GameStateAppUpdate.h"
#include "GameStateRegisterUser.h"

#include "UIManager.h"
#include "UIPanel.h"
#include "UIList.h"
#include "NaPlatformAPI.h"
#include "PathManager.h"

#include "VehicleObjectBase.h"

#include "TypeNames.h"

GameStateBase::GameStateBase(Game *pGame)
{
	m_pGame = pGame;
	m_pOldState = nullptr;
	m_nStateFrame = 0;
}

GameStateBase::~GameStateBase()
{
}

long GameStateBase::GetBackgroundColor()
{
	if (CUR_STAGE)
		return CUR_STAGE->GetBackgroundColor();

	return 0;
}

void GameStateBase::Init()
{
	
}

void GameStateBase::Process()
{

}

void GameStateBase::Render()
{
	
}

void GameStateBase::DeleteOldState()
{
	if (m_pOldState)
	{
		delete m_pOldState;
		m_pOldState = nullptr;
	}
}

void GameStateBase::PopState()
{
	if (m_pOldState)
	{
		//m_pGame->InitState(m_pOldState);

		m_pGame->m_pGameState = m_pOldState;
		m_nStateFrame++;
		m_pGame->m_nNextState = -1;

		NaDebugOut(L"Init GameState: %d:%ls (by PopState)\n", m_pOldState->m_nType, g_szGameStates[m_pOldState->m_nType]);
	}
}

void GameStateBase::MoveCameraToPlayer()
{
	if (CUR_STAGE->m_bCanGoBack)
	{
		CAMERA->SetPosition(
			CUR_PLAYER_OBJ->m_fX - (m_pGame->Width() / 2),
			CUR_PLAYER_OBJ->m_fY - (m_pGame->Height() / 2));
	}
	else
	{
		CAMERA->SetPosition(
			CUR_PLAYER_OBJ->m_fX - (TILE_XS * 3), 
			CUR_PLAYER_OBJ->m_fY - (m_pGame->Height() / 2));
	}
}

void GameStateBase::MoveCameraToExit(PipeExitInfo *pPipeExitInfo)
{
	NaRect &rc = pPipeExitInfo->m_rcExit;
	if (CUR_STAGE->m_bCanGoBack)
	{
		CAMERA->SetPosition(rc.left + 
			(rc.Width() / 2) - 
			(m_pGame->Width() / 2), 
			rc.top - (m_pGame->Height() / 2)
		);
	}
	else
	{
		CAMERA->SetPosition(
			rc.left - (TILE_XS * 3), 
			rc.top - (m_pGame->Height() / 2)
		);
	}
}

void GameStateBase::MovePlayerObjectToExit(PipeExitInfo *pPipeExitInfo)
{
	//NaDebugOut(L"Move Player to PipeExit(0x%08x). ExitType: %d\n", pPipeInfo, pPipeInfo->m_nExitType);

	GameObjectBase *pControlObj = CUR_PLAYER->GetLastControlObject();

	NaRect &rc = pPipeExitInfo->m_rcExit;
	switch (pPipeExitInfo->m_nType)
	{
	case PIPEENTRANCE_UPTODOWN:
		pControlObj->SetPosition(
			rc.left + rc.Width() / 2,
			rc.bottom + 32
		);
		pControlObj->ChangeState(STATE_PIPEUP);
		break;
	case PIPEENTRANCE_DOWNTOUP:
		pControlObj->SetPosition(
			rc.left + rc.Width() / 2,
			rc.top - 16
		);
		pControlObj->ChangeState(STATE_PIPEDOWN);
		break;
	case PIPEENTRANCE_LEFTTORIGHT:
		pControlObj->m_bFlip = true;
		pControlObj->SetPosition(
			rc.left + 8,
			rc.bottom
		);
		pControlObj->ChangeState(STATE_PIPELEFT);
		break;
	case PIPEENTRANCE_RIGHTTOLEFT:
		pControlObj->m_bFlip = false;
		pControlObj->SetPosition(
			rc.left - 8,
			rc.bottom
		);
		pControlObj->ChangeState(STATE_PIPERIGHT);
		break;
	}

	// Move Piranha Plant into Pipe if exists
	PiranhaPlant *pPiranhaPlant = (PiranhaPlant*)FindPiranhaPlantAtExit(rc);
	if (pPiranhaPlant)
	{
		if (pPiranhaPlant->IsAlive())
		{
			pPiranhaPlant->HideForExitPipe();
		}
	}
}

void GameStateBase::MovePlayerObjectToSkyExit()
{
	SOUND_MANAGER->StopBGM();

	GameObjectBase *pControlObj = CUR_PLAYER->GetLastControlObject();

	pControlObj->SetPosition(CUR_STAGE->m_ptSkyExit.x, 0);
	pControlObj->ResetSpeed();
	pControlObj->SetState(STATE_FALL);

	CAMERA->SetPosition(CUR_STAGE->m_ptSkyExit.x - (16 * 5), 0);

	CUR_STAGE->PlayBGM();
}

void GameStateBase::MovePlayerObjectToPairDoor(GameObjectBase * pEntrance)
{
	GameObjectBase *pObj = FindPairDoor(pEntrance);
	if (pObj)
	{
		GameObjectBase *pControlObj = CUR_PLAYER->GetLastControlObject();

		pControlObj->SetPosition(pObj->m_fX, pObj->m_fY);
		pControlObj->ResetSpeed();
		
		pControlObj->MoveCarryObject();
		return;
	}
	
	NaDebugOut(L"Error: Cannot find pair door!\n");
}

PipeExitInfo * GameStateBase::FindFirstExitOnlyPipe()
{
	return FindPipeExit(0);
}

PipeExitInfo * GameStateBase::FindPipeExit(int nIndex)
{
	PipeExitInfo *pInfo = nullptr;
	BEGIN_VECTOR_LOOP(PipeExitInfo*, CUR_STAGE->m_vecPipeExitInfo) {
		if (pObj->m_nIndex == nIndex)
			return pObj;
	} END_VECTOR_LOOP();

	return nullptr;
}

GameObjectBase * GameStateBase::FindPairDoor(GameObjectBase * pEntrance)
{
	BEGIN_VECTOR_LOOP(GameObjectBase*, CUR_STAGE->m_vecEventObject) {
		if (pObj->m_nType != EVENTOBJECT_DOOR)
			continue;

		if (pObj == pEntrance)
			continue;

		if (((Door*)pObj)->m_nPairIndex == ((Door*)pEntrance)->m_nPairIndex)
			return pObj;
	} END_VECTOR_LOOP();

	return nullptr;
}

GameObjectBase * GameStateBase::FindPiranhaPlantAtExit(NaRect rcExit)
{
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		if ((*it)->m_nType == ENEMY_PIRANHA_PLANT)
		{
			rcExit.top -= 100;

			NaRect rcPiranha = (*it)->GetRect();
			if (rcExit.IsOverlapped(rcPiranha))
				return (*it);
		}
	}

	return nullptr;
}

void GameStateBase::ClearEnemyInViewport()
{
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		GameObjectBase *pObj = *it;
		if (pObj->IsOutOfCamera() == false)
			pObj->SetState(STATE_DEAD);
	}
}

void GameStateBase::ChangeEnemyInViewportToCoin()
{
	CUR_PLAYER_OBJ->StopCombo();

	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		if ((*it)->IsOutOfCamera() == false)
		{
			GameObjectBase *pObj = *it;

			int nScore = CUR_PLAYER_OBJ->CalcScoreFrom(100);
			CUR_PLAYER->AddScoreWithEffect(nScore, pObj);
			CUR_PLAYER_OBJ->AddCombo();

			NaRect rc = pObj->GetRect();
			CUR_STAGE->CreateEffect(rc.Center(), rc.Middle(), EFFECT_POP);
			pObj->SetState(STATE_DEAD);
		}
	}
}

void GameStateBase::ChangeVehicleInViewportToCoin()
{
	VehicleObjectBase *pVehicle = ((VehicleObjectBase*)(CUR_PLAYER_OBJ->m_pVehicle));
	if (pVehicle)
	{
		pVehicle->OnGetOffPassenger();

		int nScore = CUR_PLAYER_OBJ->CalcScoreFrom(10000);
		CUR_PLAYER->AddScoreWithEffect(nScore, pVehicle);
		CUR_PLAYER_OBJ->AddCombo();

		NaRect rc = pVehicle->GetRect();
		CUR_STAGE->CreateEffect(rc.Center(), rc.Middle(), EFFECT_POP);
		pVehicle->SetState(STATE_DEAD);
	}
}

void GameStateBase::SetEnemyInViewportState(int nState)
{
	std::vector<GameObjectBase*>::iterator it = CUR_STAGE->m_vecEnemy.begin();
	for (; it != CUR_STAGE->m_vecEnemy.end(); ++it)
	{
		if ((*it)->IsOutOfCamera() == false)
		{		
			GameObjectBase *pObj = *it;
			if (!pObj->IsAlive())
				continue;
			if (!pObj->CanPowed())
				continue;
			if (pObj->m_bCollision[COLLISION_BOTTOM] == false && pObj->m_nState != STATE_STACKED)
				continue;

			switch (pObj->m_nState)
			{
			case STATE_MOVEUP:
			case STATE_MOVEDOWN:
				if (IS_ENEMY(pObj))
				{
					if (((EnemyObjectBase*)pObj)->m_bWinged)
						continue;
					if (pObj->m_fYS != 0)
						continue;
				}
				break;
			default:
				if (pObj->m_fYS != 0)
					continue;
				break;
			}

			int nScore = CUR_PLAYER_OBJ->CalcScoreFrom(100);
			CUR_PLAYER->AddScoreWithEffect(nScore, pObj);
			CUR_PLAYER_OBJ->AddCombo();

			pObj->ChangeState(nState);
		}
	}

	CUR_PLAYER_OBJ->StopCombo();
}

void GameStateBase::InitCustomStageSelectUI(int nFirstStageIDUI, int nBackIDUI)
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int _y = 0;
	UI_MANAGER->CreateText(0, 0, _y, L"Select Stage");
	_y += 16;

	// Load Custom Stage List
	{
		UIList *pList = (UIList*)UI_MANAGER->CreateList(0, 0, _y, m_pGame->m_nWidth - 64, 24 * 7);
		_y += (24 * 7);
		_y += 8;

		NaString strMapPath = PathManager::GetMapPath(m_pGame);
		NaStrArray arSubDir = NaPlatform::GetSubDirectoryList(strMapPath);

		// Root Path
		NaStrArray ar = NaPlatform::GetFileList(strMapPath);
		int nLen;

		nLen = arSubDir.GetCount();
		for (int i = 0; i<nLen; i++)
		{
			NaString strSubDir;
			strSubDir.Format(L"%ls", arSubDir[i].wstr());
			if (strSubDir.CompareNoCase(L"Download") == 0)
				continue;

			StagePack sp(m_pGame);
			sp.BuildPropertyMap();
			sp.Load(strSubDir);

			// Pass StagePack Directory
			if (sp.IsValid())
				continue;
			
			NaString strSubPath;
			strSubPath.Format(L"%ls\\%ls", strMapPath.wstr(), strSubDir.wstr());

			NaStrArray arSub = NaPlatform::GetFileList(strSubPath);
			for (int j=0; j<arSub.GetCount(); j++)
			{
				NaString strSubDirStage;
				strSubDirStage.Format(L"%ls\\%ls", strSubDir.wstr(), arSub[j].wstr());

				ar.Add(strSubDirStage);
			}
		}

		NaDebugOut(L"Load CustomStage List, %d Files Found.\n", ar.GetCount());
		nLen = ar.GetCount();
		for (int i = 0, j = 0; i < nLen; i++)
		{
			NaString strStage = ar[i];
			int nExtIdx = strStage.Find(L".map");
			if (nExtIdx > 0 && strStage[0] != L'_')
			{
				strStage.Format(L"%ls", ar[i].Left(nExtIdx).wstr());
				NaDebugOut(L"Stage #%03d: %ls\n", i, strStage.wstr());

				UIBase *pUI = UI_MANAGER->CreateSingleStageButton(
					nFirstStageIDUI + j,
					0,
					j * 24,
					strStage, CUR_PLAYER, pList);

				j++;
			}
		}

		if (pList->m_vecUI.size() > 0)
		{
			UI_MANAGER->m_pCurPanel->m_pFocused = pList;
			pList->m_pFocused = pList->m_vecUI[0];
			pList->EnsureVisible(pList->m_pFocused);
		}
	}

	UI_MANAGER->CreateButton(nBackIDUI, 0, _y, L"Back");
}

// Factory function
GameStateBase * GameStateBase::CreateInstance(Game * pGame, int nType)
{
	GameStateBase *pState = nullptr;
	switch (nType)
	{
	case GAMESTATE_TEST:			pState = new GameStateTest(pGame);				break;
#if !defined(NDEBUG)
	case GAMESTATE_PLAY:			pState = new GameStateTest(pGame);				break;
#else
	case GAMESTATE_PLAY:			pState = new GameStatePlay(pGame);				break;
#endif
	case GAMESTATE_DESIGNPLAY:		pState = new GameStateDesignPlay(pGame);		break;

	case GAMESTATE_CREDITS:			pState = new GameStateCredits(pGame);			break;
	case GAMESTATE_LOGO:			pState = new GameStateLogo(pGame);				break;
	case GAMESTATE_APPUPDATE:		pState = new GameStateAppUpdate(pGame);			break;
	case GAMESTATE_REGISTERUSER:	pState = new GameStateRegisterUser(pGame);		break;
	case GAMESTATE_TITLE:			pState = new GameStateTitle(pGame);				break;
	case GAMESTATE_STAGEINTRO:		pState = new GameStateStageIntro(pGame);		break;
	case GAMESTATE_SHAPECHANGE:		pState = new GameStateShapeChange(pGame);		break;
	case GAMESTATE_DIE:				pState = new GameStateDie(pGame);				break;
	case GAMESTATE_MESSAGEBLOCK:	pState = new GameStateMessageBlock(pGame);		break;
	case GAMESTATE_ENTERPIPE:		pState = new GameStateEnterPipe(pGame);			break;
	case GAMESTATE_EXITPIPE:		pState = new GameStateExitPipe(pGame);			break;
	case GAMESTATE_ENTERSKY:		pState = new GameStateEnterSky(pGame);			break;
	case GAMESTATE_SKYINTRO:		pState = new GameStateSkyIntro(pGame);			break;
	case GAMESTATE_ENTERDOOR:		pState = new GameStateEnterDoor(pGame);			break;
	case GAMESTATE_EXITDOOR:		pState = new GameStateExitDoor(pGame);			break;
	case GAMESTATE_FLAGDOWN:		pState = new GameStateFlagDown(pGame);			break;
	case GAMESTATE_HITGOALBOX:		pState = new GameStateHitGoalBox(pGame);		break;
	case GAMESTATE_HITGOALPOST:		pState = new GameStateHitGoalPost(pGame);		break;
	case GAMESTATE_GOAL:			pState = new GameStateGoal(pGame);				break;
	case GAMESTATE_ENTERSTAGE:		pState = new GameStateEnterStage(pGame);		break;
	case GAMESTATE_GAMEOVER:		pState = new GameStateGameOver(pGame);			break;
	case GAMESTATE_TIMEUP:			pState = new GameStateTimeUp(pGame);			break;
	case GAMESTATE_CUTBRIDGE:		pState = new GameStateCutBridge(pGame);			break;
	case GAMESTATE_RESCUEPRINCESS:	pState = new GameStateRescuePrincess(pGame);	break;

	case GAMESTATE_SINGLESTAGECLEAR:	pState = new GameStateSingleStageClear(pGame);		break;
	case GAMESTATE_SINGLESTAGEGAMEOVER:	pState = new GameStateSingleStageGameOver(pGame);	break;
	}
	return pState;
}
