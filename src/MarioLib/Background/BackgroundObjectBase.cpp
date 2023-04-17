#include "BackgroundObjectBase.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "FModManager.h"
#include "ThemeManager.h"

#include "BackgroundUnderWaterLayer.h"
#include "BackgroundLavaLayer.h"

BackgroundObjectBase::BackgroundObjectBase(Stage *pStage)
{
    m_pStage = pStage;
    m_pGame = pStage->m_pGame;
    m_pSpriteBackground = pStage->m_pSpriteBackground;
    m_nType = -1;
}

BackgroundObjectBase::~BackgroundObjectBase()
{
}

void BackgroundObjectBase::Render()
{
    if (m_pSpriteBackground == nullptr)
        return;

    NaPointT<float> ptCamera = CAMERA->GetPosition();

    m_pSpriteBackground->RenderToQueue(
        m_nX - ptCamera.x,
        m_nY - ptCamera.y,
        m_nType, false, false, -1, 1.0f, 1.0f, Z_BACKGROUND);
}

bool BackgroundObjectBase::IsOnCamera()
{
    NaRect rcView = CAMERA->GetViewport();
    NaRect rc = GetRect();

    if (rcView.IsOverlapped(rc))
        return true;

    return false;
}

SIZE BackgroundObjectBase::GetSize()
{
    SIZE s = { 0, 0 };

    if (m_pSpriteBackground != nullptr && m_nType >= 0)
    {
        RECT &rc = m_pSpriteBackground->m_vecFrames[m_nType].m_rc;
        s.cx = rc.right - rc.left;
        s.cy = rc.bottom - rc.top;
    }

    return s;
}

NaRect BackgroundObjectBase::GetRect()
{
    NaRect rc;
    SIZE s = GetSize();
    rc.left = m_nX - s.cx / 2;
    rc.right = m_nX + s.cx / 2;
    rc.top = m_nY - s.cy;
    rc.bottom = m_nY;

    return rc;
}

// BackgroundObject Factory
BackgroundObjectBase * BackgroundObjectBase::CreateInstance(Game *pGame, Stage *pStage, int nType)
{
    BackgroundObjectBase *pObj = nullptr;
    switch (nType)
    {
    case 20:    pObj = new BackgroundUnderWaterLayer(pStage);   break;
    case 25:    pObj = new BackgroundLavaLayer(pStage);         break;
    default:    pObj = new BackgroundObjectBase(pStage);        break;
    }

    if (pObj)
        pObj->m_nType = nType;

    return pObj;
}
