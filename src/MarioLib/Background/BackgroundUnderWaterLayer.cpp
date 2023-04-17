#include "BackgroundUnderWaterLayer.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"

BackgroundUnderWaterLayer::BackgroundUnderWaterLayer(Stage* pStage) :
    BackgroundObjectBase(pStage)
{
}

BackgroundUnderWaterLayer::~BackgroundUnderWaterLayer()
{
}

void BackgroundUnderWaterLayer::Render()
{
    if (m_pSpriteBackground == nullptr)
        return;

    NaPointT<float> ptCamera = CAMERA->GetPosition();

    int _x = -((int)ptCamera.x % TILE_XS) - TILE_XS;
    int _xEnd = _x + m_pGame->m_nWidth + (GameDefaults::nPageTileWidth * 3);
    int nType = GetFrame();

    for (int x = _x; x < _xEnd; x += 16)
    {
        m_pSpriteBackground->RenderToQueue(
            x,
            (GameDefaults::nPageTileHeight * 3) - ptCamera.y,
            nType, false, false, -1, 1.0f, 1.0f, Z_BACKGROUND);
    }
}

bool BackgroundUnderWaterLayer::IsOnCamera()
{
    return true;
}

SIZE BackgroundUnderWaterLayer::GetSize()
{
    return { 0, 0 };
}

NaRect BackgroundUnderWaterLayer::GetRect()
{
    return { 0, 0, 0, 0 };
}

int BackgroundUnderWaterLayer::GetFrame()
{
    if (m_pGame->m_bModernStyle)
    {
        int nFrame = m_pGame->m_nGameFrame % 80;
        if (nFrame < 20)
            return 21;
        if (nFrame < 40)
            return 22;
        if (nFrame < 60)
            return 23;
        if (nFrame < 80)
            return 24;
    }

    return 20;
}
