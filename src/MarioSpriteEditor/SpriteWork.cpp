#include "stdafx.h"
#include "SpriteWork.h"

#include "Macros.h"

BEGIN_IMPL_PROPERTYMAP(SpriteWork)
	PROP_STR("Newlines", VT_ETC_NASTR, "", nullptr, "WorkInfo"),
END_IMPL_PROPERTYMAP()

SpriteAnimationFrames::SpriteAnimationFrames()
{
	m_strName = L"";
	m_strFrames = L"";
}

SpriteWork::SpriteWork() :
	PropertyObjectBase()
{
	m_bLoaded = false;
	m_bBitmapChanged = false;
	m_strNewlines = L"";
}


SpriteWork::~SpriteWork()
{
	SAFE_DELETE_OBJPTR_VECTOR(SpriteAnimationFrames*, m_vecAnimationFrames);
}

