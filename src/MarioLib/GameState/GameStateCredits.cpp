#include "GameStateCredits.h"

#include "Game.h"
#include "Stage.h"
#include "Player.h"
#include "SoundManager.h"
#include "RenderManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Mario.h"
#include "Sprite.h"
#include "FModManager.h"

GameStateCredits::GameStateCredits(Game *pGame)
	: GameStateBase(pGame)
{
	m_nType = GAMESTATE_CREDITS;

	m_nSoundCooltime = 0;
}

GameStateCredits::~GameStateCredits()
{

}

long GameStateCredits::GetBackgroundColor()
{
	return 0;
}

void GameStateCredits::Init()
{
	CAMERA->m_ptPos.x = 0;
	CAMERA->m_ptPos.y = 0;

	SOUND_MANAGER->StopBGM();

	m_fScroll = 0;
	m_fTotalScrollSize = 0;

	AddCredit(L"Super fanmade Mario Bros", L"since 2016.06.17 ", L"", L"");
	AddCredit(L"Super fanmade Mario Bros", L":", L"", L"");
	AddCredit(L"Super fanmade Mario Bros", L":", L"", L"");
	AddCredit(L"Super fanmade Mario Bros", L" ", L"", L"");

	AddCredit(L"Original Game By", L"Nintendo", L"", L"");
	AddCredit(L"Original Game By", L"Super Mario Bros.", L"", L"");
	AddCredit(L"Original Game By", L"Super Mario Maker", L"", L"");

	AddCredit(L"Development", L"Programer", L"neoarc", L"");
	AddCredit(L"Development", L"MacOS Porting", L"neoarc", L"");
	AddCredit(L"Development", L"Some Sprite", L"neoarc", L"");

	AddCredit(L"Special Thanks to", L"My Wife", L"HyeYoung Kim", L"");

	AddCredit(L"Contributors", L"Issue Management", L"Michal Klincewicz", L"");
	AddCredit(L"Contributors", L"Server Idea", L"rhasya", L"");
	AddCredit(L"Contributors", L"Assistant", L"Mad Nyle", L"");
	AddCredit(L"Contributors", L"Title BGM", L"Ian Harrison", L"");
	AddCredit(L"Contributors", L"Sprite Helper", L"DevHN", L"");

	AddCredit(L"VIP Testers", L"(was)VIP Tester", L"Tom H", L"");
	AddCredit(L"VIP Testers", L"Stage Creator", L"JeffersonB", L"");
	AddCredit(L"VIP Testers", L"VIP Tester", L"JooJ - Freddy Esperto", L"");
	AddCredit(L"VIP Testers", L"VIP Tester", L"Henrique Nichet", L"");
	AddCredit(L"VIP Testers", L"VIP Tester", L"Blastsick", L""); // Nicolas Kyc

	AddCredit(L"Sprites", L"Flag Pole (smb3, smw)", L"Geoshi", L"");
	AddCredit(L"Sprites", L"Shell sliding (smb3)", L"MJCabbages", L""); // MJCabbages (Discord)

	AddCredit(L"Beta Testers", L"Sprite Helper", L"aaucuy", L""); // Radithya Pasha 
	AddCredit(L"Beta Testers", L"Nice Tester", L"JoaoPedroGaming", L""); 
	AddCredit(L"Beta Testers", L"Mega Mario", L"Alzter", L"");
	AddCredit(L"Beta Testers", L"Bug Report", L"Panduso", L"");
	AddCredit(L"Beta Testers", L"Bug Report", L"Fgz Art", L"");

	// custom theme helper
	/*
	tom
	nicolas kyc
	michal klincewicz
	zenon
	oxiriarkana kaytoon
	alessio cameroni
	jan golebiowski
	radithya pasha
		*/

	AddCredit(L"Beta Testers", L" ", L"", L"");
	AddCredit(L"Beta Testers", L"Meowtist", L"", L""); // Pedro Henrique Macedo Antunes 
	AddCredit(L"Beta Testers", L"William Whitney", L"", L"");
	AddCredit(L"Beta Testers", L"ZenonXD170", L"", L"");

	AddCredit(L"Beta Testers", L"Jan Golebiowski", L"", L"");
	AddCredit(L"Beta Testers", L"Endermite Malaki", L"", L"");
	AddCredit(L"Beta Testers", L"IgnaMundox", L"", L"");	
	AddCredit(L"Beta Testers", L"Fptbb", L"", L"");
	AddCredit(L"Beta Testers", L"Meltix", L"", L"");
	AddCredit(L"Beta Testers", L"Numa", L"", L"");
	AddCredit(L"Beta Testers", L"Gabriel Flynn", L"", L"");

	SOUND_MANAGER->PlayBGM(L"StageList");
}

