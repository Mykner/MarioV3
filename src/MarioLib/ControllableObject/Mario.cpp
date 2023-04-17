#include "Mario.h"

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

#include "PathManager.h"
#include "JsonManager.h"

Mario::Mario(Game *pGame, Stage *pStage) :
	ControllableObject(pGame, pStage)
{
	m_nType = PLAYEROBJECT_MARIO;

	RequestSprites(pStage);

	m_nSpriteIndex = SPRIDX_MARIO_STAND;
	m_nSparklingColor = 0xffffffff;
}

Mario::~Mario()
{

}

void Mario::RequestSprites(Stage * pStage)
{
	if (pStage == nullptr)
		pStage = CUR_STAGE;

	/*
	m_pSprite = SPR_MANAGER->Request(L"Mario");

	if (pStage &&
		pStage->m_strGameTheme.GetLength() != 0 &&
		(m_pSprite == nullptr || pStage->m_strGameTheme.CompareNoCase(m_pSprite->m_strGameTheme) == 0))
	{
		m_bAltVersion = true;
		NaDebugOut(L"Alt Version Mario Mode\n");
	}
	else
	{
		m_bAltVersion = false;
		NaDebugOut(L"Original Version Mario Mode\n");
	}
	*/
}

bool Mario::IsSitSpriteIndex()
{
 	if (m_nSpriteIndex == SPRIDX_MARIO_SIT)
 		return true;

	return false;
}

