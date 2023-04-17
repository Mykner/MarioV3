#include "stdafx.h"
#include "MFCPropertyGridPropertyEnum.h"

#include "afxcontrolbarutil.h"
#include "afxpropertygridctrl.h"

#include "afxcolorbar.h"
#include "afxwinappex.h"
#include "afxshellmanager.h"
#include "afxmenuimages.h"
#include "afxvisualmanager.h"
#include "afxtoolbarcomboboxbutton.h"
#include "afxpopupmenu.h"
#include "afxdrawmanager.h"
#include "afxmaskededit.h"
#include "afxspinbuttonctrl.h"
#include "afxtagmanager.h"
#include "afxctrlcontainer.h"

CMFCPropertyGridPropertyEnum::CMFCPropertyGridPropertyEnum(const CString & strGroupName, DWORD_PTR dwData, BOOL bIsValueList) :
	CMFCPropertyGridProperty(strGroupName, dwData, bIsValueList)
{

}

CMFCPropertyGridPropertyEnum::CMFCPropertyGridPropertyEnum(const CString & strName, const COleVariant & varValue, LPCTSTR lpszDescr, DWORD_PTR dwData, LPCTSTR lpszEditMask, LPCTSTR lpszEditTemplate, LPCTSTR lpszValidChars) :
	CMFCPropertyGridProperty(strName, varValue, lpszDescr, dwData, lpszEditMask, lpszEditTemplate, lpszValidChars)
{

}

CMFCPropertyGridPropertyEnum::~CMFCPropertyGridPropertyEnum()
{
}

CString CMFCPropertyGridPropertyEnum::FormatProperty()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	COleVariant& var = m_varValue;

	CString strVal;

	if (m_bIsValueList)
	{
		/*
		for (POSITION pos = m_lstSubItems.GetHeadPosition(); pos != NULL;)
		{
			CMFCPropertyGridProperty* pProp = m_lstSubItems.GetNext(pos);
			ASSERT_VALID(pProp);

			strVal += pProp->FormatProperty();

			if (pos != NULL)
			{
				strVal += m_pWndList->m_cListDelimeter;
				strVal += _T(' ');
			}
		}

		return strVal;
		*/
		return __super::FormatProperty();
	}

	// Only for Integer Format
	switch (var.vt)
	{
	case VT_I4:
	case VT_INT:
		{
			if (m_lstOptions.GetCount() == 0)
				strVal.Format(L"%ld", (long)var.lVal);
			else
			{
				int nIdx = var.intVal;
				if (nIdx >= m_lstOptions.GetCount())
				{
					strVal.Format(L"%d (*** OutOfRange ***)", nIdx);
				}
				else
				{
					POSITION pos = m_lstOptions.GetHeadPosition();
					for (int i = 0; i < nIdx; i++)
					{
						m_lstOptions.GetNext(pos);
					}

					CString strText = m_lstOptions.GetAt(pos);
					strVal.Format(L"%s", strText);
				}
			}
		}
		break;
	default:
		// Unsupported type
		strVal = _T("*** error ***");
	}

	return strVal;
}

void CMFCPropertyGridPropertyEnum::SetValue(const COleVariant & varValue)
{
	ASSERT_VALID(this);

	if (m_varValue.vt != VT_EMPTY && m_varValue.vt != varValue.vt)
	{
		ASSERT(FALSE);
		return;
	}

	BOOL bInPlaceEdit = m_bInPlaceEdit;
	if (bInPlaceEdit)
	{
		OnEndEdit();
	}

	m_varValue = varValue;

	Redraw();

	if (bInPlaceEdit)
	{
		ASSERT_VALID(m_pWndList);
		m_pWndList->EditItem(this);
	}
}