void GameStateCredits::Process()
{
	CUR_STAGE->ProcessEffects();

	if (m_nSoundCooltime == 0 &&
		(m_pGame->m_pInputManager->m_bMouseClick || KE(VK_X)))
	{
		wchar_t* szSounds[] =
		{
			L"1UP",
			L"ItemPop",
			L"JumpSmall",
			L"ClapLong",
			L"Coin",
			L"ThrowFireball",
			L"Cannon",
			L"PowerUp",
			L"PowerDown",
		};

		int nSoundLen = sizeof(szSounds) / sizeof(wchar_t*);
		int nRandom = rand() % (nSoundLen);
		PLAYSOUND(szSounds[nRandom]);
		
		m_nSoundCooltime = 60;
	}

	if (m_nSoundCooltime > 0)
		m_nSoundCooltime--;

	if (KE(VK_ESCAPE))
	{
		ClearCredit();

		SOUND_MANAGER->StopBGM();
		PopState();
	}

	if (KS(VK_SPACE))
	{
		m_fScroll += 0.5f;
	}

	m_fScroll += 0.25f;

	if (m_fScroll >= m_fTotalScrollSize)
		m_fScroll = 0;
}

void GameStateCredits::AddCredit(NaString strCategory, NaString strPosition, NaString strName, NaString strComment)
{
	Credit *pCredit = new Credit;
	pCredit->m_strPosition = strPosition;
	pCredit->m_strName = strName;
	pCredit->m_strComment = strComment;

	CreditCategory *pCat = nullptr;
	BEGIN_VECTOR_LOOP(CreditCategory*, m_vecCreditCetegory) {
		if (pObj->m_strName.CompareNoCase(strCategory) == 0)
		{
			pCat = pObj;
		}
	} END_VECTOR_LOOP();

	if (pCat == nullptr)
	{
		pCat = new CreditCategory;
		pCat->m_strName = strCategory;
		m_vecCreditCetegory.push_back(pCat);
	}

	pCat->m_vecMembers.push_back(pCredit);
}

void GameStateCredits::ClearCredit()
{
	SAFE_DELETE_OBJPTR_VECTOR(CreditCategory*, m_vecCreditCetegory);
}

void GameStateCredits::CenterText(float fY, NaString str, int nMode, float fScale)
{
	int nWidth = m_pGame->Width();
	int nStringWidth = (str.GetLength() * 8.0f * fScale);

	float fX;

	switch (nMode)
	{
	case 0:
		fX = (nWidth / 2) - (nStringWidth / 2.0f);
		break;
	case 1:
		fX = (nWidth / 2) - (nStringWidth) - 10;
		break;
	case 2:
		fX = (nWidth / 2) + 10;
		break;
	}

	RENDER_MANAGER->Text(fX, fY, str, -1, fScale);
}

void GameStateCredits::Render()
{
	int nHeight = m_pGame->Height();
	NaString str;

	float fY = nHeight - m_fScroll;
	m_fTotalScrollSize = nHeight / 2 + 48;

	/*
	// for test
	str.Format(L"%.1f", m_fScroll);
	RENDER_MANAGER->Text(0, 0, str);
	*/

	CREDIT_CATEGORY_VECTOR::iterator it_cat = m_vecCreditCetegory.begin();
	for (; it_cat != m_vecCreditCetegory.end(); ++it_cat)
	{
		NaString strTitle = (*it_cat)->m_strName;
		CenterText(fY, strTitle, CREDIT_TEXT_CENTER, 2.0f);
		fY += 16*2;
		m_fTotalScrollSize += 16 * 2;

		CREDIT_TEAM_VECTOR &vec = (*it_cat)->m_vecMembers;

		BEGIN_VECTOR_LOOP(Credit*, vec) {
			if (pObj->m_strName.GetLength() == 0)
			{
				CenterText(fY, pObj->m_strPosition, CREDIT_TEXT_CENTER, 1.0f);
			}
			else
			{
				CenterText(fY, pObj->m_strPosition, CREDIT_TEXT_LEFT, 1.0f);
				CenterText(fY, pObj->m_strName, CREDIT_TEXT_RIGHT, 1.0f);
			}

			fY += 16;
			m_fTotalScrollSize += 16;

			//CenterText(fY, pObj->m_strComment, 1.0f);
			//fY += 16;
		} END_VECTOR_LOOP();

		fY += 64;
		m_fTotalScrollSize += 64;
	}
}
