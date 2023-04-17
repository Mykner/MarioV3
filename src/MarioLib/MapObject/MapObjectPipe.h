#pragma once

#include "MapObjectBase.h"

#include "PropertyMap.h"

class Game;
class Sprite;
class MapObjectPipe : public MapObjectBase
{
public:
	MapObjectPipe(Game *pGame, Stage *pStage);
	virtual ~MapObjectPipe();

	virtual bool Tileize(int nPhase = 0);

	void TileizeUpToDown(int nPhase);
	void TileizeDownToUp(int nPhase);
	void TileizeLeftToRight(int nPhase);
	void TileizeRightToLeft(int nPhase);

	bool IsEmptyTile(int x, int y);

	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual RECT GetRect();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	
	void RenderUpToDown(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderDownToUp(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderLeftToRight(int nColor = 0xffffffff, int nZOrder = -1);
	void RenderRightToLeft(int nColor = 0xffffffff, int nZOrder = -1);

	MapObjectBase* FindExit(int nIndex);
	NaRect GetEntranceRect();

	int GetPipeDir(MapObjectPipe * pPipe);

	SIZE m_sizeHead;
	SIZE m_sizeBody;
	Sprite* m_pSpriteDesign;

	int m_nPipeType;
	int m_nExitIndex;
	NaString m_strStageName;
	union 
	{
		// Deprecated
		bool m_bUpToDown;
		bool m_bLeftToRight;
		bool m_bUseNewStyle;
	};

	int m_nUseAsExitIndex;
	int m_nDirection;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"PipeType", &m_nPipeType },
		{ L"StageName", &m_strStageName },
		{ L"ExitIndex", &m_nExitIndex },
		DECL_PROP("UpToDown", &m_bUpToDown)
		DECL_PROP("UseAsExitIndex", &m_nUseAsExitIndex)
		DECL_PROP("Direction", &m_nDirection)
	END_DECLARE_PROPERTYMAP()
};

