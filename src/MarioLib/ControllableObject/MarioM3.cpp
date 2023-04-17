#include "MarioM3.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "Stage.h"
#include "PipeInfo.h"
#include "Fireball.h"

#include "RenderManager.h"
#include "InputManager.h"
#include "Player.h"
#include "FModManager.h"
#include "EventObjectBase.h"
#include "VehicleObjectBase.h"

#include "UIBase.h"
#include "SpeechBubble.h"

MarioM3::MarioM3(Game *pGame, Stage *pStage) :
	ControllableObject(pGame, pStage)
{
	m_nType = PLAYEROBJECT_MARIO;
	m_pSprite = SPR_MANAGER->Request(L"MarioM3");

	m_pSpriteBody = SPR_MANAGER->Request(L"MarioM3", L"MarioM3Body");
	m_pSpriteTunic = SPR_MANAGER->Request(L"MarioM3", L"MarioM3Tunic");
	m_pSpritePants = SPR_MANAGER->Request(L"MarioM3", L"MarioM3Pants");

	m_nSpriteIndex = SPRIDX_MARIOM3_STAND;
}

MarioM3::~MarioM3()
{

}

int MarioM3::GetSpriteIndex()
{
	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
	case STATE_IDLE:
	case STATE_STAND:
	case STATE_SPECIALPOSE:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRIDX_MARIOM3_STAND;
		else
			nFrame = SPRIDX_MARIOM3_BIG_STAND;
		break;
	case STATE_SITJUMP:
	case STATE_SIT:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRIDX_MARIOM3_SIT;
		else
			nFrame = SPRIDX_MARIOM3_BIG_SIT;
		break;
	case STATE_WALK:
		if (m_nShape == SHAPE_MINI)
		{
			if (abs(m_fXS) >= 3.0f)
				nFrame = (SPRIDX_MARIOM3_RUN1 + (m_nWalkFrame % 2));
			else
				nFrame = (SPRIDX_MARIOM3_WALK1 + (m_nWalkFrame % 2) - 1);
		}
		else
		{
			if (abs(m_fXS) >= 3.0f)
				nFrame = (SPRIDX_MARIOM3_BIG_RUN1 + (m_nWalkFrame % 3));
			else
				nFrame = (SPRIDX_MARIOM3_BIG_WALK1 + (m_nWalkFrame % 3) - 1);
		}
		break;
	case STATE_SHELLRUN:
	case STATE_SHELLIDLE:
		nFrame = SPRIDX_MARIOM3_DIE;
		break;
	case STATE_BLOCKJUMP:
		if (m_nShape == SHAPE_MINI)
		{
			if (m_nStateFrame < GameDefaults::nBlockJumpFreeze)
				nFrame = (SPRIDX_MARIOM3_WALK1 + (m_nWalkFrame % 2));
			else
				nFrame = SPRIDX_MARIOM3_JUMP;
		}
		else
		{
			if (m_nStateFrame < GameDefaults::nBlockJumpFreeze)
				nFrame = (SPRIDX_MARIOM3_BIG_WALK1 + (m_nWalkFrame % 3));
			else
				nFrame = SPRIDX_MARIOM3_BIG_JUMP;
		}
		break;
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
		if (m_nStateFrame % 5 == 0)
			m_nWalkFrame++;
		
		if (m_nShape == SHAPE_MINI)
			nFrame = (SPRIDX_MARIOM3_WALK1 + (m_nWalkFrame % 2));
		else
			nFrame = (SPRIDX_MARIOM3_BIG_WALK1 + (m_nWalkFrame % 3));
		break;
	case STATE_BREAK:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRIDX_MARIOM3_BREAK;
		else
			nFrame = SPRIDX_MARIOM3_BIG_BREAK;
		break;
	case STATE_GOTOFFVEHICLE:
	case STATE_JUMP:
	case STATE_PRESSJUMP:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRIDX_MARIOM3_JUMP;
		else
			nFrame = SPRIDX_MARIOM3_BIG_JUMP;
		break;
	case STATE_FALL:
		break;
	case STATE_TRAMPOLINJUMP:
		if (m_nShape == SHAPE_MINI) 
		{
			if (nFrame != SPRIDX_MARIOM3_WALK1)
				nFrame = SPRIDX_MARIOM3_WALK1;
		}
		else
		{
			if (nFrame != SPRIDX_MARIOM3_BIG_WALK1)
				nFrame = SPRIDX_MARIOM3_BIG_WALK1;
		}
		break;
	case STATE_DIE:
		nFrame = SPRIDX_MARIOM3_DIE;
		break;
	case STATE_FLAGDOWN:
		if (m_nShape == SHAPE_MINI)
		{
			if (m_fYS == 0)
				nFrame = SPRIDX_MARIOM3_CLIMB2;
			else if (m_nStateFrame % 10 < 5)
				nFrame = SPRIDX_MARIOM3_CLIMB1;
			else
				nFrame = SPRIDX_MARIOM3_CLIMB2;
		}
		else
		{
			if (m_fYS == 0)
				nFrame = SPRIDX_MARIOM3_BIG_CLIMB2;
			else if (m_nStateFrame % 10 < 5)
				nFrame = SPRIDX_MARIOM3_BIG_CLIMB1;
			else
				nFrame = SPRIDX_MARIOM3_BIG_CLIMB2;
		}
		break;
	case STATE_CLIMBIDLE:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRIDX_MARIOM3_CLIMB2;
		else
			nFrame = SPRIDX_MARIOM3_BIG_CLIMB2;
		break;
	case STATE_CLIMBMOVE:
	case STATE_VINEUP:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_CLIMB1, 2, 5);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_BIG_CLIMB1, 2, 5);
		break;
	case STATE_SWIM:
		if (m_nShape == SHAPE_MINI)
		{
			if (m_fYS > 0)
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_SWIM1, 2, 4);
			else
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_SWIM1, 6, 4);
		}
		else
		{
			if (m_fYS > 0)
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_BIG_SWIM1, 2, 4);
			else
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_BIG_SWIM1, 5, 4);
		}
		break;
	case STATE_SWIMJUMP:
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_SWIM1, 5, 4);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOM3_BIG_SWIM1, 5, 4);
		break;
	case STATE_RIDE:
		if (m_pVehicle)
		{
			int nRideType = ((VehicleObjectBase*)m_pVehicle)->m_nRideType;
			if (nRideType == STATE_STAND)
				nFrame = SPRIDX_MARIOM3_STAND;
			else if (nRideType == STATE_RIDE)
				nFrame = SPRIDX_MARIOM3_STAND;
		}
		else
			nFrame = SPRIDX_MARIOM3_STAND;
		break;
	}

	// Throw Fireball ignore action frames
	switch (m_nState)
	{
	case STATE_STAND:
	case STATE_WALK:
	case STATE_JUMP:
	case STATE_FALL:
	case STATE_RIDE:
		if (m_nThrowFireFrame > 0)
		{
			nFrame = SPRIDX_MARIOM3_BIG_THROWFIRE;
			m_nThrowFireFrame--;
		}
		break;
	}	

	 m_nSpriteIndex = nFrame;

	return m_nSpriteIndex;
}

