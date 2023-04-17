#pragma once

#include "PropertyMap.h"

#include <vector>

class SpriteAnimationFrames
{
public:
	SpriteAnimationFrames();

	NaString m_strName;
	NaString m_strFrames;
};

class SpriteWork : public PropertyObjectBase
{
public:
	SpriteWork();
	virtual ~SpriteWork();

	bool m_bLoaded;
	bool m_bBitmapChanged;

	NaString m_strNewlines;
	std::vector<SpriteAnimationFrames*> m_vecAnimationFrames;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Newlines", &m_strNewlines)
	END_DECLARE_PROPERTYMAP()
};
