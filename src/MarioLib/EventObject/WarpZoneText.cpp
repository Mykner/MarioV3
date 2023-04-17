#include "WarpZoneText.h"

#include "SpriteManager.h"
#include "Camera.h"
#include "Player.h"
#include "RenderManager.h"
#include "Stage.h"
#include "TypeNames.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(WarpZoneText)
	PROP_INT("Text",		VT_ETC_NASTR,	L"",				nullptr, "WarpZoneText"),
	PROP_INT("IsFront",		VT_BOOL,		false,				nullptr, "WarpZoneText"),
	PROP_INT("Shadow",		VT_BOOL,		false,				nullptr, "WarpZoneText"),
	PROP_INT("ActionType",	VT_I4,			TEXTACTION_NONE,	g_szTextActionTypes, "WarpZoneText"),
	//
	// #TODO Z-Order selection, Shadow, MoveAnimation, etc...
	//
END_IMPL_PROPERTYMAP()

WarpZoneText::WarpZoneText(Game *pGame, Stage *pStage) :
	EventObjectBase(pGame, pStage)
{
	m_nType = EVENTOBJECT_WARPZONETEXT;
	m_nState = STATE_IDLE;

	m_bVisible = false;
	
	m_strText = L"";
	m_bIsFront = false;
	m_bShadow = false;
	m_nActionType = TEXTACTION_NONE;
}

WarpZoneText::~WarpZoneText()
{
}

void WarpZoneText::CheckCollisionPlayer()
{
	
}

void WarpZoneText::Process()
{
	if (!m_bVisible)
	{
		NaPointT<float>pt = CAMERA->GetPosition();
		int nPixelPerPage = GameDefaults::nPageWidth;

		int px = (int)((x - 8) / nPixelPerPage) * nPixelPerPage + nPixelPerPage - m_pGame->Width();
		if (px <= pt.x + m_pGame->Width())
		{
			m_bVisible = true;
		}
	}
	else
	{
		switch (m_nActionType)
		{
		case TEXTACTION_MOVEUP:
			m_fY -= 0.1f;
			break;
		}
	}
}

void WarpZoneText::OnBeforeProcess()
{
	CheckCollisionPlayer();
}

void WarpZoneText::Render(int nColor, int nZOrder)
{
	NaPointT<float>pt = CAMERA->GetPosition();
	float _x = x - pt.x - 8;
	float _y = y - pt.y - 16;

	if (m_bVisible)
	{
		if (m_nActionType == TEXTACTION_BLINK)
		{
			if (m_nStateFrame % 48 < 24)
				return;
		}

		if (nZOrder == -1)
		{
			if (m_bIsFront)
				nZOrder = Z_WARPZONE_TEXT;
			else
				nZOrder = Z_BEHIND_MAP;
		}

		RENDER_MANAGER->Text(_x, _y, m_strText, nColor,
			1.0f, nZOrder, m_bShadow);
	}
}

void WarpZoneText::RenderDesign(int nColor, int nZOrder)
{
	NaPointT<float>pt = CAMERA->GetPosition();
	float _x = x - pt.x - 8;
	float _y = y - pt.y - 16;

	int nIndicatorColor = 0xffffffff;
	if (m_strText.GetLength() > 0)
	{
		nIndicatorColor = 0x90ffffff;
	}

	Sprite *pSpriteDesign = SPR_MANAGER->Find(L"Design");
	pSpriteDesign->RenderToQueue(_x, _y, SPRIDX_DESIGN_TEXT_INDICATOR,
		false, false, nIndicatorColor, 1.0f, 1.0f, Z_TEXT);
	
	if (nZOrder == -1)
	{
		if (m_bIsFront)
			nZOrder = Z_WARPZONE_TEXT;
		else
			nZOrder = Z_BEHIND_MAP;
	}

	RENDER_MANAGER->Text(_x, _y, m_strText, nColor,
		1.0f, nZOrder, m_bShadow);
}

bool WarpZoneText::CanPushedUp()
{
	return false;
}

SIZE WarpZoneText::GetSize()
{
	SIZE s = { 16, 16 };
	return s;
}
