#pragma once

#include "NaString.h"
#include "NaRect.h"

#include "PropertyMap.h"

class Game;
class GameConfig : public PropertyObjectBase
{
public:	
	static GameConfig* GetGameConfig(Game* pGame = nullptr);
	static void Release();

	void Save();
	void ApplyOptions(Game *pGame);

	NaString m_strName;
	NaString m_strUniqueId;
	NaString m_strWindowRect;
	NaRect m_rcWindow;
	NaString m_strRenderEngine;
	bool m_bSoftwareFrameSkip;
	NaString m_strInputKeyCode;
	int m_nInputKeyCode[6];
	int m_nStageSortMethod;
	int m_nStageDifficultyFilter;
	int m_nStageThemeFilter;
	int m_nZoomFactor;

	Game* m_pGame;

protected:
	GameConfig(Game* pGame);
	~GameConfig();

	void Load();

	static GameConfig* s_pGameConfig;

	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Name", &m_strName)
		DECL_PROP("UniqueId", &m_strUniqueId)
		DECL_PROP("WindowRect", &m_strWindowRect)
		DECL_PROP("RenderEngine", &m_strRenderEngine)
		DECL_PROP("SoftwareFrameSkip", &m_bSoftwareFrameSkip)
		DECL_PROP("InputKeyCode", &m_strInputKeyCode)
		DECL_PROP("StageSortMethod", &m_nStageSortMethod)
		DECL_PROP("StageDifficultyFilter", &m_nStageDifficultyFilter)
		DECL_PROP("StageThemeFilter", &m_nStageThemeFilter)
		DECL_PROP("ZoomFactor", &m_nZoomFactor)
	END_DECLARE_PROPERTYMAP()
};