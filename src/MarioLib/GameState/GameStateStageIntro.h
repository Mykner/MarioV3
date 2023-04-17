#pragma once

#include "GameStateBase.h"

class GameStateStageIntro : public GameStateBase
{
public:
	GameStateStageIntro(Game *pGame);
	virtual ~GameStateStageIntro();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	std::vector<GameObjectBase*> m_vecLifeObject;
	GameObjectBase* m_pLifeObject;
};

