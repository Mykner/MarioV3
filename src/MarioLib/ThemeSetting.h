#pragma once

#include <vector>
#include "PropertyMap.h"

class ThemeSetting : public PropertyObjectBase
{
public:
	ThemeSetting();

	void Clear();
	void OnLoad();

	int m_nControllableObjectSmallHeight;
	int m_nControllableObjectBigHeight;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("ControllableObjectSmallHeight", &m_nControllableObjectSmallHeight)
		DECL_PROP("ControllableObjectBigHeight", &m_nControllableObjectBigHeight)
	END_DECLARE_PROPERTYMAP()
};
