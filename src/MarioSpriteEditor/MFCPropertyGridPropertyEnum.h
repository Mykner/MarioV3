#pragma once

#include "afxpropertygridctrl.h"

class CMFCPropertyGridPropertyEnum :
	public CMFCPropertyGridProperty
{
public:
	// Group constructor
	CMFCPropertyGridPropertyEnum(const CString& strGroupName, DWORD_PTR dwData = 0, BOOL bIsValueList = FALSE);

	// Simple property
	CMFCPropertyGridPropertyEnum(const CString& strName, const COleVariant& varValue, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0,
		LPCTSTR lpszEditMask = NULL, LPCTSTR lpszEditTemplate = NULL, LPCTSTR lpszValidChars = NULL);

	virtual ~CMFCPropertyGridPropertyEnum();

	virtual CString FormatProperty();
	virtual void SetValue(const COleVariant& varValue);
};