int Mario::GetSpriteIndex()
{
	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
	case STATE_IDLE:
	case STATE_STAND:
		if (m_pCarry)
			nFrame = SPRIDX_MARIO_HOLDRUN1;
		else
			nFrame = SPRIDX_MARIO_STAND;
		break;
	case STATE_SPECIALPOSE:
		if (m_pCarry)
			nFrame = SPRIDX_MARIO_HOLDRUN1;
		else
			nFrame = SPRIDX_MARIO_VICTORY;
		break;
	case STATE_SITJUMP:
	case STATE_SIT:
		nFrame = SPRIDX_MARIO_SIT;
		break;
	case STATE_WALK:
		if (m_pCarry)
			nFrame = SPRIDX_MARIO_HOLDRUN1 + (m_nWalkFrame % 4);
 		else if (abs(m_fXS) >= 4.0f)
 			nFrame = (SPRIDX_MARIO_RUN1 + (m_nWalkFrame % 3));
 		else
			nFrame = (SPRIDX_MARIO_WALK1 + (m_nWalkFrame % 3));
		break;
	case STATE_SHELLRUN:
 		if (m_fXS != 0 && m_fYS == 0 && m_nStateFrame < 16)
 			nFrame = SPRIDX_MARIO_SHELLSLIDE;
 		else
 			nFrame = SPRIDX_MARIO_SHELLRUN1 + (m_nWalkFrame % 3);
		break;
	case STATE_SHELLIDLE:
 		nFrame = SPRIDX_MARIO_SHELLRUN1;
		break;
	case STATE_BLOCKJUMP:
		if (m_nStateFrame < GameDefaults::nBlockJumpFreeze)
		{
			if (m_pCarry)
				nFrame = SPRIDX_MARIO_HOLDRUN2 + (m_nWalkFrame % 4);
			else
				nFrame = (SPRIDX_MARIO_WALK1 + (m_nWalkFrame % 3));
		}
		else
		{
			if (m_pCarry)
				nFrame = SPRIDX_MARIO_HOLDRUN3;
			else
				nFrame = SPRIDX_MARIO_JUMP;
		}
		break;
	case STATE_SPINJUMP:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIO_DRILJUMP1, 4, 2);
		break;
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
		if (m_nStateFrame % 5 == 0)
			m_nWalkFrame++;
		if (m_pCarry)
			nFrame = SPRIDX_MARIO_HOLDRUN1 + (m_nWalkFrame % 4);
		else
			nFrame = (SPRIDX_MARIO_WALK1 + (m_nWalkFrame % 3));
		break;
	case STATE_BREAK:
		if (m_pCarry)
			nFrame = SPRIDX_MARIO_HOLDRUN3;
		else
			nFrame = SPRIDX_MARIO_BREAK;
		break;
	case STATE_GOTOFFVEHICLE:
	case STATE_JUMP:
	case STATE_PRESSJUMP:
		if (m_pCarry)
		{
			if (m_nShape == SHAPE_MINI)
				nFrame = SPRIDX_MARIO_HOLDRUN4;
			else
				nFrame = SPRIDX_MARIO_HOLDRUN2;
		}
		else
			nFrame = SPRIDX_MARIO_JUMP;
		break;
	case STATE_FALL:
		if (m_nNextShape == SHAPE_COSTUME)
			nFrame = SPRIDX_MARIO_JUMP;
		else if (nFrame > SPRIDX_MARIO_VICTORY)
			nFrame = SPRIDX_MARIO_JUMP;
		break;
	case STATE_TRAMPOLINJUMP:
		{
			if (nFrame < SPRIDX_MARIO_WALK1 || nFrame > SPRIDX_MARIO_WALK3)
			{
				if (m_pCarry)
					nFrame = SPRIDX_MARIO_HOLDRUN2;
				else
					nFrame = SPRIDX_MARIO_WALK1;
			}
		}
		break;
	case STATE_DIE:
		nFrame = SPRIDX_MARIO_DIE;
		break;
	case STATE_FLAGDOWN:
		if (m_fYS == 0)
			nFrame = SPRIDX_MARIO_CLIMB2;
		else if (m_nStateFrame % 10 < 5)
			nFrame = SPRIDX_MARIO_CLIMB1;
		else
			nFrame = SPRIDX_MARIO_CLIMB2;
		break;
	case STATE_CLIMBIDLE:
		nFrame = SPRIDX_MARIO_CLIMB2;
		break;
	case STATE_CLIMBMOVE:
	case STATE_VINEUP:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIO_CLIMB1, 2, 5);
		break;
	case STATE_SWIM:
		if (m_fYS > 0 || m_pCarry)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIO_SWIM1, 2, 4);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIO_SWIM1, 6, 4);
		break;
	case STATE_SWIMJUMP:
		break;
	case STATE_RIDE:
		if (m_pVehicle)
		{
			int nRideType = ((VehicleObjectBase*)m_pVehicle)->m_nRideType;
			switch (nRideType)
			{
			case STATE_STAND:
				nFrame = SPRIDX_MARIO_STAND;
				break;
			case STATE_RIDE:
				nFrame = SPRIDX_MARIO_CLIMB2;
				break;
			case STATE_CROUCH:
				nFrame = SPRIDX_MARIO_HEAD;
				break;
			}
		}
		else
			nFrame = SPRIDX_MARIO_STAND;
		break;
	}

	// Throw Fireball ignore action frames
	if (m_nThrowFireFrame > 0)
	{
		switch (m_nState)
		{
		case STATE_STAND:
		case STATE_WALK:
		case STATE_JUMP:
		case STATE_FALL:
 			nFrame = SPRIDX_MARIO_THROWFIRE;
			break;
		case STATE_RIDE:
			{
				switch (m_pVehicle->m_nRideType)
				{
				case STATE_CROUCH:
					break;
				default:
 					nFrame = SPRIDX_MARIO_THROWFIRE;
					break;
				}
			}
			break;
		}
		m_nThrowFireFrame--;
	}

	if (m_nKickFrame > 0)
	{
		if (m_nShape == SHAPE_MINI)
			nFrame = SPRIDX_MARIO_HOLDRUN4;
		else
			nFrame = SPRIDX_MARIO_HOLDRUN2;
		m_nKickFrame--;
	}

 	 m_nSpriteIndex = nFrame;

	return m_nSpriteIndex;
}

int Mario::GetMiniSpriteIndex()
{
	return 0;
}

int Mario::GetBigSpriteIndex()
{
	return 0;
}

int Mario::GetShellSpriteIndex()
{
	return 0;
}

