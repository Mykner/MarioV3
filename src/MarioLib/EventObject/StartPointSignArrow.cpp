#include "StartPointSignArrow.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"

#include "SpeechBubble.h"

StartPointSignArrow::StartPointSignArrow(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_STARTPOINT_SIGN;
	m_nState = STATE_IDLE;
}

StartPointSignArrow::~StartPointSignArrow()
{
}

void StartPointSignArrow::Process()
{
}

void StartPointSignArrow::OnBeforeProcess()
{	
}

void StartPointSignArrow::OnAfterProcess()
{	
}

void StartPointSignArrow::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = false;

	(*m_ppSprite)->RenderToQueue(_x, _y, SPRIDX_MAPOBJ_STARTPOINT_SIGNARROW,
		false, false, nColor, 1.0f, 1.0f, Z_MAP, bShadow);
}

SIZE StartPointSignArrow::GetSize()
{
	SIZE s = { 16, 16 };
	return s;
}
