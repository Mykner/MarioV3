#include "SoundManager.h"

#include "Game.h"
#include "Macros.h"
#include "Player.h"
#include "FModManager.h"
#include "Stage.h"
#include "GameObjectBase.h"
#include "Camera.h"
#include "Themes.h"

SoundManager::SoundManager(Game *pGame)
{
	m_pGame = pGame;
	m_strCurrentBGM = L"";
	m_strCurrentGameTheme = L"";
}

SoundManager::~SoundManager()
{
}

void SoundManager::PlaySoundEffect(NaString str, GameObjectBase * pObj)
{
	NaRect rcView = CAMERA->GetViewport();
	if (pObj && pObj->IsOutOfCamera())
		return;

	//FMOD::Sound *pSound = nullptr;
	//pSound = m_pGame->m_pFMod->PlaySound(str.wstr());

	// Apply GameTheme!
	FMOD::Sound* pSound = nullptr;
	NaString strGameTheme = CUR_STAGE->m_strGameTheme;
	if (strGameTheme.GetLength() > 0)
		pSound = m_pGame->m_pFMod->RequestPlaySoundQueue(str.wstr(), strGameTheme.wstr());
	else
		pSound = m_pGame->m_pFMod->RequestPlaySoundQueue(str.wstr(), nullptr);
}

void SoundManager::PlayHurryUp()
{
	StopBGM();
	PlayBGM(L"HurryUp", false);
}

void SoundManager::PlayStageBGM()
{
	if (!CanPlayStageBGM())
	{
		NaDebugOut(L"BGM: Cannot Play Stage BGM now.\n");
		return;
	}

	int nTheme = CUR_STAGE->m_nTheme;
	NaString strBGM = L"";
	FMOD::Sound *pSound = nullptr;
	switch (nTheme)
	{
	case STAGETHEME_OVERWORLD:
	default:
		strBGM = L"Overworld";
		break;
	case STAGETHEME_UNDERGROUND:
		strBGM = L"Underground";
		break;
	case STAGETHEME_CASTLE:
	case STAGETHEME_CASTLEUNDERWATER:
		strBGM = L"Castle";
		break;
	case STAGETHEME_UNDERWATER:
		strBGM = L"UnderWater";
		break;
	case STAGETHEME_GHOSTHOUSE:
		strBGM = L"GhostHouse";
		break;
	case STAGETHEME_AIRSHIP:
		strBGM = L"AirShip";
		break;
	case STAGETHEME_SNOW:
		strBGM = L"Snow";
		break;

	}

	if (CUR_STAGE->m_nEntranceType == STAGEENTRANCE_GROUNDTOSKY)
	{
		strBGM = L"Starman";
	}

	if (CUR_PLAYER->m_nPSwitchTime > 0)
	{
		strBGM = L"PSwitch";
	}
	else if (CUR_PLAYER_OBJ->m_nInvinsibleTime > 8 * 24)
	{
		strBGM = L"Starman";
	}
	else if (CUR_PLAYER_OBJ->m_nGiantTime > 0)
	{
		strBGM = L"Giant";
	}

	NaDebugOut(L"BGM: %s\n", strBGM.wstr());

	NaString strGameTheme = CUR_STAGE->m_strGameTheme;
	if (strBGM.Compare(m_strCurrentBGM) != 0 ||
		strGameTheme.Compare(m_strCurrentGameTheme) != 0)
	{
		pSound = PlayBGM(strBGM);
	}

	if (CUR_PLAYER->m_nTime < 100 && CUR_PLAYER->m_nTime >= 0)
	{
		// #TODO Speed up BGM
		// Temporary
		m_pGame->m_pFMod->m_pChannel[CHANNEL_BGM]->setFrequency(60000);
	}
}

void SoundManager::PlayInvinsibleBGM()
{
	NaDebugOut(L"BGM: Starman\n");

	if (!CanPlayStageBGM())
		return;

	PlayBGM(L"Starman");
}

void SoundManager::PlayGiantBGM()
{
	NaDebugOut(L"BGM: Giant\n");

	if (!CanPlayStageBGM())
		return;

	PlayBGM(L"Giant");
}

void SoundManager::PlayPSwitchBGM()
{
	NaDebugOut(L"BGM: PSwitch\n");

	if (!CanPlayStageBGM())
		return;

	PlayBGM(L"PSwitch");
}

FMOD::Sound* SoundManager::PlayBGM(NaString str, bool bLoop)
{
	m_pGame->m_pFMod->StopBGM();
	
	//FMOD::Sound* pSound = m_pGame->m_pFMod->PlayBGM(str, bLoop);

	// Apply GameTheme!
	FMOD::Sound* pSound = nullptr;
	NaString strGameTheme = CUR_STAGE->m_strGameTheme;
	if (strGameTheme.GetLength() > 0)
		pSound = m_pGame->m_pFMod->RequestPlayBGM(str, strGameTheme.wstr(), bLoop);
	else
		pSound = m_pGame->m_pFMod->RequestPlayBGM(str, nullptr, bLoop);

	m_strCurrentBGM = str;
	m_strCurrentGameTheme = strGameTheme;

	return pSound;
}

void SoundManager::PauseBGM(bool bPause)
{
	m_pGame->m_pFMod->PauseBGM(bPause);
}

void SoundManager::StopBGM()
{
	m_pGame->m_pFMod->StopBGM();
	m_strCurrentBGM = L"";
}

void SoundManager::StopSoundEffect()
{
	m_pGame->m_pFMod->StopSound();
}

bool SoundManager::CanPlayStageBGM()
{
	int nType = m_pGame->m_pGameState->m_nType;
	switch (nType)
	{
	case GAMESTATE_SINGLESTAGECLEAR:
	case GAMESTATE_FLAGDOWN:
	case GAMESTATE_GOAL:
	case GAMESTATE_CUTBRIDGE:
	case GAMESTATE_RESCUEPRINCESS:
		return false;
		break;
	}

	return true;
}