SIZE Mario::GetSize()
{
	SIZE s;
	s.cx = 12;
	s.cy = 16;

	switch (m_nSpriteIndex)
	{
	case SPRIDX_MARIO_TO_GIANT3:
		if (m_nNextShape == SHAPE_GIANT)
			s.cy = 31;

		s.cx *= m_fScaleX;
		s.cy *= m_fScaleY;
		return s;
		break;
	case SPRIDX_MARIO_SIT:
		s.cy = 16;

		s.cx *= m_fScaleX;
		s.cy *= m_fScaleY;
		return s;
	case SPRIDX_MARIO_HEAD:
		if (m_nShape == SHAPE_MINI)
			s.cy = 7;
		else
			s.cy = 10;
		return s;
		break;
	}

	return ControllableObject::GetSize();
}

int Mario::GetColor()
{
	int nColor = 0xffffffff;
	bool bSparkling = false;
	bool bChangeSparklingColor = false;

	if (m_nNextShape == SHAPE_FIRE ||
		m_nNextShape == SHAPE_GIANT ||
		(m_nShape == SHAPE_FIRE && m_nNextShape != -1))
	{
		switch (m_pGame->m_pGameState->m_nStateFrame % 4)
		{
		case 0:
		case 2:
			break;
		case 1:
			bChangeSparklingColor = true;
		case 3:
			bSparkling = true;
			break;
		}
	}

	if (m_nInvinsibleTime > 0)
	{
		// normal, white+green, white+red, normal+black
		if (m_nInvinsibleTime < 8 * 24)
			bChangeSparklingColor = ((m_nInvinsibleTime % 4) == 0);
		else
			bChangeSparklingColor = ((m_nInvinsibleTime % 2) == 0);

		bSparkling = true;
	}

	if (bSparkling)
	{
		if (bChangeSparklingColor)
		{
			nColor = 0xa0aaaaaa;
			switch (rand() % 4)
			{
			case 0: nColor |= 0x0000ffff; break;
			case 1: nColor |= 0x00ff00ff; break;
			case 2: nColor |= 0x00ffff00; break;
			//case 3: nColor = 0x70000000; break;
			}

			m_nSparklingColor = nColor;
		}

		nColor = m_nSparklingColor;
	}

	return nColor;
}

bool Mario::CanSit()
{
	if (m_pCarry)
		return false;

	if (m_pGame->IsActiveAbility(ABILITY_AUTORUN))
		return false;

	if (m_nShape > SHAPE_MINI)
		return true;

	return false;
}

void Mario::Render(int nColor, int nZOrder)
{
	if (m_nShape == SHAPE_COSTUME)
	{
		RenderCostume(nColor, nZOrder);
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

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	_x = _x - ptCamera.x;
	_y = y - ptCamera.y;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	bool bShadow = (nColor == -1) && IS_MODERN;
	int nSpriteIdx = GetSpriteIndex();
	
	int nMaskColor = 0xffffffff;
	if (nColor != -1 || nColor != 0)
		nMaskColor = GetColor();

	if (CheckSkipRender())
		nColor &= SpriteDefaults::dwColorSkipFilter;
	
	if (nZOrder == -1)
		nZOrder = GetZOrder();

	if (m_nState == STATE_PARALYZE && m_nStateFrame % 2 == 1)
		_x += 1;

	Sprite *pSprite = nullptr;
	Sprite *pSpriteMask = nullptr;
	GetSprite(&pSprite, &pSpriteMask);

	pSprite->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, bShadow);

	if (nMaskColor != 0xffffffff)
		pSpriteMask->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nMaskColor, fScaleX, fScaleY, nZOrder, false);

	// For test HitBox
	/*
	UIBase u(m_pGame);
	NaRect rc = GetRect();
	rc.Offset(-ptCamera.x, -ptCamera.y);

	u.RenderBox(rc, 0x90ff0000);
	//*/

	/*
	SpeechBubble b(m_pGame);
	NaString str;
	str.Format(L"Mario");
	b.RenderWithTextAutoSized(_x, _y - GetSize().cy - 4, str);
	//*/
}

