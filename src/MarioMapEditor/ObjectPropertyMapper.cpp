#include "stdafx.h"

#include "ObjectPropertyMapper.h"
#include "NaCommon.h"
#include "MFCPropertyGridPropertyEnum.h"

int ObjectPropertyMapper::s_nID = 0x1001;

ObjectPropertyMapper::ObjectPropertyMapper()
{
	m_pCtrl = nullptr;
}

ObjectPropertyMapper::~ObjectPropertyMapper()
{
}

void ObjectPropertyMapper::Clear()
{
	s_nID = 0x1001;

	m_mapProperty.clear();

	if (m_pCtrl)
		m_pCtrl->RemoveAll();
}

void ObjectPropertyMapper::BindControl(CMFCPropertyGridCtrl * pCtrl)
{
	m_pCtrl = pCtrl;
}

void ObjectPropertyMapper::InitControl()
{
	if (m_pCtrl == nullptr)
		return;

	std::map<NaString, CMFCPropertyGridProperty*> GroupMap;
	std::map<int, ObjectPropertyInfo> PropertyMap; // sort by id`
	{
		std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.begin();
		for (; it != m_mapProperty.end(); ++it)
		{
			ObjectPropertyInfo info = it->second;
			PropertyMap.insert(
				std::pair<int, ObjectPropertyInfo>(info.id, info)
			);
		}
	}

	std::map<int, ObjectPropertyInfo>::iterator it = PropertyMap.begin();
	for (; it != PropertyMap.end(); ++it)
	{
		// Find/Create Group
		std::map<NaString, CMFCPropertyGridProperty*>::iterator it_group = GroupMap.find(it->second.group);
		CMFCPropertyGridProperty *pGroup;
		if (it_group == GroupMap.end())
		{
			pGroup = new CMFCPropertyGridProperty(it->second.group.wstr());
			m_pCtrl->AddProperty(pGroup);
			GroupMap.insert(
				std::pair<NaString, CMFCPropertyGridProperty*>(it->second.group.wstr(), pGroup)
			);
		}
		else
			pGroup = it_group->second;
		
		// Add Property
		CMFCPropertyGridProperty *pProp = nullptr;
		switch (it->second.vt)
		{
		case VT_I4:
			if (it->second.szTypeNameArray != nullptr)
			{
				pProp = new CMFCPropertyGridPropertyEnum(
					it->second.name.wstr(),
					(_variant_t)0l,
					it->second.name.wstr(), // description
					it->second.id
				);
				pProp->EnableSpinControl(TRUE, -1024, 1024);

				wchar_t **szArray = it->second.szTypeNameArray;
				for (int i=0; ; i++)
				{
					if (szArray[i] == nullptr)
						break;
					NaString strOption;
					strOption.Format(L"%d (%s)", i, szArray[i]);
					pProp->AddOption(strOption.wstr());
				}
				pProp->AllowEdit(FALSE);
			}
			else
			{
				pProp = new CMFCPropertyGridProperty(
					it->second.name.wstr(),
					(_variant_t)0l,
					it->second.name.wstr(), // description
					it->second.id
				);
				pProp->EnableSpinControl(TRUE, -1024, 1024);
			}
			break;
		case VT_UI4:
			pProp = new CMFCPropertyGridProperty(
				it->second.name.wstr(),
				(_variant_t)(unsigned long)0l,
				it->second.name.wstr(), // description
				it->second.id
			);
			pProp->EnableSpinControl(TRUE, 0, 0xffffffff);
			break;
		case VT_ETC_COLOR:
			CMFCPropertyGridColorProperty *pColorProp;
			pProp = pColorProp = new CMFCPropertyGridColorProperty(
				it->second.name.wstr(),
				RGB(0, 0, 0), 
				nullptr,
				it->second.name.wstr(), // description
				it->second.id
			);
			pColorProp->EnableOtherButton(_T("기타..."));
			pColorProp->EnableAutomaticButton(_T("기본값"), ::GetSysColor(COLOR_3DFACE));
			break;
		case VT_R4:
			pProp = new CMFCPropertyGridProperty(
				it->second.name.wstr(),
				(_variant_t)0.0f,
				it->second.name.wstr(), // description
				it->second.id
			);
			break;
		case VT_ETC_NASTR:
			pProp = new CMFCPropertyGridProperty(
				it->second.name.wstr(),
				(_variant_t)L"",
				it->second.name.wstr(), // description
				it->second.id
			);
			break;
		case VT_ETC_NALIST:
			pProp = new CMFCPropertyGridProperty(
				it->second.name.wstr(),
				(_variant_t)L"",
				it->second.name.wstr(), // description
				it->second.id
			);
			
			if (it->second.arEnumStrList.GetCount() > 0)
			{
				for (int i=0; i<it->second.arEnumStrList.GetCount(); i++)
				{
					pProp->AddOption(it->second.arEnumStrList[i].wstr());
				}
			}
			break;
		case VT_BOOL:
			pProp = new CMFCPropertyGridProperty(
				it->second.name.wstr(),
				(_variant_t)L"true",
				it->second.name.wstr(), // description
				it->second.id
			);
			pProp->AddOption(L"true");
			pProp->AddOption(L"false");
			pProp->AllowEdit(FALSE);
			break;
		default:
			NaDebugOut(L"(OPM) Not supported variant type: %d\n", it->second.vt);
			DebugBreak();
			break;
		}

		pProp->Enable(it->second.bEnable);
		pGroup->AddSubItem(pProp);
	}
}