void MarioM3::GetColor(int *pnColor)
{
	int nType = 0;
	switch (m_nShape)
	{
	case SHAPE_FIRE:	nType = 1; break;
	case SHAPE_BIG:		nType = 0; break;
	default:			nType = 0; break;
	}

	if (m_nNextShape == SHAPE_FIRE ||
		m_nNextShape == SHAPE_GIANT ||
		(m_nShape == SHAPE_FIRE && m_nNextShape != -1))
	{
		nType = (m_pGame->GetStateFrame() / 4 % 4) + 1;
	}

	if (m_nInvinsibleTime > 0)
	{
		// normal, white+green, white+red, normal+black
		int nVal = 0;
		if (m_nInvinsibleTime < 8 * 24)
			nVal = (m_nInvinsibleTime % 16) / 4;
		else
			nVal = (m_nInvinsibleTime % 8) / 2;

		nType = 5 + nVal;
	}

	// Body, Tunic, Pants
	int nColorTable[] =
	{
		0xffffd9b2, 0xfff73910, 0xff000000, // Normal(MarioMaker)
		0xffffd9b2, 0xffffb259, 0xfff73910, // Fire
		0xffffffff, 0xffffa200, 0xff4aaa10, // ShapeChange(Green)
		0xffffffff, 0xffffa200, 0xffe75100, // ShapeChange(Orange)
		0xffffcbbd, 0xffc67100, 0xff000000, // ShapeChange(Black)

		0xffffa200, 0xff8c8a00, 0xffe75100, // 5. Invinsible1
		0xffffffff, COLOR_XRGB(230, 156, 33), COLOR_XRGB(58, 132, 0), // Invinsible2
		0xffffffff, COLOR_XRGB(230, 156, 33), COLOR_XRGB(177, 52, 37), // Invinsible3
		COLOR_XRGB(255, 206, 197), COLOR_XRGB(156, 74, 0), 0xff000000, // Invinsible4

	};

	if (m_pGame->m_nStagePackType != STAGEPACK_CLASSIC)
	{
		// SMM Style Invinsible color cycle WIP
		/*
		if (m_nInvinsibleTime > 0)
		{
			pnColor[0] = COLOR_XRGB(255, 180 + rand() % 70, rand() % 255);
			pnColor[1] = COLOR_XRGB(100 + rand() % 120, 150 + rand() % 50, 150 + rand() % 50);
			pnColor[2] = COLOR_XRGB(150 + rand() % 100, 80 + rand() % 170, 150 + rand() % 50);
			return;
		}
		*/
	}

	pnColor[0] = nColorTable[nType * 3];
	pnColor[1] = nColorTable[nType * 3 + 1];
	pnColor[2] = nColorTable[nType * 3 + 2];
}

