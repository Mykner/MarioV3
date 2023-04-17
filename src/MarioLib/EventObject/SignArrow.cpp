#include "SignArrow.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "FModManager.h"
#include "Stage.h"
#include "TypeNames.h"

#include "SpeechBubble.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(SignArrow)
	PROP_INT("SignDirection", VT_I4, SIGNDIR_R,	nullptr, "SignArrow"),
END_IMPL_PROPERTYMAP()

SignArrow::SignArrow(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_SIGN;
	m_nState = STATE_IDLE;

	m_nSignDirection = SIGNDIR_R;
}

SignArrow::~SignArrow()
{
}

void SignArrow::Process()
{
}

void SignArrow::OnBeforeProcess()
{	
}

void SignArrow::OnAfterProcess()
{	
}

void SignArrow::Render(int nColor, int nZOrder)
{
	NaPointT<float> pt = CAMERA->GetPosition();
	float _x = x - pt.x;
	float _y = y - pt.y;
	bool bShadow = false;
	float fAngle = (m_nSignDirection) * (45.0f);

	(*m_ppSprite)->RenderToQueue(_x, _y, SPRIDX_MAPOBJ_SIGNARROW,
		false, false, nColor, 1.0f, 1.0f, Z_MAP, bShadow, fAngle);
}

SIZE SignArrow::GetSize()
{
	SIZE s = { 16, 16 };
	return s;
}
