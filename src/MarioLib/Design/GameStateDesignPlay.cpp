#include "GameStateDesignPlay.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "FModManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "TypeNames.h"

#include "EnemyObjectBase.h"
#include "DesignGame.h"
#include "DesignPlayGame.h"
#include "DesignStage.h"

#include "ControllableObject.h"
#include "UIBase.h"

GameStateDesignPlay::GameStateDesignPlay(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_DESIGNPLAY;
}

GameStateDesignPlay::~GameStateDesignPlay()
{
}

void GameStateDesignPlay::Init()
{
	CUR_PLAYER->UnlockInput();
}

void GameStateDesignPlay::Process()
{
#if !defined(NDEBUG)
	if (m_nStateFrame == 0)
	{
		m_pGame->m_pInputManager->ClearStatus();
	}

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

	if (KE(VK_ADD))
	{
		m_nObserveIdx++;
	}

	if (KE(VK_SUBTRACT))
	{
		m_nObserveIdx--;
	}


	if (KS(VK_P))
		m_bFrameCheck = false;
	if (KE(VK_F))
		m_bFrameCheck = true;
#endif

	if (!m_bFrameCheck || KE(VK_F))
	{
		CUR_STAGE->OnBeforeProcess();
		CUR_PLAYER->OnBeforeProcess();

		CUR_STAGE->Process();
		CUR_PLAYER->Process();
		CUR_PLAYER->ProcessPSwitchTime();

		CUR_STAGE->OnAfterProcess();
		CUR_PLAYER->OnAfterProcess();

		CUR_STAGE->ProcessEffects();

		CAMERA->Process();
		CAMERA->ProcessPlayerBound();
	}

	DesignPlayGame *pDesignPlayGame = ((DesignPlayGame*)m_pGame);
	DesignGame *pDesignGame = (DesignGame*)(pDesignPlayGame->m_pGame);
	
	if (m_pGame->m_nGameFrame % 3 == 0)
	{
		NaPointT<float> pt;
		pt.x = CUR_PLAYER_OBJ->m_fX;
		pt.y = CUR_PLAYER_OBJ->m_fY;

		std::vector<NaPointT<float>> &vec = ((DesignStage*)pDesignGame->m_pCurStage)->m_vecPlayedShadow;
		vec.push_back(pt);

		if (vec.size() > 100)
			vec.erase(vec.begin());
	}
}

void GameStateDesignPlay::Render()
{
	CUR_STAGE->RenderBackground();
	CUR_STAGE->Render();
	CUR_PLAYER->RenderControlObj();
	CUR_STAGE->RenderEffects();

	CUR_PLAYER->RenderStatus();

	NaString str = L"";
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
				strType.Format(L"Type:%ls (%d/%d)\n",
					g_szVehicleObjectTypes[pObj->m_nType - VEHICLE_BEGIN],
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
			1.0f, Z_TEXT, true);
	}
}