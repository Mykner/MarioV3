#include "Vortex.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "Camera.h"

Vortex::Vortex(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_VORTEX;
	m_nState = STATE_IDLE;

	m_nWidth = 1;
}

Vortex::~Vortex()
{
}

void Vortex::OnBeforeProcess()
{
	//CheckCollision();
	CheckCollisionPlayer();
}

void Vortex::OnAfterProcess()
{
	m_fX += m_fXS;
	m_fY += m_fYS;

	m_nStateFrame++;
	if (m_nNextState != -1)
	{
		m_nState = m_nNextState;
		m_nNextState = -1;
		m_nStateFrame = 0;

		OnChangeState(m_nState);
	}
}

void Vortex::Process()
{
	if (m_bCollisionPlayer)
	{
		CUR_PLAYER_OBJ->OnApplyEvent(this);
	}
}

void Vortex::RenderDesign(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	int _x = x - pt.x;
	int _y = y - pt.y;

	SIZE s = GetSize();

	// Vortex is auto-create object by BackgroundUnderWater
}

SIZE Vortex::GetSize()
{
	SIZE s = 
	{ 
		(m_nWidth + 1) * TILE_XS,
		GameDefaults::nPageHeight
	};
	return s;
}
