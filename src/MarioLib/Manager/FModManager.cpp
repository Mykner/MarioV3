#include "FModManager.h"

#include "Game.h"
#include "PathManager.h"

FModManager::FModManager(Game *pGame)
{
	m_pGame = pGame;
	m_bInitialized = false;
}

FModManager::~FModManager()
{

}

void FModManager::Init()
{
	if (m_pGame &&
		!m_pGame->m_Option.m_bPlayBGM &&
		!m_pGame->m_Option.m_bPlaySound)
		return;

	FMOD_RESULT result;
	result = FMOD::System_Create(&m_pSystem);
	
	result = m_pSystem->getVersion(&m_nVersion);

	if (m_nVersion < FMOD_VERSION) 
	{
		// #TODO error
	}

	result = m_pSystem->init(128, FMOD_INIT_NORMAL, 0);

	for (int i = 0; i < CHANNEL_COUNT; i++)
		m_pChannel[i] = nullptr;

	for (int i = 0; i < GROUP_COUNT; i++)
		m_pGroup[i] = nullptr;

	m_pSystem->createChannelGroup("BGM", &m_pGroup[GROUP_BGM]);
	m_pSystem->createChannelGroup("Sound", &m_pGroup[GROUP_SOUND]);

	m_pSystem->getMasterChannelGroup(&m_pMasterGroup);
	
	m_pMasterGroup->addGroup(m_pGroup[GROUP_BGM]);
	m_pMasterGroup->addGroup(m_pGroup[GROUP_SOUND]);

	m_pChannel[CHANNEL_BGM]->setChannelGroup(m_pGroup[GROUP_BGM]);
	m_pChannel[CHANNEL_SUBBGM]->setChannelGroup(m_pGroup[GROUP_BGM]);
	m_pChannel[CHANNEL_SOUND]->setChannelGroup(m_pGroup[GROUP_SOUND]);

	m_bInitialized = true;
}

void FModManager::Release()
{
	if (!m_bInitialized)
		return;

	FMOD_RESULT result;
	
	NamedSoundMap::iterator it = m_mapBGM.begin();
	for (; it != m_mapBGM.end(); ++it)
	{
		FMOD::Sound *pSound = it->second;
		result = pSound->release();
	}
	m_mapBGM.clear();

	it = m_mapSound.begin();
	for (; it != m_mapSound.end(); ++it)
	{
		FMOD::Sound *pSound = it->second;
		result = pSound->release();
	}
	m_mapSound.clear();

	for (int i = 0; i<GROUP_COUNT; i++)
	{
		if (m_pGroup[i] != nullptr) 
		{
			result = m_pGroup[i]->release();
		}
	}

	result = m_pSystem->close();
	result = m_pSystem->release();
}

void FModManager::Process()
{
	if (m_bInitialized)
	{
		SoundQueueMap::iterator it = m_mapReqPlaySound.begin();
		for (; it != m_mapReqPlaySound.end(); ++it)
		{
			if (it->second.GetLength() > 0)
				RequestPlaySound(it->first, it->second);
			else
				RequestPlaySound(it->first, nullptr);
		}

		m_mapReqPlaySound.clear();

		// Release Played Channel
		m_pSystem->update();
	}
}

void FModManager::LoadBGM(const wchar_t* name, const wchar_t *theme)
{
	if (!m_bInitialized)
		return;

	NaString strName;

	if (theme == nullptr)
		strName.Format(L"%ls\\%ls.mp3", PathManager::GetBGMPath(m_pGame), name);
	else
		strName.Format(L"%ls\\%ls.mp3", PathManager::GetBGMPath(m_pGame, theme), name);

#ifdef WIN32
#else
    strName.ReplaceAll(L"\\", L"/");
#endif
    
	FMOD::Sound *pSound;
	FMOD_RESULT result;
	result = m_pSystem->createSound(
		strName.cstr(),
		FMOD_DEFAULT,
		0,
		&pSound
	);
	
	if (theme != NULL &&
		(result == FMOD_ERR_FILE_NOTFOUND || result == FMOD_ERR_FILE_EOF))
		return;

	if (theme == nullptr)
	{
		strName.Format(L"BGM_%ls", name);
		NaDebugOut(L"Load BGM: %ls\n", name);
	}
	else
	{
		if (result == FMOD_ERR_FILE_NOTFOUND || result == FMOD_ERR_FILE_EOF)
			return;
		strName.Format(L"BGM_%ls_%ls", theme, name);
		NaDebugOut(L"Load BGM: %ls (theme: %ls)\n", name, theme);
	}
	m_mapBGM.insert(NamedSoundPair(strName, pSound));
}

