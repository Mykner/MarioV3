#pragma once

#include "NaString.h"
#include <map>
#include "PropertyMap.h"

class ObjectPropertyInfo
{
public:
	NaString name;
	void* pRefVariable;
	int vt;
	NaString group;
	int id;
	bool bEnable;
	wchar_t** szTypeNameArray;
};

class ObjectPropertyMapper
{
public:
	ObjectPropertyMapper();
	~ObjectPropertyMapper();

	void Clear();

	void BindControl(CMFCPropertyGridCtrl *pCtrl);
	void InitControl();
	void RefreshControl();
	void SetControlValue(NaString strName, VARIANT *pVar);

	void BindProperty(LPCWSTR szName, void *pRefVariable);

	void AddProperty(LPCWSTR szName, void *pRefVariable, int vt, LPCWSTR szGroup, bool bEnable = true);
	void AddProperties(PropertyObjectBase *pObj);
	void SetPropertyEnable(NaString strName, bool bEnable);

	void SetProperty(NaString strName, VARIANT *pVar);
	void SetPropertyByID(int nID, VARIANT *pVar);
	VARIANT GetProperty(NaString strName);
	VARIANT GetPropertyByID(int nID);
	NaString GetPropertyNameByID(int nID);

	static int GenerateID();

	CMFCPropertyGridCtrl* m_pCtrl;
	std::map<NaString, ObjectPropertyInfo> m_mapProperty;
	static int s_nID;
};