void ObjectPropertyMapper::RefreshControl()
{
	if (m_pCtrl == nullptr)
		return;

	int nCnt = m_pCtrl->GetPropertyCount();
	for (int i = 0; i < nCnt; i++)
	{
		CMFCPropertyGridProperty *pProp = m_pCtrl->GetProperty(i);
		int nSubCnt = pProp->GetSubItemsCount();
		for (int j = 0; j < nSubCnt; j++)
		{
			CMFCPropertyGridProperty *pSubProp = pProp->GetSubItem(j);
			if (pSubProp)
			{
				DWORD dwData = pSubProp->GetData();
				VARIANT var = GetPropertyByID(dwData);
				if (var.vt != 0x001f && var.vt != VT_NULL)
				{
					if (pSubProp->IsKindOf(RUNTIME_CLASS(CMFCPropertyGridColorProperty)))
					{
						CMFCPropertyGridColorProperty *pColorProp = (CMFCPropertyGridColorProperty*)pSubProp;
						pColorProp->SetColor(
							RGB(
								GetBValue(var.intVal),
								GetGValue(var.intVal),
								GetRValue(var.intVal)
							)
						);
					}
					else if (pSubProp->GetOptionCount() > 0 && var.vt == VT_I4)
					{
						COleVariant var2(var);
						pSubProp->SetValue(var2);
					}
					else
					{						
						COleVariant var2(var);
						pSubProp->SetValue(var2);
					}
				}
			}
		}
	}
}

void ObjectPropertyMapper::BindProperty(LPCWSTR szName, void * pRefVariable)
{
	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.find(NaString(szName));
	if (it != m_mapProperty.end())
	{
		NaDebugOut(L"(OPM) Bind Property: %s, 0x%08x\n", szName, pRefVariable);
		it->second.pRefVariable = pRefVariable;
	}
}

void ObjectPropertyMapper::AddProperty(LPCWSTR szName, void * pRefVariable, int vt, LPCWSTR szGroup, bool bEnable)
{
	ObjectPropertyInfo info;
	info.name = szName;
	info.pRefVariable = pRefVariable;
	info.vt = vt;
	info.group = szGroup;
	info.id = GenerateID();
	info.bEnable = bEnable;

	m_mapProperty.insert(
		std::pair<NaString, ObjectPropertyInfo>(info.name, info)
	);
}

void ObjectPropertyMapper::AddProperties(PropertyObjectBase *pObj)
{
	PropertyMap *pMap = pObj->GetPropertyMap();

	std::map<NaString, PropertyInfo>::iterator it = pMap->begin();
	for (; it != pMap->end(); ++it)
	{
		PropertyInfo info = it->second;
		void* pRefVariable = pObj->GetRefPropertyValue(info.strPropName);

		ObjectPropertyInfo oi;
		oi.name = info.strPropName;
		oi.pRefVariable = pRefVariable;
		oi.vt = info.vt;
		oi.group = info.strGroup;
		if (oi.group.GetLength() == 0)
			oi.group = L"Etc";
		oi.id = GenerateID();
		oi.bEnable = true;
		oi.szTypeNameArray = info.szTypeNameArray;

		m_mapProperty.insert(
			std::pair<NaString, ObjectPropertyInfo>(oi.name, oi)
		);
	}
}

void ObjectPropertyMapper::SetPropertyEnable(NaString strName, bool bEnable)
{
	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.find(strName);
	if (it != m_mapProperty.end())
	{
		it->second.bEnable = bEnable;
	}
}