void FModManager::LoadSound(const wchar_t* name, const wchar_t* ext, const wchar_t *theme)
{
	if (!m_bInitialized)
		return;

	NaString strName;

	if (theme == nullptr)
		strName.Format(L"%ls\\%ls.%ls", PathManager::GetSoundPath(m_pGame), name, ext);
	else
		strName.Format(L"%ls\\%ls.%ls", PathManager::GetSoundPath(m_pGame, theme), name, ext);
#ifdef WIN32
#else
    strName.ReplaceAll(L"\\", L"/");
#endif
    
	FMOD_RESULT result;
	FMOD::Sound *pSound;
	result = m_pSystem->createSound(
		strName.cstr(),
		FMOD_DEFAULT,
		0,
		&pSound
	);

	if (theme != NULL && 
		(result == FMOD_ERR_FILE_NOTFOUND || result == FMOD_ERR_FILE_EOF))
		return;

	if (theme == nullptr)
	{
		strName.Format(L"Sound_%ls", name);
		NaDebugOut(L"Load Sound: %ls\n", name);
	}
	else
	{
		if (result == FMOD_ERR_FILE_NOTFOUND || result == FMOD_ERR_FILE_EOF)
			return;

		strName.Format(L"Sound_%ls_%ls", theme, name);
		NaDebugOut(L"Load Sound: %ls (theme: %ls)\n", name, theme);
	}
	m_mapSound.insert(NamedSoundPair(strName, pSound));
}

FMOD::Sound* FModManager::PlayBGM(const wchar_t * name, bool bLoop)
{
	if (!m_bInitialized)
		return nullptr;
	if (m_pGame->m_Option.m_bPlayBGM == false)
		return nullptr;

	FMOD_RESULT result;

	NaString strName;
	strName.Format(L"BGM_%ls", name);
	NamedSoundMap::iterator it = m_mapBGM.find(strName);
	if (it != m_mapBGM.end())
	{
		FMOD::Sound *pSound = &*it->second;

		if (bLoop)
			pSound->setMode(FMOD_LOOP_NORMAL);
		else
			pSound->setMode(FMOD_LOOP_OFF);

#ifdef WIN32
		result = m_pSystem->playSound(
			FMOD_CHANNEL_FREE,
			pSound,
			false,
			&m_pChannel[CHANNEL_BGM]);
#else
        result = m_pSystem->playSound(pSound, m_pGroup[GROUP_BGM], false, &m_pChannel[CHANNEL_BGM]);
#endif
        
		if (result != FMOD_OK)
		{
			NaDebugOut(L"Error PlayBGM: %ls\n", strName.wstr());
			return nullptr;

			//DebugBreak();
		}

		return pSound;
	}
	else
	{
		NaDebugOut(L"NotFound BGM: %ls\n", strName.wstr());
	}

	return nullptr;
}

FMOD::Sound* FModManager::PlaySound(const wchar_t * name)
{
	if (!m_bInitialized)
		return nullptr;
	if (m_pGame->m_Option.m_bPlaySound == false)
		return nullptr;

	FMOD_RESULT result;

	NaString strName;
	strName.Format(L"Sound_%ls", name);
	NamedSoundMap::iterator it = m_mapSound.find(strName);
	if (it != m_mapSound.end())
	{
		FMOD::Sound *pSound = &*it->second;

		pSound->setMode(FMOD_LOOP_OFF);
        
#ifdef WIN32
		result = m_pSystem->playSound(
			FMOD_CHANNEL_FREE,
			pSound,
			false,
			&m_pChannel[CHANNEL_SOUND]);
#else
        result = m_pSystem->playSound(pSound, m_pGroup[GROUP_SOUND], false, &m_pChannel[CHANNEL_SOUND]);
#endif

		if (result != FMOD_OK)
		{
			NaDebugOut(L"Error PlaySound: %ls\n", strName.wstr());
			return nullptr;

			//DebugBreak();
		}

		return pSound;
	}
	else
	{
		NaDebugOut(L"NotFound Sound: %ls\n", strName.wstr());
	}

	return nullptr;
}

