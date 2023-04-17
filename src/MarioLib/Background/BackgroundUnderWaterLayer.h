#pragma once

#include "BackgroundObjectBase.h"

class Stage;
class BackgroundUnderWaterLayer : public BackgroundObjectBase
{
public:
    BackgroundUnderWaterLayer(Stage *pStage);
    virtual ~BackgroundUnderWaterLayer();

    virtual void Render();

    virtual bool IsOnCamera();

    virtual SIZE GetSize();
    virtual NaRect GetRect();
    virtual int GetFrame();
};

