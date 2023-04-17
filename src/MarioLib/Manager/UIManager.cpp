#include "UIManager.h"

#include "Game.h"
#include "UIBase.h"
#include "UIButton.h"
#include "UIPanel.h"
#include "UIText.h"
#include "UIEdit.h"
#include "InputManager.h"
#include "Player.h"
#include "UISingleStageButton.h"
#include "UserProfile.h"
#include "UIList.h"
#include "UIContainer.h"
#include "UICustomGameButton.h"
#include "UIImageButton.h"
#include "Sprite.h"
#include "StagePack.h"
#include "UIWorldStageButton.h"
#include "StageMetaInfo.h"
#include "UIRectangle.h"

#include "EffectSceneCircle.h"
#include "EffectSceneFade.h"

UIManager::UIManager(Game *pGame)
{
	m_pGame = pGame;

	m_pClicked = nullptr;
	m_pCurPanel = new UIPanel(pGame);
	m_bTransition = false;
}

UIManager::~UIManager()
{
	Clear();

	if (m_pCurPanel)
	{
		delete m_pCurPanel;
		m_pCurPanel = nullptr;
	}
}

void UIManager::Process()
{
	m_pClicked = nullptr;

	if (m_pCurPanel->m_vecUI.size() == 0)
		return;

	if (m_bTransition && m_TransitionCallback)
	{
		bool bFinished = false;
		if (m_pGame->m_vecEffect.size() == 0)
		{
			bFinished = true;
		}
		else
		{
			bFinished = true;
			BEGIN_VECTOR_LOOP(GameObjectBase*, m_pGame->m_vecEffect) {
				switch(pObj->m_nType)
				{
				case EFFECT_SCENE_CIRCLE:
				case EFFECT_SCENE_CIRCLEIN:
				case EFFECT_SCENE_CIRCLEOUT:
					if (((EffectSceneCircle*)pObj)->m_bFinished == false)
						bFinished = false;
					break;
				case EFFECT_SCENE_FADE:
				case EFFECT_SCENE_FADEIN:
				case EFFECT_SCENE_FADEOUT:
					if (((EffectSceneFade*)pObj)->m_bFinished == false)
						bFinished = false;
					break;
				}
			} END_VECTOR_LOOP();
		}

		if (bFinished)
		{
			m_TransitionCallback();
			m_bTransition = false;
		}
	}
	else
	{
		m_pCurPanel->Process();
	}

	InputManager *pInputManager = m_pGame->m_pInputManager;
	m_ptLastMouse = pInputManager->m_ptMouse;
}

void UIManager::Render()
{
	m_pCurPanel->Render();	
}

void UIManager::Clear()
{
	while (m_pCurPanel && m_pCurPanel->m_pParent)
	{
		m_pCurPanel->Clear();
		UIPanel *pParent = (UIPanel*)m_pCurPanel->m_pParent;

		delete m_pCurPanel;
		m_pCurPanel = pParent;
	}

	if (m_pCurPanel)
		m_pCurPanel->Clear();

	m_pClicked = nullptr;
}

void UIManager::PopPanel()
{
	if (m_pCurPanel->m_pParent)
	{
		m_pCurPanel->Clear();
		UIPanel *pParent = (UIPanel*)m_pCurPanel->m_pParent;

		delete m_pCurPanel;
		m_pCurPanel = pParent;
	}
}

