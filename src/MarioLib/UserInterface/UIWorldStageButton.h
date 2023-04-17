#pragma once

#include "UIButton.h"

#include "NaString.h"
#include "NaRect.h"
#include "StageMetaInfo.h"

class Game;
class UIWorldStageButton : public UIButton
{
public:
	UIWorldStageButton(Game *pGame);
	virtual ~UIWorldStageButton();

	virtual void Process();
	virtual void Render();

	virtual NaRect GetRect();

	int m_nGameTheme;
	bool m_bPlayed;

	NaString m_strBestTime;
	NaString m_strBestScore;
	Sprite* m_pSpriteEtc;

	StageMetaInfo* m_pMetaInfo;

	static bool Compare(const UIWorldStageButton *t1, const UIWorldStageButton *t2)
	{
		return StageMetaInfo::Compare(t1->m_pMetaInfo, t2->m_pMetaInfo);
	};
};

