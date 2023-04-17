#include "Camera.h"

#include "Game.h"
#include "GameObjectBase.h"
#include "Stage.h"
#include "VehicleObjectBase.h"
#include "Themes.h"

Camera::Camera(Game *pGame)
{
	m_pGame = pGame;

	m_ptPos.x = 0;
	m_ptPos.y = 0;
	m_fXS = 0;
	m_fYS = 0;

	m_pTarget = nullptr;
    
    m_nEarthQuakeCooltime = 0;
	m_nEarthQuakePower = 2;
}

Camera::~Camera()
{
}

void Camera::LookAt(GameObjectBase * pTarget)
{
	m_pTarget = pTarget;
}

void Camera::Process()
{
	const int nMaxSpeed = 1024;
	const int nMaxSpeedY = 32;
	if (m_pTarget)
	{
		GameObjectBase *pTarget = m_pTarget;
		if (pTarget->m_pVehicle && pTarget->m_pVehicle->IsAlive())
			pTarget = pTarget->m_pVehicle;

		NaRect rcViewport = GetViewport();
		bool bAutoScrollX = false;
		bool bAutoScrollY = false;

		// Horz movement
		{
			if (CUR_STAGE->m_nAutoScroll < AUTOSCROLL_HORZ_SLOW || 
				CUR_STAGE->m_nAutoScroll > AUTOSCROLL_HORZ_GOD)
			{
				bool bCanGoBack = CUR_STAGE->m_bCanGoBack;

				int nMaxX = CUR_STAGE->m_ptStopPoint.x - m_pGame->m_nWidth;
				float fCenterX = m_ptPos.x + (m_pGame->m_nWidth / 2);
				float fLeftBoundary = fCenterX - (m_pGame->m_nWidth / 16.0f);
				float fRightBoundary = fCenterX + (m_pGame->m_nWidth / 16.0f);

				// Base camera movement
				if (pTarget->m_fX < fLeftBoundary)
					m_fXS = pTarget->m_fX - fLeftBoundary;
				else if (pTarget->m_fX > fRightBoundary)
					m_fXS = pTarget->m_fX - fRightBoundary;

				if (m_fXS > nMaxSpeed)
					m_fXS = nMaxSpeed;
				else if (!bCanGoBack && m_fXS < 0)
					m_fXS = 0;
				else if (m_fXS < -nMaxSpeed)
					m_fXS = -nMaxSpeed;
			}
			else
			{
				bAutoScrollX = true;
				switch(CUR_STAGE->m_nAutoScroll)
				{
				case AUTOSCROLL_HORZ_SLOW:		m_fXS = 1; break;
				case AUTOSCROLL_HORZ_NORMAL:	m_fXS = 2; break;
				case AUTOSCROLL_HORZ_FAST:		m_fXS = 3; break;
				case AUTOSCROLL_HORZ_GOD:		m_fXS = 4; break;
				}				
			}
		}

		// Vert movement
		{
			if (CUR_STAGE->m_nAutoScroll < AUTOSCROLL_VERT_SLOW ||
				CUR_STAGE->m_nAutoScroll > AUTOSCROLL_VERT_GOD)
			{
				int nMaxY = ((CUR_STAGE->m_sizeTile.cy - 1) * TILE_YS) - m_pGame->m_nHeight;
				float fCenterY = m_ptPos.y + (m_pGame->m_nHeight / 2);
				float fTopBoundary = fCenterY - (m_pGame->m_nHeight / 16.0f);
				float fBottomBoundary = fCenterY + (m_pGame->m_nHeight / 16.0f);

				// Base camera movement
				if (pTarget->m_fY < fTopBoundary && pTarget->m_fY < nMaxY + (TILE_YS * 3))
					m_fYS = pTarget->m_fY - fTopBoundary;
				else if (pTarget->m_fY > fBottomBoundary)
					m_fYS = pTarget->m_fY - fBottomBoundary;

				if (m_fYS > nMaxSpeedY)
					m_fYS = nMaxSpeedY;
				else if (CUR_STAGE->m_sizeTile.cy <= 16 && m_fYS < 0)
					m_fYS = 0;
				else if (m_fYS < -nMaxSpeedY)
					m_fYS = -nMaxSpeedY;
			}
			else
			{
				bAutoScrollY = true;
				switch (CUR_STAGE->m_nAutoScroll)
				{
				case AUTOSCROLL_VERT_SLOW:		m_fYS = -0.3f; break;
				case AUTOSCROLL_VERT_NORMAL:	m_fYS = -0.5f; break;
				case AUTOSCROLL_VERT_FAST:		m_fYS = -1.0f; break;
				case AUTOSCROLL_VERT_GOD:		m_fYS = -2.0f; break;
				}
			}
		}

		if (!bAutoScrollX)
			m_ptPos.x += (m_fXS / 8.0f);
		else
			m_ptPos.x += m_fXS;

		if (!bAutoScrollY)
			m_ptPos.y += (m_fYS / 8.0f);
		else
			m_ptPos.y += m_fYS;

		// Limit min&max of camera
		ProcessLimit();

		m_fXS = 0;
		m_fYS = 0;
	}

	if (m_nEarthQuakeCooltime > 0)
	{
		int nRange = m_nEarthQuakePower;
		m_ptEarthQuakeOffset.y = ((rand() % (nRange * 2 + 1)) - nRange);
		m_nEarthQuakeCooltime--;
	}
}

