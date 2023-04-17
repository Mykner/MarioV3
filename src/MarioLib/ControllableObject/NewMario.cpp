#include "NewMario.h"

#include "Game.h"
#include "Sprite.h"
#include "SpriteManager.h"
#include "Camera.h"
#include "Stage.h"
#include "PipeInfo.h"

#include "RenderManager.h"
#include "InputManager.h"
#include "Player.h"
#include "FModManager.h"

NewMario::NewMario(Game *pGame, Stage *pStage) :
	Mario(pGame, pStage)
{
	m_nType = PLAYEROBJECT_NEWMARIO;
	m_pSprite = SPR_MANAGER->Request(L"NewMario");

	m_pSpriteBody = SPR_MANAGER->Request(L"NewMario", L"NewMarioBody");
	m_pSpriteTunic = SPR_MANAGER->Request(L"NewMario", L"NewMarioTunic");
	m_pSpritePants = SPR_MANAGER->Request(L"NewMario", L"NewMarioPants");
}

NewMario::~NewMario()
{

}

void NewMario::Render(int nColor, int nZOrder)
{
	if (m_nDamageCooltime > 0)
	{
		if (m_nDamageCooltime % 2 == 1)
			return;
	}

	float _x = x, _y;
	if (m_pClimb)
	{
		if (!m_bFlip)
			_x = m_pClimb->m_fX - 6;
		else
			_x = m_pClimb->m_fX + 6;
	}

	POINT ptCamera = CAMERA->GetPosition();
	_x = _x - ptCamera.x;
	_y = y - ptCamera.y;

	int nSpriteIdx = GetSpriteIndex();
	int nColors[3];
	GetColor(nColors);

	nZOrder = GetZOrder();
	m_pSpriteBody->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[0], m_fScaleX, m_fScaleY, nZOrder);
	m_pSpriteTunic->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[1], m_fScaleX, m_fScaleY, nZOrder);
	m_pSpritePants->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColors[2], m_fScaleX, m_fScaleY, nZOrder);
}

void NewMario::RenderShapeChange()
{
	NaPointT<float> pt = CAMERA->GetPosition();

	int nFrame = m_pGame->GetStateFrame();
	int nSpriteIdx = 0;
	if (m_nNextShape == SHAPE_MINI)
	{
		if (nFrame % 2 == 1)
			return;
		if (nFrame < 8)
			nSpriteIdx = SPRIDX_NEWMARIO_BIG_JUMP + 8; // jump top
		else
		{
			if (nFrame % 8 < 4)
				nSpriteIdx = SPRIDX_NEWMARIO_WALK + 5; // swim mini
			else
				nSpriteIdx = SPRIDX_NEWMARIO_BIG_WALK + 7; // swim big
		}
	}
	else if (m_nNextShape == SHAPE_BIG ||
		(m_nShape == SHAPE_MINI && m_nNextShape == SHAPE_FIRE))
	{
		if (nFrame < 8)
			nSpriteIdx = SPRIDX_NEWMARIO_IDLE; // stand mini
		else if (nFrame < 42)
		{
			if (nFrame % 8 < 4)
				nSpriteIdx = SPRIDX_NEWMARIO_BIG_SIT; // stand middle
			else
				nSpriteIdx = SPRIDX_NEWMARIO_IDLE;
		}
		else
			nSpriteIdx = SPRIDX_NEWMARIO_BIG_IDLE; // stand big
	}
	else if (m_nNextShape == SHAPE_FIRE)
	{
		nSpriteIdx = GetSpriteIndex();
	}

	int nColor[3];
	GetColor(nColor);

	int nZOrder = GetZOrder();

	m_pSpriteBody->RenderToQueue(x - pt.x, y - pt.y, nSpriteIdx, m_bFlip, m_bFlipV, nColor[0], m_fScaleX, m_fScaleY, nZOrder);
	m_pSpriteTunic->RenderToQueue(x - pt.x, y - pt.y, nSpriteIdx, m_bFlip, m_bFlipV, nColor[1], m_fScaleX, m_fScaleY, nZOrder);
	m_pSpritePants->RenderToQueue(x - pt.x, y - pt.y, nSpriteIdx, m_bFlip, m_bFlipV, nColor[2], m_fScaleX, m_fScaleY, nZOrder);
}

bool NewMario::CanSit()
{
	return true;
}

void NewMario::GetColor(int * pnColor)
{
	int nType = 0;
	switch (m_nShape)
	{
	case SHAPE_FIRE:	nType = 1; break;
	case SHAPE_BIG:		nType = 0; break;
	default:			nType = 0; break;
	}

	if (m_nNextShape == SHAPE_FIRE)
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

	// ColorMap: normal, fire, green, orange, black, invinsible1, invinsible2, invinsible3
	const DWORD dwBodyColors[] = {
		COLOR_XRGB(252, 142, 46), 0xffffa200, 0xffffffff, 0xffffffff, 0xffffcbbd, 0xffffa200, 0xffffffff, 0xffffffff0, COLOR_XRGB(255, 206, 197)
	};
	const DWORD dwTunicColors[] = {
		COLOR_XRGB(126, 101, 0), 0xffe75100, 0xffffa200, 0xffffa200, 0xffc67100, 0xff8c8a00, COLOR_XRGB(230, 156, 33), COLOR_XRGB(230, 156, 33), COLOR_XRGB(156, 74, 0)
	};
	const DWORD dwPantsColors[] = {
		COLOR_XRGB(212, 32, 0), 0xffffdba5, 0xff4aaa10, 0xffe75100, 0xff000000, 0xffe75100, COLOR_XRGB(58, 132, 0), COLOR_XRGB(177, 52, 37), COLOR_XRGB(0, 0, 0)
	};

	pnColor[0] = dwBodyColors[nType];
	pnColor[1] = dwTunicColors[nType];
	pnColor[2] = dwPantsColors[nType];
}