FMOD::Sound * FModManager::RequestPlayBGM(const wchar_t *name, const wchar_t *theme, bool bLoop)
{
	if (!m_bInitialized)
		return nullptr;
	if (m_pGame->m_Option.m_bPlayBGM == false)
		return nullptr;

	FMOD_RESULT result;

	bool bNotExist = true;
	NaString strName;
	NamedSoundMap::iterator it;

	// Try play GameThemed BGM
	if (theme != nullptr)
	{
		// BGM_SMB3_XXX
		strName.Format(L"BGM_%ls_%ls", theme, name);
		it = m_mapBGM.find(strName);
		if (it == m_mapBGM.end())
		{
			// Not found, Try load...
			LoadBGM(name, theme);
			it = m_mapBGM.find(strName);
		}

		if (it != m_mapBGM.end())
			bNotExist = false;
	}

	if (bNotExist)
	{
		strName.Format(L"BGM_%ls", name);
		it = m_mapBGM.find(strName);
	}

	if (it != m_mapBGM.end())
	{
		FMOD::Sound *pSound = &*it->second;

		if (bLoop)
			pSound->setMode(FMOD_LOOP_NORMAL);
		else
			pSound->setMode(FMOD_LOOP_OFF);

#ifdef WIN32
		result = m_pSystem->playSound(
			FMOD_CHANNEL_FREE,
			pSound,
			false,
			&m_pChannel[CHANNEL_BGM]);
#else
		result = m_pSystem->playSound(pSound, m_pGroup[GROUP_BGM], false, &m_pChannel[CHANNEL_BGM]);
#endif

		if (result != FMOD_OK)
		{
#if defined(USE_FMOD_BREAK)
			DebugBreak();
#else
			NaDebugOut(L"Maybe bad BGM: %ls\n", strName.wstr());
			return nullptr;
#endif
		}

		return pSound;
	}
	else
	{
		NaDebugOut(L"NotFound BGM: %ls\n", strName.wstr());
	}

	return nullptr;
}

FMOD::Sound * FModManager::RequestPlaySound(const wchar_t * name, const wchar_t * theme)
{
	if (!m_bInitialized)
		return nullptr;
	if (m_pGame->m_Option.m_bPlaySound == false)
		return nullptr;

	FMOD_RESULT result;

	bool bNotExist = true;
	NaString strName;
	NamedSoundMap::iterator it;

	// Try play GameThemed Sound
	if (theme != nullptr)
	{
		// Sound_SMB3_XXX
		strName.Format(L"Sound_%ls_%ls", theme, name);
		it = m_mapSound.find(strName);
		if (it == m_mapSound.end())
		{
			// Not found, Try load...
			LoadSound(name, L"wav", theme);
			it = m_mapSound.find(strName);
		}

		if (it != m_mapSound.end())
			bNotExist = false;
	}

	if (bNotExist)
	{
		strName.Format(L"Sound_%ls", name);
		it = m_mapSound.find(strName);
	}

	if (it != m_mapSound.end())
	{
		FMOD::Sound *pSound = &*it->second;

		pSound->setMode(FMOD_LOOP_OFF);

#ifdef WIN32
		result = m_pSystem->playSound(
			FMOD_CHANNEL_FREE,
			pSound,
			false,
			&m_pChannel[CHANNEL_SOUND]);
#else
		result = m_pSystem->playSound(pSound, m_pGroup[GROUP_SOUND], false, &m_pChannel[CHANNEL_SOUND]);
#endif

		if (result != FMOD_OK)
		{
#if defined(USE_FMOD_BREAK)
			DebugBreak();
#else
			NaDebugOut(L"Maybe bad Sound: %ls\n", strName.wstr());
			return nullptr;
#endif
		}

		return pSound;
	}
	else
	{
		NaDebugOut(L"NotFound Sound: %ls\n", strName.wstr());
	}

	return nullptr;
}

FMOD::Sound * FModManager::RequestPlaySoundQueue(const wchar_t * name, const wchar_t * theme)
{
	if (m_pGame->m_Option.m_bPlaySound == false)
		return nullptr;

	// Block sound spam. just play once in 1 frame
	if (m_mapReqPlaySound.find(name) == m_mapReqPlaySound.end())
	{
		m_mapReqPlaySound.insert(
			SoundQueuePair(name, theme)
		);
	}
	return nullptr;
}

void FModManager::PauseBGM(bool bPause)
{
	if (!m_bInitialized)
		return;

	m_pChannel[CHANNEL_BGM]->setPaused(bPause);
}

void FModManager::StopBGM()
{
	if (!m_bInitialized)
		return;

	m_pChannel[CHANNEL_BGM]->stop();
}

void FModManager::StopSound()
{
	if (!m_bInitialized)
		return;

	m_pChannel[CHANNEL_SOUND]->stop();
}

bool FModManager::IsPlayingBGM()
{
	if (!m_bInitialized)
		return false;

	bool bRet;
	m_pChannel[CHANNEL_BGM]->isPlaying(&bRet);

	return bRet;
}
