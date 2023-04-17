#pragma once

#include "NaString.h"
#include "PropertyMap.h"

class Game;
class StagePack : public PropertyObjectBase
{
public:
	StagePack(Game *pGame);
	virtual ~StagePack();

	void Load(NaString strName);
	bool IsValid();

	Game *m_pGame;
	bool m_bValid;
	NaString m_strFileName;

	NaString m_strName;
	NaString m_strInitialStage;
	NaString m_strCreator;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		DECL_PROP("Name", &m_strName)
		DECL_PROP("Creator", &m_strCreator)
		DECL_PROP("InitialStage", &m_strInitialStage)
	END_DECLARE_PROPERTYMAP()
};