void Mario::RenderShapeChange()
{
	NaPointT<float> pt = CAMERA->GetPosition();

	int nFrame = m_pGame->GetStateFrame();
	int nSpriteIdx = 0;
	Sprite *pSprite = nullptr;
	Sprite *pSpriteMask = nullptr;

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	if (m_nShape == SHAPE_COSTUME || m_nNextShape == SHAPE_COSTUME)
	{
		RenderCostumeShapeChange();
		return;
	}
	else if (m_nNextShape == SHAPE_MINI)
	{
		if (nFrame % 2 == 1)
			return;

		GetBigToMiniSpriteIndex(&pSprite, &pSpriteMask, &nSpriteIdx, &fScaleX, &fScaleY);
	}	
	else if (m_nNextShape == SHAPE_GIANT)
	{
		GetAnyToGiantSpriteIndex(&pSprite, &pSpriteMask, &nSpriteIdx, &fScaleX, &fScaleY);
	}
	else if (m_nShape == SHAPE_MINI)
	{
		GetMiniToBigSpriteIndex(&pSprite, &pSpriteMask, &nSpriteIdx, &fScaleX, &fScaleY);
	}	
	else if ((m_nShape != SHAPE_BIG) && (m_nNextShape != SHAPE_BIG))
	{
		nSpriteIdx = GetSpriteIndex();

		if (nFrame < 8)
			GetSprite(&pSprite, &pSpriteMask, m_nShape);
		else 
		{
			if (nFrame % 8 < 4)
				GetSprite(&pSprite, &pSpriteMask, m_nShape);
			else
				GetSprite(&pSprite, &pSpriteMask, m_nNextShape);

			if (nFrame % 8 < 4)
				fScaleY *= (0.7f + ((float)nFrame / 60) * 0.3f);
		}
	}
	else
	{
		nSpriteIdx = GetSpriteIndex();

		if (nFrame < 8)
			GetSprite(&pSprite, &pSpriteMask, m_nShape);
		else
		{
			if (nFrame % 8 < 4)
				GetSprite(&pSprite, &pSpriteMask, m_nShape);
			else
				GetSprite(&pSprite, &pSpriteMask, m_nNextShape);
		}
	}

	m_nSpriteIndex = nSpriteIdx;

	int nColor = GetColor();
	int nZOrder = GetZOrder();
	float _x = x - pt.x;
	float _y = y - pt.y;	
	bool bShadow = m_pGame->m_bModernStyle;
	
	pSprite->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, 0xffffffff, fScaleX, fScaleY, nZOrder, bShadow);

	if (nColor != 0xffffffff)
		pSpriteMask->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColor, fScaleX, fScaleY, nZOrder, false);
}

bool Mario::CheckSkipRender()
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

void Mario::GetMiniToBigSpriteIndex(Sprite **pSprite, Sprite** pSpriteMask, int * pnIndex, float * pfScaleX, float * pfScaleY)
{
	// Total 60 frame
	int nFrame = m_pGame->GetStateFrame();
	if (m_pGame->m_bModernStyle)
	{
		*pnIndex = GetSpriteIndex();

		if (nFrame >= 8)
		{
			GetSprite(pSprite, pSpriteMask, m_nNextShape);

			if (nFrame % 8 < 4)
				*pfScaleY *= (0.7f + ((float)nFrame / 60) * 0.3f);
			else
				*pfScaleY *= (0.8f + ((float)nFrame / 60) * 0.2f);
		}
		else
		{
			GetSprite(pSprite, pSpriteMask, SHAPE_MINI);
		}
	}
	else
	{
		if (nFrame < 8)
		{
			GetSprite(pSprite, pSpriteMask, SHAPE_MINI);
			*pnIndex = SPRIDX_MARIO_STAND;
		}
		else if (nFrame < 42)
		{
			if (nFrame % 8 < 4)
			{
				GetSprite(pSprite, pSpriteMask, SHAPE_BIG);
				*pnIndex = SPRIDX_MARIO_MEDIUM;
			}
			else
			{
				GetSprite(pSprite, pSpriteMask, SHAPE_MINI);
				*pnIndex = SPRIDX_MARIO_STAND;
			}
		}
		else
		{
			GetSprite(pSprite, pSpriteMask, SHAPE_BIG);
			*pnIndex = SPRIDX_MARIO_STAND;
		}
	}
}

