#pragma once

#include "MapObjectPipe.h"

class Game;
class MapObjectPipeHorz : public MapObjectPipe
{
public:
	MapObjectPipeHorz(Game *pGame, Stage *pStage);
	virtual ~MapObjectPipeHorz();

	virtual bool Tileize(int nPhase = 0);
	virtual void CalcSize();
	virtual SIZE GetSize();
	virtual RECT GetRect();
	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"PipeType", &m_nPipeType },
		{ L"StageName", &m_strStageName },
		{ L"ExitIndex", &m_nExitIndex },
		DECL_PROP("LeftToRight", &m_bLeftToRight)
		DECL_PROP("UseAsExitIndex", &m_nUseAsExitIndex)
	END_DECLARE_PROPERTYMAP()
};

