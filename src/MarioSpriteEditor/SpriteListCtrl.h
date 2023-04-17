#pragma once

#include "Sprite.h"

// CSpriteListCtrl

class CSpriteListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CSpriteListCtrl)

public:
	CSpriteListCtrl();
	virtual ~CSpriteListCtrl();

	Sprite *m_pSprite;
	HBITMAP m_hBitmap;

protected:
	DECLARE_MESSAGE_MAP()
public:

	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	virtual void PreSubclassWindow();

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
};
