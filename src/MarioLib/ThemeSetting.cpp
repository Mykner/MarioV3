#include "ThemeSetting.h"

#include "Stage.h"
#include "JsonManager.h"

// Property Interface
BEGIN_IMPL_PROPERTYMAP(ThemeSetting)
	PROP_INT("ControllableObjectSmallHeight", VT_I4, -1, nullptr, "ThemeSetting"),
	PROP_INT("ControllableObjectBigHeight", VT_I4, -1, nullptr, "ThemeSetting"),
END_IMPL_PROPERTYMAP()

ThemeSetting::ThemeSetting()
{
	Clear();
}

void ThemeSetting::Clear()
{
	m_nControllableObjectSmallHeight = 16;
	m_nControllableObjectBigHeight = 32;
}

void ThemeSetting::OnLoad()
{
#define SET_RANGE_FORCE(v, min, max) \
	if (v < min) v = min; \
	if (v > max) v = max;

	SET_RANGE_FORCE(m_nControllableObjectSmallHeight, 14, 16);
	SET_RANGE_FORCE(m_nControllableObjectBigHeight, 28, 32);
}
