#pragma once

#include "GameStateBase.h"

#include <vector>
#include <map>

enum CreditTextAlign
{
	CREDIT_TEXT_CENTER = 0,
	CREDIT_TEXT_LEFT,
	CREDIT_TEXT_RIGHT,
};

class Credit
{
public:
	NaString m_strPosition;
	NaString m_strName;
	NaString m_strComment;
};

#define CREDIT_TEAM_VECTOR			std::vector<Credit*>

class CreditCategory 
{
public:
	~CreditCategory()
	{
		SAFE_DELETE_OBJPTR_VECTOR(Credit*, m_vecMembers);
	}
	NaString m_strName;
	CREDIT_TEAM_VECTOR m_vecMembers;
};

#define CREDIT_CATEGORY_VECTOR		std::vector<CreditCategory*>

class GameStateCredits : public GameStateBase
{
public:
	GameStateCredits(Game *pGame);
	virtual ~GameStateCredits();

	virtual long GetBackgroundColor();

	virtual void Init();
	virtual void Process();
	virtual void Render();

	void ClearCredit();
	void AddCredit(NaString strCategory, NaString strPosition, NaString strName, NaString strComment);
	void CenterText(float fY, NaString str, int nMode = 0, float fScale = 1.0f);

	CREDIT_CATEGORY_VECTOR m_vecCreditCetegory;
	float m_fScroll;
	float m_fTotalScrollSize;
	int m_nSoundCooltime;
};
