#include "GameOption.h"

#include "Common/Common.h"

GameOption::GameOption()
{
	m_bUseDirectX = false;
	m_bSoftwareFrameSkip = GameDefaults::bFrameSkip;
	m_bDetailCircleEffect = false;
	m_bPlaySound = true;
	m_bPlayBGM = true;
	m_bTestMode = false;
}

void GameOption::ParseOption(NaString strOptionName)
{
	if (strOptionName.CompareNoCase(L"directx") == 0 ||
		strOptionName.CompareNoCase(L"dx") == 0)
		m_bUseDirectX = true;

	if (strOptionName.CompareNoCase(L"softwareframeskip") == 0 ||
		strOptionName.CompareNoCase(L"sf") == 0)
		m_bSoftwareFrameSkip = true;

	if (strOptionName.CompareNoCase(L"detailedcircleeffct") == 0 ||
		strOptionName.CompareNoCase(L"dce") == 0)
		m_bDetailCircleEffect = true;

	if (strOptionName.CompareNoCase(L"nosound") == 0 ||
		strOptionName.CompareNoCase(L"ns") == 0)
		m_bPlaySound = false;

	if (strOptionName.CompareNoCase(L"nobgm") == 0 ||
		strOptionName.CompareNoCase(L"nb") == 0)
		m_bPlayBGM = false;

	if (strOptionName.CompareNoCase(L"test") == 0 ||
		strOptionName.CompareNoCase(L"t") == 0)
		m_bTestMode = true;
}