void Mario::GetBigToMiniSpriteIndex(Sprite **pSprite, Sprite** pSpriteMask, int * pnIndex, float * pfScaleX, float * pfScaleY)
{
	// Total 60 frame
	int nFrame = m_pGame->GetStateFrame();
	if (m_pGame->m_bModernStyle)
	{
		GetSprite(pSprite, pSpriteMask, m_nShape);
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
		{
			GetSprite(pSprite, pSpriteMask, SHAPE_BIG);
			*pnIndex = SPRIDX_MARIO_JUMP;
		}
		else
		{
			if (nFrame % 8 < 4)
			{
				GetSprite(pSprite, pSpriteMask, SHAPE_MINI);
				*pnIndex = SPRIDX_MARIO_SWIM2;
			}
			else
			{
				GetSprite(pSprite, pSpriteMask, SHAPE_BIG);
				*pnIndex = SPRIDX_MARIO_SWIM2;
			}
		}
	}
}

void Mario::GetAnyToGiantSpriteIndex(Sprite **pSprite, Sprite** pSpriteMask, int * pnIndex, float * pfScaleX, float * pfScaleY)
{
	int nFrame = m_pGame->GetStateFrame();

	GetSprite(pSprite, pSpriteMask, SHAPE_GIANT);
	if (nFrame < 35)
		*pnIndex = SPRIDX_MARIO_TO_GIANT1;
	else if (nFrame < 43)
		*pnIndex = SPRIDX_MARIO_TO_GIANT2;
	else
		*pnIndex = SPRIDX_MARIO_TO_GIANT3;
}

