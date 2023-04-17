#include "PropertyMap.h"

PropertyMap PropertyObjectBase::s_mapProperty;

bool PropertyObjectBase::HasProperty(NaString strPropName)
{
	PropertyValueMap::iterator it = m_mapPropertyValue.find(strPropName);
	if (it != m_mapPropertyValue.end())
		return true;

	return false;
}

void * PropertyObjectBase::GetRefPropertyValue(NaString strPropName)
{
	PropertyValueMap::iterator it = m_mapPropertyValue.find(strPropName);
	if (it != m_mapPropertyValue.end())
		return it->second;

	return nullptr;
}

PropertyMap * PropertyObjectBase::GetPropertyMap()
{
	return &s_mapProperty;
}
