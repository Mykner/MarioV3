#pragma once

#include "Common.h"
#include "Mario.h"

enum SpriteIndexNewMario
{
	SPRIDX_NEWMARIO_IDLE = 0, // ~15, 72~87
	SPRIDX_NEWMARIO_WALK = 16, // ~25, 88~98
	SPRIDX_NEWMARIO_RUN = 26, // ~46, 99~123
	SPRIDX_NEWMARIO_BREAK = 47, // , 124
	SPRIDX_NEWMARIO_DIE = 48, // ~54, 125~131
	SPRIDX_NEWMARIO_JUMP = 55, // ~66, 132~148
	SPRIDX_NEWMARIO_CLIMB = 67, // ~69, 149~151
	SPRIDX_NEWMARIO_SIT = 70, // ~71, 152~153

	SPRIDX_NEWMARIO_BIG_IDLE = 72, // ~15, 72~87
	SPRIDX_NEWMARIO_BIG_WALK = 88, // ~25, 88~98
	SPRIDX_NEWMARIO_BIG_RUN = 99, // ~46, 99~123
	SPRIDX_NEWMARIO_BIG_BREAK = 124, // , 124
	SPRIDX_NEWMARIO_BIG_DIE = 125, // ~54, 125~131
	SPRIDX_NEWMARIO_BIG_JUMP = 132, // ~66, 132~148
	SPRIDX_NEWMARIO_BIG_CLIMB = 149, // ~69, 149~151
	SPRIDX_NEWMARIO_BIG_SIT = 152, // ~71, 152~153

	SPRIDX_NEWMARIO_THROWFIRE = 154, // ~159

	SPRIDX_NEWMARIO_
};

class Game;
class NewMario : public Mario
{
public:
	NewMario(Game *pGame, Stage *pStage);
	virtual ~NewMario();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderShapeChange();

	virtual bool CanSit();

	virtual void GetColor(int* pnColor);
	virtual int GetSpriteIndex();

	Sprite *m_pSpriteBody;
	Sprite *m_pSpritePants;
	Sprite *m_pSpriteTunic;
};

