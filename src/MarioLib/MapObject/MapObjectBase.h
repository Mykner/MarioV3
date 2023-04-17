#pragma once

#include "PropertyMap.h"

#include "Common.h"
#include "Types.h"
#include "SpriteIndex.h"

class Game;
class Sprite;
class Stage;
class MapObjectBase : public PropertyObjectBase
{
public:
	MapObjectBase(Game *pGame, Stage *pStage);
	virtual ~MapObjectBase();

	bool HitTest(int x, int y);
	virtual bool Tileize(int nPhase = 0) { return true; };
	virtual void Install() {};

	virtual void CalcSize() {};
	virtual SIZE GetSize();
	virtual RECT GetRect();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1) {};

	static bool IsSingletonType(int nType);
	static MapObjectBase* CreateInstance(Game *pGame, Stage *pStage, int nType);

	union 
	{
		int x;
		int m_nX;
	};
	union 
	{
		int y;
		int m_nY;
	};

	int m_nTX;
	int m_nTY;
	
	int m_nType;
	NaString m_strOption; // will be deprecate!

	Game *m_pGame;
	Stage *m_pStage;
	Sprite *m_pSprite;
	Sprite **m_ppSprite;
};

