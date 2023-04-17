#pragma once

#include "BackgroundObjectBase.h"

class Stage;
class BackgroundLavaLayer : public BackgroundObjectBase
{
public:
    BackgroundLavaLayer(Stage *pStage);
    virtual ~BackgroundLavaLayer();

    virtual void Render();

    virtual bool IsOnCamera();

    virtual SIZE GetSize();
    virtual NaRect GetRect();
    virtual int GetFrame();
};

