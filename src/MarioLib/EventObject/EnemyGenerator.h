#pragma once

#include "Common.h"

#include "EventObjectBase.h"
#include "PropertyMap.h"

class EnemyGenerator : public EventObjectBase
{
public:
	EnemyGenerator(Game *pGame, Stage *pStage);
	virtual ~EnemyGenerator();

	virtual void OnBeforeProcess();
	virtual void OnAfterProcess();

	virtual void Render(int nColor = 0xffffffff, int nZOrder = -1);
	virtual void RenderDesign(int nColor = 0xffffffff, int nZOrder = -1);

	virtual void OnIdle();

	virtual NaRect GetRect();
	virtual NaRectT<float> GetRectFloat();
	virtual SIZE GetSize();

	int m_nEnemyType; // will be deprecated
	int m_nGenerateType;
	int m_nGenerateTypeIndex;
	int m_nInterval;
	int m_nChance;
	int m_nMaxCount;
	int m_nRange;
	int m_nGeneratePosition;

	// Property Interface
	BEGIN_DECLARE_PROPERTYMAP()
		{ L"EnemyType", &m_nEnemyType },
		{ L"GenerateType", &m_nGenerateTypeIndex },
		{ L"Interval", &m_nInterval },
		{ L"Chance", &m_nChance },
		{ L"MaxCount", &m_nMaxCount },
		{ L"Range", &m_nRange },
		{ L"GeneratePosition", &m_nGeneratePosition },
	END_DECLARE_PROPERTYMAP()
};