UIBase * UIManager::CreateButton(int id, int x, int y, NaString str, UIContainer *pParent)
{
	UIButton *pUI = new UIButton(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_strText = str;

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateImageButton(int id, int x, int y, int width, int height, NaString str, Sprite * pSprite, int nSpriteIndex, UIContainer * pParent)
{
	UIImageButton *pUI = new UIImageButton(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_nWidth = width;
	pUI->m_nHeight = height;
	pUI->m_strText = str;
	pUI->m_pSprite = pSprite;
	pUI->m_nSpriteIndex = nSpriteIndex;

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateText(int id, int x, int y, NaString str, UIContainer *pParent)
{
	UIText *pUI = new UIText(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_strText = str;

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateEdit(int id, int x, int y, NaString str, NaString strTitle, UIContainer *pParent)
{
	UIEdit *pUI = new UIEdit(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_strText = str;
	pUI->m_strTitleText = strTitle;

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateList(int id, int x, int y, int width, int height, UIContainer *pParent)
{
	UIList *pUI = new UIList(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_nWidth = width;
	pUI->m_nHeight = height;
	
	pUI->m_rc.left = x;
	pUI->m_rc.top = y;
	pUI->m_rc.right = x + width;
	pUI->m_rc.bottom = y + height;

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateRectangle(int id, int x, int y, int width, int height, NaString str, DWORD dwFillColor, Sprite * pSprite, int nSpriteIndex, UIContainer * pParent)
{
	UIRectangle *pUI = new UIRectangle(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_nWidth = width;
	pUI->m_nHeight = height;
	pUI->m_strText = str;
	pUI->m_pSprite = pSprite;
	pUI->m_nSpriteIndex = nSpriteIndex;
	pUI->m_dwFillColor = dwFillColor;

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateSingleStageButton(
	int id, 
	int x, 
	int y, 
	NaString str,
	Player *pPlayer,
	UIContainer *pParent)
{
	UISingleStageButton *pUI = new UISingleStageButton(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_strPathName = str;

	if (str.Find(L"\\") > 0)
	{
		int nIdx = str.Find(L"\\");
		pUI->m_strFolder = str.Left(nIdx);
		pUI->m_strFile = str.Mid(nIdx + 1);
		pUI->m_strText = pUI->m_strFile;
	}
	else
	{
		pUI->m_strText = str;
	}

	if (pPlayer && pPlayer->m_pProfile)
	{
		int nBestTime = pPlayer->m_pProfile->GetSingleStageBestTime(str);
		if (nBestTime > 0)
		{
			pUI->m_strBestTime.Format(L"%02d:%02d:%02d.%02d",
				(nBestTime / 1000 / 60 / 60) % 24, // hour
				(nBestTime / 1000 / 60) % 60, // min
				(nBestTime / 1000) % 60, // sec
				(nBestTime / 10) % 10 // milli sec
			);
		}
		int nBestScore = pPlayer->m_pProfile->GetSingleStageBestScore(str);
		if (nBestScore > 0)
		{
			pUI->m_strBestScore.Format(L"%8d", nBestScore);
		}
	}

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateCustomGameButton(
	int id, 
	int x, 
	int y, 
	StagePack *pSp, 
	Player *pPlayer, 
	UIContainer * pParent)
{
	UICustomGameButton *pUI = new UICustomGameButton(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_strText = pSp->m_strName;
	pUI->m_strCreator = pSp->m_strCreator;

	if (pPlayer && pPlayer->m_pProfile)
	{
		// #TODO Additional Info
		StagePackRecord *pRecord = pPlayer->m_pProfile->GetStagePackRecord(pSp->m_strFileName);
		if (pRecord)
		{
			pUI->m_strLastStage = pRecord->m_strLastStage;
			pUI->m_nCoin = pRecord->m_nCoin;
			pUI->m_nLife = pRecord->m_nLife;
			pUI->m_nShape = pRecord->m_nShape;
		}
	}

	AddUI(pUI, pParent);
	return pUI;
}

UIBase * UIManager::CreateWorldStageButton(int id, int x, int y, StageMetaInfo * pMeta, Player * pPlayer, UIContainer * pParent)
{
	UIWorldStageButton *pUI = new UIWorldStageButton(m_pGame);
	pUI->m_nID = id;
	pUI->m_nX = x;
	pUI->m_nY = y;
	pUI->m_pMetaInfo = pMeta;
	pUI->m_strText = pMeta->m_strTitle;

	if (pPlayer && pPlayer->m_pProfile)
	{
		int nBestTime = pPlayer->m_pProfile->GetSingleStageBestTime(pMeta->m_strUniqueId);
		if (nBestTime > 0)
		{
			pUI->m_strBestTime.Format(L"%02d:%02d:%02d.%02d",
				(nBestTime / 1000 / 60 / 60) % 24, // hour
				(nBestTime / 1000 / 60) % 60, // min
				(nBestTime / 1000) % 60, // sec
				(nBestTime / 10) % 10 // milli sec
			);
			pUI->m_bPlayed = true;
		}
		int nBestScore = pPlayer->m_pProfile->GetSingleStageBestScore(pMeta->m_strUniqueId);
		if (nBestScore > 0)
		{
			pUI->m_strBestScore.Format(L"%8d", nBestScore);
			pUI->m_bPlayed = true;
		}
	}

	AddUI(pUI, pParent);
	return pUI;
}

UIPanel* UIManager::AddPanel(int x, int y)
{
	UIPanel *pPanel = new UIPanel(m_pGame);
	pPanel->m_nX = x;
	pPanel->m_nY = y;
	pPanel->m_pParent = m_pCurPanel;
	m_pCurPanel = pPanel;

	return pPanel;
}

void UIManager::AddUI(UIBase * pUI, UIContainer *pParent)
{
	if (pParent == nullptr)
		m_pCurPanel->AddUI(pUI);
	else
		pParent->AddUI(pUI);
}

UIBase * UIManager::GetFocus()
{
	UIBase *pFocus = m_pCurPanel->m_pFocused;
	while (pFocus)
	{
		if (pFocus->m_nType == UI_LIST)
			pFocus = ((UIList*)pFocus)->m_pFocused;
		else
			break;
	}

	return pFocus;
}

UIBase * UIManager::GetControl(int id, UIContainer *pParent)
{
	if (pParent == nullptr)
		pParent = GetFocus()->m_pParent;
	
	if (pParent == nullptr)
		return nullptr;

	pParent->m_vecUI;
	BEGIN_VECTOR_LOOP(UIBase*, pParent->m_vecUI) {
		if (pObj->m_nID == id)
			return pObj;
	} END_VECTOR_LOOP();

	return nullptr;
}

void UIManager::Transition(std::function<void()> fnCallback)
{
	m_bTransition = true;
	m_TransitionCallback = fnCallback;
}