void ObjectPropertyMapper::SetPropertyOptionList(NaString strName, NaStrArray ar)
{
	DWORD dwId = -1;
	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.find(strName);
	if (it != m_mapProperty.end())
	{
		it->second.arEnumStrList = ar;
	}
}

void ObjectPropertyMapper::SetProperty(NaString strName, VARIANT *pVar)
{
#define _SET_REFVAL(_t, _v) \
	{ _t *pRefValue = (_t*)it->second.pRefVariable; \
	*((_t*)(pRefValue)) = pVar->_v; }

	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.find(strName);
	if (it != m_mapProperty.end())
	{
		switch (it->second.vt)
		{
		case VT_I4:
			_SET_REFVAL(long, intVal);
			break;
		case VT_ETC_COLOR:
			{
				long *pRefValue = (long*)it->second.pRefVariable;
				*((long*)(pRefValue)) = (COLORREF)RGB(
					GetBValue(pVar->intVal),
					GetGValue(pVar->intVal),
					GetRValue(pVar->intVal)
				);
			}
			break;
		case VT_UI4:
			_SET_REFVAL(unsigned long, uintVal);
			break;
		case VT_R4:
			_SET_REFVAL(float, fltVal);
			break;
		case VT_ETC_NASTR:
		case VT_ETC_NALIST:
			{
				NaString *pRefValue = (NaString*)(it->second.pRefVariable);
				pRefValue->Format(L"%s", pVar->bstrVal);
			}
			break;
		case VT_BOOL:
			{
				bool *pRefValue = (bool*)(it->second.pRefVariable);
				NaString strBool = pVar->bstrVal;
				if (strBool.Compare(L"true") == 0)
					*((bool*)(pRefValue)) = true;
				else
					*((bool*)(pRefValue)) = false;
			}
		}
	}
}

void ObjectPropertyMapper::SetPropertyByID(int nID, VARIANT *pVar)
{
	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.begin();
	for ( ; it != m_mapProperty.end(); ++it)
	{
		if (it->second.id == nID)
		{
			NaString strName = it->second.name;
			SetProperty(strName, pVar);
			return;
		}
	}
}

VARIANT ObjectPropertyMapper::GetProperty(NaString strName)
{
	VARIANT var;
	var.vt = VT_NULL;

	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.find(strName);
	if (it != m_mapProperty.end())
	{
		void *pRefValue = it->second.pRefVariable;
		if (pRefValue == nullptr)
		{
			NaDebugOut(L"(OPM) RefValue is null: %s\n", it->second.name.wstr());
			return var;
		}

		VARENUM vt;
		switch (it->second.vt)
		{
		case VT_ETC_COLOR:
			vt = (VARENUM)VT_I4;
			break;
		case VT_ETC_NASTR:
		case VT_ETC_NALIST:
			vt = VT_BSTR;
			var.bstrVal = SysAllocString(((NaString*)pRefValue)->wstr());
			break;
		case VT_BOOL:
			{
				vt = VT_BSTR;
				if (*(bool*)pRefValue == true)
					var.bstrVal = SysAllocString(L"true");
				else
					var.bstrVal = SysAllocString(L"false");
			}
			break;
		default:
			vt = (VARENUM)it->second.vt;
			break;
		}

		var.vt = vt;
		switch (vt)
		{
		case VT_I4:
			var.intVal = *(long*)pRefValue;
			break;
		case VT_UI4:
			var.uintVal = *(unsigned long*)pRefValue;
			break;
		case VT_R4:
			var.fltVal = *(float*)pRefValue;
			break;
		
		}
	}

	return var;
}

VARIANT ObjectPropertyMapper::GetPropertyByID(int nID)
{
	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.begin();
	for (; it != m_mapProperty.end(); ++it)
	{
		if (it->second.id == nID)
		{
			NaString strName = it->second.name;
			return GetProperty(strName);
		}
	}

	VARIANT var;
	var.vt = VT_NULL;
	return var;
}

NaString ObjectPropertyMapper::GetPropertyNameByID(int nID)
{
	std::map<NaString, ObjectPropertyInfo>::iterator it = m_mapProperty.begin();
	for (; it != m_mapProperty.end(); ++it)
	{
		if (it->second.id == nID)
		{
			NaString strName = it->second.name;
			return strName;
		}
	}

	return NaString(L"");
}

int ObjectPropertyMapper::GenerateID()
{
	return s_nID++;
}
