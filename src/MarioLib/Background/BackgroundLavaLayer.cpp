#include "BackgroundLavaLayer.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"

BackgroundLavaLayer::BackgroundLavaLayer(Stage* pStage) :
    BackgroundObjectBase(pStage)
{
}

BackgroundLavaLayer::~BackgroundLavaLayer()
{
}

void BackgroundLavaLayer::Render()
{
    if (m_pSpriteBackground == nullptr)
        return;

    NaPointT<float> ptCamera = CAMERA->GetPosition();

    int _x = -((int)ptCamera.x % TILE_XS) - TILE_XS;
    int _xEnd = _x + m_pGame->m_nWidth + (GameDefaults::nPageTileWidth * 3);
    int nType = GetFrame();

    int _y = (m_pStage->m_sizeTile.cy - 1) * TILE_YS;
    // Mykner> TODO
    // #TODO
    /*int n = m_pGame->m_pThemeManager->

    v10 = *(_DWORD *)(*(_DWORD *)(*(_DWORD *)(this + 24) + 156) + 284);
    if ( v10 >= 5 && v10 <= 8 )
    {
      Camera::GetPosition(*(int **)(*(_DWORD *)(this + 24) + 160), &v16, 0);
      LOBYTE(v22) = 1;
      v13 = v16.y;
      if ( v16.y != (float)(int)v16.y )
        v13 = v16.y + 1.0;
      if ( *(_DWORD *)(*(_DWORD *)(this + 20) + 632) % 32 < 16 )
        v13 = v13 + 1.0;
      v14 = v13 - (float)((*(_DWORD *)(*(_DWORD *)(this + 20) + 164) - 1) << 8);
      v11 = (int)(float)((float)v11 + v14);
      if ( *(_DWORD *)(this + 28) != -1
        && (unsigned int)sub_FE0100((void *)(*(_DWORD *)(this + 20) + 728)) > *(_DWORD *)(this + 28) )
      {
        NaRect::NaRect(&v18);
        LOBYTE(v22) = 2;
        v18.left = -16;
        v18.top = (int)(float)((float)(16 * (*(_DWORD *)(*(_DWORD *)(this + 20) + 744) - 1) - 8) + v14);
        v18.right = 16 * (*(_DWORD *)(*(_DWORD *)(this + 20) + 740) + 2);
        v18.bottom = v18.top + 16;
        v3 = v18.top;
        v4 = v18.right;
        v5 = v18.top + 16;
        v2 = (NaRect *)sub_FE7DC0((void *)(*(_DWORD *)(this + 20) + 728), *(_DWORD *)(this + 28));
        v2->left = -16;
        v2->top = v3;
        v2->right = v4;
        v2->bottom = v5;
        LOBYTE(v22) = 1;
        NaRect::dtor((ATL::CComCriticalSection *)&v18);
      }
      LOBYTE(v22) = 0;
      ATL::CComCriticalSection::~CComCriticalSection((ATL::CComCriticalSection *)&v16);
    }*/

    for (int x = _x; x < _xEnd; x += 16)
    {
        m_pSpriteBackground->RenderToQueue(
            x,
            _y - ptCamera.y,
            nType, false, false, -1, 1.0f, 1.0f, Z_MAPOBJECT_TOP);
    }

    if (m_pGame->m_bIsDesignGame)
    {
        CUR_STAGE->m_pSprite->RenderToQueue(
            -TILE_XS,
            (_y - ptCamera.y) + TILE_YS,
            99, false, false, -1, m_pGame->m_nWidth + (TILE_XS * 2), 1.0f, Z_MAPOBJECT_TOP);
    }
}

bool BackgroundLavaLayer::IsOnCamera()
{
    return true;
}

SIZE BackgroundLavaLayer::GetSize()
{
    return { 0, 0 };
}

NaRect BackgroundLavaLayer::GetRect()
{
    return { 0, 0, 0, 0 };
}

int BackgroundLavaLayer::GetFrame()
{
    if (m_pGame->m_bModernStyle)
    {
        int nFrame = m_pGame->m_nGameFrame % 80;
        if (nFrame < 20)
            return 26;
        if (nFrame < 40)
            return 27;
        if (nFrame < 60)
            return 28;
        if (nFrame < 80)
            return 29;
    }

    return 25;
}