bool MarioM3::CanSit()
{
	if (m_nShape > SHAPE_MINI)
		return true;

	return false;
}

void MarioM3::Render(int nColor, int nZOrder)
{
	if (CheckSkipRender())
		return;

	float _x = x, _y;
	if (m_pClimb)
		_x = m_pClimb->m_fX;

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	_x = _x - ptCamera.x;
	_y = y - ptCamera.y;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	int nSpriteIdx = GetSpriteIndex();
	int nColors[3];
	GetColor(nColors);
	
	if (nColor != -1 && nColor != 0)
	{
		nColors[0] &= nColor;
		nColors[1] &= nColor;
		nColors[2] &= nColor;
	}
	
	if (nZOrder == -1)
		nZOrder = GetZOrder();
	bool bShadow = m_pGame->m_bModernStyle;

	if (m_nState == STATE_PARALYZE && m_nStateFrame % 2 == 1)
		_x += 1;

	m_pSpriteBody->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[0], fScaleX, fScaleY, nZOrder, bShadow);
	m_pSpriteTunic->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[1], fScaleX, fScaleY, nZOrder, bShadow);
	m_pSpritePants->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[2], fScaleX, fScaleY, nZOrder, bShadow);
}

void MarioM3::RenderShapeChange()
{
	NaPointT<float> pt = CAMERA->GetPosition();

	int nFrame = m_pGame->GetStateFrame();
	int nSpriteIdx = 0;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (m_nNextShape == SHAPE_MINI)
	{
		if (nFrame % 2 == 1)
			return;

		GetBigToMiniSpriteIndex(&nSpriteIdx, &fScaleX, &fScaleY);
	}	
	else if (m_nShape == SHAPE_MINI)
	{
		GetMiniToBigSpriteIndex(&nSpriteIdx, &fScaleX, &fScaleY);
	}	
	else if ((m_nShape != SHAPE_BIG) && (m_nNextShape != SHAPE_BIG))
	{
		nSpriteIdx = GetSpriteIndex();

		if (nFrame < 8)
			nSpriteIdx = CheckValidSpriteIndex(nSpriteIdx, m_nShape);
		else 
		{
			if (nFrame % 8 < 4)
				nSpriteIdx = CheckValidSpriteIndex(nSpriteIdx, m_nShape);
			else
				nSpriteIdx = CheckValidSpriteIndex(nSpriteIdx, m_nNextShape);

			if (nFrame % 8 < 4)
				fScaleY *= (0.7f + ((float)nFrame / 60) * 0.3f);
		}
	}
	else
	{
		nSpriteIdx = GetSpriteIndex();
	}

	int nColors[3];
	GetColor(nColors);

	int nZOrder = GetZOrder();
	float _x = x - pt.x;
	float _y = y - pt.y;	
	bool bShadow = m_pGame->m_bModernStyle;
	
	m_pSpriteBody->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[0], fScaleX, fScaleY, nZOrder, bShadow);
	m_pSpriteTunic->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[1], fScaleX, fScaleY, nZOrder, bShadow);
	m_pSpritePants->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[2], fScaleX, fScaleY, nZOrder, bShadow);
}

