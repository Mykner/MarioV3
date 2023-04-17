#include "GameStateTest.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "TypeNames.h"
#include "NaPlatformAPI.h"
#include "UIManager.h"
#include "UIPanel.h"
#include "UIBase.h"
#include "EnemyObjectBase.h"
#include "ResourceHolder.h"
#include "ControllableObject.h"
#include "SoundManager.h"
#include "UIButton.h"
#include "UISingleStageButton.h"

#include "Mario.h"
#include "PathManager.h"
#include "JsonManager.h"
#include "UIEdit.h"
#include "ServerRequest.h"

GameStateTest::GameStateTest(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_TEST;

	m_bFrameCheck = false; 
	m_nStatusMode = 0;
	m_nObserveIdx = 0;

	m_Transaction.Init(pGame);
	m_pCurMetaInfo = nullptr;
}

GameStateTest::~GameStateTest()
{
}

void GameStateTest::Init()
{
	m_pGame->m_bModernStyle = true;

	if (m_pOldState == nullptr)
	{
		CUR_PLAYER->m_nTime = CUR_STAGE->m_nTime;
		CUR_STAGE->PlayBGM();
	}

	DeleteOldState();

	CUR_PLAYER->UnlockInput();
	CUR_PLAYER->UnholdInputAll();
}

