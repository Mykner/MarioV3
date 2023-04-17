#pragma once

#include "EnemyGenerator.h"

class ObjectGenerator : public EnemyGenerator
{
public:
    ObjectGenerator(Game *pGame, Stage *pStage);
    virtual ~ObjectGenerator();

    // Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"GenerateType", &m_nGenerateTypeIndex },
		{ L"Interval", &m_nInterval },
		{ L"Chance", &m_nChance },
		{ L"MaxCount", &m_nMaxCount },
		{ L"Range", &m_nRange },
		{ L"GeneratePosition", &m_nGeneratePosition },
	END_DECLARE_PROPERTYMAP()
};