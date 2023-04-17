#pragma once

#include "NaString.h"

class GameOption
{
public:
	GameOption();

	void ParseOption(NaString strOptionName);

	bool m_bUseDirectX;
	bool m_bSoftwareFrameSkip;
	bool m_bDetailCircleEffect;
	bool m_bPlaySound;
	bool m_bPlayBGM;
	bool m_bTestMode;
};