void GameStateTest::Process()
{
	if (UI_MANAGER->m_pCurPanel->m_vecUI.size() == 0 &&
		m_pGame->m_pInputManager->m_bMouseClick)
	{
		NaPointT<float> ptCamera = CAMERA->GetPosition();
		//POINT ptCamera = CAMERA->GetPosition();
		POINT pt = m_pGame->m_pInputManager->m_ptLButtonDown;
		pt.x += ptCamera.x;
		pt.y += ptCamera.y;

		//m_pGame->m_pGameState->ChangeEnemyInViewportToCoin();
		
		//CUR_PLAYER_OBJ->SetPosition(pt.x, pt.y);

		/*
		int nIdx = CUR_STAGE->GetTileIndex(pt.x / TILE_XS, pt.y / TILE_YS);		
		CUR_STAGE->m_mapPressFrame.insert(
			std::pair<int, int>(nIdx, 15)
		);
		*/

		CUR_STAGE->CreateEffect(pt.x, pt.y, EFFECT_DEATHPOSITION);

		/*
		CreateParameter param;
		param.nDuration = 32;
		param.nStartDelay = 10;
		m_pGame->CreateSceneEffect(
			CUR_PLAYER_OBJ->m_fX - ptCamera.x,
			CUR_PLAYER_OBJ->m_fY - ptCamera.y - 16,
			EFFECT_SCENE_CIRCLEOUT, &param);
			*/
	}

	if (m_Transaction.IsJobFinished())
	{
		m_Transaction.DeleteThread();

		switch (m_Transaction.m_nJobType)
		{
		case JOB_RATE_END:
			UI_MANAGER->Clear();
			SOUND_MANAGER->PauseBGM(false);
			break;
		}

		m_Transaction.ChangeJob(JOB_NONE);
	}

	if (UI_MANAGER->m_pCurPanel->m_vecUI.size() == 0)
	{
		if (KE(VK_1))
		{
			CUR_STAGE->CreateItem(CUR_PLAYER_OBJ->x, CUR_PLAYER_OBJ->y, ITEM_MUSHROOM);
		}
		if (KE(VK_2))
		{
			CUR_STAGE->CreateItem(CUR_PLAYER_OBJ->x, CUR_PLAYER_OBJ->y, ITEM_FIREFLOWER);
		}
		if (KE(VK_3))
		{
			CUR_STAGE->CreateItem(CUR_PLAYER_OBJ->x, CUR_PLAYER_OBJ->y, ITEM_SUPERSTAR);
		}
		if (KE(VK_4))
		{
			CUR_STAGE->CreateItem(CUR_PLAYER_OBJ->x, CUR_PLAYER_OBJ->y, ITEM_GIANTMUSHROOM);
		}
		if (KE(VK_5))
		{
			CUR_PLAYER_OBJ->m_nInvinsibleTime = (((unsigned int)-1) / 2);
		}
		if (KE(VK_6))
		{
			CUR_STAGE->CreateVehicle(CUR_PLAYER_OBJ->x + 24, CUR_PLAYER_OBJ->y - 48, VEHICLE_CLOUD);
		}
		if (KE(VK_7))
		{
			CUR_STAGE->CreateVehicle(CUR_PLAYER_OBJ->x + 24, CUR_PLAYER_OBJ->y - 48, VEHICLE_YOSHI);
		}
		if (KE(VK_8))
		{
			CUR_STAGE->CreateItem(CUR_PLAYER_OBJ->x, CUR_PLAYER_OBJ->y, ITEM_TURTLESHELL);
		}

		if (KE(VK_ESCAPE))
		{
			m_nStatusMode = 0;
		}
		if (KE(VK_Q))
		{
			m_nStatusMode = 1;
		}
		if (KE(VK_W))
		{
			m_nStatusMode = 2;
		}
		if (KE(VK_E))
		{
			m_nStatusMode = 3;
		}
		if (KE(VK_R))
		{
			m_nStatusMode = 4;
		}
		if (KE(VK_T))
		{
			m_nStatusMode = 5;
		}
		if (KE(VK_Y))
		{
			m_nStatusMode = 6;
		}
		if (KE(VK_U))
		{
			m_nStatusMode = 7;
		}

		/*
		if (KE(VK_4))
		{
		CUR_STAGE->AddEnemy(194, 45, ENEMY_REDTURTLE);
		CUR_STAGE->m_vecEnemy[CUR_STAGE->m_vecEnemy.size() - 1]->m_fScaleX = 2.0f;
		CUR_STAGE->m_vecEnemy[CUR_STAGE->m_vecEnemy.size() - 1]->m_fScaleY = 2.0f;
		}
		if (KE(VK_5))
		{
		//GameObjectBase *pObj = new Mario(m_pGame, m_pGame->m_pCurStage);
		//pObj->x = 100;
		//pObj->y = 100;
		//m_pGame->m_pCurStage->m_vecEnemy.push_back(pObj);

		//CUR_PLAYER_OBJ->m_fScaleX = CUR_PLAYER_OBJ->m_fScaleY = 0.5f;
		}
		*/

		if (KE(VK_F1))
		{
			ChangeZoomFactor(1);
		}
		if (KE(VK_F2))
		{
			ChangeZoomFactor(2);
		}
		if (KE(VK_F3))
		{
			ChangeZoomFactor(3);
		}

		if (KE(VK_F5))
		{
			CUR_STAGE->m_strGameTheme = L"";
			CUR_STAGE->InitTheme();
		}

		if (KE(VK_F6))
		{
			CUR_STAGE->m_strGameTheme = L"smb3";
			CUR_STAGE->InitTheme();
		}

		if (KE(VK_F7))
		{
			CUR_STAGE->m_strGameTheme = L"smw";
			CUR_STAGE->InitTheme();
		}

		if (KE(VK_F8))
		{
			CUR_STAGE->m_strGameTheme = L"smb2";
			CUR_STAGE->InitTheme();
		}

		if (KE(VK_F9))
		{
			CUR_STAGE->m_strGameTheme = L"smas";
			CUR_STAGE->InitTheme();
		}

		if (KE(VK_F10))
		{
			CUR_STAGE->m_strGameTheme = L"sml2";
			CUR_STAGE->InitTheme();
		}

		/*
		if (KE(VK_F7))
		{
		((ControllableObject*)CUR_PLAYER_OBJ)->m_nShape = SHAPE_COSTUME;
		((ControllableObject*)CUR_PLAYER_OBJ)->m_nCostume--;
		if (((ControllableObject*)CUR_PLAYER_OBJ)->m_nCostume < 0)
		((ControllableObject*)CUR_PLAYER_OBJ)->m_nCostume = SPRIDX_COSTUME_LAST;
		}

		if (KE(VK_F8))
		{
		((ControllableObject*)CUR_PLAYER_OBJ)->m_nShape = SHAPE_COSTUME;
		((ControllableObject*)CUR_PLAYER_OBJ)->m_nCostume++;
		if (((ControllableObject*)CUR_PLAYER_OBJ)->m_nCostume > SPRIDX_COSTUME_LAST)
		((ControllableObject*)CUR_PLAYER_OBJ)->m_nCostume = 0;
		}
		*/

		if (KS(VK_SHIFT) && KE(VK_F12))
		{
			m_pGame->m_nWidth = m_pGame->m_nHeight / 9 * 16;

			int nZoomFactor = GameWindowDefaults::nZoomFactor;
			NaRect rcWindow;
			rcWindow.left = 0;
			rcWindow.top = 0;
			rcWindow.right = m_pGame->Width()*nZoomFactor;
			rcWindow.bottom = m_pGame->Height()*nZoomFactor;
			NaPlatform::AdjustWindowRect(rcWindow);

			SIZE s = NaPlatform::GetScreenSize();
			int nWidth = rcWindow.right - rcWindow.left;
			int nHeight = rcWindow.bottom - rcWindow.top;
			int nX = s.cx / 2 - nWidth / 2;
			int nY = s.cy / 2 - nHeight / 2;

			NaPlatform::MoveWindow(m_pGame->m_hWindow, nX, nY, nWidth, nHeight);

			m_pGame->m_pRenderManager;
		}

		if (KE(VK_ADD))
		{
			//CUR_PLAYER_OBJ->m_fScaleX += 0.1f;
			//CUR_PLAYER_OBJ->m_fScaleY += 0.1f; 
			m_nObserveIdx++;
		}

		if (KE(VK_SUBTRACT))
		{
			//CUR_PLAYER_OBJ->m_fScaleX -= 0.1f;
			//CUR_PLAYER_OBJ->m_fScaleY -= 0.1f;
			m_nObserveIdx--;
		}

		if (KS(VK_P))
			m_bFrameCheck = false;
		if (KE(VK_F))
			m_bFrameCheck = true;

		if (!m_bFrameCheck || KE(VK_F))
		{
			CUR_PLAYER->ProcessTime();

			CUR_STAGE->OnBeforeProcess();
			CUR_PLAYER->OnBeforeProcess();

			CUR_STAGE->Process();
			CUR_PLAYER->Process();

			CUR_STAGE->OnAfterProcess();
			CUR_PLAYER->OnAfterProcess();

			CUR_STAGE->ProcessEffects();

			CAMERA->Process();
			CAMERA->ProcessPlayerBound();
		}
		else
		{
			//CUR_STAGE->OnBeforeProcess();
			//CUR_PLAYER_OBJ->OnBeforeProcess();
		}

		if (KE(VK_ESCAPE))
		{
			InitTestMenuUI();

			PLAYSOUND(L"Pause");
			SOUND_MANAGER->PauseBGM(true);
		}
	}
	else
	{
		UI_MANAGER->Process();

		if (KE(VK_ESCAPE))
		{
			UI_MANAGER->Clear();
			SOUND_MANAGER->PauseBGM(false);
		}

		if (!m_Transaction.IsWorking() && UI_MANAGER->m_pClicked)
		{
			int nIDUI = UI_MANAGER->m_pClicked->m_nID;
			switch (nIDUI)
			{
			case UI_GOTO_TITLE:
				UI_MANAGER->Clear();
				m_pGame->ChangeState(GAMESTATE_TITLE);
				break;
			case UI_RESET_CURRENT_STAGE:
				{
					if (CUR_STAGE->m_bSwapCoinBlock)
						CUR_STAGE->SwapCoinAndBrick();

					UI_MANAGER->Clear();
					SOUND_MANAGER->PauseBGM(false);
					CUR_STAGE->Clear();

					Stage *pTopStage = m_pGame->m_pCurStage;
					while (true)
					{
						if (pTopStage->m_pParentStage)
							pTopStage = pTopStage->m_pParentStage;
						else
							break;
					}

					m_pGame->PopAllStage();
					CUR_STAGE->Load(pTopStage->m_strName.wstr());

					GameObjectBase *pObj = CUR_PLAYER->GetLastControlObject();
					
					pObj->ChangeState(STATE_STAND);
					pObj->m_fX = CUR_STAGE->m_ptStartPoint.x;
					pObj->m_fY = CUR_STAGE->m_ptStartPoint.y;

					CUR_PLAYER->m_nLife++;
					CUR_PLAYER->OnLostLife();

					m_pGame->InitPlayerObject();
					m_pGame->InitCamera();
				}
				break;
			case UI_RESET_POWERUPS:
				{
					if (CUR_PLAYER->m_pControlObj->m_nGiantTime > 0)
						CUR_PLAYER->m_pControlObj->m_nGiantTime = 1;
					CUR_PLAYER->m_pControlObj->m_nInvinsibleTime = 0;

					CUR_PLAYER->m_pControlObj->m_nShape = SHAPE_MINI;
					CUR_PLAYER->m_pControlObj->OnChangeShape(SHAPE_MINI);
				}
				break;
			case UI_RESET_TIME:
				{
					CUR_PLAYER->m_nTime = CUR_STAGE->m_nTime;
					SOUND_MANAGER->PlayStageBGM();
				}
				break;
			case UI_SELECT_STAGE:
				InitCustomStageSelectUI(UI_CUSTOMSTAGE_000, UI_BACK);
				break;
			case UI_BACK:
				UI_MANAGER->PopPanel();
				break;
			case UI_RELOAD_TEXTURES:
				{
					BEGIN_VECTOR_LOOP(Sprite*, m_pGame->m_pSpriteManager->m_vecSprites) {
						pObj->ReloadTexture();
						if (pObj->m_pSpritePiece)
							pObj->m_pSpritePiece->ReloadTexture();
					} END_VECTOR_LOOP();

					UI_MANAGER->Clear();
				}
				break;
			case UI_LOAD_INITIAL_STAGE:
				{
					UI_MANAGER->Clear();

					CUR_STAGE->Clear();
					CUR_STAGE->Load(L"1-1");

					GameObjectBase *pObj = CUR_PLAYER->GetLastControlObject();

					pObj->ChangeState(STATE_STAND);
					pObj->x = CUR_STAGE->m_ptStartPoint.x;
					pObj->y = CUR_STAGE->m_ptStartPoint.y;

					CAMERA->m_ptPos.x = 0;
				}
				break;
			case UI_TEST_STATE:
				m_pGame->ChangeState(GAMESTATE_CREDITS);
				break;
			case UI_TEST_FUNC:
				{
					Mario *m = ((Mario*)CUR_PLAYER_OBJ);

					NaString strName;
					strName.Format(L"%ls\\MarioPhysics.json", PathManager::GetGameRootPath(m_pGame));
					Json::Value *j = JsonManager::LoadFile(strName.cstr());
					if (j)
					{
						JsonManager::LoadJsonObjectProperty(&m->m_PhysicalProps, *j);
						delete j;
						j = nullptr;
					}
					else
					{
						NaDebugOut(L"MarioPhysics.json not found.\n");

						Json::Value j2;
						if (JsonManager::SaveJsonObjectProperty(&m->m_PhysicalProps, j2))
							JsonManager::SaveFile(strName.cstr(), &m->m_PhysicalProps, j2);
					}
				}
				break;
			case UI_TOGGLE_MODERNSTYLE:
				m_pGame->m_bModernStyle = !m_pGame->m_bModernStyle;
				break;
			case UI_GIVEUP:
				UI_MANAGER->Clear();
				SOUND_MANAGER->PauseBGM(false);

				CUR_PLAYER_OBJ->ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
				break;
			case UI_GIVEUP_ALL:
				UI_MANAGER->Clear();
				SOUND_MANAGER->PauseBGM(false);

				CUR_PLAYER->m_nLife = 1;
				CUR_PLAYER_OBJ->ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
				break;
			case UI_RATE_STAGE:
				InitRateMenuUI();
				break;
			case UI_RATE_INPUT:
				if (UI_MANAGER->m_pClicked->m_bEnable)
				{
					UIEdit *pUI = (UIEdit*)UI_MANAGER->GetFocus();
					if (pUI->m_strText.GetLength() > 0)
					{
						m_Transaction.m_pJobThread = new std::thread(RateStage, this);
					}
				}
				break;
			case UI_CHANGE_GAMETHEME:
				{
					CUR_STAGE->m_strGameTheme = ((UIEdit*)UI_MANAGER->m_pClicked)->m_strText;
					CUR_STAGE->InitTheme();
					CUR_STAGE->m_nBackgroundTheme = -1;
					CUR_STAGE->BuildBackground();
					
					SOUND_MANAGER->StopBGM();
					SOUND_MANAGER->PlayStageBGM();
				}
				break;
			case UI_CHANGE_THEMESET:
				{
					CUR_STAGE->m_nThemeSet = ((UIEdit*)UI_MANAGER->m_pClicked)->m_strText.ToInt();
					CUR_STAGE->InitTheme();
					CUR_STAGE->m_nBackgroundTheme = -1;
					CUR_STAGE->BuildBackground();

					SOUND_MANAGER->StopBGM();
					SOUND_MANAGER->PlayStageBGM();
				}
				break;
			case UI_CLOSE_MENU:
				UI_MANAGER->Clear();
				SOUND_MANAGER->PauseBGM(false);
				break;			
			}

			if (nIDUI >= UI_CUSTOMSTAGE_000 && nIDUI <= UI_CUSTOMSTAGE_999)
			{
				m_pGame->m_strStagePack = L".";
				m_pGame->m_nStagePackType = STAGEPACK_SINGLE;

				UISingleStageButton *pButton = (UISingleStageButton*)UI_MANAGER->m_pClicked;
				m_pGame->m_strInitialStage = pButton->m_strPathName.wstr();

				m_pGame->PopAllStage();
				CUR_STAGE->Load(m_pGame->m_strInitialStage.wstr());

				UI_MANAGER->Clear();
				m_pGame->ChangeState(GAMESTATE_STAGEINTRO);
				return;
			}
		}
	}
}

