#pragma once

#include "NaString.h"
#include "NaPoint.h"
#include <vector>
#include <functional>

class Game;
class UIBase;
class UIPanel;
class Player;
class UIContainer;
class Sprite;
class StagePack;
class StageMetaInfo;
class UIManager
{
public:
	UIManager(Game *pGame);
	virtual ~UIManager();

	void Process();
	void Render();

	void Clear();
	void PopPanel();

	UIBase* CreateButton(int id, int x, int y, NaString str, UIContainer *pParent = nullptr);
	UIBase* CreateImageButton(int id, int x, int y, int width, int height, NaString str, Sprite *pSprite, int nSpriteIndex, UIContainer *pParent = nullptr);
	UIBase* CreateText(int id, int x, int y, NaString str, UIContainer *pParent = nullptr);
	UIBase* CreateEdit(int id, int x, int y, NaString str, NaString strTitle = L"", UIContainer *pParent = nullptr);
	UIBase* CreateList(int id, int x, int y, int width, int height, UIContainer *pParent = nullptr);
	UIBase* CreateRectangle(int id, int x, int y, int width, int height, NaString str, DWORD dwFillColor, Sprite *pSprite, int nSpriteIndex, UIContainer *pParent = nullptr);

	// Custom UI
	UIBase* CreateSingleStageButton(int id, int x, int y, NaString str, Player *pPlayer, UIContainer *pParent = nullptr);
	UIBase* CreateCustomGameButton(int id, int x, int y, StagePack *pSp, Player *pPlayer, UIContainer *pParent = nullptr);
	UIBase* CreateWorldStageButton(int id, int x, int y, StageMetaInfo *pMeta, Player *pPlayer, UIContainer *pParent = nullptr);

	UIPanel* AddPanel(int x, int y);
	void AddUI(UIBase* pUI, UIContainer *pParent = nullptr);

	UIBase* GetFocus();
	UIBase* GetControl(int id, UIContainer *pParent = nullptr);

	// Do something after transition
	void Transition(std::function<void()> fnCallback);

	Game *m_pGame;
	std::vector<UIBase*> m_vecUI;
	UIBase* m_pClicked;
	UIPanel* m_pCurPanel;
	POINT m_ptLastMouse;

	bool m_bTransition;
	std::function<void()> m_TransitionCallback;
};

