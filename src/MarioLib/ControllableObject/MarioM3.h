#pragma once

#include "Common.h"
#include "ControllableObject.h"

enum SpriteIndexMarioM3
{
	SPRIDX_MARIOM3_DIE = 0,
	SPRIDX_MARIOM3_STAND, // 1
	SPRIDX_MARIOM3_WALK1,
	SPRIDX_MARIOM3_BREAK,
	SPRIDX_MARIOM3_JUMP,
	SPRIDX_MARIOM3_SIT,
	SPRIDX_MARIOM3_CLIMB1,
	SPRIDX_MARIOM3_CLIMB2,
	SPRIDX_MARIOM3_SWIM1,
	SPRIDX_MARIOM3_SWIM2,
	SPRIDX_MARIOM3_SWIM3,
	SPRIDX_MARIOM3_SWIM4,
	SPRIDX_MARIOM3_SWIM5,
	SPRIDX_MARIOM3_RUN1,
	SPRIDX_MARIOM3_RUN2,
	SPRIDX_MARIOM3_HOLDRUN1,
	SPRIDX_MARIOM3_HOLDRUN2,

	SPRIDX_MARIOM3_BIG_STAND, // 28
	SPRIDX_MARIOM3_BIG_WALK1,
	SPRIDX_MARIOM3_BIG_WALK2,
	SPRIDX_MARIOM3_BIG_BREAK,
	SPRIDX_MARIOM3_BIG_JUMP,
	SPRIDX_MARIOM3_BIG_SIT,
	SPRIDX_MARIOM3_BIG_CLIMB1,
	SPRIDX_MARIOM3_BIG_CLIMB2,
	SPRIDX_MARIOM3_BIG_SWIM1,
	SPRIDX_MARIOM3_BIG_SWIM2,
	SPRIDX_MARIOM3_BIG_SWIM3,
	SPRIDX_MARIOM3_BIG_SWIM4,
	SPRIDX_MARIOM3_BIG_SWIM5,
	SPRIDX_MARIOM3_BIG_RUN1,
	SPRIDX_MARIOM3_BIG_RUN2,
	SPRIDX_MARIOM3_BIG_RUN3,
	SPRIDX_MARIOM3_BIG_HOLDRUN1,
	SPRIDX_MARIOM3_BIG_HOLDRUN2,
	SPRIDX_MARIOM3_BIG_HOLDRUN3,

	// Special action sprite
	SPRIDX_MARIOM3_BIG_THROWFIRE,
	SPRIDX_MARIOM3_ROLLINGJUMP,

	SPRIDX_MARIOM3_
};

class Game;
class MarioM3 : public ControllableObject
{
public:
	MarioM3(Game *pGame, Stage *pStage);
	virtual ~MarioM3();

	virtual int GetSpriteIndex();
	virtual void GetColor(int * pnColor);

	virtual bool CanSit();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderShapeChange();

	int CheckValidSpriteIndex(int nIndex, int nShape = -1);
	bool CheckSkipRender();

	void GetMiniToBigSpriteIndex(int *pnIndex, float *pfScaleX, float *pfScaleY);
	void GetBigToMiniSpriteIndex(int *pnIndex, float *pfScaleX, float *pfScaleY);
	void GetAnyToGiantSpriteIndex(int *pnIndex, float *pfScaleX, float *pfScaleY);
	
	Sprite *m_pSpriteBody;
	Sprite *m_pSpriteTunic;
	Sprite *m_pSpritePants;
};