void GameStateTest::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();

	UI_MANAGER->Render();

	UIBase* pFocusedUI = UI_MANAGER->GetFocus();
	if (pFocusedUI)
	{
		NaRect rc = pFocusedUI->GetScreenRect();
		DRAWTEXT(
			rc.left - 12,
			rc.top,
			L"\x08");
	}

	if (m_Transaction.IsWorking())
	{
		m_Transaction.Render(m_pGame->m_nWidth - 25, m_pGame->m_nHeight - 25, 1.5f);
	}

	// For test
	NaString str;
	/*
	str.Format(L"TEST MODE: %ls", CUR_STAGE->m_strName.wstr());
	DRAWTEXT(22, 14, str);

	str.Format(L"$*%02d", CUR_PLAYER->m_nCoin);
	DRAWTEXT(22, 24, str);

	str.Format(L"%06d", CUR_PLAYER->m_nScore);
	DRAWTEXT(256 - 22 - 6 * 8, 24, str);
	*/
	CUR_PLAYER->RenderStatus();

	str = L"";
	GameObjectBase *pObj = nullptr;
	
	switch (m_nStatusMode)
	{
	case 1:
		{
			Stage *pTopStage = m_pGame->m_pCurStage;
			while (true)
			{
				if (pTopStage->m_pParentStage)
					pTopStage = pTopStage->m_pParentStage;
				else
					break;
			}

			NaString strNext = pTopStage->m_strNextStageName;
			if (strNext.GetLength() == 0)
				strNext = L"Undefined";

			NaString str2;
			str2.Format(L"--STAGE--\n%ls to %ls\n",
				pTopStage->m_strName.wstr(),
				strNext.wstr()
			);
			str += str2;

			str2.Format(L"Camera X,Y:%.2f,%.2f Speed:%.4f,%.4f\n",
				CAMERA->m_ptPos.x,
				CAMERA->m_ptPos.y,
				CAMERA->m_fXS,
				CAMERA->m_fYS);
			str += str2;

			str2.Format(L"Item:%d\nEnemy:%d\nEffect:%d\nBullet:%d",
				CUR_STAGE->m_vecItem.size(),
				CUR_STAGE->m_vecEnemy.size(),
				CUR_STAGE->m_vecEffect.size(),
				CUR_STAGE->m_vecBullet.size());
			str += str2;
		}
		break;
	case 2:
		pObj = CUR_PLAYER_OBJ;
		str += L"--PLAYER--\n";
		{
			NaString strType;
			strType.Format(L"Shape:%d (Next:%d)\n",
				pObj->m_nShape, pObj->m_nNextShape);
			str += strType;
			
			if (pObj->m_nShape == SHAPE_COSTUME)
			{
				strType.Format(L"Costume:%d\n", ((ControllableObject*)pObj)->m_nCostume);
				str += strType;
			}

			strType.Format(L"Key:%d\n", CUR_PLAYER->m_vecDoorKey.size());
			str += strType;
		}
		break;
	case 3:
		{
			str += L"--ENEMY--\n";
			int nSize = CUR_STAGE->m_vecEnemy.size();
			if (m_nObserveIdx < 0)
				m_nObserveIdx = nSize - 1;
			else if (m_nObserveIdx >= nSize && nSize > 0)
				m_nObserveIdx = m_nObserveIdx % nSize;
			if (nSize > 0)
				pObj = CUR_STAGE->m_vecEnemy[m_nObserveIdx % nSize];
			if (pObj)
			{
				NaString strType;
				strType.Format(L"Type:%ls (%d/%d) 0x%08x\n",
					g_szEnemyTypes[pObj->m_nType - ENEMY_BEGIN],
					m_nObserveIdx + 1,
					nSize,
					pObj);
				str += strType;

				if (((EnemyObjectBase*)pObj)->m_pOnMyHead)
				{
					strType.Format(L"Stack:0x%08x\n",
						((EnemyObjectBase*)pObj)->m_pOnMyHead);
					str += strType;
				}

				if (((EnemyObjectBase*)pObj)->m_pUnderMyFoot)
				{
					strType.Format(L"Stacked:0x%08x\n",
						((EnemyObjectBase*)pObj)->m_pUnderMyFoot);
					str += strType;
				}

				strType.Format(L"CanPressed: %d\n", pObj->CanPressed()); str += strType;
				strType.Format(L"CanDamageEnemy: %d\n", pObj->CanDamageEnemy()); str += strType;
			}
		}
		break;
	case 4:
		{
			str += L"--ITEM--\n";
			int nSize = CUR_STAGE->m_vecItem.size();
			if (m_nObserveIdx < 0)
				m_nObserveIdx = nSize - 1;
			else if (m_nObserveIdx >= nSize && nSize > 0)
				m_nObserveIdx = m_nObserveIdx % nSize;
			if (nSize > 0)
				pObj = CUR_STAGE->m_vecItem[m_nObserveIdx % nSize];
		}
		break;
	case 5:
		{
			str += L"--VEHICLE--\n";
			int nSize = CUR_STAGE->m_vecVehicle.size();
			if (m_nObserveIdx < 0)
				m_nObserveIdx = nSize - 1;
			else if (m_nObserveIdx >= nSize && nSize > 0)
				m_nObserveIdx = m_nObserveIdx % nSize;
			if (nSize > 0)
				pObj = CUR_STAGE->m_vecVehicle[m_nObserveIdx % nSize];
			if (pObj)
			{
				NaString strType;
				strType.Format(L"Type: ?? (%d/%d)\n",
					m_nObserveIdx + 1,
					CUR_STAGE->m_vecEnemy.size());
				str += strType;
			}
		}
		break;
	case 6:
		str += L"--PLAYER VEHICLE--\n";
		pObj = (GameObjectBase*)CUR_PLAYER_OBJ->m_pVehicle;
		break;
	case 7:
		str += L"--EVENT OBJECT--\n";
		int nSize = CUR_STAGE->m_vecEventObject.size();
		if (m_nObserveIdx < 0)
			m_nObserveIdx = nSize - 1;
		else if (m_nObserveIdx >= nSize && nSize > 0)
			m_nObserveIdx = m_nObserveIdx % nSize;
		if (nSize > 0)
			pObj = CUR_STAGE->m_vecEventObject[m_nObserveIdx % nSize];
		if (pObj)
		{
			NaString strType;
			strType.Format(L"Type:%ls (%d/%d) 0x%08x\n",
				g_szEventObjectTypes[pObj->m_nType - EVENTOBJECT_BEGIN],
				m_nObserveIdx + 1,
				nSize,
				pObj);
			str += strType;
		}
		break;
	}

	if (pObj)
	{
		NaString str2;
		float fScaleX, fScaleY;
		pObj->GetScale(fScaleX, fScaleY);

		str2.Format(L"Position:%.2f,%.2f\nSpeed:%.2f,%.2f Scale:%.1f,%.1f\n",
			pObj->m_fX, pObj->m_fY,
			pObj->m_fXS, pObj->m_fYS,
			fScaleX, fScaleY);
		str += str2;

		str2.Format(L"State:%ls(%d)(f:%d)\nCollision:\n__%ls\n%ls_*_%ls\n__%ls\n",
			pObj->GetStateName().wstr(),
			pObj->m_nState,
			pObj->m_nStateFrame,
			pObj->m_bCollision[COLLISION_TOP] ? L"O" : L"X",
			pObj->m_bCollision[COLLISION_LEFT] ? L"O" : L"X",
			pObj->m_bCollision[COLLISION_RIGHT] ? L"O" : L"X",
			pObj->m_bCollision[COLLISION_BOTTOM] ? L"O" : L"X");
		str += str2;

		str2.Format(L"Combo:%d", pObj->m_nCombo);
		str += str2;

		{
			UIBase u(m_pGame);
			NaRect rc = pObj->GetRect();
			POINT ptCamera = CAMERA->GetPosition();
			rc.Offset(-ptCamera.x, -ptCamera.y);

			u.RenderBox(rc, 0xb0ff0000);
		}
	}

	if (str.GetLength() > 0)
	{		
		RENDER_MANAGER->Text(22, 34, str, 0xffffffff, 
			0.5f, Z_TEXT, true);
	}
}