int Mario::GetCostumeSpriteIndex()
{
	int nFrame = m_nSpriteIndex;
	switch (m_nState)
	{
	case STATE_PIPEDOWN:
	case STATE_PIPEUP:
	case STATE_IDLE:
	case STATE_STAND:
		nFrame = SPRIDX_MARIOCOS_STAND;
		break;
	case STATE_SPECIALPOSE:
		nFrame = SPRIDX_MARIOCOS_VICTORY;
		break;
	case STATE_SITJUMP:
	//case STATE_SITFALL:	
	case STATE_SIT:
		nFrame = SPRIDX_MARIOCOS_SIT;
		break;
	case STATE_WALK:
		if (abs(m_fXS) >= 3.0f)
			nFrame = (SPRIDX_MARIOCOS_RUN1 + m_nWalkFrame % 3);
		else
			nFrame = (SPRIDX_MARIOCOS_WALK1 + m_nWalkFrame % 3);
		break;
	case STATE_BLOCKJUMP:
		if (m_nStateFrame < GameDefaults::nBlockJumpFreeze)
			nFrame = (SPRIDX_MARIOCOS_WALK1 + m_nWalkFrame % 3);
		else
			nFrame = SPRIDX_MARIOCOS_JUMP;
		break;
	case STATE_PIPERIGHT:
	case STATE_PIPELEFT:
		if (m_nStateFrame % 5 == 0)
			m_nWalkFrame++;
		if (m_nWalkFrame > 2)
			m_nWalkFrame = 0;
		nFrame = (SPRIDX_MARIOCOS_WALK1 + m_nWalkFrame % 3);
		break;
	case STATE_BREAK:
		nFrame = SPRIDX_MARIOCOS_BREAK;
		break;
	case STATE_GOTOFFVEHICLE:
	case STATE_JUMP:
	case STATE_PRESSJUMP:
		nFrame = SPRIDX_MARIOCOS_JUMP;
		break;
	case STATE_FALL:
		nFrame = SPRIDX_MARIOCOS_FALL;
		break;
	case STATE_TRAMPOLINJUMP:
		{
			if (nFrame < SPRIDX_MARIOCOS_WALK1 || nFrame > SPRIDX_MARIOCOS_WALK3)
				nFrame = SPRIDX_MARIOCOS_WALK1;
		}
		break;
	case STATE_DIE:
		nFrame = -1;
		break;
	case STATE_FLAGDOWN:
		if (m_fYS == 0)
			nFrame = SPRIDX_MARIOCOS_FLAGCLIMB2;
		else if (m_nStateFrame % 10 < 5)
			nFrame = SPRIDX_MARIOCOS_FLAGCLIMB1;
		else
			nFrame = SPRIDX_MARIOCOS_FLAGCLIMB2;
		break;
	case STATE_CLIMBIDLE:
		nFrame = SPRIDX_MARIOCOS_CLIMB2;
		break;
	case STATE_CLIMBMOVE:
	case STATE_VINEUP:
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOCOS_CLIMB1, 2, 5);
		break;
	case STATE_SWIM:
		if (m_fYS > 0)
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOCOS_SWIM1, 2, 4);
		else
			nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOCOS_SWIM1, 6, 4);
		break;
	case STATE_SWIMJUMP:
		// #TODO improvement
		nFrame = SPRITE_INDEX_CYCLE(SPRIDX_MARIOCOS_SWIM1, 6, 4);
		break;
	case STATE_RIDE:
		if (m_pVehicle)
		{
			int nRideType = ((VehicleObjectBase*)m_pVehicle)->m_nRideType;
			switch (nRideType)
			{
			case STATE_STAND:
				nFrame = SPRIDX_MARIOCOS_STAND;
				break;
			case STATE_RIDE:
				nFrame = SPRIDX_MARIOCOS_CLIMB2;
				break;
			case STATE_CROUCH:
				nFrame = SPRIDX_MARIOCOS_HEAD;
				break;
			}
		}
		else
			nFrame = SPRIDX_MARIOCOS_STAND;
		break;
	default:
		NaDebugOut(L"Missing SpriteIndex: %ls\n", GetStateName());
		nFrame = SPRIDX_MARIOCOS_JUMP;
		break;
	}

	m_nSpriteIndex = nFrame;

	return m_nSpriteIndex;
}

void Mario::RenderCostume(int nColor, int nZOrder)
{
	float _x = x, _y;
	if (m_pClimb)
	{
		if (!m_bFlip)
			_x = m_pClimb->m_fX - 6;
		else
			_x = m_pClimb->m_fX + 6;
	}

	NaPointT<float> ptCamera = CAMERA->GetPosition();
	_x = _x - ptCamera.x;
	_y = y - ptCamera.y + 1; // +1 : Temporary

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	Sprite* pSprite = GetCostumeSprite();
	int nSpriteIdx = GetCostumeSpriteIndex();
	float fAngle = 0.0f;

	// Sonic's jump must spin.
	if (m_nCostume == 89 && 
		(m_nState == STATE_JUMP || m_nState == STATE_FALL))
	{
		fAngle = m_nStateFrame * 30.0f;
	}

	if (nColor == -1)
	{
		if (m_nInvinsibleTime > 0)
		{
			int nFreq = 4;
			if (m_nInvinsibleTime < 8 * 24)
				nFreq = 8;

			if (m_nInvinsibleTime % nFreq < nFreq / 2)
				nColor = COLOR_XRGB(0, 0, 0);
			else
				nColor = COLOR_XRGB(255, 255, 255);
		}
	}

	if (CheckSkipRender())
	{
		nColor &= SpriteDefaults::dwColorSkipFilter;
	}

	if (nZOrder == -1)
		nZOrder = GetZOrder();
	bool bShadow = m_pGame->m_bModernStyle;

	if (m_nState == STATE_PARALYZE && m_nStateFrame % 2 == 1)
		_x += 1;

	pSprite->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, nColor,
		fScaleX, fScaleY, nZOrder, bShadow, fAngle);
}

