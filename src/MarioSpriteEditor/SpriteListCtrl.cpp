// SpriteListCtrl.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MarioSpriteEditor.h"
#include "SpriteListCtrl.h"

#include "NaCommon.h"

// CSpriteListCtrl

IMPLEMENT_DYNAMIC(CSpriteListCtrl, CListCtrl)

CSpriteListCtrl::CSpriteListCtrl()
{
	m_pSprite = 0;
	m_hBitmap = 0;
}

CSpriteListCtrl::~CSpriteListCtrl()
{
}


BEGIN_MESSAGE_MAP(CSpriteListCtrl, CListCtrl)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

void CSpriteListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 32;
}

void CSpriteListCtrl::PreSubclassWindow()
{
	ModifyStyle(0, LVS_OWNERDRAWFIXED);

	CListCtrl::PreSubclassWindow();
}

void CSpriteListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Must Set: Owner Draw Fixed 
	LONG prev_left = lpDrawItemStruct->rcItem.left;

	LV_COLUMN column_data;
	memset(&column_data, 0, sizeof(LV_COLUMN));
	column_data.mask = LVCF_WIDTH | LVCF_FMT;

	// 컬럼수 만큼 루프를 돌린다. 
	for(int column_index = 0; GetColumn(column_index, &column_data); column_index++)
	{
		RECT rcItem;
		{
			rcItem.top = lpDrawItemStruct->rcItem.top;
			rcItem.bottom = lpDrawItemStruct->rcItem.bottom;
			rcItem.left = (prev_left);
			rcItem.right = (prev_left += column_data.cx);
		}

		LV_ITEM item_data;
		wchar_t szText[256];
		{
			item_data.mask = LVIF_TEXT | LVIF_PARAM;
			item_data.iItem = lpDrawItemStruct->itemID;
			item_data.iSubItem = column_index;
			item_data.pszText = szText;
			item_data.cchTextMax = sizeof(szText);
			VERIFY(GetItem(&item_data));
		}

		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		{
			// Culumn customize
			switch(column_index)
			{
			case 0:
				if (m_pSprite && m_hBitmap)
				{
					SpriteFrame *f = (SpriteFrame*)lpDrawItemStruct->itemData;

					CDC memdc;
					memdc.CreateCompatibleDC(pDC);
					HGDIOBJ hOldObj = memdc.SelectObject(m_hBitmap);

					pDC->StretchBlt(
						rcItem.left + 1,
						rcItem.top + 1,
						rcItem.right - rcItem.left - 2,
						rcItem.bottom - rcItem.top - 2,
						&memdc,
						f->m_rc.left,
						f->m_rc.top,
						f->m_rc.right - f->m_rc.left,
						f->m_rc.bottom - f->m_rc.top,
						SRCCOPY);

					memdc.SelectObject(hOldObj);
					memdc.DeleteDC();
				}
				break;
			case 1:
				if (lpDrawItemStruct->itemState & ODS_SELECTED)
				{
					pDC->FillSolidRect(&rcItem, GetSysColor(COLOR_HIGHLIGHT));
					pDC->SetTextColor(RGB(0, 0, 0));
					pDC->DrawText(szText, wcslen(szText), &rcItem, DT_LEFT);
				}
				else
				{
					pDC->SetTextColor(RGB(0, 0, 0));
					pDC->DrawText(szText, wcslen(szText), &rcItem, DT_LEFT);
				}
				break;
			}
		}
	}
}