void GameStateTest::ChangeZoomFactor(int nZoomFactor)
{
	NaRect rcWindow;
	rcWindow.left = 0;
	rcWindow.top = 0;
	rcWindow.right = m_pGame->Width()*nZoomFactor;
	rcWindow.bottom = m_pGame->Height()*nZoomFactor;
	NaPlatform::AdjustWindowRect(rcWindow);

	RECT rcWindow2;
	NaPlatform::GetWindowRect(m_pGame->m_hWindow, &rcWindow2);

	NaPlatform::MoveWindow(m_pGame->m_hWindow, rcWindow2.left, rcWindow2.top, rcWindow.Width(), rcWindow.Height());
}

void GameStateTest::InitTestMenuUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int i = 0, y = 0;
	NaString str;
	UIBase *pUI;
	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"--- TEST MENU ---");

	pUI = UI_MANAGER->CreateButton(
		UI_GOTO_TITLE, 0, y + ((i++) * 16), L"TITLE MENU");

	pUI = UI_MANAGER->CreateButton(
		UI_RESET_CURRENT_STAGE, 0, y + ((i++) * 16), L"RESET STAGE");

	pUI = UI_MANAGER->CreateButton(
		UI_RESET_POWERUPS, 0, y + ((i++) * 16), L"RESET POWER UPS");
	
	pUI = UI_MANAGER->CreateButton(
		UI_RESET_TIME, 0, y + ((i++) * 16), L"RESET TIME");
	
	pUI = UI_MANAGER->CreateButton(
		UI_SELECT_STAGE, 0, y + ((i++) * 16), L"SELECT STAGE");

	/*
	pUI = UI_MANAGER->CreateButton(
		UI_RELOAD_TEXTURES, 0, y + ((i++) * 16), L"RELOAD TEXTURES");

	pUI = UI_MANAGER->CreateButton(
		UI_LOAD_INITIAL_STAGE, 0, y + ((i++) * 16), L"LOAD INITIAL STAGE");

	pUI = UI_MANAGER->CreateButton(
		UI_TEST_STATE, 0, y + ((i++) * 16), L"CHANGE TO TEST STATE");

	pUI = UI_MANAGER->CreateButton(
		UI_TEST_FUNC, 0, y + ((i++) * 16), L"TEST FUNCTION");

	pUI = UI_MANAGER->CreateButton(
		UI_TOGGLE_MODERNSTYLE, 0, y + ((i++) * 16), L"TOGGLE MODERN STYLE");
	*/

	pUI = UI_MANAGER->CreateButton(
		UI_GIVEUP, 0, y + ((i++) * 16), L"GIVE UP 1 LIFE");

	pUI = UI_MANAGER->CreateButton(
		UI_GIVEUP_ALL, 0, y + ((i++) * 16), L"GIVE UP ALL LIVES");

	pUI = UI_MANAGER->CreateButton(
		UI_RATE_STAGE, 0, y + ((i++) * 16), L"RATE THIS STAGE");

	pUI = UI_MANAGER->CreateEdit(
		UI_CHANGE_GAMETHEME, 0, y + ((i++) * 16), CUR_STAGE->m_strGameTheme, L"GAME THEME: ");

	str.Format(L"%d", CUR_STAGE->m_nThemeSet);
	pUI = UI_MANAGER->CreateEdit(
		UI_CHANGE_THEMESET, 0, y + ((i++) * 16), str, L"THEME SET: ");

	pUI = UI_MANAGER->CreateButton(
		UI_CLOSE_MENU, 0, y + ((i++) * 16), L"CLOSE MENU");
	
	pPanel->m_pFocused = pUI;
}