void Mario::RenderCostumeShapeChange()
{
	NaPointT<float> pt = CAMERA->GetPosition();

	int nFrame = m_pGame->GetStateFrame();
	int nSpriteIdx = GetSpriteIndex();

	float fScaleX, fScaleY;
	GetScale(fScaleX, fScaleY);

	bool bCostume = false;
	int nNonCostumeShape = SHAPE_MINI;

	if (m_nNextShape == SHAPE_COSTUME)
	{
		nNonCostumeShape = m_nShape;

		// Any -> Costume
		if (nFrame < 8)
			bCostume = false;
		else
		{
			if (nFrame % 8 < 4)
			{
				if (nNonCostumeShape != SHAPE_MINI && nNonCostumeShape != SHAPE_COSTUME)
					fScaleY *= (0.9f - ((float)nFrame / 60) * 0.3f);
				else
					fScaleY *= (1.0f + ((float)nFrame / 60) * 0.2f);
				bCostume = false;
			}
			else
			{
				bCostume = true;
			}
		}
	}
	else if (m_nShape == SHAPE_COSTUME)
	{
		nNonCostumeShape = m_nNextShape;

		// Costume -> Mini
		if (nFrame < 8)
			bCostume = true;
		else
		{
			if (nFrame % 8 < 4)
			{
				if (nNonCostumeShape != SHAPE_MINI && nNonCostumeShape != SHAPE_COSTUME)
				{
					// Costume -> Big or Fire or Shell
					fScaleY *= (0.7f + ((float)nFrame / 60) * 0.3f);
				}
				else
				{
					// Costume ->Big or Costume
					fScaleY *= (1.0f + ((float)nFrame / 60) * 0.2f);
				}
				bCostume = false;
			}
			else
			{
				bCostume = true;
			}
		}

		if (bCostume)
		{
			nNonCostumeShape = SHAPE_MINI;
			bCostume = false;
		}
	}

	int nZOrder = GetZOrder();
	int _x = x - pt.x;
	int _y = y - pt.y;
	bool bShadow = m_pGame->m_bModernStyle;

	if (bCostume)
	{
		Sprite *pSprite = GetCostumeSprite();
		nSpriteIdx = GetCostumeSpriteIndex();

		pSprite->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, 0xffffffff, fScaleX, fScaleY, nZOrder, bShadow);
	}
	else
	{
		nSpriteIdx = GetSpriteIndex();

		Sprite *pSprite = nullptr;
		Sprite *pSpriteMask = nullptr;
		GetSprite(&pSprite, &pSpriteMask, nNonCostumeShape);

		pSprite->RenderToQueue(_x, _y, nSpriteIdx, m_bFlip, m_bFlipV, 0xffffffff, fScaleX, fScaleY, nZOrder, bShadow);
	}
}

void Mario::GetSprite(Sprite** pSprite, Sprite** pSpriteMask, int nShape)
{
	if (nShape == -1)
		nShape = m_nShape;

	switch (nShape)
	{
	case SHAPE_SHELL:
		*pSprite = SPR_MANAGER->Request(L"MarioShell");
		*pSpriteMask = SPR_MANAGER->Request(L"MarioShell", L"MarioShellMask");
		break;
	case SHAPE_FIRE:
		*pSprite = SPR_MANAGER->Request(L"MarioFire");
		*pSpriteMask = SPR_MANAGER->Request(L"MarioFire", L"MarioFireMask");
		break;
	case SHAPE_GIANT:
	case SHAPE_BIG:
		*pSprite = SPR_MANAGER->Request(L"MarioBig");
		*pSpriteMask = SPR_MANAGER->Request(L"MarioBig", L"MarioBigMask");
		break;
	case SHAPE_MINI:
	default:
		*pSprite = SPR_MANAGER->Request(L"MarioSmall");
		*pSpriteMask = SPR_MANAGER->Request(L"MarioSmall", L"MarioSmallMask");
		break;
	}
}