int NewMario::GetSpriteIndex()
{
	bool bIsBig = m_nShape > SHAPE_MINI ? true : false;
	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
	case STATE_IDLE:	
	case STATE_STAND:
		{
			if (!bIsBig)
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_IDLE, 16, 4);
			else
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_BIG_IDLE, 16, 4);
		}
		break;
	case STATE_SITJUMP:
	case STATE_SIT:
		{
			if (!bIsBig)
			{
				if (nFrame != SPRIDX_NEWMARIO_SIT + 1)
					nFrame = SPRITE_INDEX_NOCYCLE(SPRIDX_NEWMARIO_SIT, 2, 2);
			}
			else
			{
				if (nFrame != SPRIDX_NEWMARIO_BIG_SIT + 1)
				nFrame = SPRITE_INDEX_NOCYCLE(SPRIDX_NEWMARIO_BIG_SIT, 2, 2);
			}
		}
		break;
	case STATE_WALK:
	{
		int nFriction;
		if (abs(m_fXS) < 1.5f) 
			nFriction = 3;
		else 
			nFriction = MAX(1, (3 - abs(m_fXS)));

		if (m_nStateFrame % (nFriction) == 0 && m_nNextShape == -1) 
			m_nWalkFrame++;

		int nMax = bIsBig ? 24 : 20;
		if (m_nWalkFrame > nMax)
			m_nWalkFrame = 0;

		if (!bIsBig)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_RUN, 20, nFriction);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_BIG_RUN, 24, nFriction);
		break;
	}
	case STATE_PIPERIGHT:
		if (!bIsBig)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_RUN, 20, 1);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_BIG_RUN, 24, 1);
		break;
	case STATE_BREAK:
		if (!bIsBig)
			nFrame = SPRIDX_NEWMARIO_BREAK; 
		else
			nFrame = SPRIDX_NEWMARIO_BIG_BREAK;
		break;
	case STATE_JUMP:
		{
			int nJumpBase = !bIsBig ? SPRIDX_NEWMARIO_JUMP : SPRIDX_NEWMARIO_BIG_JUMP;
			if (m_nStateFrame == 0)
				nFrame = nJumpBase;
			else if (m_fYS < 0 && nFrame < nJumpBase + 8)
				nFrame++;
			else
			{
				if (m_fYS <= 0)
					nFrame = nJumpBase + 8;
				else if (nFrame < nJumpBase + 11)
					nFrame++;
				else
					nFrame = nJumpBase + 11;
			}
		}
		break;
	case STATE_FALL:
		{
			//if (nFrame >= SPRIDX_NEWMARIO_JUMP && nFrame < SPRIDX_NEWMARIO_JUMP + 11)
			//	nFrame++;
		}
		break;
	case STATE_DIE:
		if (!bIsBig)
			nFrame = SPRITE_INDEX_NOCYCLE(SPRIDX_NEWMARIO_DIE, 6, 8);
		else
			nFrame = SPRITE_INDEX_NOCYCLE(SPRIDX_NEWMARIO_BIG_DIE, 6, 8);
		break;
	case STATE_FLAGDOWN:
		if (m_fYS != 0)
		{
			if (!bIsBig)
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_CLIMB, 3, 5);
			else
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_BIG_CLIMB, 3, 5);
		}
		else
		{
			if (!bIsBig)
				nFrame = SPRIDX_NEWMARIO_CLIMB;
			else
				nFrame = SPRIDX_NEWMARIO_BIG_CLIMB;
		}
		break;
	case STATE_CLIMBIDLE:	
		if (!bIsBig)
			nFrame = SPRIDX_NEWMARIO_CLIMB;
		else
			nFrame = SPRIDX_NEWMARIO_BIG_CLIMB;
		break;
	case STATE_CLIMBMOVE:
	case STATE_VINEUP:
		{
			if (!bIsBig)
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_CLIMB, 3, 5);
			else
				nFrame = SPRITE_INDEX_CYCLE(SPRIDX_NEWMARIO_BIG_CLIMB, 3, 5);
		}
	default:
		break;
	};

	// Throw Fireball ignore action frames
	switch (m_nState)
	{
	case STATE_STAND:
	case STATE_WALK:
	case STATE_JUMP:
	case STATE_FALL:
		if (m_nThrowFireFrame > 0)
		{
			int nFireFrame = 8 - m_nThrowFireFrame;
			if (nFireFrame > 5)
				nFireFrame = 5;
			nFrame = SPRIDX_NEWMARIO_THROWFIRE + nFireFrame;
			m_nThrowFireFrame--;
		}
	}	

	m_nSpriteIndex = nFrame;

	return nFrame;
}
