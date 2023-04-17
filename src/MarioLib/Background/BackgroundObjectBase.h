#pragma once

#include "Common.h"
#include "Types.h"
#include "SpriteIndex.h"

class Stage;
class Game;
class Sprite;
class BackgroundObjectBase
{
public:
    BackgroundObjectBase(Stage *pStage);
    virtual ~BackgroundObjectBase();

    virtual void Render();

    virtual bool IsOnCamera();

    virtual SIZE GetSize();
    virtual NaRect GetRect();

    static BackgroundObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

    int m_nX;
    int m_nY;
    int m_nType;

    Sprite* m_pSpriteBackground;

    Stage* m_pStage;
    Game* m_pGame;
};

