#pragma once

#include "NaString.h"
#include <map>

enum VARENUM_ETC
{
	VT_ETC_COLOR = 0x10000,
	VT_ETC_NASTR,
	VT_ETC_NALIST,
};

class PropertyInfo;
typedef std::map<NaString, PropertyInfo> PropertyMap;
typedef std::map<NaString, void*> PropertyValueMap;

#define BEGIN_DECLARE_PROPERTYMAP() \
	static PropertyMap s_mapProperty; \
	virtual PropertyMap* GetPropertyMap() { return &s_mapProperty; }; \
	virtual void BuildPropertyMap() \
	{ \
		struct PropValueRef { NaString strPropName; void* pRefValue; }; \
		PropValueRef info[] = \
		{ 

#define DECL_PROP(_name, _var) { L##_name, _var },

#define END_DECLARE_PROPERTYMAP() \
		}; \
		int nLen = sizeof(info) / sizeof(PropValueRef); \
		for (int i = 0; i < nLen; i++) \
		{ \
			m_mapPropertyValue.insert( \
				std::pair<NaString, void*>(info[i].strPropName, info[i].pRefValue) \
			); \
		} \
	};

#define BEGIN_IMPL_PROPERTYMAP(_class) \
	PropertyMap _class::s_mapProperty = \
	{

#define PROP_INT(_name, _vt, _default, _enumptr, _group) \
	{ L##_name, { L##_name, _vt, _enumptr, L##_group } }

#define PROP_FLOAT(_name, _vt, _default, _enumptr, _group) \
	{ L##_name, { L##_name, _vt, _enumptr, L##_group } }

#define PROP_LONG(_name, _vt, _default, _enumptr, _group) \
	{ L##_name, { L##_name, _vt, _enumptr, L##_group } }

#define PROP_STR(_name, _vt, _default, _enumptr, _group) \
	{ L##_name, { L##_name, _vt, _enumptr, L##_group } }

#define PROP_BOOL(_name, _vt, _default, _enumptr, _group) \
	{ L##_name, { L##_name, _vt, _enumptr, L##_group } }

#define END_IMPL_PROPERTYMAP() \
	};

class PropertyInfo
{
public:
	NaString strPropName;
	int vt;

	/*
	// #TODO PropertyMap DefaultValue handling
	union
	{
		int nDefaultIntValue;
		float fDefaultFloatValue;
		long lDefaultLongValue;
		NaString strDefaultStrValue;
		bool bDefaultBooleanValue;
	};
	*/

	wchar_t** szTypeNameArray;
	NaString strGroup;
};

class PropertyObjectBase
{
public:
	PropertyObjectBase() {};
	virtual ~PropertyObjectBase() {};

	virtual void BuildPropertyMap() {};
	virtual PropertyMap* GetPropertyMap();

	bool HasProperty(NaString strPropName);
	void* GetRefPropertyValue(NaString strPropName);

	static PropertyMap s_mapProperty;
	PropertyValueMap m_mapPropertyValue;
};