int MarioM3::CheckValidSpriteIndex(int nIndex, int nShape)
{
	if (nShape == -1)
		nShape = m_nShape;

	switch (nShape)
	{
	case SHAPE_MINI:
		if (nIndex >= SPRIDX_MARIOM3_BIG_STAND)
			nIndex -= (SPRIDX_MARIOM3_BIG_STAND - SPRIDX_MARIOM3_STAND - 1);
		break;
	case SHAPE_BIG:
	case SHAPE_FIRE:
	case SHAPE_GIANT:
		{
			if (nIndex < SPRIDX_MARIOM3_BIG_THROWFIRE)
			{
				if (nIndex < SPRIDX_MARIOM3_BIG_STAND)
					nIndex += (SPRIDX_MARIOM3_BIG_STAND - SPRIDX_MARIOM3_STAND);
			}
		}
		break;
	}

	return nIndex;
}

bool MarioM3::CheckSkipRender()
{
	if (m_nDamageCooltime > 0)
	{
		if (m_pGame->m_nGameFrame % 2 == 1)
			return true;
	}

	if (m_nGiantTime > 0)
	{
		if (m_nGiantTime < 8 * 24)
		{
			if (m_pGame->m_nGameFrame % 4 < 2)
				return true;
		}
		else if (m_nGiantTime < 4 * 24)
		{
			if (m_pGame->m_nGameFrame % 2 < 1)
				return true;
		}
	}

	return false;
}

void MarioM3::GetMiniToBigSpriteIndex(int * pnIndex, float * pfScaleX, float * pfScaleY)
{
	// Total 60 frame
	int nFrame = m_pGame->GetStateFrame();
	*pnIndex = GetSpriteIndex();

	if (nFrame >= 8)
	{
		*pnIndex += (SPRIDX_MARIOM3_BIG_STAND - SPRIDX_MARIOM3_STAND);

		if (nFrame % 8 < 4)
			*pfScaleY *= (0.7f + ((float)nFrame / 60) * 0.3f);
		else
			*pfScaleY *= (0.8f + ((float)nFrame / 60) * 0.2f);
	}
}

void MarioM3::GetBigToMiniSpriteIndex(int * pnIndex, float * pfScaleX, float * pfScaleY)
{
	// Total 60 frame
	int nFrame = m_pGame->GetStateFrame();
	if (m_pGame->m_bModernStyle)
	{
		*pnIndex = GetSpriteIndex();
		
		if (nFrame >= 8)
		{
			if (nFrame % 8 < 4)
				*pfScaleY *= (0.9f - ((float)nFrame / 60) * 0.3f);
			else
				*pfScaleY *= (1.0f - ((float)nFrame / 60) * 0.2f);
		}
	}
	else
	{
		if (nFrame < 8)
			*pnIndex = SPRIDX_MARIOM3_BIG_JUMP;
		else
		{
			if (nFrame % 8 < 4)
				*pnIndex = SPRIDX_MARIOM3_SWIM2;
			else
				*pnIndex = SPRIDX_MARIOM3_BIG_SWIM2;
		}
	}
}

void MarioM3::GetAnyToGiantSpriteIndex(int * pnIndex, float * pfScaleX, float * pfScaleY)
{
	int nFrame = m_pGame->GetStateFrame();

	*pnIndex = SPRIDX_MARIOM3_DIE;
}