void GameStateTest::InitRateMenuUI()
{
	UIPanel *pPanel = UI_MANAGER->AddPanel(0, 0);
	pPanel->m_bAutoAlignX = true;
	pPanel->m_bAutoAlignY = true;
	pPanel->m_nPanelType = UIPanel::PANEL_ROUNDSHADOWBOX;

	int i = 0, y = 0;
	NaString str;
	UIBase *pUI;
	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"--- DEVELOPER'S RATING ---");

	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L" 0: Unbeatable");

	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L" 0: Spamming enemies or items");

	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L" 0: Test stage");

	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"-1: Using bug");

	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"-1: Too hard");

	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"-1: Bad title or comment");
	
	pUI = UI_MANAGER->CreateText(
		UI_UNDEFINED, 0, y + ((i++) * 16), L"-1: Infinite lives");

	pUI = UI_MANAGER->CreateEdit(
		UI_RATE_INPUT, 0, y + ((i++) * 16), L"", L"Rating: ");

	pUI = UI_MANAGER->CreateButton(
		UI_BACK, 0, y + ((i++) * 16), L"BACK");

	pPanel->m_pFocused = pUI;
}

void GameStateTest::RateStage(void * pParam)
{
	GameStateTest *pThis = (GameStateTest*)pParam;

	int nScore = -1;
	UIEdit *pUI = (UIEdit*)pThis->m_pGame->m_pUIManager->GetFocus();
	if (pUI->m_strText.GetLength() > 0)
	{
		nScore = pUI->m_strText.ToInt();
	}
	else
	{
		pThis->m_Transaction.ChangeJob(JOB_RATE_END);
		pThis->m_Transaction.EndJob();
		return;
	}

	pThis->m_Transaction.BeginJob(JOB_RATE);

	ServerRequest req;
	try
	{
		if (pThis->m_pCurMetaInfo == nullptr)
		{
			auto pTopStage = pThis->m_pGame->m_pCurStage->GetTopStage();
			auto pMetaInfo = req.GetMetaInfo(pTopStage->m_strUniqueId);
			pThis->m_pCurMetaInfo = pMetaInfo;
		}

		pThis->m_pCurMetaInfo->m_nDeveloperRating = nScore;

		req.UpdateMetaInfo(pThis->m_pCurMetaInfo, UPDATE_METAINFO_DEVELOPERRATING);

	}
	catch (NaException &e)
	{
		NaDebugOut(L"Error: %ls\n", NaString(e.m_szWhat).wstr());
		MessageBox(nullptr, NaString(e.m_szWhat).wstr(), L"Error", MB_OK | MB_ICONERROR);
	}

	pThis->m_Transaction.ChangeJob(JOB_RATE_END);
	pThis->m_Transaction.EndJob();
}