void Camera::ProcessPlayerBound()
{
	if (m_pTarget)
	{
		GameObjectBase *pTarget = m_pTarget;
		GameObjectBase *pRealTarget = pTarget;
		if (pTarget->m_pVehicle && pTarget->m_pVehicle->IsAlive())
			pTarget = pTarget->m_pVehicle;
		
		NaRect rcViewport = GetViewport();
		NaRect rcTarget = pTarget->GetRect();

		// Player cannot exit from camera
		if (rcTarget.left < rcViewport.left)
		{
			if (pTarget->m_bCollision[COLLISION_RIGHT] == false)
			{
				pTarget->m_fX = rcViewport.left + (pTarget->GetSize().cx / 2);
				pTarget->m_fXS = 0;
			}
			else
			{
				// Dead by scroll...?!
				pRealTarget->ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
			}
		}

		if (rcTarget.right > rcViewport.right)
		{
			pTarget->m_fX = rcViewport.right - (pTarget->GetSize().cx / 2);
			pTarget->m_fXS = 0;
		}

		// if Vertical scrolling..
		if (CUR_STAGE->m_nAutoScroll >= AUTOSCROLL_VERT_SLOW &&
			CUR_STAGE->m_nAutoScroll <= AUTOSCROLL_VERT_GOD)
		{
			if (pTarget->IsAlive() && rcTarget.top >= rcViewport.bottom)
			{
				pRealTarget->ChangeState(STATE_DIE);
				m_pGame->ChangeState(GAMESTATE_DIE);
			}
		}

		// Limit in underwater
		if (CUR_STAGE->m_nBackgroundTheme == STAGEBGTHEME_UNDERWATER)
		{
			if (pTarget->m_fY < 48 && pTarget->m_fYS < 1)
			{
				//pTarget->m_fYS += 0.5f;

				pTarget->m_fY = 48;
				pTarget->m_fYS = 0;
			}
		}
	}
}

void Camera::ProcessLimit()
{
	int nStopPointX = CUR_STAGE->m_ptStopPoint.x;
	if (nStopPointX == 0)
		nStopPointX = GameDefaults::nMaxCameraPosX;

	int nMaxX = nStopPointX - m_pGame->m_nWidth;
	
	if (m_ptPos.x >= nMaxX)
		m_ptPos.x = nMaxX;
	if (m_ptPos.x < 0)
		m_ptPos.x = 0;

	int nMaxY = ((CUR_STAGE->m_nMaxVertPage - 1) * GameDefaults::nPageHeight);

	if (m_ptPos.y >= nMaxY)
		m_ptPos.y = nMaxY;
	if (m_ptPos.y < 0)
		m_ptPos.y = 0;
}

NaPointT<float> Camera::GetPosition(int nPin)
{
	NaPointT<float> pt = m_ptPos;

	switch (nPin)
	{
	case CAMERA_PIN_NORMAL:
		if (m_nEarthQuakeCooltime > 0)
			pt.y += m_ptEarthQuakeOffset.y;

		pt.y += m_ptAirShipOffset.y;
		break;
	case CAMERA_PIN_BACKGROUND:
		if (m_nEarthQuakeCooltime > 0)
			pt.y += m_ptEarthQuakeOffset.y;

		pt.y += (m_ptAirShipOffset.y / 5.0f);
		break;
	case CAMERA_PIN_FARBACKGROUND:
		break;
	}

	return pt;
}

void Camera::SetPosition(float fX, float fY)
{
	m_ptPos.x = fX;
	m_ptPos.y = fY;

	ProcessLimit();
}

// Coord that Player can see
NaRect Camera::GetViewport()
{
	CheckViewport();

	return m_rcViewport;
}

// Tile index that Player can see
NaRect Camera::GetTileViewport()
{
	NaRect rc = GetViewport();
	rc.left = rc.left / TILE_XS;
	rc.top = rc.top / TILE_YS;
	rc.right = rc.right / TILE_XS + 1;
	rc.bottom = rc.bottom / TILE_YS + 1;

	return rc;
}

void Camera::CheckViewport()
{
	m_rcViewport.left = m_ptPos.x;
	m_rcViewport.top = m_ptPos.y;
	m_rcViewport.right = m_ptPos.x + m_pGame->m_nWidth;
	m_rcViewport.bottom = m_ptPos.y + m_pGame->m_nHeight;
}

bool Camera::IsOutOfCamera(GameObjectBase * pTarget)
{
	if (pTarget)
	{
		NaRect rc2 = pTarget->GetRect();
		NaRect rc = GetViewport();
		rc.left -= (GameDefaults::nTilePerScreenWidth * GameDefaults::nTileWidth) / 2;
		rc.right += (GameDefaults::nTilePerScreenWidth * GameDefaults::nTileWidth) / 8;
		rc.top -= (GameDefaults::nTilePerScreenHeight * GameDefaults::nTileHeight);
		rc.bottom += (GameDefaults::nTilePerScreenHeight * GameDefaults::nTileHeight) / 2;

		if (rc.IsOverlapped(rc2))
			return false;

		return true;
	}

	return true;
}

void Camera::MakeEarthquake(int nPower)
{
	if (m_nEarthQuakeCooltime > 0)
	{
		if (m_nEarthQuakePower < nPower)
			m_nEarthQuakePower = nPower;
	}
	else
	{
		m_nEarthQuakePower = nPower;
	}

	m_nEarthQuakeCooltime = 24;
}

void Camera::StopEarthquake()
{
	m_ptEarthQuakeOffset = { 0, 0 };
	m_nEarthQuakeCooltime = 0;
}
